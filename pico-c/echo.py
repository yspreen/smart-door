import socket

def start_tcp_server(ip, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((ip, port))
    server_socket.listen(1)
    print(f"Listening on {ip}:{port}")

    while True:
        client_socket, client_address = server_socket.accept()
        print(f"Accepted connection from {client_address}")

        while True:
            data = client_socket.recv(1024)
            if not data:
                break

            print(f"Received: {data.decode()}")
            client_socket.sendall(data)

        client_socket.close()

# Change the IP and port as needed
start_tcp_server('0.0.0.0', 12346)
