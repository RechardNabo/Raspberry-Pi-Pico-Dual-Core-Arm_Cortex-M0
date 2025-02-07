from machine import Pin
import utime

class BLINK_LED:
    def __init__(self):
        self.Internal_LED = Pin(25, Pin.OUT)
        self.External_LED = Pin(6 , Pin.OUT)
    def blink(self):
        while True:
            self.Internal_LED.value(0)
            self.External_LED.value(1)
            print("External ON")
            utime.sleep_ms(1000)        
            self.Internal_LED.value(1)
            self.External_LED.value(0)
            print("Internal ON")
            utime.sleep(1)

def main():
    blink = BLINK_LED()
    blink.blink()

if __name__=="__main__":
    main()