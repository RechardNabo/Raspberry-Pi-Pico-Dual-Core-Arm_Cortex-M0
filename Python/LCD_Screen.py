from machine import Pin
from gpio_lcd import GpioLcd
import utime

class Counter:
    def __init__(self):
        self.number = 0x00
    def Count(self):
        self.number -=-0x01
        utime.sleep(1)
        return self.number
def main():
    RS = Pin(16 , Pin.OUT)
    EN = Pin(17 , Pin.OUT)
    D4 = Pin(18, Pin.OUT)
    D5 = Pin(19, Pin.OUT)
    D6 = Pin(20, Pin.OUT)
    D7 = Pin(21, Pin.OUT)
    LCD = GpioLcd(RS,EN,D4,D5,D6,D7,
                num_lines=2, num_columns=16)
    C = Counter()
    while True:
        LCD.move_to(0,0)
        LCD.putstr('Counter : '+str(C.Count()))
    

if __name__=="__main__":
    main()