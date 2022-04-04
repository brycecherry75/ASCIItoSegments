#include "ASCIItoSegments.h"

uint8_t ASCIItoSegmentsClass::Convert7Segment(uint8_t ASCIIdata, uint8_t DecimalPointSegment, bool DecimalPointActive, uint8_t *SegmentOrder, bool PROGMEMused) {
  uint8_t ASCIItoSegmentsResult = 0;
  if (ASCIIdata >= DigitASCIIoffset && ASCIIdata <= DigitASCIIlimit) {
    if (ASCIIdata >= 'a' && ASCIIdata <= 'z') { // character offset starts from the lowercase set of letters
      ASCIIdata -= LowercaseLetterOffset;
    }
    if (ASCIIdata >= LowercaseLetterSkipStart) { // character offset starts after the lowercase set of letters
      ASCIIdata -= LowercaseLetterCount;
    }
    ASCIIdata -= DigitASCIIoffset;
    for (int i = 0; i < 7; i++) {
      uint8_t CurrentSegment;
      if (PROGMEMused == true) {
        CurrentSegment = pgm_read_byte_near(SegmentOrder + i);
      }
      else {
        CurrentSegment = SegmentOrder[i];
      }
      uint8_t temp = pgm_read_byte_near(ASCIItoSegmentsDigits_7segment + ASCIIdata);
      temp >>= i;
      temp &= 0x01;
      if (temp != 0) {
        uint8_t temp2 = 1;
        temp2 <<= CurrentSegment;
        ASCIItoSegmentsResult |= temp2;
      }
    }
    if (DecimalPointActive == true) {
      uint16_t temp = 1;
      temp <<= DecimalPointSegment;
      ASCIItoSegmentsResult |= temp;
    }
  }
  return ASCIItoSegmentsResult;
}

uint16_t ASCIItoSegmentsClass::Convert14Segment(uint8_t ASCIIdata, uint8_t DecimalPointSegment, bool DecimalPointActive, uint8_t * SegmentOrder, bool PROGMEMused) {
  uint16_t ASCIItoSegmentsResult = 0;
  if (ASCIIdata >= DigitASCIIoffset && ASCIIdata <= DigitASCIIlimit) {
    if (ASCIIdata >= 'a' && ASCIIdata <= 'z') { // character offset starts from the lowercase set of letters
      ASCIIdata -= LowercaseLetterOffset;
    }
    if (ASCIIdata >= LowercaseLetterSkipStart) { // character offset starts after the lowercase set of letters
      ASCIIdata -= LowercaseLetterCount;
    }
    ASCIIdata -= DigitASCIIoffset;
    for (int i = 0; i < 14; i++) {
      uint8_t CurrentSegment;
      if (PROGMEMused == true) {
        CurrentSegment = pgm_read_byte_near(SegmentOrder + i);
      }
      else {
        CurrentSegment = SegmentOrder[i];
      }
      uint16_t temp = pgm_read_word_near(ASCIItoSegmentsDigits_14segment + ASCIIdata);
      temp >>= i;
      temp &= 0x01;
      if (temp != 0) {
        uint16_t temp2 = 1;
        temp2 <<= CurrentSegment;
        ASCIItoSegmentsResult |= temp2;
      }
    }
    if (DecimalPointActive == true) {
      uint16_t temp = 1;
      temp <<= DecimalPointSegment;
      ASCIItoSegmentsResult |= temp;
    }
  }
  return ASCIItoSegmentsResult;
}

uint32_t ASCIItoSegmentsClass::Convert16Segment(uint8_t ASCIIdata, uint8_t DecimalPointSegment, bool DecimalPointActive, uint8_t * SegmentOrder, bool PROGMEMused) {
  uint32_t ASCIItoSegmentsResult = 0;
  if (ASCIIdata >= DigitASCIIoffset && ASCIIdata <= DigitASCIIlimit) {
    if (ASCIIdata >= 'a' && ASCIIdata <= 'z') { // character offset starts from the lowercase set of letters
      ASCIIdata -= LowercaseLetterOffset;
    }
    if (ASCIIdata >= LowercaseLetterSkipStart) { // character offset starts after the lowercase set of letters
      ASCIIdata -= LowercaseLetterCount;
    }
    ASCIIdata -= DigitASCIIoffset;
    for (int i = 0; i < 16; i++) {
      uint8_t CurrentSegment;
      if (PROGMEMused == true) {
        CurrentSegment = pgm_read_byte_near(SegmentOrder + i);
      }
      else {
        CurrentSegment = SegmentOrder[i];
      }
      uint32_t temp = pgm_read_word_near(ASCIItoSegmentsDigits_16segment + ASCIIdata);
      temp >>= i;
      temp &= 0x01;
      if (temp != 0) {
        uint32_t temp2 = 1;
        temp2 <<= CurrentSegment;
        ASCIItoSegmentsResult |= temp2;
      }
    }
    if (DecimalPointActive == true) {
      uint32_t temp = 1;
      temp <<= DecimalPointSegment;
      ASCIItoSegmentsResult |= temp;
    }
  }
  return ASCIItoSegmentsResult;
}

ASCIItoSegmentsClass ASCIItoSegments;