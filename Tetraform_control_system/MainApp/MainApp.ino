#include "WS2801.h"
#include "OneWire.h" 

/***************************************
 THERMOMETER WIRE PINOUTS:
****************************************
             RED    - VCC (5V DC)
             BLACK  - GND 
             YELLOW - CON   (Pin 12)          
****************************************/
int DS18S20_Pin = 12;

/****************************************
 LED WIRE PINOUTS:
*****************************************
             RED    - VCC (5V DC)
             YELLOW - GND 
             GREEN  - DATA   (Pin 3) 
             BLUE   - CLOCK  (Pin 2)
*****************************************/
int dataPin    = 3;     // Green
int clockPin   = 2;    // Blue

/***************************************
 RELAY WIRE PINOUTS:
****************************************
             RED    - VCC (5V DC)
             BLACK  - GND 
             YELLOW - CON   (Pin 11)          
****************************************/
int relayPin = 11;


// Configuration
int startup_cycles = 1;
int cycle_counter  = 0;
int numberLEDs = 5;

int relayStatus   = 0;
int warningStatus = 0;
int timeoutStatus = 0;

int countdownTimer = 50000; // ms

WS2801 SpheroDisplay = WS2801(numberLEDs, dataPin, clockPin);
OneWire ds(DS18S20_Pin);

void setup() {
  SpheroDisplay.begin();
  SpheroDisplay.show();
  
  pinMode(relayPin, OUTPUT);
  
  Serial.begin(9600);
  delay(100);
  Serial.println(" Tetraform v1.0");
}






/*************************************************************
##############################################################
##########            Tetraform MAIN loop           ##########
##############################################################                      
*************************************************************/

void loop(void) {
  
  // STARTUP SEQUENCE
  
  if(cycle_counter <= startup_cycles){
    rainbowCycle(10);
  }
  else{
    
    // RELAY STATUS
    
    if(relayStatus == 0 && warningStatus == 0){
      relayStatus = 1;
      digitalWrite(relayPin, HIGH); 
    }
    else if(relayStatus == 1 && warningStatus == 1){
      relayStatus = 0;
      digitalWrite(relayPin, LOW); 
      
      delay(200);
      
      // Flash until reset
      while(warningStatus == 1){
        Serial.println("WARNING OVERHEAT!!!");
        flash_red(10,20);
      }
      
    }
      
    
    
    // TEMPERATURE DISPLAY
  
    float temperature = getTemp();

    if(covertToFerenheit(temperature) < 70.0){
      for(int k=0;k<numberLEDs;k++){
        SpheroDisplay.setPixelColor(k, Color(0, 20, 158));
      }
    }
    else if(covertToFerenheit(temperature) < 81.0){
      for(int k=0;k<numberLEDs;k++){
        SpheroDisplay.setPixelColor(k, Color(0, 160, 20));
      }
    }
    else if(covertToFerenheit(temperature) < 100.5){
      for(int k=0;k<numberLEDs;k++){
        SpheroDisplay.setPixelColor(k, Color(200, 0, 0));
      }
    }
    else{
      // Overheat Warning - Shutdown.
      warningStatus = 1;
    }
    
    SpheroDisplay.show();
    
    Serial.println(covertToFerenheit(temperature));
    delay(100); 
  }
  
  
  
}



/*************************************************************
                      Display Routines 
*************************************************************/

//  EXAMPLES OF DISPLAY FUNCTIONS
/**
  colorWipeUniform(Color(50, 0, 0), 1);
  delay(1000);
  colorWipe(Color(0, 50, 50), 1);
  delay(1000);
  
   rainbow(200);
   rainbowCycle(20);

  SpheroDisplay.setPixelColor(0, Color(255, 0, 0));
  SpheroDisplay.setPixelColor(1, Color(200, 0, 0));
  SpheroDisplay.setPixelColor(2, Color(40, 0, 0));
  SpheroDisplay.setPixelColor(3, Color(30, 0, 0));
  SpheroDisplay.setPixelColor(4, Color(1, 0, 0));
  SpheroDisplay.show();
  delay(10);
 
  flash_red(10,20);
  rainbowCycle(20);
  
**/


void flash_red(uint8_t wait, uint8_t brightness) {
  int i, j;
  int bx2 = brightness * 2;
  for (j=0; j < bx2; j++) { 
    for (i=0; i < SpheroDisplay.numPixels(); i++) {
        if(j<brightness){
          SpheroDisplay.setPixelColor(i, Color(j,0,0));
        }
        else{
          SpheroDisplay.setPixelColor(i, Color(bx2-j,0,0));
        }
     }  
    SpheroDisplay.show();   
    delay(wait);
  }
}

void flash_green(uint8_t wait, uint8_t brightness) {
  int i, j;
  int bx2 = brightness * 2;
  for (j=5; j < bx2; j++) { 
    for (i=0; i < SpheroDisplay.numPixels(); i++) {
        if(j<brightness){
          SpheroDisplay.setPixelColor(i, Color(0,j,0));
        }
        else{
          SpheroDisplay.setPixelColor(i, Color(0,bx2-j,0));
        }
     }  
    SpheroDisplay.show();   
    delay(wait);
  }
}

void flash_white(uint8_t wait, uint8_t brightness) {
  int i, j;
  int bx2 = brightness * 2;
  for (j=5; j < bx2; j++) { 
    for (i=0; i < SpheroDisplay.numPixels(); i++) {
        if(j<brightness){
          SpheroDisplay.setPixelColor(i, Color(j,j,j));
        }
        else{
          SpheroDisplay.setPixelColor(i, Color(bx2-j,bx2-j,bx2-j));
        }
     }  
    SpheroDisplay.show();   
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  int i, j;
  cycle_counter++;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < SpheroDisplay.numPixels(); i++) {
      SpheroDisplay.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    SpheroDisplay.show();   
    delay(wait);
  }
}

// makes the rainbow wheel equally distributed along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  cycle_counter++;
  
  for (j=0; j < 256; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < SpheroDisplay.numPixels(); i++) {
      SpheroDisplay.setPixelColor(i, Wheel( ((i * 256 / SpheroDisplay.numPixels()) + j) % 256) );
    }  
    SpheroDisplay.show();   
    delay(wait);
  }
}


// wipe the entire chain one by one.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < SpheroDisplay.numPixels(); i++) {
      SpheroDisplay.setPixelColor(i, c);
      SpheroDisplay.show();
      delay(wait);
  }
}

// wipe the entire chain, COLOR then wait time.
void colorWipeUniform(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < SpheroDisplay.numPixels(); i++) {
      SpheroDisplay.setPixelColor(i, c);
  }
  SpheroDisplay.show();
  delay(wait);
}

/*************************************************************
                       Color functions 
*************************************************************/

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/*************************************************************
                       Temperature functions 
*************************************************************/

float covertToFerenheit(float tempInCelsius){
    return ( ((tempInCelsius * 9) / 5) + 32);
}

float getTemp(){
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); 

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); 

  
  for (int i = 0; i < 9; i++) { 
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); 
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}





