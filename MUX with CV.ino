//program combines Cyclic voltommetry (CV) and mux. Turns on one sensor at a time, runs CV, and then goes to next sensor 

#include <LMP91000.h>// potentiostat library
#include <Wire.h> // library allows you to communicate with I2C / TWI devices

int S0 = 20 //pin 20 is D2 on the Arduino nano. The 4:1 Mux has 2 select lines, S0 is the least significant bit of the select lines
int S1 = 21 //pin 21 is D3 on the nano. S1 is most significat bit of the select lines
int enable = 22 //pin 22 is D4 on the nano
 
LMP91000 pstat = LMP91000(); //initialize LMP91000 object
int rate = 200;
int settling_time = 50;
float volt_array[14] = {0, 0.025, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6};

int  muxChannel [] = {0, 1, 2, 3};//The mux has 4 channels labeled 0-3 
//N1...............0, binary = 00
//N2...............1, binary = 01
//N3...............2, binary = 10
//N4...............3, binary = 11
 
void setup(){
 
  pinMode(S0, OUTPUT);    
  pinMode(S1, OUTPUT);    
  pinMode(enable, OUTPUT);    
  
  Wire.begin(); //Initiate the Wire library and join the I2C bus 
  Serial.begin(9600); // start the serial
  delay(50);
  pstat.standby(); //Places the device in standby mode which allows quick warm-up in between tests
  delay(50);
  pstat.disableFET(); //disable FET for deep sleep mode
  pstat.setGain(5); // (35k Ohms) Sets the transimpedance amplifier gain
  pstat.setRLoad(0);  //sets the load for compensating voltage differences between working and reference electrodes
  pstat.setIntRefSource(); //Sets the voltage reference source to supply voltage (Vdd)
  pstat.setIntZ(1); //Sets the internal zero of the device, particularly the transimpedance amplifier.
  pstat.setThreeLead(); //Sets the first three bits of the Mode Control Register to 011. This enables the LMP91000 for 3-electrode potentiometric measurements.
  delay(2000); //warm-up time for the gas sensor
 
  digitalWrite(enable, 1);  //Sends high voltage to enable pin (3.3V)
  
  for (int i = 0; i < 4; i++) { //loop through each channel 
    
    int channel = muxChannel[i]; 
    
    int b0 = bitRead(channel,0); //bitRead() -> parameter 1 = binary sequence, parameter 2 = which bit to read, starting from the right most bit
    int b1 = bitRead(channel,1); 

    digitalWrite(S0, b0); // send the bits to the digital pins 
    digitalWrite(S1, b1);
   
    Serial.print("Channel ");
    Serial.print(i+1); //prints 1,2,3,4 instead of 0,1,2,3
    Serial.println(" data");
    Seial.println("==================");
  
    for(int i = 0; i < 3;i++){
    
      runCV();
    
    }//end CV loop
  }//end mux channel loop
}//end setup
 
  
void loop () {

}
 
void runCV (){
  pstat.setPosBias();
  for (int j = 13; j >=0; j--){
      pstat.setBias(j);
      delay(settling_time);
      Serial.print(volt_array[j]);
      Serial.print(" , ");
      delay(1);
      Serial.println(analogRead(A0));////after sending binary sequence, the mux determines which channel to read from and sends it to analog input
      delay(rate);
  } //end positive bias voltage sweep
  
    pstat.setNegBias();
    for (int j = 1; j <= 13; j++){
      pstat.setBias(j);
      delay(settling_time);
      Serial.print(volt_array[j]*-1);
      Serial.print(" , ");
      delay(1);
      Serial.println(analogRead(A0));
      delay(rate);
    }
    for (int j = 12; j >=0 ; j--){
      pstat.setBias(j);
      delay(settling_time);
      Serial.print(volt_array[j]*-1);
      Serial.print(" , ");
      delay(1);
      Serial.println(analogRead(A0));
      delay(rate);
    }
    
    pstat.setPosBias();
    for (int j = 1; j <= 13; j++){
      pstat.setBias(j);
      delay(settling_time);
      Serial.print(volt_array[j]);
      Serial.print(" , ");
      delay(1);
      Serial.println(analogRead(A0));
      delay(rate);
    }
    pstat.setBias(0);
    delay (5000); // wait 5 seconds before going to next channel 
 }//end CV
    
   
