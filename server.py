import socket
import pytesseract
from PIL import Image
import requests
from io import BytesIO

# Configure Tesseract OCR path if needed
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'  
def download_image(url):
    response = requests.get(url)
    response.raise_for_status()  # Ensure we got a valid response
    return Image.open(BytesIO(response.content))

def extract_text(image):
    return pytesseract.image_to_string(image)

def start_server(host='127.0.0.1', port=65432):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"Server listening on {host}:{port}")
        
        while True:
            client_socket, client_address = server_socket.accept()
            print(f"Connection from {client_address}")
            with client_socket:
                while True:
                    # Receive URL
                    url = client_socket.recv(1024).decode()
                    if not url:
                        break
                    
                    if url.lower() == 'exit':
                        print("Client closed the connection.")
                        break

                    print(f"Received URL: {url}")
                    try:
                        # Process the image
                        image = download_image(url)
                        text = extract_text(image)
                        
                        # Send text back to the client
                        client_socket.sendall(text.encode())
                    except Exception as e:
                        error_message = f"Error: {str(e)}"
                        client_socket.sendall(error_message.encode())


if __name__ == "__main__":
    start_server()