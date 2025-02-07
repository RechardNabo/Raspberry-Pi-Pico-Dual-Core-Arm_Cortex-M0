from machine import Pin

class Control_LED:
    def __init__(self):
        self.Internal_LED = Pin(25, Pin.OUT)
        self.External_LED = Pin(6 , Pin.OUT)
        self.Buttton = Pin(7 , Pin.IN)
    def Ccontrol(self):
        
        print(self.Buttton.value)
        if(self.Buttton.value() == 0x01):
            self.External_LED.value(1)
            print("LED ON")
        if(self.Buttton.value() == 0x00):
            self.External_LED.value(0)
            print("LED OFF")

def main():
    C = Control_LED()
    while True:
        C.Ccontrol()
        print(__name__)

if __name__=="__main__":
    main()

