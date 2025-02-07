from machine import Pin ,SPI
import pcd8544
import utime

class Nokia_Screen:
    def __init__(self):
        self.SPI = SPI(0x01)
        self.SPI.init(baudrate = 2000000, polarity = 0x00, phase = 0x00)
        self.CS = Pin(2)
        self.DC = Pin(15)
        self.RST = Pin(0)
        
        self.BL = Pin(12,Pin.OUT)
        self.BL.value(0x01)
        
        
        self.LCD = pcd8544.PCD8544(self.SPI , self.CS, self.DC , self.RST)
    
    def Display(self):
        #self.LCD.text('HELLO', 0, 12 , 1 )
        self.LCD.clear()
        #self.LCD.show()
        #self.LCD.fill(0)

def main():
    N = Nokia_Screen()
    print("Program Running. . .")
    while True:
        
        Internal = Pin(25, Pin.OUT)
        Internal.value(1)
        N.Display()
if __name__=="__main__":
    main()