
import hmac
from hashlib import sha256
import json
import uuid
import zmq
from datetime import datetime

if __name__ == '__main__':
    ctx = zmq.Context()

    shell_ch = ctx.socket(zmq.DEALER)
    shell_ch.connect("tcp://127.0.0.1:50839")

    sub_ch = ctx.socket(zmq.SUB)
    sub_ch.connect("tcp://127.0.0.1:50840")
    sub_ch.setsockopt(zmq.SUBSCRIBE, b"")

    session_id = uuid.uuid4()

    auth = hmac.HMAC("12859715-d85fe7b7edb2dd06fba11fb6".encode("utf8"), digestmod=sha256)

    for i in range(1):
        header = {
            "msg_id": f"{i}", 
            "session": f"{session_id}", 
            "username": "andy", 
            "data": f"{datetime.utcnow()}",
            # "msg_type": "kernel_info_request", 
            "msg_type": "execute_request", 
            "version": "5.3"
            }

        msg = {
            "header": header, 
            "parent_header": {}, 
            "metadata": {}, 
            "content": {
                "code": "a=1+2",
                "silent": False,
                "store_history": False,
                'user_expressions': {

                },
                'allow_stdin' : False,
                'stop_on_error' : True
            }
        }
        # msg["buffers"] = []

        real_message = [
            json.dumps(msg["header"]).encode('utf8'),
            json.dumps(msg["parent_header"]).encode('utf8'),
            json.dumps(msg["metadata"]).encode('utf8'),
            json.dumps(msg["content"]).encode('utf8')
        ]

        signature = auth.copy()
        for item in real_message:
            signature.update(item)
        signature = signature.hexdigest().encode()

        DELIM = b"<IDS|MSG>"
        to_send = [DELIM, signature]
        to_send.extend(real_message)

        for j in to_send[:-1]:
            shell_ch.send(j, zmq.SNDMORE)
        shell_ch.send(to_send[-1])

        print("succeed to  Request Info--------------------")

        poller = zmq.Poller()
        poller.register(shell_ch, zmq.POLLIN)
        poller.register(sub_ch, zmq.POLLIN)

        while True:
            socks = dict(poller.poll())

            if shell_ch in socks and socks[shell_ch] == zmq.POLLIN:
                print("")
                print("Begin to get Reply--------------------------")
                print(shell_ch.recv().decode('utf8'))
                while shell_ch.get(zmq.RCVMORE):
                    part = shell_ch.recv().decode('utf8')
                    print(part)

            if sub_ch in socks and socks[sub_ch] == zmq.POLLIN:
                print("")
                print("Begin to get Sub--------------------------")
                print(sub_ch.recv().decode('utf8'))
                while sub_ch.get(zmq.RCVMORE):
                    part = sub_ch.recv().decode('utf8')
                    print(part)
