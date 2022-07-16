#include <Adafruit_NeoPixel.h>
#ifdef _AVR_
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN      3
#define NUMPIXELS 100


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500
#include <SoftwareSerial.h>
SoftwareSerial BT(11, 10); //Rx/Tx

#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

const int r1[] = {24, 54, 45, 75};
const int r2[] = {24, 54, 45, 75, 15, 16, 23, 43, 44, 46, 53, 55, 56, 76, 83, 84};
const int r3[] = {24, 54, 45, 75, 15, 16, 17, 22, 23, 43, 44, 46, 53, 55, 56, 76, 83, 84, 12, 13, 14, 35, 36, 37, 42, 47, 52, 57, 64, 62, 63, 77, 82, 87, 85, 86};
const int r4[] = {5, 6, 7, 8, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 23, 24, 31, 32, 33, 34, 35, 36, 37, 38, 41, 42, 43, 44, 45, 46, 47, 48, 51, 52, 53, 54, 55, 56, 57, 58, 61, 62, 63, 64, 65, 66, 67, 68, 75, 76, 77, 78, 81, 82, 83, 84, 85, 86, 87, 88, 91, 92, 93, 94};
const int r5[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};


// checksum variables
byte  generatedChecksum = 0;
byte  checksum = 0;
int   payloadLength = 0;
byte  payloadData[64] = {0};
byte  poorQuality = 0;
byte  attention = 0;
byte  meditation = 0;

// system variables
long    lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup() {
  pinMode(LED, OUTPUT);
  BT.begin(BAUDRATE);           // Software serial port  (ATMEGA328P)
  Serial.begin(BAUDRATE);           // USB
  pixels.begin();
}
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte() {
  int ByteRead;
  while (!BT.available());
  ByteRead = BT.read();

#if DEBUGOUTPUT
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop() {
  // Look for sync bytes
  if (ReadOneByte() == 170) {
    if (ReadOneByte() == 170) {
      payloadLength = ReadOneByte();
      if (payloadLength > 169)                     //Payload length can not be greater than 169
        return;

      generatedChecksum = 0;
      for (int i = 0; i < payloadLength; i++) {
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }

      checksum = ReadOneByte();                      //Read checksum byte from stream
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

      if (checksum == generatedChecksum) {

        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for (int i = 0; i < payloadLength; i++) {   // Parse the payload
          switch (payloadData[i]) {
            case 2:
              i++;
              poorQuality = payloadData[i];
              bigPacket = true;
              break;
            case 4:
              i++;
              attention = payloadData[i];

              break;
            case 5:
              i++;
              meditation = payloadData[i];
              break;
            case 0x80:
              i = i + 3;
              break;
            case 0x83:
              i = i + 25;
              break;
            default:
              break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        if (bigPacket) {
          if (poorQuality == 0)  digitalWrite(LED, HIGH);
          else  digitalWrite(LED, LOW);

          for (int i = 1; i < 11; i++)
          {
            if (attention <= 20)
            { Serial.print("attention level 1:");
              Serial.print(attention);
              Serial.print("\n");
              pixels.clear();
              pixels.setBrightness(30);
              first(5);
              pixels.show();
              break;
            }
            if (attention <= 40)
            { Serial.print("attention level 2:");
              Serial.print(attention);
              Serial.print("\n");
              pixels.clear();
              pixels.setBrightness(30);
              second(5);
              pixels.show();
              break;
            }
            if (attention <= 60)
            { Serial.print("attention level 3:");
              Serial.print(attention);
              Serial.print("\n");
              pixels.clear();
              pixels.setBrightness(30);
              third(5);
              pixels.show();
              break;
            }
            if (attention <= 80)
            { Serial.print("attention level 4:");
              Serial.print(attention);
              Serial.print("\n");
              pixels.clear();
              pixels.setBrightness(30);
              fourth(5);
              pixels.show();
              break;
            }
            if (attention <= 100)
            { Serial.print("attention level 5:");
              Serial.print(attention);
              Serial.print("\n");
              pixels.clear();
              pixels.setBrightness(30);
              fifth(5);
              pixels.show();
              break;
            }
            
          }



        }
#endif
        bigPacket = false;
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}
void first(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(24, Wheel((i + j) & 255));
      pixels.setPixelColor(54, Wheel((i + j) & 255));
      pixels.setPixelColor(45, Wheel((i + j) & 255));
      pixels.setPixelColor(75, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(5);
  }
}
void second(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<16; i++) {
      pixels.setPixelColor(r2[i], Wheel((i+j) & 255));
     
    }
    pixels.show();
    delay(5);
  }
}
void third(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<36; i++) {
      pixels.setPixelColor(r3[i], Wheel((i+j) & 255));
     
    }
    pixels.show();
    delay(5);
  }
}
void fourth(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<64; i++) {
      pixels.setPixelColor(r4[i], Wheel((i+j) & 255));
     
    }
    pixels.show();
    delay(5);
  }
}
void fifth(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<100; i++) {
      pixels.setPixelColor(r5[i], Wheel((i+j) & 255));
     
    }
    pixels.show();
    delay(5);
  }
}
