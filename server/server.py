import asyncio
import websockets
from http import HTTPStatus
import json
import google_api
import config
import secret


async def ws_authenticate(path, request_headers):
    if 'Token' not in request_headers or request_headers['Token'] != secret.WS_TOKEN:
        return (HTTPStatus.FORBIDDEN, {}, b'')


async def ws_handler(ws, path):
    try:
        # send initial data
        events = google_api.get_events()
        await ws.send(json.dumps(events))
        print('initial data sent')

        # block wait
        msg = await ws.recv()

    except websockets.exceptions.ConnectionClosedOK:
        print('connection closed')



def main():
    google_api.init()

    print('starting server')
    server = websockets.serve(ws_handler, port=config.PORT, process_request=ws_authenticate)
    asyncio.get_event_loop().run_until_complete(server)
    asyncio.get_event_loop().run_forever()


if __name__ == '__main__':
    main()
