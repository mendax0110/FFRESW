import socket
import logging
import argparse
from enum import Enum

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

# Service codes in hexstring
class Service(Enum):
    SET = "01"               # Standard SET command
    GET = "0B"               # Standard GET command
    SET_VOLATILE = "2B"      # SET without saving in NV memory
    COMPOUND_SET = "28"      # Compound SET command
    COMPOUND_GET = "29"      # Compound GET command
    COMPOUND_SET_GET = "30"  # Compound SET/GET command

# Parameter IDs for standard settings and compound commands
class ParameterID(Enum):
    # Standard parameter examples
    FORMAT_INTEGER = "06020201"       # Format setting for all integer values
    FORMAT_FLOATING_POINT = "06020202"  # Format setting for all floating point values

    # Compound parameter IDs for Ethernet 
    COMPOUND_1 = "A90A0100"  # Compound 1 for SET commands (up to 20 members)
    COMPOUND_2 = "A90A0200"  # Compound 2 for GET commands (up to 20 members)
    COMPOUND_3 = "A90A0300"  # Compound 3 for SET/GET commands (up to 20 members)
    COMPOUND_4 = "A90A0400"  # Compound 4 for future use

class ErrorCode(Enum):
    NO_ERROR = "00"                      
    WRONG_COMMAND_LENGTH = "0C"          
    VALUE_TOO_LOW = "1C"                 
    VALUE_TOO_HIGH = "1D"                
    RESULTING_ZERO_ADJUST_OFFSET = "20"  
    SENSOR_VOLTAGE_TOO_HIGH_LOW = "21"   
    NOT_VALID_NO_SENSOR = "22"           
    WRONG_ACCESS_MODE = "50"             
    TIMEOUT = "51"                       
    NV_MEMORY_NOT_READY = "6D"           
    WRONG_PARAMETER_ID = "6E"            
    PARAMETER_NOT_SETTABLE = "70"        
    PARAMETER_NOT_READABLE = "71"        
    WRONG_PARAMETER_INDEX = "73"         
    WRONG_VALUE_WITHIN_RANGE = "76"      
    NOT_ALLOWED_IN_THIS_STATE = "78"     
    SETTING_LOCK = "79"                  
    WRONG_SERVICE = "7A"                 
    PARAMETER_NOT_ACTIVE = "7B"          
    PARAMETER_SYSTEM_ERROR = "7C"        
    COMMUNICATION_ERROR = "7D"           
    UNKNOWN_SERVICE = "7E"               
    UNEXPECTED_CHARACTER = "7F"          
    NO_ACCESS_RIGHTS = "80"              
    NO_ADEQUATE_HARDWARE = "81"          
    WRONG_OBJECT_STATE = "82"            
    NO_SLAVE_COMMAND = "84"              
    COMMAND_TO_UNKNOWN_SLAVE = "85"      
    COMMAND_TO_MASTER_ONLY = "87"        
    ONLY_G_COMMAND_ALLOWED = "88"        
    NOT_SUPPORTED = "89"                 
    FUNCTION_DISABLED = "A0"             
    ALREADY_DONE = "A1"

# SET Compound
class Compound1(Enum):
    CONTROL_MODE = "0F020000"      # Index 00: Control Mode
    TARGET_POSITION = "11020000"   # Index 01: Target Position
    TARGET_PRESSURE = "07020000"   # Index 02: Target Pressure
    # Further indexes (unused) should be "00000000"

# GET Compound
class Compound2(Enum):
    ACCESS_MODE = "0F0B0000"         # Index 00: Access Mode
    CONTROL_MODE = "0F020000"        # Index 01: Control Mode
    ACTUAL_POSITION = "10010000"      # Index 02: Actual Position
    POSITION_STATE = "00100000"      # Index 03: Position State
    ACTUAL_PRESSURE = "07010000"      # Index 04: Actual Pressure
    TARGET_PRESSURE_USED = "07030000" # Index 05: Target Pressure Used
    WARNING_BITMAP = "0F300100"      # Index 06: Warning Bitmap
    # Unused indexes set to "00000000"

# SET and GET Compound
class Compound3(Enum):
    # SET part (indexes 00, 01, 02)
    CONTROL_MODE_SET = "0F020000"      # Index 00: Control Mode (SET)
    TARGET_POSITION_SET = "11020000"   # Index 01: Target Position (SET)
    TARGET_PRESSURE_SET = "07020000"   # Index 02: Target Pressure (SET)
    # Separation index (Index 03) between SET and GET is always "00000000"
    SEPARATION = "00000000"
    # GET part (indexes 04, 05, 06, 07, etc.)
    ACCESS_MODE_GET = "0F0B0000"         # Index 04: Access Mode (GET)
    CONTROL_MODE_GET = "0F020000"        # Index 05: Control Mode (GET)
    ACTUAL_POSITION_GET = "10010000"      # Index 06: Actual Position (GET)
    POSITION_STATE_GET = "00100000"      # Index 07: Position State (GET)
    ACTUAL_PRESSURE_GET = "07010000"      # Index 08: Actual Pressure (GET)
    TARGET_PRESSURE_USED_GET = "07030000" # Index 09: Target Pressure Used (GET)
    WARNING_BITMAP_GET = "0F300100"      # Index 10: Warning Bitmap (GET)
    # Further unused indexes set to "00000000"

# Main control class
class ValveEthernetControl:
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.sock = None
        logging.debug("ValveEthernetControl initialized with IP: %s, Port: %s", ip, port)

    def connect(self):
        """Establish connection to the valve on the specified IP and port."""
        logging.debug("Attempting to connect to %s:%s", self.ip, self.port)
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.ip, self.port))
            logging.info("Connected to %s:%s", self.ip, self.port)
        except Exception as e:
            logging.error("Failed to connect to %s:%s - %s", self.ip, self.port, str(e))
            return False
        return True

    def send_command(self, service, parameter_id, index, value=None, termination='\n'):
        """
        Send a command to the valve.
        Constructs the command in the following format (no spaces):
        p:{service}{parameter_id}{index}{value}
        For compound commands, the 'value' field will be a semicolon-separated list.
        """
        if not self.sock:
            logging.error("Cannot send command: not connected.")
            return None
        
        command = f"p:{service.value}{parameter_id.value}{index}"
        
        if value is not None:
            command += f"{value}"
        message = f"{command}{termination}"
        message_length = len(message)
        
        # if message_length < 20:
        #     logging.error("Command length is too short, must be at least 20 characters: %d", message_length)
        #     return None
        if message_length > 100:
            logging.error("Command length exceeds maximum allowed length: %d", message_length)
            return None
        
        logging.debug("Sending command: %s", message)
        try:
            self.sock.sendall(message.encode())
            logging.debug("Command sent, waiting for response...")
            response = self.sock.recv(1024).decode()
            logging.debug("Received response: %s", response)
            return response
        except Exception as e:
            logging.error("Error sending command: %s", str(e))
            return None

    def send_compound_command(self, service, compound_parameter_id, values, termination='\n'):
        """
        Send a compound command to the valve.
        The command format is:
          p:{service}{compound_parameter_id}{index}{value;value;value;...}
        Here, index is fixed to "00".
        """
        index = "00"
        value_str = ';'.join(values)
        logging.debug("Sending compound command with values: %s", value_str)
        return self.send_command(
            service,
            compound_parameter_id,
            index,
            value=value_str,
            termination=termination
        )

    def close_connection(self):
        """Close the socket connection."""
        if self.sock:
            logging.debug("Closing connection.")
            self.sock.close()
            logging.info("Connection closed.")
        else:
            logging.debug("Connection already closed or was never established.")
        self.sock = None

def main():
    parser = argparse.ArgumentParser(description="Valve Ethernet Control Command Sender")
    
    parser.add_argument("service", choices=[s.name for s in Service], help="Service to be used (SET, GET, etc.)")
    parser.add_argument("parameter_id", help="Compound enum or parameter id to be used (e.g., Compound1.TARGET_POSITION)")
    parser.add_argument("value", nargs="?", help="Value to be sent with the command")
    
    args = parser.parse_args()

    # Configure the valve IP and port
    valve_ip = "192.168.1.10"
    valve_port = 503

    valve_control = ValveEthernetControl(valve_ip, valve_port)

    if valve_control.connect():
        logging.debug("Connected successfully, proceeding with command.")
        
        service = Service[args.service]
        try:
            if '.' in args.parameter_id:
                compound_name, enum_value = args.parameter_id.split('.')
                compound_enum = globals().get(compound_name)
                if compound_enum:
                    parameter_id = compound_enum[enum_value]
                else:
                    logging.error(f"Compound {compound_name} not found.")
                    return
            else:
                parameter_id = ParameterID[args.parameter_id]
        except KeyError:
            logging.error(f"Invalid parameter ID or compound ID provided: {args.parameter_id}")
            return
        
        if service == Service.GET and args.value:
            logging.warning("GET command does not require a value. Ignoring the value provided.")
            return

        if service == Service.SET or Service.GET:
            logging.debug("Values to be sent: %s", args.value)
            response = valve_control.send_command(
                service,
                parameter_id,
                "00",  # Default index for compound commands
                value=args.value
            )
         # TODO: fix this setter and getter for compound commands
        elif service == Service.COMPOUND_SET or Service.COMPOUND_GET or Service.COMPOUND_SET_GET:
            logging.debug("Values to be sent: %s", args.value)
            response = valve_control.send_compound_command(
                service,
                parameter_id,
                args.value.split(';')
            )
        
        logging.info("Response: %s", response)
        valve_control.close_connection()
    else:
        logging.error("Connection failed. Exiting.")

if __name__ == "__main__":
    main()
    
    
# examples
# python test.py SET Compound1.CONTROL_MODE 4 // postion 100%
# python test.py SET Compound1.CONTROL_MODE 3 // postion 0%
# python test.py GET Compound2.ACTUAL_POSITION
# python test.py GET Compound2.ACTUAL_PRESSURE
# python test.py GET Compound2.WARNING_BITMAP
# python test.py SETGET SET Compound2.ACCESS_MODE 1
