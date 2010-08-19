#!/usr/bin/python

# Python script to convert a 14x9 pixel animated gif into a format that can
# be displayed on the LoLShield.
#
# By Matt Mets
#
# Requires Python and the Python Imaging Library (PIL)
# It would be nice if this was available using a more common language.

import sys, os, Image


class ImageSequence:
    def __init__(self, im):
        self.im = im
    def __getitem__(self, ix):
        try:
            if ix:
                self.im.seek(ix)
            return self.im
        except EOFError:
            raise IndexError # end of sequence

# Open image
filename = sys.argv[1]
filenameBase = os.path.splitext(os.path.split(filename)[1])[0]
im = Image.open(filename)

# Print the variable declaration
print "uint16_t " + filenameBase + "[][9] PROGMEM = {"

# For each frame in the image, convert it to black & white, then into the
# LoLShield format
for frame in ImageSequence(im):
    # Convert to black and white
    converted = frame.convert("1")

    print " {",
    frameString = converted.tostring()

    # For each row in the image
    for row in range (0, 9):
        charA = ord(frameString[row*2])
        charB = ord(frameString[row*2 + 1])

        rowTotal = 0
        # Handle the first 8 bits
        for col in range (0, 8):
            shiftAmount = 7-col
            rowTotal += ((charA >> shiftAmount) & 1) * 2**col
        # Then the next 6
        for col in range (0, 6):
            shiftAmount = 7-col
            rowTotal += ((charB >> shiftAmount) & 1) * 2**(col+8)
        # And output the total for this row
        print rowTotal, ",",
    print "},"

print "};"
