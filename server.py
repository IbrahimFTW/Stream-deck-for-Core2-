#importing whats needed
import serial
import subprocess

PORT = "COM3"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
#temporary can remove after
print(f"Connected to M5Stack on {PORT}")
print("Waiting for buttons...")

#The stream deck sends messages 
#Note to self when editing this make sure the Aurino and the server 
#Get the same text otherwise it wont work and make sure to check
#That the cmd naming scheme is right otherwise it wont find the app
while True:
    message = ser.readline().decode("utf-8", errors="ignore").strip()

    if not message:
        continue

    print(f"Received: {message}")

    if message == "YOUTUBE":
        subprocess.Popen(
            # make it just open the web since theres no app :p
            ["cmd", "/c", "start", "", "https://www.youtube.com:"],
            shell=False
        )

    elif message == "STEAM":
        print("Opening Steam...")
        subprocess.Popen(
            ["cmd", "/c", "start", "", "steam:"],
            shell=False
        )

    elif message == "EDGE":
        print("Opening Edge...")
        subprocess.Popen(
            ["cmd", "/c", "start", "", "msedge"],
            shell=False
        )

    elif message == "MEDIA":
        print("Media button pressed!")
