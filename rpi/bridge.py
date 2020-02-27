import traceback
import asyncio
import aiohttp
import json
import time
from apscheduler.schedulers.asyncio import AsyncIOScheduler
from datetime import datetime
from datetime import timedelta
from RF24 import *
import RPi.GPIO as GPIO
import secret
from users import users

CE_PIN = 22
CSN_PIN = 0
IRQ_PIN = 23

tx_pipe = 0xF0F0F0F0E1
rx_pipe = 0xF0F0F0F0D2
payload_size = 32

server = 'https://cse237a-wi20-roomsign.sxn.dev/websocket'
summary_length = 21
time_length = 21
creator_length = 21
key_length = 4

class Event:
    def __init__(self, raw_event):
        self.start = datetime.fromisoformat(raw_event['start']['dateTime'])
        self.end = datetime.fromisoformat(raw_event['end']['dateTime'])

        # remove tzinfo
        self.start = self.start.replace(tzinfo=None)
        self.end = self.end.replace(tzinfo=None)

        if 'summary' in raw_event:
            self.summary = raw_event['summary']
        else:
            self.summary = '(No title)'

        self.creator = raw_event['creator']['email']


class Radio:
    def __init__(self):
        self.radio = RF24(CE_PIN, CSN_PIN, BCM2835_SPI_SPEED_8MHZ)
        self.payload = None

    def init(self):
        print('initializing RF24')
        self.radio.begin()
        self.radio.setDataRate(RF24_250KBPS)
        self.radio.setAutoAck(1)
        self.radio.setRetries(15, 15)
        self.radio.enableDynamicPayloads()
        self.radio.openWritingPipe(tx_pipe)
        self.radio.openReadingPipe(1, rx_pipe)

        # setup interrupt
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(IRQ_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(IRQ_PIN, GPIO.FALLING, callback=self.interrupt_handler)

        self.radio.printDetails()
        self.radio.startListening()

    def interrupt_handler(self, channel=0):
        if self.radio.available():
            print('interrupt!')
            operation = self.radio.read(1)
            if operation[0] == 1:
                # OPERATION_FETCH
                self.send_payload()
            else:
                print('unknown operation', operation)


    def build_bytes(self, s, length):
        b = bytes(s[:length-1], 'ascii')
        if len(b) < length:
            b += bytes([ord('\0')] * (length - len(b)))
        return b

    def time_to_str(self, event):
        s = event.start.strftime('%I:%M%p') + '-' + event.end.strftime('%I:%M%p')
        if event.end.date() > event.start.date():
            s += '+{}'.format((event.end.date() - event.start.date()).days)
        return s

    def send_payload(self):
        self.radio.stopListening()
        print('sending (size={})'.format(len(self.payload)), self.payload)
        succeed = False
        while not succeed:
            succeed = True
            for i in range(0, len(self.payload), payload_size):
                if (not self.radio.write(self.payload[i:i+payload_size])):
                    print('failed')
                    # succeed = False
                    # time.sleep(1)  # retry after 1 second
                    succeed = True # force exit
                    break
        self.radio.startListening()

    def send_data(self, event):
        # generate payload
        if event is None:
            available = b'\x01'
            summary = self.build_bytes('', summary_length)
            time = self.build_bytes('', time_length)
            creator = self.build_bytes('', time_length)
            key_id = self.build_bytes('', key_length)
        else:
            available = b'\x00'
            summary = self.build_bytes(event.summary, summary_length)
            time = self.build_bytes(self.time_to_str(event), time_length)
            if event.creator in users:
                creator = self.build_bytes(users[event.creator]['name'], creator_length)
                key_id = bytes(users[event.creator]['key_id'])
        self.payload = available + summary + time + creator + key_id

        self.send_payload()


class Bridge:
    def __init__(self):
        self.events = []
        self.curr_event = None
        self.initialized = False

        self.events_lock = asyncio.Lock()

        self.radio = Radio()

        self.scheduler = AsyncIOScheduler()
        self.scheduler.start()
        self.scheduler_job = None

    def compare_events(self, e1, e2):
        if e1 is None and e2 is None:
            return True
        if e1 is None or e2 is None:
            return False
        return e1.start == e2.start and e1.end == e2.end and e1.summary == e2.summary and e1.creator == e2.creator

    async def job_handler(self):
        self.scheduler_job = None
        await self.update_curr_event()

    async def update_curr_event(self):
        async with self.events_lock:
            now = datetime.now()
            new_curr_event = None
            next_event_time = None
            for event in self.events:
                if event.start > now:
                    next_event_time = event.start
                    break
                if event.end >= now:
                    new_curr_event = event
                    next_event_time = event.end
                    break
            if not self.initialized or not self.compare_events(self.curr_event, new_curr_event):
                if not self.initialized:
                    self.initialized = True
                    self.radio.init()
                self.curr_event = new_curr_event
                print('curr_event updated')
                self.radio.send_data(self.curr_event)
            if self.scheduler_job is not None:
                self.scheduler_job.remove()
                self.scheduler_job = None
            if next_event_time is not None:
                self.scheduler_job = self.scheduler.add_job(self.job_handler, 'date', run_date=next_event_time)   


    async def parse_events(self, raw_events):
        async with self.events_lock:
            # parse events
            self.events = []
            for raw_event in raw_events:
                self.events.append(Event(raw_event))
        await self.update_curr_event()



    async def client(self):
        try:
            session = aiohttp.ClientSession()
            async with session.ws_connect(
                url=server,
                headers={'Authorization': 'Bearer ' + secret.WS_TOKEN}) as ws:

                async for msg in ws:
                    if msg.type == aiohttp.WSMsgType.ERROR:
                        print('ws connection closed with exception', ws.exception())
                        break
                    elif msg.type == aiohttp.WSMsgType.TEXT:
                        raw_events = json.loads(msg.data)
                        print('new data received')
                        await self.parse_events(raw_events)
            await session.close()
        except Exception as e:
            traceback.print_tb(e.__traceback__)
            print(e)


    def main(self):
        while True:
            asyncio.get_event_loop().run_until_complete(self.client())
            print('retry in 10s')
            time.sleep(10)


if __name__ == '__main__':
    Bridge().main()
