#define APA102_USE_FAST_GPIO
#include <Arduino.h>
#include <FastGPIO.h>
#include <APA102.h>

// Set constants
const uint16_t ledCount PROGMEM = 144;
// Avoiding pins 0 + 1 (serial communication)
const uint8_t dataPinU PROGMEM = 2;
const uint8_t dataPinS PROGMEM = 3;
const uint8_t dataPinW PROGMEM = 5;
const uint8_t clockPin PROGMEM = 6;
const uint8_t dataPinD PROGMEM = 7;
const uint8_t dataPinE PROGMEM = 8;
const uint8_t dataPinN PROGMEM = 9;
APA102<dataPinU, clockPin> ledStripU;
APA102<dataPinD, clockPin> ledStripD;
APA102<dataPinN, clockPin> ledStripN;
APA102<dataPinS, clockPin> ledStripS;
APA102<dataPinE, clockPin> ledStripE;
APA102<dataPinW, clockPin> ledStripW;
const uint8_t uCorners[] PROGMEM = {0,142, 35,36, 71,72, 107};
const uint8_t dCorners[] PROGMEM = {140, 33, 68,69, 105};
const uint8_t nCorners[] PROGMEM = {0,143, 35,36, 71,72, 107,108};
const uint8_t sCorners[] PROGMEM = {0,143, 35,36, 71,72, 107};
const uint8_t eCorners[] PROGMEM = {0, 36, 71,72, 107};
const uint8_t wCorners[] PROGMEM = {0, 36, 71,72, 107};
// Set the brightness to use (software maximum is 31).
// CHILL. 14A MAXIMUM ON 16GA WIRE, MAKE SOFTWARE LOCK AT WHATEVER 10A COMES OUT TO.
// DO TESTING.
uint8_t brightness = 1;
uint8_t saturation = 255;
uint8_t vibrance = 15;
bool up = true, down = true, north = true, south = true, east = true, west = true;
rgb_color mainColor = rgb_color(100,100,100);
// Create a buffer for holding the colors (3 bytes per color).
rgb_color colorBuffer[ledCount];
uint32_t timer;


rgb_color hsvToRgb(uint32_t h, uint8_t s, uint8_t v){
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}

bool contains(const uint8_t* arr, int size, uint8_t value) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == value) {
      return true;
    }
  }
  return false;
}

void updateUp(rgb_color inputColorBuffer[]){
  ledStripU.write(inputColorBuffer, ledCount, brightness);
}

void updateDown(rgb_color inputColorBuffer[]){
  ledStripD.write(inputColorBuffer, ledCount, brightness);
}

void updateNorth(rgb_color inputColorBuffer[]){
  ledStripN.write(inputColorBuffer, ledCount, brightness);
}

void updateSouth(rgb_color inputColorBuffer[]){
  ledStripS.write(inputColorBuffer, ledCount, brightness);
}
 
void updateEast(rgb_color inputColorBuffer[]){
  ledStripE.write(inputColorBuffer, ledCount, brightness);
}

void updateWest(rgb_color inputColorBuffer[]){
  ledStripW.write(inputColorBuffer, ledCount, brightness);
}

void setup() {
  Serial.begin(9600); 
}  // setup()

void loop() {
  // uint32_t time0, time6; // time1, time2, time3, time4, time5,

  if (Serial.available() > 0) {
    uint8_t firstByte = Serial.read();

    if(firstByte == 0xFF){ // config!!
      while (Serial.available() < 2){}
      uint8_t incomingByte = Serial.read();
      uint8_t nextByte = Serial.read();

      switch (incomingByte){

        case 0x01: // update saturation
          if (nextByte >= 0 && nextByte <= 255) {
            saturation = static_cast<uint8_t>(nextByte);
            Serial.println(saturation);
          }
          break;

        case 0x02: // update vibrance
          if (nextByte >= 0 && nextByte <= 15) {
            vibrance = static_cast<uint8_t>(nextByte);
            Serial.println(vibrance);
          }
          break;

        case 0x03: // update brightness
          if (nextByte >= 0 && nextByte <= 15){
            brightness = static_cast<uint8_t>(nextByte);
            Serial.println(brightness);
          }
          break;

        case 0x10: 
          up=down=north=south=east=west = true;
          // Serial.println(F("all"));
          break;

        case 0x11: // update up
          up = true;
          down = north = south = east = west = false;
          // Serial.println(F("u"));
          break;
    
        case 0x12: // update down
          down = true;
          up = north = south = east = west = false;
          // Serial.println(F("d"));
          break;
        
        case 0x13: // update north
          north = true;
          down = up = south = east = west = false;
          // Serial.println(F("n"));
          break;
    
        case 0x14: // update south
          south = true;
          down = north = up = east = west = false;
          // Serial.println(F("s"));
          break;
    
        case 0x15: // update east
          east = true;
          down = north = south = up = west = false;
          // Serial.println(F("e"));
          break;
        
        case 0x16: // update west
          west = true;
          down = north = south = east = up = false;
          // Serial.println(F("w"));
          break;
        
        default:
          break;
        }
    } else { // update colors!!

      uint32_t hue = static_cast<uint32_t>(firstByte) * 360 / 255;
      for(uint8_t i = 0; i < ledCount; i++)
      {
        colorBuffer[i] = hsvToRgb(hue, saturation, vibrance);
      }

      //timing measurements
      // time0 = micros();
      
      if(up){
        updateUp(colorBuffer);
      }
      // time1 = micros();
      if(down){
        updateDown(colorBuffer);
      }
      // time2 = micros();
      if(north){
        updateNorth(colorBuffer);
      }
      // time3 = micros();
      if(south){
        updateSouth(colorBuffer);
      }
      // time4 = micros();
      if(east){
        updateEast(colorBuffer);
      }
      // time5 = micros();
      if(west){
        updateWest(colorBuffer);
      }

      //timing measurements
      // time6 = micros();
      // uint32_t iterTime = (time6 - time0) / 6;
      // timer = iterTime;
      // Serial.println(timer);
    }    
  }
}  // loop()
