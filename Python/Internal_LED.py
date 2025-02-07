from machine import Pin

class BLINK_LED:
    def __init__(self):
        self.Internal_LED = Pin(25, OUTPUT)
    def blink(self):
        self.Internal_LED.high()

def main():
    LED = BLINK_LED()
    LED.blink()

if __name__=="main":
    main()