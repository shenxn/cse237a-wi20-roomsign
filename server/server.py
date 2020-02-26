import asyncio
from http import HTTPStatus
import aiohttp
from aiohttp import web
import json
import google_api
import config
import secret


async def ws_authenticate(path, request_headers):
    if 'Token' not in request_headers or request_headers['Token'] != secret.WS_TOKEN:
        return (HTTPStatus.FORBIDDEN, {}, b'')

class Server:
    def __init__(self):
        self.ws = None
        self.ws_lock = asyncio.Lock()
        self.lock = asyncio.Lock()
        self.last_data = None

    async def get_send(self, refresh):
        # get events from Google Calendar API and send through websocket

        async with self.lock:
            if not refresh or self.last_data is None:
                events = google_api.get_events()
                self.last_data = events
            await self.ws.send_str(self.last_data)
            print('data sent')

    async def websocket_handler(self, request):
        async with self.ws_lock:
            if (self.ws is not None):
                # support only one websocket connection
                await self.ws.close()
            
            # authentication
            if not 'Authorization' in request.headers:
                return web.HTTPUnauthorized()

            if request.headers['Authorization'] != 'Bearer ' + secret.WS_TOKEN:
                return web.HTTPForbidden()

            ws = web.WebSocketResponse()
            await ws.prepare(request)
            self.ws = ws

        await self.get_send(refresh=False)

        # block wait
        async for msg in ws:
            if msg.type == aiohttp.WSMsgType.ERROR:
                print('ws connection closed with exception', ws.exception())
        
        self.ws = None
        return ws

    async def webhook_handler(self, request):
        if self.ws is None:
            return web.HTTPOk()
        # TODO check incoming traffic
        await self.get_send(refresh=True)
        return web.HTTPOk()

    def main(self):
        google_api.init()

        print('starting server')
        app = web.Application()
        app.add_routes([
            web.post('/webhook', self.webhook_handler),
            web.get('/websocket', self.websocket_handler)
        ])
        web.run_app(app, port=config.PORT)
        # server = websockets.serve(ws_handler, port=config.PORT, process_request=ws_authenticate)
        # asyncio.get_event_loop().run_until_complete(server)
        # asyncio.get_event_loop().run_forever()


if __name__ == '__main__':
    Server().main()
