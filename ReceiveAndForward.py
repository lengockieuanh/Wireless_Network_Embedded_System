import socket
import threading

def forward_packet(data, target_ip, target_port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as forward_socket:
        try:
            forward_socket.connect((target_ip, target_port))
            forward_socket.sendall(data)
            print(f"Da chuyen tiep goi tin toi {target_ip}:{target_port}")
        except Exception as e:
            print(f"Loi khi chuyen tiep goi tin: {e}")

def start_server(listen_ip, listen_port, target_ip, target_port):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((listen_ip, listen_port))
        server_socket.listen(5)
        print(f"Dang lang nghe tren {listen_ip}:{listen_port}")

        while True:
            client_socket, client_address = server_socket.accept()
            print(f"Nhan ket noi tu {client_address}")
            data = client_socket.recv(4096)
            if data:
                threading.Thread(target=forward_packet, args=(data, target_ip, target_port)).start()
            client_socket.close()

listen_ip = '0.0.0.0'
listen_port = 12345 
target_ip = '192.168.1.20' 
target_port = 1008

start_server(listen_ip, listen_port, target_ip, target_port)