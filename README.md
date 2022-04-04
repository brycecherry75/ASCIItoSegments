#ASCIItoSegments Library#

A library for ASCII to 7/14/16 segment alphanumeric display conversion.

##Revisions:##
1.0.0 First release

##HOW TO USE:##

ASCIItoSegments(7/14/16)segment(ASCIIdata, DecimalPointSegment, DecimalPointActive, *SegmentOrder, PROGMEMused) - DecimalPointActive and PROGMEMused are a bool and all others are a byte; return value is uint8_t for 7 segment displays, uint16_t for 14 segment displays and uint32_t for 16 segment displays.

SegmentOrder is a byte array with element numbers corresponding to display segments in order as per ASCIItoSegmentLCDlayout_segmentorder.gif and can be changed for hardware configurations which do not have segments connected in order.

The following constants are available for checking bounds:

DigitASCIIoffset

DigitASCIIlimit

LowercaseLetterCount

ASCIItoSegments_DigitCount

LowercaseLetterSkipCount

AvailableASCIIcharacters

LowercaseLetterOffset

A spreadsheet calculator for converting alphanumeric display digits to hexadecimal in any bit order is also included.