import socket
import errno


def get_message_and_remainder(data):
    if not data:
        return None, data
    if data[0] == ":":
        parsed = data[1:].split("\r\n")[0]
        return int(parsed), data[len(parsed) + 3 :]

    if data[0] in ["-", "+"]:
        parsed = data[1:].split("\r\n")[0]
        return parsed, data[len(parsed) + 3 :]

    if data[0] == "$":
        num, after = f"{data}\r\n".split("\r\n", 1)
        after = after[:-2]
        num = int(num[1:])
        if len(after) - 2 < num:
            return None, data
        return after[:num], after[num + 2 :]

    if data[0] == "*":
        data_ = data
        num, after = f"{data}\r\n".split("\r\n", 1)
        data = after[:-2]
        num = int(num[1:])
        arr = list()

        for _ in range(num):
            chunk, data = get_message_and_remainder(data)
            if chunk is None:
                return None, data_
            arr.append(chunk)
        return arr, data

    return None, data


class Redis:
    def __init__(self, host, port=6379, username=None, password=None):
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.socket = None
        self.unprocessed_bytes = ""
        self.subscription = None

    def connect(self):
        self.socket = s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        addr = socket.getaddrinfo(self.host, self.port)[0][-1]
        s.connect(addr)
        if not password:
            return
        if not username:
            s.sendall(f"AUTH {password}\r\n".encode())
            return
        s.sendall(f"AUTH {username} {password}\r\n".encode())

    def send(self, cmd, unsubscribe_before=True):
        if unsubscribe_before and self.subscription is not None:
            self.send(f"UNSUBSCRIBE {self.subscription}", False)
            self.subscription = None

        try:
            self.socket.sendall(f"{cmd}\r\n".encode())
        except Exception:
            self.connect()
            self.socket.sendall(f"{cmd}\r\n".encode())

    def read_message(self):
        s = self.socket
        data = self.unprocessed_bytes
        if get_message_and_remainder(data)[0] is not None:
            msg, self.unprocessed_bytes = get_message_and_remainder(data)
            return msg

        try:
            data += s.recv(1024).decode()
        except OSError as e:
            if e.errno == errno.ETIMEDOUT:
                return None
            self.connect()
        except Exception as e:
            self.connect()

        while True:
            message, data = get_message_and_remainder(data)
            if message is not None:
                self.unprocessed_bytes = data
                return message
            try:
                data += s.recv(1024).decode()
            except OSError as e:
                if e.errno == errno.ETIMEDOUT:
                    return None

    def first_channel_message(self, channel):
        if self.subscription != channel:
            self.send(f"SUBSCRIBE {channel}")
            self.subscription = channel

        self.socket.settimeout(0.1)
        msg = self.read_message()
        self.socket.settimeout(None)

        if isinstance(msg, list) and msg[0] == "message":
            return msg[2]


door = "***REMOVED***"

host = "***REMOVED***"
port = ***REMOVED***
username = "admin"
password = "***REMOVED***"

# print(f"redis://{username}:{password}@{host}:{port}")

# r = Redis(host, port, username, password)
# print(r.first_channel_message("test"))
# print(r.first_channel_message("test"))

client = Redis(host, port, username, password)


def send_locked(is_locked):
    cmd = "lock" if is_locked else "unlock"
    return client.send(f"PUBLISH door_{door} {cmd}")


def next_door_message():
    return client.first_channel_message(f"door_{door}")
