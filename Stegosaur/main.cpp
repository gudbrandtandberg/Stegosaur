//
//  main.cpp
//  Stegosaur
//
//  Created by Gudbrand Tandberg on 28/04/16.
//  Copyright (c) 2016 Duff Development. All rights reserved.
//

#include "Steg.h"

#define SUCCESS 0
#define ERROR 1

using namespace steg;

// Reads and returns the contents of filename
const char *readEntireFile(const char *filename);

// Main function for stegosaur program
//
// Either "hides" a message inside an image file,
// or extracts a already hidden message from an image
//
// Usage:
// stegosaur messagefilename bitmapfilename
// stegosaur bitmapfilename
int main(int argc, const char * argv[]) {
	if (argc < 2) {

		cout << "Usage:\nstegosaur messagefilename bitmapfilename\nOR \n"
				"stegosaur bitmapfilename\n";
		return ERROR;
		
	} else if (argc == 2) {  //SEEK
		
		char *message = extractMessageFromBitmap(argv[1]);
		if (!message) {
			cout << "STEGOSAUR FAILED :(\n";
			return ERROR;

		} else {
			cout << "STEGOSAUR FOUND A MESSAGE!:\n";
			cout << message << '\n';
		}
	
	} else if (argc == 3) {  //HIDE
		
		const char *message = readEntireFile(argv[1]);
		if (!message) {
			cout << "STEGOSAUR FAILED :(\n";
			return ERROR;
		}
		if (!hideMessageInBitmap(message, argv[2])) {
			cout << "STEGOSAUR FAILED :(\n";
			return ERROR;
		} else {
			cout << "STEGOSAUR HID YOUR MESSAGE!\n";
		}
		
	} else {
		cout << "Too many arguments! \n";
		return ERROR;
	}
	
    return SUCCESS;
}

const char *readEntireFile(const char *filename) {
	ifstream file;
	file.open(filename, ifstream::binary);
	
	if (file.fail()) {
		cout << "Could not find message file\n";
		return NULL;
	}
	
	file.seekg(0, file.end);
	int fileLength = (int) file.tellg();
	
	if (fileLength > USHRT_MAX) {
		cout << "Message too long! Message must at least be shorter than "
			 << USHRT_MAX << " bytes\n";
		return NULL;
	}
	
	file.seekg(0, file.beg);
	char *fileContents = (char *) malloc(fileLength + 1);
	file.read(fileContents, fileLength);
	
	return fileContents;
}
