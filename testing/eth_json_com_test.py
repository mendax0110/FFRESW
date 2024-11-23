import socket
import json

TARGET_IP = "192.168.1.3"
PORT = 80


def main():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(5)  # Set a 5-second timeout
            print(f"Connecting to {TARGET_IP}:{PORT}...")
            s.connect((TARGET_IP, PORT))
            print("Connected successfully.")

            # Send a simple HTTP GET request
            request = "GET / HTTP/1.1\r\nHost: {}\r\nConnection: close\r\n\r\n".format(TARGET_IP)
            s.sendall(request.encode('utf-8'))

            # Receive response
            response = b""
            try:
                while True:
                    data = s.recv(1024)
                    if not data:
                        break
                    response += data
            except socket.timeout:
                print("Socket timed out while waiting for data.")

            # Decode and process the response
            response_str = response.decode('utf-8')
            print("Raw Response:")
            print(response_str)

            try:
                json_start = response_str.find("{")
                if json_start != -1:
                    json_data = response_str[json_start:]
                    parsed_json = json.loads(json_data)
                    print("\nParsed JSON Output:")
                    print(json.dumps(parsed_json, indent=4))
                else:
                    print("No JSON found in the response.")
            except json.JSONDecodeError as e:
                print(f"JSON parsing error: {e}")

    except (ConnectionError, socket.timeout) as e:
        print(f"Connection error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")


if __name__ == "__main__":
    main()
