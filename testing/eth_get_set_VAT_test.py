import socket
import struct
import time
import argparse
import re

TARGET_IP = "192.168.1.10"
PORT = 503

class Service:
    SET = "01"
    GET = "0B"
    SETGET = "30"
    GET_COMPOUND = "29"
    SET_COMPOUND = "28"

class Compound1:
    PARAMETERS = [
        ("Control Mode", "0F020000"),
        ("Target Position", "11020000"),
        ("Target Pressure", "07020000"),
        ("Not Used", "00000000"),
    ]

class Compound2:
    PARAMETERS = [
        ("Access Mode", "0F0B0000"),
        ("Control Mode", "0F020000"),
        ("Actual Position", "10010000"),
        ("Position State", "00100000"),
        ("Actual Pressure", "07010000"),
        ("Target Pressure Used", "07030000"),
        ("Warning Bitmap", "0F300100"),
        ("Not Used", "00000000"),
    ]

class Compound3:
    PARAMETERS = [
        ("Control Mode", "0F020000"),
        ("Target Position", "11020000"),
        ("Target Pressure", "07020000"),
        ("Separation", "00000000"),
        ("Access Mode", "0F0B0000"),
        ("Actual Position", "10010000"),
        ("Position State", "00100000"),
        ("Actual Pressure", "07010000"),
        ("Target Pressure Used", "07030000"),
        ("Warning Bitmap", "0F300100"),
        ("Not Used", "00000000"),
    ]

ERROR_CODES = {
    "00": "no error",
    "0C": "wrong command length",
    "1C": "value too low",
    "1D": "value too high",
    "20": "resulting zero adjust offset",
    "21": "not valid because no sensor enabled",
    "50": "wrong access mode",
    "51": "timeout",
    "6D": "NV Memory not ready",
    "6E": "wrong parameter ID",
    "70": "parameter not settable",
    "71": "parameter not readable",
    "73": "wrong parameter index",
    "76": "wrong value within range",
    "78": "not allowed in this state",
    "79": "setting lock",
    "7A": "wrong service",
    "7B": "parameter not active",
    "7C": "parameter system error",
    "7D": "communication error",
    "7E": "unknown service",
    "7F": "unexpected character",
    "80": "no access rights",
    "81": "no adequately hardware",
    "82": "wrong object state",
    "84": "no slave command",
    "85": "command to unknown slave",
    "87": "command to master only",
    "88": "only G command allowed",
    "89": "not supported",
    "A0": "function is disabled",
    "A1": "already done",
}

def format_value(value, is_float=True):
    """Convert value to IEEE-754 if float, otherwise return integer representation."""
    if is_float:
        return format(struct.unpack('<I', struct.pack('<f', float(value)))[0], '08X')
    return format(int(value), 'X')

def parse_response(response):
    """Parse the response, converting IEEE-754 hex to float when necessary."""
    match = re.findall(r"([A-F0-9]{8})", response)
    parsed_values = []
    for val in match:
        try:
            parsed_values.append(struct.unpack('<f', bytes.fromhex(val))[0])  # Try float conversion
        except:
            parsed_values.append(int(val, 16))  # Fallback to integer conversion
    return parsed_values if parsed_values else response

def check_error_code(response):
    """Check if response contains error codes and return readable messages."""
    errors = []
    for code, message in ERROR_CODES.items():
        if code in response:
            errors.append(f"{message} (Code {code})")
    if errors:
        return f"[ERROR] {', '.join(errors)}"
    return response

def send_command(command: str):
    """Send the command to the target and return the response."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((TARGET_IP, PORT))
            s.sendall(command.encode() + b'\n')
            response = s.recv(1024).decode().strip()
            return check_error_code(response)
        except Exception as e:
            return f"[ERROR] {str(e)}"

def build_set_compound_command(compound, values):
    """Construct SET Compound command."""
    if len(values) != len(compound.PARAMETERS):
        return "[ERROR] Invalid number of values"
    formatted_values = ";".join([format_value(v) for v in values])
    compound_id = "A9" + compound.PARAMETERS[0][1][:2] + "0A" + compound.PARAMETERS[0][1][4:6] + "00"
    return f"p:{Service.SET_COMPOUND}{compound_id}{formatted_values}"

def build_get_compound_command(compound):
    """Construct GET Compound command."""
    compound_id = "A9" + compound.PARAMETERS[0][1][2:6] + "00"
    return f"p:{Service.GET_COMPOUND}{compound_id}00"

def set_compound(compound, values):
    """Build and send the SET command for the given compound and values."""
    cmd = build_set_compound_command(compound, values)
    print(f"Sending SET Compound command: {cmd}")
    response = send_command(cmd)
    for code, message in ERROR_CODES.items():
        if code in response:
            return f"[ERROR] {message} (Code {code})"
    return response

def get_compound(compound, param_name=None):
    """Build and send the GET command for the given compound and parameter."""
    if param_name:
        param_hex = find_param(compound, param_name)
        if param_hex:
            cmd = f"p:{Service.GET}{param_hex}00"
            print(f"Sending GET command for {param_name}: {cmd}")
            return send_command(cmd)
        return f"[ERROR] Parameter {param_name} not found"
    
    cmd = build_get_compound_command(compound)
    print(f"Sending GET Compound command: {cmd}")
    return send_command(cmd)

def find_param(compound, param_name):
    """Find the parameter hex ID by name, allowing case-insensitive lookup."""
    for param, hex_value in compound.PARAMETERS:
        if param_name.lower() in param.lower():
            return hex_value
    return None

def set_param(param_name, value, compound):
    """Build and send the SET command for a single parameter."""
    param_hex = find_param(compound, param_name)
    if param_hex:
        cmd = f"p:{Service.SET}{param_hex}00{format_value(value)}"
        print(f"Sending SET command for {param_name}: {cmd}")
        response = send_command(cmd)
        for code, message in ERROR_CODES.items():
            if code in response:
                return f"[ERROR] {message} (Code {code})"
        return response
    return f"[ERROR] Parameter {param_name} not found"

def get_param(param_name, compound):
    """Build and send the GET command for a single parameter."""
    param_hex = find_param(compound, param_name)
    if param_hex:
        cmd = f"p:{Service.GET}{param_hex}00"
        print(f"Sending GET command for {param_name}: {cmd}")
        response = send_command(cmd)
        for code, message in ERROR_CODES.items():
            if code in response:
                return f"[ERROR] {message} (Code {code})"
        return response
    return f"[ERROR] Parameter {param_name} not found"

def test_all():
    """Test all possible commands for all compounds."""
    compounds = {"1": Compound1, "2": Compound2, "3": Compound3}
    for compound_id, compound in compounds.items():
        print(f"\n===== Testing Compound {compound_id} =====")
        test_values = [round(10.5 + i * 1.5, 2) for i in range(len(compound.PARAMETERS))]
        print("\n[1] Testing SET...")
        print(f"Response: {set_compound(compound, test_values)}")
        print("\n[2] Testing GET...")
        print(f"Response: {get_compound(compound)}")
        print("\n[3] Testing SETGET...")
        print(f"SET Response: {set_compound(compound, test_values)}")
        time.sleep(0.5)
        print(f"GET Response: {get_compound(compound)}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Send Compound commands over Ethernet.")
    parser.add_argument("--compound", choices=["1", "2", "3"], help="Compound type")
    parser.add_argument("--service", choices=["SET", "GET", "SETGET", "TEST_ALL"], required=True, help="Service type")
    parser.add_argument("--values", nargs='+', type=str, help="Values for SET command")
    parser.add_argument("--param", type=str, help="Specify a parameter to get (e.g., 'Target Position')")

    args = parser.parse_args()
    
    compounds = {"1": Compound1, "2": Compound2, "3": Compound3}

    if args.service == "TEST_ALL":
        test_all()
    else:
        compound = compounds[args.compound]

        if args.service == "SET":
            if args.param:
                if args.values is None or len(args.values) != 1:
                    parser.error("--values must have exactly 1 entry when setting a single parameter")
                print(set_param(args.param, args.values[0], compound))
            else:
                if args.values is None or len(args.values) != len(compound.PARAMETERS):
                    parser.error(f"--values is required with exactly {len(compound.PARAMETERS)} entries for SET command")
                print(set_compound(compound, args.values))
        elif args.service == "GET":
            print(get_param(args.param, compound) if args.param else get_compound(compound))
        elif args.service == "SETGET":
            print(set_compound(compound, args.values))
            time.sleep(0.5)
            print(get_compound(compound, args.param))
