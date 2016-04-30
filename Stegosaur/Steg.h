//
//  Steg.h
//  Stegosaur
//
//  Created by Gudbrand Tandberg on 28/04/16.
//  Copyright (c) 2016 Duff Development. All rights reserved.
//

#ifndef __Stegosaur__Steg__
#define __Stegosaur__Steg__

#include <iostream>
#include <fstream>

using std::ifstream;
using std::ofstream;
using std::cout;

namespace steg {

/*
 * MAIN FUNCTIONS:
 */

// Attempts to hide message in the bitmap image provided using plain
// least-significant-bit steganography technique
int hideMessageInBitmap(const char *message, const char* filename);

// Attempts to extract a steganographic message from the input file
char *extractMessageFromBitmap(const char *filename);

/*
 * UTILITY FUNCTIONS:
 */

// Reads eight bytes from input, sets the last bits according to the byte to be
//hidden, then writes the modified bytes to output
void hideByteInStream(int messageByte, ifstream &input, ofstream &output);

// Sets last bit in 'byte' to 'bit'
void setLastBitInByte(int bit, int &byte);

// reads 8 bytes from input and collects the last bits to a single char
unsigned char extractByteFromStream(ifstream &input);

// Infixes ".steg" between a filename and its extention
char *infixDotSteg(const char *filename);

// Reads the BMP and DIB header and extracts useful information from input
char *readBitmapHeader(ifstream &input, int &width, int &height, int &dataOffset);

// Reads the next four bytes as an integer
int readInt(ifstream &input);

// Reads the next two bytes as a short
short readShort(ifstream &input);

// Converts a four-character array to a int
int toInt(const char* bytes);

// Converts a two-character array to a short
short toShort(const char* bytes);

// Converts a short to a two-character array (assumes sizeof(short) == 16)
char *shortToString(short number);

} //namespace steg

#endif /* defined(__Stegosaur__Steg__) */
