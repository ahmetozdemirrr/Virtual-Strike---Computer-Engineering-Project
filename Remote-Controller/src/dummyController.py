import timeimport serial

ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)  # Added timeout for robustness

while True:

    try:
        input = ser.readline()        
        data = input.decode("utf-8").strip()  # Added strip to remove possible newline or carriage return
        print(data)    

    except UnicodeDecodeError:
        print("Received non-utf8 data")
        