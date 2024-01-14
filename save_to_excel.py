import serial
import pandas as pd
import os

ser = serial.Serial(
    port='COM4',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_TWO,
    bytesize=serial.EIGHTBITS
)

ser.isOpen()

data_list = []

try:
    while True:
        line = ser.readline().decode("utf-8").strip() # data being sent like "temp,press,alt\r\n"

        line_bytes = line.encode("utf-8") # Encode string to bytes
        line_bytes = line_bytes.replace(b'\x00', b'')  # takeout null bytes (I think it's cause i do \r in firmware)
        line = line_bytes.decode("utf-8") # Decode bytes back to string

        temperature, pressure, altitude = [float(value) for value in line.split(',') if value]

        data_list.append([temperature, pressure, altitude])

except KeyboardInterrupt: # keyboard interrupt (Ctrl+C) to exit and save data
    excel_file_path = os.path.join('C:/Desktop', 'bmp390data.xlsx')
    df = pd.DataFrame(data_list, columns=['Temperature (*C)', 'Pressure (hPa)', 'Altitude (m)'])
    df.to_excel(excel_file_path , index=False)

finally:
    ser.close()