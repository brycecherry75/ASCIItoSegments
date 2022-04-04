/*

  ASCIItoSegments demo by Bryce Cherry

  BIT_ORDER_(7/14/16)SEG segment_A_bit segment_B_bit ... : Assign bits to display segments
  DP_BIT_(7/14/16)SEG decimal_point_bit : Assign bit to decimal point segment
  DUMP_(7/14/16): Displays ASCII-segment results
  DRAW hexadecimal_ASCII_value: Draw a character on the LCD - bit order will be reset to default

*/

#include <ASCIItoSegments.h>
#include <Adafruit_GFX.h>       // Core graphics library https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_ST7735.h>    // Hardware-specific library https://github.com/adafruit/Adafruit-ST7735-Library - LCD resolution used here is 128 x 128 but a display of a minimum of 84 x 48 can be used here

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // +percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

byte bit_order_7seg[7] = {0, 1, 2, 3, 4, 5, 6};
byte decimal_point_7seg = 7;
byte bit_order_14seg[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
byte decimal_point_14seg = 14;
byte bit_order_16seg[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
byte decimal_point_16seg = 16;

const word commandSize = 50;
char command[commandSize];
const byte FieldSize = 30;

const byte LCD_CS = 10; // also Shift/_Load for 74HC165 shift registers
const byte LCD_DC = A3;
const byte LCD_RESET = A5;

Adafruit_ST7735 lcd = Adafruit_ST7735(LCD_CS, LCD_DC, LCD_RESET);

// LCD definitions

const byte Digit_7segmentOffset_X = 2;
const byte Digit_7segmentOffset_Y = 2;
const byte Digit_14segmentOffset_X = 30;
const byte Digit_14segmentOffset_Y = 2;
const byte Digit_16segmentOffset_X = 58;
const byte Digit_16segmentOffset_Y = 2;

const byte SegmentStart_X_7[7] PROGMEM = {2, 18, 18, 2,   0,  0, 2};
const byte SegmentStart_Y_7[7] PROGMEM = {0,  2, 25, 44, 42,  2, 22};
const byte SegmentEnd_X_7[7] PROGMEM =  {16, 18, 18, 16,  0,  0, 16};
const byte SegmentEnd_Y_7[7] PROGMEM =  {0,  19, 42, 44, 25, 19, 22};
const byte SegmentStart_X_14[14] PROGMEM = {2, 18, 18, 2,   0,  0,  2, 12,  9, 16,  11,  9,  7, 7};
const byte SegmentStart_Y_14[14] PROGMEM = {0,  2, 25, 44, 42,  2, 22, 22,  2,  2,  25, 25, 25, 19};
const byte SegmentEnd_X_14[14] PROGMEM =  {16, 18, 18, 16,  0,  0,  6, 16,  9, 11, 16,   9,  2, 2};
const byte SegmentEnd_Y_14[14] PROGMEM =  { 0, 19, 42, 44, 25, 19, 22, 22, 19, 19, 42,  42, 42, 2};
const byte SegmentStart_X_16[16] PROGMEM = {2, 12, 18, 18, 16,  6,  0,  0,  2,  9, 16, 12, 11,  9,  7, 2};
const byte SegmentStart_Y_16[16] PROGMEM = {0,  0,  2, 25, 44, 44, 42, 19,  2,  2,  2, 22, 25, 25, 25, 22};
const byte SegmentEnd_X_16[16] PROGMEM =   {6, 16, 18, 18, 12,  2,  0,  0,  7,  9, 11, 16, 16,  9,  2, 6};
const byte SegmentEnd_Y_16[16] PROGMEM =   {0,  0, 19, 42, 44, 44, 25,  2, 19, 19, 19, 22, 42, 42, 42, 22};

void FlushSerialBuffer() {
  while (true) {
    if (Serial.available() > 0) {
      byte dummy = Serial.read();
      while (Serial.available() > 0) { // flush additional bytes from serial buffer if present
        dummy = Serial.read();
      }
      if (TimePerByte <= 16383) {
        delayMicroseconds(TimePerByte); // delay in case another byte may be received via the serial port
      }
      else { // deal with delayMicroseconds limitation
        unsigned long DelayTime = TimePerByte;
        DelayTime /= 1000;
        if (DelayTime > 0) {
          delay(DelayTime);
        }
        DelayTime = TimePerByte;
        DelayTime %= 1000;
        if (DelayTime > 0) {
          delayMicroseconds(DelayTime);
        }
      }
    }
    else {
      break;
    }
  }
}

void getField (char* buffer, int index) {
  int commandPos = 0;
  int fieldPos = 0;
  int spaceCount = 0;
  while (commandPos < commandSize) {
    if (command[commandPos] == 0x20) {
      spaceCount++;
      commandPos++;
    }
    else if (command[commandPos] == 0x0D || command[commandPos] == 0x0A) {
      break;
    }
    if (spaceCount == index) {
      buffer[fieldPos] = command[commandPos];
      fieldPos++;
    }
    commandPos++;
  }
  for (int ch = 0; ch < strlen(buffer); ch++) { // correct case of command
    buffer[ch] = toupper(buffer[ch]);
  }
  buffer[fieldPos] = '\0';
}

void setup() {
  lcd.initR(INITR_144GREENTAB); // initialize a ST7735S chip, black tab
  lcd.setRotation(3);
  lcd.setTextWrap(false);
  lcd.fillScreen(0x0000); // black
  Serial.begin(SerialPortRate);
}

void loop() {
  static int ByteCount = 0;
  if (Serial.available() > 0) {
    char inData = Serial.read();
    if (inData != '\n' && ByteCount < commandSize) {
      command[ByteCount] = inData;
      ByteCount++;
    }
    else {
      bool ValidField = true;
      ByteCount = 0;
      char field[FieldSize];
      getField(field, 0);
      if (strcmp(field, "BIT_ORDER_7SEG") == 0) {
        for (int i = 0; i < 7; i++) {
          getField(field, (1 + i));
          bit_order_7seg[i] = atoi(field);
        }
      }
      else if (strcmp(field, "BIT_ORDER_14SEG") == 0) {
        for (int i = 0; i < 14; i++) {
          getField(field, (1 + i));
          bit_order_14seg[i] = atoi(field);
        }
      }
      else if (strcmp(field, "BIT_ORDER_16SEG") == 0) {
        for (int i = 0; i < 16; i++) {
          getField(field, (1 + i));
          bit_order_16seg[i] = atoi(field);
        }
      }
      else if (strcmp(field, "DP_BIT_7SEG") == 0) {
        getField(field, 1);
        decimal_point_7seg = atoi(field);
      }
      else if (strcmp(field, "DP_BIT_14SEG") == 0) {
        getField(field, 1);
        decimal_point_14seg = atoi(field);
      }
      else if (strcmp(field, "DP_BIT_16SEG") == 0) {
        getField(field, 1);
        decimal_point_16seg = atoi(field);
      }
      else if (strcmp(field, "DUMP_7") == 0) {
        for (int i = 0; i < AvailableASCIIcharacters; i++) {
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 7 segment display is 0x"));
          byte result = ASCIItoSegments.Convert7Segment((DigitASCIIoffset + i), decimal_point_7seg, false, bit_order_7seg, false);
          if (result < 0x10) {
            Serial.print(F("0"));
          }
          Serial.println(result, HEX);
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 7 segment display with decimal point is 0x"));
          result = ASCIItoSegments.Convert7Segment((DigitASCIIoffset + i), decimal_point_7seg, true, bit_order_7seg, false);
          if (result < 0x10) {
            Serial.print(F("0"));
          }
          Serial.println(result, HEX);
        }
      }
      else if (strcmp(field, "DUMP_14") == 0) {
        for (int i = 0; i < AvailableASCIIcharacters; i++) {
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 14 segment display is 0x"));
          word result = ASCIItoSegments.Convert14Segment((DigitASCIIoffset + i), decimal_point_14seg, false, bit_order_14seg, false);
          word mask = 0xF000;
          for (int MaskDigits = 0; MaskDigits < 3; i++) {
            if ((mask & result) != 0) {
              break;
            }
            Serial.print(F("0"));
            mask >>= 4;
          }
          Serial.println(result, HEX);
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 14 segment display with decimal point is 0x"));
          result = ASCIItoSegments.Convert14Segment((DigitASCIIoffset + i), decimal_point_14seg, true, bit_order_14seg, false);
          mask = 0xF000;
          for (int MaskDigits = 0; MaskDigits < 3; i++) {
            if ((mask & result) != 0) {
              break;
            }
            Serial.print(F("0"));
            mask >>= 4;
          }
          Serial.println(result, HEX);
        }
      }
      else if (strcmp(field, "DUMP_16") == 0) {
        for (int i = 0; i < AvailableASCIIcharacters; i++) {
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 16 segment display is 0x"));
          unsigned long result = ASCIItoSegments.Convert16Segment((DigitASCIIoffset + i), decimal_point_16seg, false, bit_order_16seg, false);
          unsigned long mask = 0xF0000000;
          for (int MaskDigits = 0; MaskDigits < 7; i++) {
            if ((mask & result) != 0) {
              break;
            }
            Serial.print(F("0"));
            mask >>= 4;
          }
          Serial.println(result, HEX);
          Serial.print(F("ASCII character 0x"));
          Serial.print((DigitASCIIoffset + i), HEX);
          Serial.print(F(" to 16 segment display with decimal point is 0x"));
          result = ASCIItoSegments.Convert16Segment((DigitASCIIoffset + i), decimal_point_16seg, true, bit_order_16seg, false);
          mask = 0xF0000000;
          for (int MaskDigits = 0; MaskDigits < 7; i++) {
            if ((mask & result) != 0) {
              break;
            }
            Serial.print(F("0"));
            mask >>= 4;
          }
          Serial.println(result, HEX);
        }
      }
      else if (strcmp(field, "DRAW") == 0) {
        lcd.fillScreen(0x0000); // black
        getField(field, 1);
        byte ASCIIcode = strtoul(field, 0, 16);
        Serial.print(F("Drawing ASCII character 0x"));
        if (ASCIIcode < 0x10) {
          Serial.print(F("0"));
        }
        Serial.print(ASCIIcode, HEX);
        Serial.println(F(" on LCD"));
        unsigned long segments = ASCIItoSegments.Convert7Segment(ASCIIcode, decimal_point_7seg, false, bit_order_7seg, false);
        for (int i = 0; i < 7; i++) {
          bit_order_7seg[i] = i;
        }
        for (int i = 0; i < 7; i++) {
          if ((segments & 0x01) != 0) {
            lcd.drawLine((pgm_read_byte_near(SegmentStart_X_7 + i) + Digit_7segmentOffset_X), (pgm_read_byte_near(SegmentStart_Y_7 + i) + Digit_7segmentOffset_Y), (pgm_read_byte_near(SegmentEnd_X_7 + i) + Digit_7segmentOffset_X), (pgm_read_byte_near(SegmentEnd_Y_7 + i) + Digit_7segmentOffset_Y), 0xFFFF);
          }
          segments >>= 1;
        }
        segments = ASCIItoSegments.Convert14Segment(ASCIIcode, decimal_point_14seg, false, bit_order_14seg, false);
        for (int i = 0; i < 14; i++) {
          bit_order_14seg[i] = i;
        }
        for (int i = 0; i < 14; i++) {
          if ((segments & 0x01) != 0) {
            lcd.drawLine((pgm_read_byte_near(SegmentStart_X_14 + i) + Digit_14segmentOffset_X), (pgm_read_byte_near(SegmentStart_Y_14 + i) + Digit_14segmentOffset_Y), (pgm_read_byte_near(SegmentEnd_X_14 + i) + Digit_14segmentOffset_X), (pgm_read_byte_near(SegmentEnd_Y_14 + i) + Digit_14segmentOffset_Y), 0xFFFF);
          }
          segments >>= 1;
        }
        segments = ASCIItoSegments.Convert16Segment(ASCIIcode, decimal_point_16seg, false, bit_order_16seg, false);
        for (int i = 0; i < 16; i++) {
          bit_order_16seg[i] = i;
        }
        for (int i = 0; i < 16; i++) {
          if ((segments & 0x01) != 0) {
            lcd.drawLine((pgm_read_byte_near(SegmentStart_X_16 + i) + Digit_16segmentOffset_X), (pgm_read_byte_near(SegmentStart_Y_16 + i) + Digit_16segmentOffset_Y), (pgm_read_byte_near(SegmentEnd_X_16 + i) + Digit_16segmentOffset_X), (pgm_read_byte_near(SegmentEnd_Y_16 + i) + Digit_16segmentOffset_Y), 0xFFFF);
          }
          segments >>= 1;
        }
      }
      else {
        ValidField = false;
      }
      FlushSerialBuffer();
      if (ValidField == true) {
        Serial.println(F("OK"));
      }
      else {
        Serial.println(F("ERROR"));
      }
    }
  }
}