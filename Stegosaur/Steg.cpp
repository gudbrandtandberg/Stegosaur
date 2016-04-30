//
//  Steg.cpp
//  Stegosaur
//
//  Created by Gudbsrand Tandberg on 28/04/16.
//  Copyright (c) 2016 Duff Development. All rights reserved.
//

#include "Steg.h"

namespace steg {
	
const char STEG_ID = 'G';
const int MAXFILENAME = 100;
const int BITS_PER_BYTE = 8;
const int BYTES_PER_PIXEL = 3;
const int BITS_IN_STEG_HEADER = 24;
	
// TODO c++ style casts (check types in general)
// TODO switch to uint8_t etc. (in <stdint.h>)
	
/*
 * MAIN FUNCTIONS:
 */

int hideMessageInBitmap(const char *message, const char* filename) {
	// Get message length as short and char[2]
	unsigned short bytesInMessage = (unsigned short) strlen(message);
	char *bytesInMessageString = shortToString(bytesInMessage);
	int bitsInMessage = bytesInMessage * BITS_PER_BYTE;
	
	// Open outfile for writing steganographic image
	ofstream output;
	output.open(infixDotSteg(filename));  //error check?
	
	// Open input file (bitmap) for reading
	ifstream input;
	input.open(filename, ifstream::binary);
	if (input.fail()) {cout << "Could not find image file\n"; return false;}
	
	// Extract info from bitmap header
	int width, height, dataOffset;
	char *header = readBitmapHeader(input, width, height, dataOffset);
	
	if (!header) {return false;}
	
	// Check that the image is large enough to accomodate the message
	int bytesInBitmap = width * height * BYTES_PER_PIXEL;
	
	if ((bitsInMessage + BITS_IN_STEG_HEADER) > bytesInBitmap) {
		cout << "Message is too long for this image\n";
		return false;
	}
	
	// We have now read the headers and acted accordingly.
	// Copy the header as is to the outfile
	output.write(header, dataOffset);
	
	// Before hiding the message, hide the 'Steg Header' in the image
	hideByteInStream(STEG_ID, input, output);
	
	hideByteInStream(bytesInMessageString[0], input, output);
	hideByteInStream(bytesInMessageString[1], input, output);
	
	// Start transfering the data while inserting the message
	int nextImageByte, byteCounter;
	
	for (byteCounter = 0; byteCounter < bytesInMessage; byteCounter++) {
		hideByteInStream(message[byteCounter], input, output);
	}
	
	// Simply transfer the rest of the imagebytes
	while ((nextImageByte = input.get()) != EOF) {
		output.write((char *)&nextImageByte, 1);
	}
	
	input.close();
	output.close();
	
	return true;
}

char *extractMessageFromBitmap(const char *filename) {
	// Open input file (stegged bitmap) for reading
	ifstream input;
	input.open(filename, ifstream::binary);
	if (input.fail()) {cout << "Could not find bitmap file\n"; return  NULL;}
	
	// Read bitmap header
	int width, height, dataOffset;
	if (!readBitmapHeader(input, width, height, dataOffset)) return NULL;
	
	// Before transcribing the message, we check for the STEG_ID
	// and extract the message size
	unsigned char shouldBeStegID = extractByteFromStream(input);
	
	if (shouldBeStegID != STEG_ID) {
		cout << "Image has not been stegged by stegosaur\n";
		return NULL;
	}
	
	char lengthBuff[2];  //to hold string representation of the message length
	lengthBuff[1] = extractByteFromStream(input); //big endian..
	lengthBuff[0] = extractByteFromStream(input);
	unsigned short messageLength = toShort(lengthBuff);
	
	// Then we simply extract the message, byte by byte
	char *message = (char *)malloc(messageLength + 1);  //could always fail...

	if (!message) {
		cout << "Could not allocate enough space to hold your message\n";
		return NULL;
	}
	
	int i;
	for (i = 0; i < messageLength; i++) {
		message[i] = extractByteFromStream(input);
	} message[i] = '\0';
	
	input.close();
	
	return message;
}

/*
 * UTILITY FUNCTIONS IMPLEMENTATIONS
 */

void hideByteInStream(int messageByte, ifstream &input, ofstream &output) {
	int bitCounter, nextInputByte, nextMessageBit;
	
	for (bitCounter = 7; bitCounter >= 0; bitCounter--) {
		
		nextMessageBit = (messageByte >> bitCounter) & 1;
		nextInputByte = input.get();
		
		setLastBitInByte(nextMessageBit, nextInputByte);
		
		output.write((char *)&nextInputByte, 1);
	}
}

// crazy logic!
void setLastBitInByte(int bit, int &byte) {
	
	byte ^= (-bit ^ byte) & 1;
	
}

unsigned char extractByteFromStream(ifstream &input) {
	int extractedByte = 0, nextInputByte, bitCounter;
	
	for (bitCounter = 7; bitCounter >= 0; bitCounter--) {
		nextInputByte = input.get();
		extractedByte |= ((nextInputByte & 1) << bitCounter);
	}
	
	return (unsigned char)extractedByte;
}

char *infixDotSteg(const char *filename) {
	static char outFileName[MAXFILENAME];
	int i = 0;
	
	//function needs there to be a dot in the filename!
	while (filename[i] != '.') {
		outFileName[i] = filename[i];
		i++;
	}
	
	char dotSteg[6] = ".steg";
	strcpy(&outFileName[i], dotSteg);

	while (filename[i] != '\0') {
		outFileName[i+strlen(dotSteg)] = filename[i];
		i++;
	}
	
	return outFileName;
}

char *readBitmapHeader(ifstream &input, int &width, int &height, int &dataOffset) {
	// Read BMP Header
	char buffer[2];
	input.read(buffer, 2);

	if (buffer[0] != 'B' && buffer[1] != 'M') {
		cout << "Fail: image file is not a bitmap image\n";
		return NULL;
	}
	
	input.ignore(8);
	dataOffset = readInt(input);
	
	//Read DIB Header
	int headerSize = readInt(input);
	switch(headerSize) {
		case 40:   //V3
			width = readInt(input);
			height = readInt(input);
			input.ignore(2);
			if (readShort(input) != 24) {
				cout << "Fail: image is not 24 bits per pixel\n";
				return NULL;
			}
			if (readShort(input) != 0) {
				cout << "Fail: image is compressed\n";
				return NULL;
			}
			break;
		case 12:   //OS/2 V1
			width = readShort(input);
			height = readShort(input);
			input.ignore(2);
			if (readShort(input) != 24) {
				cout << "Fail: image is not 24 bits per pixel\n";
				return NULL;
			}
			break;
		case 64:   //OS/2 V2
			cout << "Fail: cannot steg OS/2 V2 bitmaps\n";
			return NULL;
			break;
		case 108:  //Windows V4
			cout << "Fail: cannot steg V4 bitmaps\n";
			return NULL;
			break;
		case 124:  //Windows V5
			cout << "Fail: cannot steg V5 bitmaps\n";
			return NULL;
			break;
		default:
			cout << "Fail: unknown bitmap format\n";
			return NULL;
	}
	
	char *header = (char *)malloc(dataOffset + 1);
	input.seekg(0, input.beg);
	input.read(header, dataOffset);
	input.seekg(dataOffset, input.beg);
	
	return header;
}

int readInt(ifstream &input) {
	char buffer[4];
	input.read(buffer, 4);
	return toInt(buffer);
}

short readShort(ifstream &input) {
	char buffer[2];
	input.read(buffer, 2);
	return toShort(buffer);
}

int toInt(const char* bytes) {
	return (int)(((unsigned char)bytes[3] << 24) |
				 ((unsigned char)bytes[2] << 16) |
				 ((unsigned char)bytes[1] << 8) |
				 (unsigned char)bytes[0]);
}

short toShort(const char* bytes) {
	return (short)(((unsigned char)bytes[1] << 8) |
				   (unsigned char)bytes[0]);
}

char *shortToString(short number) {  //could use downcasting trick from toShort?
	
	int eightBits = 255;
	
	unsigned int firstBits = number & eightBits;
	unsigned int lastBits = (number >> 8) & eightBits;
	
	static char string[3];
	
	
	string[0] = (unsigned char) lastBits;
	string[1] = (unsigned char) firstBits;
	
	return  string;
}
	
} //namespace steg
