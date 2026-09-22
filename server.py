#importing whats needed
import serial
import subprocess
import ctypes

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


    #app part

    if message == "YOUTUBE":
        subprocess.Popen(
            # make it just open the web since theres no app :p
            ["cmd", "/c", "start", "", "https://www.youtube.com"],
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

    elif message == "DISCORD":
        print("Opening Discord...")
        subprocess.Popen(
            ["cmd", "/c", "start", "", "Discord:"],
            shell=False
        )

    elif message == "SPOTIFY":
        print("Opening Spotify...")
        subprocess.Popen(
            ["cmd", "/c", "start", "", "spotify:"],
            shell=False
        )

    elif message == "GITHUB":
        print("Opening Github...")
        subprocess.Popen(
            ["cmd", "/c", "start", "", "https://github.com/IbrahimFTW"],
            shell=False
        )

    elif message == "NVIDIA APP":
        print("Opening Nvidia App...")
        subprocess.Popen(
            [r"C:\Program Files\NVIDIA Corporation\NVIDIA App\CEF\NVIDIA App.exe"],
            shell=False
        )

    elif message == "VS CODE":
            print("Opening Vs Code")
            subprocess.Popen(
                ["cmd", "/c", "start", "", "Code.exe"],
                shell=False
            )
        
    #working on the media part now
    #ctypes will allow us to use windows multimedia commands 
    

    elif message == "MEDIA_PREVIOUS":
        print("Previous track")
        ctypes.windll.user32.keybd_event(0xB1, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xB1, 0, 2, 0)

    elif message == "MEDIA_PLAY":
        print("Play/Pause")
        ctypes.windll.user32.keybd_event(0xB3, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xB3, 0, 2, 0)

    elif message == "MEDIA_NEXT":
        print("Next track")
        ctypes.windll.user32.keybd_event(0xB0, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xB0, 0, 2, 0)


    elif message == "MEDIA_MUTE":
        print("Mute")
        ctypes.windll.user32.keybd_event(0xAD, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xAD, 0, 2, 0)

    elif message == "VOLUME_DOWN":
        print("Volume down")
        ctypes.windll.user32.keybd_event(0xAE, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xAE, 0, 2, 0)

    elif message == "VOLUME_UP":
        print("Volume up")
        ctypes.windll.user32.keybd_event(0xAF, 0, 0, 0)
        ctypes.windll.user32.keybd_event(0xAF, 0, 2, 0)


    #working on the pc area

    elif message == "PC_LOCK":
        print("locking PC...")
        subprocess.Popen(
            #this is the command to lock pc
            ["rundll32.exe", "user32.dll,LockWorkStation"],
            shell=False
        )

    elif message == "PC_SLEEP":
        print("Putting PC to sleep...")
        subprocess.Popen(
            ["powershell", "-Command", "Start-Sleep -Seconds 1; Add-Type -AssemblyName System.Windows.Forms; [System.Windows.Forms.Application]::SetSuspendState('Suspend', $false, $false)"],
            shell=False
        )

    elif message == "PC_RESTART":
        print("Restarting Pc...")
        subprocess.Popen(
            ["shutdown", "/r", "/t", "5"],
            shell=False
        )

    elif message == "PC_SHUTDOWN":
        print("Shutting down PC...")
        subprocess.Popen(
            ["shutdown", "/s", "/t", "5"],
            shell=False
        )

#python server.py

