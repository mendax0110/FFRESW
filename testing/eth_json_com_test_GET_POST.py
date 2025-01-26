import socket
import json
import time

TARGET_IP = "192.168.1.3"
PORT = 80

# List of specific endpoints to check
ENDPOINTS = [
    "/temperature_sensor_1",
    "/temperature_sensor_2"
]

def connect_and_get_data(endpoint):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(5)  # Set a 5-second timeout
            print(f"Connecting to {TARGET_IP}:{PORT}...")
            s.connect((TARGET_IP, PORT))
            print("Connected successfully.")

            # Send HTTP GET request for the specific endpoint
            request = f"GET {endpoint} HTTP/1.1\r\nHost: {TARGET_IP}\r\nConnection: close\r\n\r\n"
            s.sendall(request.encode('utf-8'))

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
            print(f"Raw Response from {endpoint}:")
            print(response_str)

            # Now check for JSON
            try:
                json_start = response_str.find("{")
                if json_start != -1:
                    json_data = response_str[json_start:]  # Extract JSON body
                    parsed_json = json.loads(json_data)    # Parse JSON

                    # Display parsed JSON fields
                    print("\nParsed JSON Output:")
                    print(f"Sensor Name: {parsed_json.get('sensor_name', 'N/A')}")
                    print(f"Value: {parsed_json.get('value', 'N/A')}")
                    print(f"Unit: {parsed_json.get('unit', 'N/A')}")
                    print(f"Timestamp: {parsed_json.get('timestamp', 'N/A')}")
                else:
                    print("No JSON found in the response.")
            except json.JSONDecodeError as e:
                print(f"JSON parsing error: {e}")

    except (ConnectionError, socket.timeout) as e:
        print(f"Connection error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")


def main():
    while True:
        for endpoint in ENDPOINTS:
            print(f"\nChecking endpoint: {endpoint}")
            connect_and_get_data(endpoint)
            print("\nWaiting for 5 seconds before checking the next endpoint...")
            time.sleep(5)  # Wait before checking the next endpoint

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nProgram manually aborted.")
