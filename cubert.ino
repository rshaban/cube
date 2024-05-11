//#include <Arduino.h>
//#include <Ethernet.h>
#include <FastGPIO.h>
#define APA102_USE_FAST_GPIO
#include <APA102.h>

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v){
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


void setup() {
  // start the Ethernet connection:
  // Serial.println("Initialize Ethernet with DHCP:");
  // if (Ethernet.begin(mac) == 0) {
  //   Serial.println("Failed to configure Ethernet using DHCP");
  //   if (Ethernet.hardwareStatus() == EthernetNoHardware) {
  //     Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
  //   } else if (Ethernet.linkStatus() == LinkOFF) {
  //     Serial.println("Ethernet cable is not connected.");
  //   }
  //   // no point in carrying on, so do nothing forevermore:
  //   while (true) {
  //     delay(1);
  //   }
  // }
  // // print your local IP address:
  // Serial.print("My IP address: ");
  // Serial.println(Ethernet.localIP());
  // // end ethernet
//ethernet shield mac
// byte mac[] = {
//   0xCA, 0xFE, 0xBA, 0xBE, 0xEE, 0xEE
// };
}  // setup()

  // LED init
  // Define which pins to use.
  const uint8_t dataPinU = 1;
  const uint8_t dataPinD = 5;
  const uint8_t dataPinN = 7;
  const uint8_t dataPinS = 2;
  const uint8_t dataPinE = 6;
  const uint8_t dataPinW = 3;

  const uint8_t clockPin = 4;
  
  // Create an object for writing to the LED strip.
  APA102<dataPinU, clockPin> ledStripU;
  APA102<dataPinD, clockPin> ledStripD;
  APA102<dataPinN, clockPin> ledStripN;
  APA102<dataPinS, clockPin> ledStripS;
  APA102<dataPinE, clockPin> ledStripE;
  APA102<dataPinW, clockPin> ledStripW;
  // Set the number of LEDs to control.
  const uint16_t ledCount = 144;

  // Create a buffer for holding the colors (3 bytes per color).
  // rgb_color colorsU[ledCount];
  // rgb_color colorsD[ledCount];
  // rgb_color colorsN[ledCount];
  // rgb_color colorsS[ledCount];
  // rgb_color colorsE[ledCount];
  // rgb_color colorsW[ledCount];
  rgb_color colorBuffer[ledCount];

/* 
a few ideas here
3 buffers, making the most of the available memory
update a pattern of north/west/up and south/east/down
is that what the wire corners work out to be?
i want to follow that

only update the necessary leds. dont rewrite the whole strip?
*
 */

  // Set the brightness to use (the maximum is 31).
  // CHILL. 14A MAXIMUM ON 16GA WIRE, MAKE SOFTWARE LOCK AT WHATEVER 10A COMES OUT TO.
  // DO TESTING.
  const uint8_t brightness = 5;

  const int upCorners = [0,142, 35,36, 71,72, 107];
  const int dnCorners = [140, 33, 68,69, 105];
  const int ntCorners = [0,143, 35,36, 71,72, 107,108];
  const int stCorners = [0,143, 35,36, 71,72, 107];
  const int etCorners = [0, 36, 71,72, 107];
  const int wtCorners = [0, 36, 71,72, 107];


void loop() {
  /*
  switch (Ethernet.maintain()) {
      case 1:
        //renewed fail
        Serial.println("Error: renewed fail");
        break;

      case 2:
        //renewed success
        Serial.println("Renewed success");
        //print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;

      case 3:
        //rebind fail
        Serial.println("Error: rebind fail");
        break;

      case 4:
        //rebind success
        Serial.println("Rebind success");
        //print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;

      default:
        //nothing happened
        break;
  }
  */
/* rail calibrations
  // top strip calibrated

  colorsU[1] = rgb_color(0,0,255),
  colorsU[0] = rgb_color(255,255,255),
  colorsU[142] = rgb_color(255,255,255);
  colorsU[141] = rgb_color(255,0,0);

  colorsU[34] = rgb_color(255,0,0),
  colorsU[35] = rgb_color(255,255,255),
  colorsU[36] = rgb_color(255,255,255),
  colorsU[37] = rgb_color(0,0,255);

  colorsU[70] = rgb_color(255,0,0);
  colorsU[71] = rgb_color(255,255,255);
  colorsU[72] = rgb_color(255,255,255);
  colorsU[73] = rgb_color(0,0,255);

  colorsU[106] = rgb_color(255,0,0),
  colorsU[107] = rgb_color(255,255,255);
  colorsU[108] = rgb_color(0,0,255);
  
 // bottom strip calibrated
  
  colorsD[0] = rgb_color(0,0,255), // fucking gap.
  colorsD[140] = rgb_color(255,255,255),
  colorsD[139] = rgb_color(255,0,0);

  colorsD[32] = rgb_color(255,0,0), // covered by wires
  colorsD[33] = rgb_color(255,255,255),
  colorsD[34] = rgb_color(0,0,255);

  colorsD[67] = rgb_color(255,0,0); 
  colorsD[68] = rgb_color(255,255,255);
  colorsD[69] = rgb_color(255,255,255);
  colorsD[70] = rgb_color(0,0,255);

  colorsD[103] = rgb_color(255,0,0),
  colorsD[104] = rgb_color(255,255,255);
  colorsD[105] = rgb_color(255,255,255);
  colorsD[106] = rgb_color(0,0,255);
  
 // north calibrated
  
  colorsN[1] = rgb_color(0,0,255),
  colorsN[0] = rgb_color(255,255,255),
  colorsN[143] = rgb_color(255,255,255),
  colorsN[142] = rgb_color(255,0,0);

  colorsN[35] = rgb_color(255,0,0),
  colorsN[36] = rgb_color(255,255,255),
  colorsN[37] = rgb_color(0,0,255);

  colorsN[70] = rgb_color(255,0,0);
  colorsN[71] = rgb_color(255,255,255);
  colorsN[72] = rgb_color(255,255,255);
  colorsN[73] = rgb_color(0,0,255);

  colorsN[107] = rgb_color(255,0,0),
  colorsN[108] = rgb_color(255,255,255);
  colorsN[109] = rgb_color(0,0,255);
  
 // south calibrated
  
  colorsS[1] = rgb_color(255,0,0),
  colorsS[0] = rgb_color(255,255,255),
  colorsS[143] = rgb_color(255,255,255),
  colorsS[142] = rgb_color(0,0,255);

  colorsS[34] = rgb_color(255,0,0),
  colorsS[35] = rgb_color(255,255,255),
  colorsS[36] = rgb_color(255,255,255),
  colorsS[37] = rgb_color(0,0,255);

  colorsS[70] = rgb_color(255,0,0);
  colorsS[71] = rgb_color(255,255,255);
  colorsS[72] = rgb_color(255,255,255);
  colorsS[73] = rgb_color(0,0,255);

  colorsS[106] = rgb_color(255,0,0),
  colorsS[107] = rgb_color(255,255,255);
  colorsS[108] = rgb_color(255,255,255);
  colorsS[109] = rgb_color(0,0,255);
  
//  // east calibrated
  
  colorsE[0] = rgb_color(255,255,255),
  colorsE[1] = rgb_color(255,0,0),
  colorsE[142] = rgb_color(0,0,255);

  colorsE[35] = rgb_color(255,0,0),
  colorsE[36] = rgb_color(255,255,255),
  colorsE[37] = rgb_color(255,255,255);
  colorsE[38] = rgb_color(0,0,255);

  colorsE[70] = rgb_color(255,0,0);
  colorsE[71] = rgb_color(255,255,255);
  colorsE[72] = rgb_color(255,255,255);
  colorsE[73] = rgb_color(0,0,255);

  colorsE[106] = rgb_color(255,0,0),
  colorsE[107] = rgb_color(255,255,255);
  colorsE[108] = rgb_color(255,255,255);
  colorsE[109] = rgb_color(0,0,255);


 // west calibrated
  
  colorsW[1] = rgb_color(0,0,255),
  colorsW[0] = rgb_color(255,255,255),
  colorsW[142] = rgb_color(255,255,255),
  colorsW[141] = rgb_color(255,0,255);

  colorsW[35] = rgb_color(255,0,0),
  colorsW[36] = rgb_color(255,255,255),
  colorsW[37] = rgb_color(255,255,255);
  colorsW[37] = rgb_color(0,0,255);

  colorsW[70] = rgb_color(255,0,0);
  colorsW[71] = rgb_color(255,255,255);
  colorsW[72] = rgb_color(255,255,255);
  colorsW[72] = rgb_color(0,0,255);

  colorsW[106] = rgb_color(255,0,0),
  colorsW[107] = rgb_color(255,255,255);
  colorsW[108] = rgb_color(0,0,255);

  
//  for(uint16_t i = 0; i < ledCount; i++)
//  {
//    colorBuffer[i] = rgb_color(0,0,50); // blue
//  }
*/

  // up
  // rainbow loop
  uint8_t time = millis() >>6;
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  colorBuffer[0] = rgb_color(25,25,25),
  colorBuffer[142] = rgb_color(25,25,25);

  colorBuffer[35] = rgb_color(25,25,25),
  colorBuffer[36] = rgb_color(25,25,25);

  colorBuffer[71] = rgb_color(25,25,25),
  colorBuffer[72] = rgb_color(25,25,25);

  colorBuffer[107] = rgb_color(25,25,25);

  ledStripU.write(colorBuffer, ledCount, brightness);

  // down
  // rainbow loop
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  
  colorBuffer[140] = rgb_color(25,25,25);

  colorBuffer[33] = rgb_color(25,25,25);

  colorBuffer[68] = rgb_color(25,25,25),
  colorBuffer[69] = rgb_color(25,25,25);

  colorBuffer[105] = rgb_color(25,25,25);
  ledStripD.write(colorBuffer, ledCount, brightness);

  // north
  // rainbow loop
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  colorBuffer[0] = rgb_color(25,25,25),
  colorBuffer[143] = rgb_color(25,25,25);

  colorBuffer[35] = rgb_color(25,25,25),
  colorBuffer[36] = rgb_color(25,25,25);

  colorBuffer[71] = rgb_color(25,25,25),
  colorBuffer[72] = rgb_color(25,25,25);

  colorBuffer[107] = rgb_color(25,25,25),
  colorBuffer[108] = rgb_color(25,25,25);
  ledStripN.write(colorBuffer, ledCount, brightness);

  // south
  // rainbow loop
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  colorBuffer[0] = rgb_color(25,25,25),
  colorBuffer[143] = rgb_color(25,25,25);

  colorBuffer[35] = rgb_color(25,25,25),
  colorBuffer[36] = rgb_color(25,25,25);

  colorBuffer[71] = rgb_color(25,25,25),
  colorBuffer[72] = rgb_color(25,25,25);

  colorBuffer[107] = rgb_color(25,25,25);
  ledStripS.write(colorBuffer, ledCount, brightness);

  // east
  // rainbow loop
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  colorBuffer[0] = rgb_color(25,25,25);

  colorBuffer[36] = rgb_color(25,25,25);

  colorBuffer[71] = rgb_color(25,25,25);
  colorBuffer[72] = rgb_color(25,25,25);

  colorBuffer[107] = rgb_color(25,25,25);
  ledStripE.write(colorBuffer, ledCount, brightness);
  
  // west
  // rainbow loop
  for(uint16_t i = 0; i < ledCount; i++)
  {
    // uint8_t p = time - i * 8;
    uint8_t p = time;
    colorBuffer[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 15);
  }
  colorBuffer[0] = rgb_color(25,25,25);

  colorBuffer[36] = rgb_color(25,25,25);

  colorBuffer[71] = rgb_color(25,25,25);
  colorBuffer[72] = rgb_color(25,25,25);

  colorBuffer[107] = rgb_color(25,25,25);
  ledStripW.write(colorBuffer, ledCount, brightness);

}  // loop()
