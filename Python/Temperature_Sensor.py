import machine
import utime
import pyb3
class Teperature_Seneor:
    def __init__(self):
      
        pass
    
    def sensor(self):
        self.ADC_Value = machine.ADC(4)
        print("\nADC Value \t: "+str(self.ADC_Value.read_u16()))
        self.conversion = 3.3 / 65535.0
        self.Voltage = self.ADC_Value.read_u16()*self.conversion
        print("Voltage \t\t: "+str(self.Voltage))
        self.temp = 27 - (self.Voltage - 0.706)/0.001721
        print("Temperature \t: "+str(self.temp))
        utime.sleep(1)

def main():
    T = Teperature_Seneor()
    while True:
        T.sensor()
if __name__=="__main__":
    main()