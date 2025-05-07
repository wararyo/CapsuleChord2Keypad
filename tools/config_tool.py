#!/usr/bin/env python3
import json
import argparse
import time
import serial
import serial.tools.list_ports
import sys
import os

# Key definitions from keycodes.h
KEY_MAP = {
    "KEY_LEFT_1": 0x01,
    "KEY_LEFT_2": 0x02,
    "KEY_LEFT_3": 0x03,
    "KEY_LEFT_4": 0x04,
    "KEY_LEFT_5": 0x05,
    "KEY_LEFT_6": 0x06,
    "KEY_LEFT_7": 0x07,
    "KEY_LEFT_8": 0x08,
    "KEY_LEFT_9": 0x09,
    
    "KEY_RIGHT_1": 0x11,
    "KEY_RIGHT_2": 0x12,
    "KEY_RIGHT_3": 0x13,
    "KEY_RIGHT_4": 0x14,
    "KEY_RIGHT_5": 0x15,
    "KEY_RIGHT_6": 0x16,
    "KEY_RIGHT_7": 0x17,
    "KEY_RIGHT_8": 0x18,
    "KEY_RIGHT_9": 0x19,
    
    "KEY_L": 0x21,
    "KEY_R": 0x22,
    "KEY_LT": 0x23,
    "KEY_RT": 0x24,
}

# Reverse mapping for display
KEY_NAMES = {code: name for name, code in KEY_MAP.items()}

class CapsuleChordConfig:
    def __init__(self, port=None, baud=115200, timeout=1):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.ser = None
        self.thresholds = {}
        self.serial_number = ""
    
    def connect(self):
        """Connect to the device"""
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=self.timeout)
            time.sleep(2)  # Give time for device to reset after connection
            # Clear any startup messages
            self.ser.reset_input_buffer()
            print(f"Connected to {self.port}")
            return True
        except Exception as e:
            print(f"Error connecting to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the device"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print(f"Disconnected from {self.port}")
    
    def send_command(self, command, wait_for_response=True):
        """Send a command to the device and return the response"""
        if not self.ser or not self.ser.is_open:
            print("Not connected to device")
            return None
        
        self.ser.write((command + '\n').encode())
        
        if wait_for_response:
            # Wait for response and collect all lines
            time.sleep(0.1)
            response = []
            start_time = time.time()
            # Read until no more data or timeout
            while (time.time() - start_time) < self.timeout:
                if self.ser.in_waiting:
                    line = self.ser.readline().decode('utf-8').strip()
                    if line:
                        response.append(line)
                    start_time = time.time()  # Reset timeout if we got data
                else:
                    # Small delay to not hog CPU
                    time.sleep(0.01)
                    # If no response in 100ms, we're probably done
                    if (time.time() - start_time) > 0.1 and response:
                        break
            
            print(f"{command} -> {' '.join(response)}")
            return response
        return None
    
    def read_thresholds(self):
        """Read all thresholds from the device"""
        response = self.send_command("threshold")
        
        if not response:
            return False
        
        # Clear any old data
        self.thresholds = {}
        
        # Process response
        for line in response:
            if line.startswith("Key 0x"):
                parts = line.split(": ")
                if len(parts) == 2:
                    key_hex = parts[0].split(" ")[1]
                    key_code = int(key_hex, 16)
                    threshold = int(parts[1])
                    
                    # Store by key name if available, otherwise by hex code
                    key_name = KEY_NAMES.get(key_code, f"KEY_0x{key_code:02X}")
                    self.thresholds[key_name] = threshold
        
        print(f"Read {len(self.thresholds)} threshold values")
        return True
    
    def read_serial_number(self):
        """Read the serial number from the device"""
        response = self.send_command("serial")
        
        if not response:
            return False
        
        for line in response:
            if line.startswith("Serial Number:"):
                self.serial_number = line.split(": ")[1].strip()
                print(f"Read serial number: {self.serial_number}")
                return True
        
        return False
    
    def read_all_settings(self):
        """Read all settings from the device"""
        thresholds_ok = self.read_thresholds()
        serial_ok = self.read_serial_number()
        return thresholds_ok and serial_ok
    
    def set_threshold(self, key, value):
        """Set threshold for a single key"""
        # Convert key name to key code if needed
        if isinstance(key, str) and key in KEY_MAP:
            key_code = KEY_MAP[key]
        else:
            try:
                key_code = int(key, 0)  # Try to convert to int (handles hex strings)
            except (ValueError, TypeError):
                print(f"Invalid key: {key}")
                return False
        
        # Format command and send
        cmd = f"threshold 0x{key_code:02X} {value}"
        response = self.send_command(cmd)
        
        # Check for success
        if response:
            for line in response:
                if line.startswith(f"Set key 0x{key_code:02X} threshold to {value}"):
                    return True
        
        return False
    
    def set_serial_number(self, serial_num):
        """Set the device serial number"""
        cmd = f"serial {serial_num}"
        response = self.send_command(cmd)
        
        # Check for success
        if response:
            for line in response:
                if line.startswith(f"Serial Number set to: {serial_num}"):
                    self.serial_number = serial_num
                    return True
        
        return False
    
    def save_to_flash(self):
        """Save settings to flash memory"""
        response = self.send_command("save")
        
        if response:
            for line in response:
                if "Settings saved to flash" in line:
                    print("Settings saved to flash successfully")
                    return True
        
        print("Failed to save settings to flash")
        return False
    
    def export_to_json(self, filename):
        """Export current settings to a JSON file"""
        config = {
            "serial_number": self.serial_number,
            "thresholds": self.thresholds
        }
        
        try:
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"Settings exported to {filename}")
            return True
        except Exception as e:
            print(f"Error exporting settings: {e}")
            return False
    
    def import_from_json(self, filename):
        """Import settings from a JSON file"""
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            if "serial_number" in config:
                self.serial_number = config["serial_number"]
            
            if "thresholds" in config:
                self.thresholds = config["thresholds"]
            
            print(f"Imported settings from {filename}")
            return True
        except Exception as e:
            print(f"Error importing settings: {e}")
            return False
    
    def apply_settings_to_device(self):
        """Apply current settings to the device"""
        success = True
        
        # Apply thresholds
        for key_name, threshold in self.thresholds.items():
            key_code = KEY_MAP.get(key_name, None)
            if key_code is None:
                try:
                    # Try parsing as hex string without 0x prefix
                    if key_name.startswith("KEY_0x"):
                        key_code = int(key_name[5:], 16)
                except:
                    print(f"Skipping unknown key: {key_name}")
                    success = False
                    continue
            
            if not self.set_threshold(key_name, threshold):
                print(f"Failed to set threshold for {key_name}")
                success = False
        
        # Apply serial number
        if self.serial_number and not self.set_serial_number(self.serial_number):
            print("Failed to set serial number")
            success = False
        
        return success

def list_serial_ports():
    """List available serial ports"""
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("No serial ports found")
        return
    
    print("Available serial ports:")
    for i, port in enumerate(ports):
        print(f"{i+1}: {port.device} - {port.description}")

def main():
    parser = argparse.ArgumentParser(description="CapsuleChord2 Configuration Tool")
    
    # Serial port settings
    parser.add_argument('--port', '-p', help='Serial port to use')
    parser.add_argument('--baud', '-b', type=int, default=115200, help='Baud rate (default: 115200)')
    parser.add_argument('--list-ports', '-l', action='store_true', help='List available serial ports')
    
    # Actions
    action_group = parser.add_argument_group('Actions')
    action_group.add_argument('--read', '-r', action='store_true', help='Read settings from device')
    action_group.add_argument('--write', '-w', action='store_true', help='Write settings to device')
    action_group.add_argument('--save', '-s', action='store_true', help='Save settings to flash after writing')
    
    # Files
    parser.add_argument('--import-file', '-i', help='Import settings from JSON file')
    parser.add_argument('--export-file', '-e', help='Export settings to JSON file')
    
    args = parser.parse_args()
    
    # List ports if requested
    if args.list_ports:
        list_serial_ports()
        return
    
    # Check if port is specified
    if not args.port:
        print("Error: Serial port must be specified")
        list_serial_ports()
        return
    
    # Create config object
    config = CapsuleChordConfig(port=args.port, baud=args.baud)
    
    # Connect to device
    if not config.connect():
        return
    
    try:
        # Import settings if specified
        if args.import_file:
            if not config.import_from_json(args.import_file):
                return
        
        # Read settings from device
        if args.read:
            if not config.read_all_settings():
                print("Failed to read settings from device")
                return
        
        # Write settings to device
        if args.write:
            if not config.apply_settings_to_device():
                print("Some settings failed to apply")
        
        # Save settings to flash if requested
        if args.save:
            config.save_to_flash()
        
        # Export settings if specified
        if args.export_file:
            if not args.read and not args.import_file:
                # If we haven't read from device or imported from file, read from device now
                if not config.read_all_settings():
                    print("Failed to read settings from device for export")
                    return
            
            config.export_to_json(args.export_file)
    
    finally:
        # Always disconnect properly
        config.disconnect()

if __name__ == "__main__":
    main()