#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int findBytesequenceLength(unsigned char);

int strlen_u(char*);
// uses findBytesequenceLength

int findBytePosition(char*, int);
// uses findBytesequenceLength

int findLetterPosition(char*, int);
// uses findBytesequenceLength

char *getc_u(FILE*, char*);
// uses findBytesequenceLength

char *substring_u(char*, char*, int, int);
// uses findBytesequenceLength
// uses strlen_u
// uses findBytePosition

int find_u(char*, char*, int, int);
// uses findBytePosition
// uses findLetterPosition


// Version 100



//**********************************************************************
int main(int argc, char *argv[]) {

    // UTF-8 Tabelle:  http://www.utf8-zeichentabelle.de/

    // This is just a long string:
    char *myLongString = malloc(1000*sizeof(char));
    //                              111111111122222222223333333333444444444455555555556666       letter pos
    //                    0123456789012345678901234567890123456789012345678901234567890123
    strcpy(myLongString, "püzzokiäuzträeszzdc²vbnm,.sälzzo³iuäzµtfürßücvzzbnhßzt@rpzzpoozz");


    // This might be a substring of the long string:
    char *mySubstring = malloc(400*sizeof(char));


    // - - - - - - - - - - - - - - - - - - - - -
    // test the find_u function
    strcpy(mySubstring, "zz");
    printf("%d\n", find_u(mySubstring, myLongString, 22, 3));


    // - - - - - - - - - - - - - - - - - - - - -
    // test the substring_u function
    strcpy(mySubstring, "");
    printf("%s\n", substring_u(myLongString, mySubstring, 12, 8));


    // - - - - - - - - - - - - - - - - - - - - -
    // test the getc_u function
    FILE *meinedatei;
    meinedatei = fopen ( "txt.txt" , "r" );

    char *z = malloc(6*sizeof(char));
    strcpy(z, "");

    for(int m=1; m<=28; m++) {
        printf("%s  ", getc_u(meinedatei, z));
    }

    fclose (meinedatei);



    // - - - - - - - - - - - - - - - - - - - - -
    // cleanup

    // free memory
    free(z);
    free(myLongString);
    free(mySubstring);

    return 0;
}
//********************************* end of main



//---------  function "getc_u" --------------------------------------
char *getc_u(FILE* inputFile, char* letter) {
	// reads a letter (meaningful character) from "inputFile", not just one byte, as getc() would do,
	// And stores it to "letter" (and returns it).

	// In many cases it is sufficiant to read and store just one byte.
    unsigned char c = getc(inputFile);
    sprintf(letter, "%c", c);

    // But if the byte is the beginning of a 2-, 3-, or 4-byte sequence, we must read the rest, too.
    int length = findBytesequenceLength(c);
    if (length > 1) {
        for(int n=2; n<=length; n++) {
        	c = getc(inputFile);
        	sprintf(letter, "%s%c", letter, c);
        }
    }

    return letter;
}
//---------  end of function "getc_u" --------------------------------------










//---------  function "findBytesequenceLength" --------------------------------------
int findBytesequenceLength(unsigned char c) {
	// returns 1 if the given character is a one-byte character;
	// returns 2, 3, or 4  if the character is the start of a 2-, 3-, or 4-Byte sequence (acc. UTF-8).

	if      (c >= 0x00 && c <= 0x7F) return 1;
	else if (c >= 0xC2 && c <= 0xDF) return 2;
	else if (c >= 0xE0 && c <= 0xEF) return 3;
	else if (c >= 0xF0 && c <= 0xF4) return 4;
	else return -1;
} // -----------------------end of function "findBytesequenceLength" --------------------




//-------- function "strlen_u" --------------------------------------------------------
int strlen_u(char* testString) {
	// Instead of just counting the Bytes (like strlen), this function counts
	// meaningful characters; some are represented by more than one Byte.
	// (UTF-8 standard)

	int letterCount=0;
	int charCount=0;
	unsigned char c;
    for (;;) {
    	c = testString[charCount];
    	if (c=='\0') break;
   		letterCount++;
       	charCount = charCount + findBytesequenceLength(c);  // checkChar() delivers 2, if c is the start of a 2-Byte sequence.
    }
	return letterCount;
} //-------- end of function "strlen_u" --------------------------------------------------------




//---------  function "findBytePosition" --------------------------------------
int findBytePosition(char* string, int targetLetterPos) {
	// The letter (meaningful character) no. 10 in a string might be byte no. 15,
	// since umlauts and other special characters are represented by more than one byte, in UTF-8.
    // This function takes a position number of a letter and gives the corresponding position number of the byte.

	int b=0; // byte counter; runs from 0 to the byte position that matches the given letter position
	int l=0; // letter counter; runs from 0 to the given target letter position

	int byteSequenceLength;

	for (;;) {
		if (l == targetLetterPos) break;
		byteSequenceLength = findBytesequenceLength((unsigned char)string[b]);
		b = b + byteSequenceLength;
		l++;
	}

    return b;
}
//---------  end of function "findBytePosition" --------------------------------------




//---------  function "findLetterPosition" --------------------------------------
int findLetterPosition(char* string, int targetBytePos) {
	// The byte no. 15 in a string might be letter (meaningful character) no. 10,
	// since umlauts and other special characters are represented by more than one byte, in UTF-8.
    // This function takes a position number of a byte and gives the corresponding position number of the letter.

	int l=0; // letter counter; runs from 0 to the letter position that matches the given byte position
	int b=0; // byte counter; runs from 0 to the given target byte position

	int byteSequenceLength;

	for (;;) {
		if (b >= targetBytePos) break;
		byteSequenceLength = findBytesequenceLength((unsigned char)string[b]);
		b = b + byteSequenceLength;
		l++;
	}

    return l;
}
//---------  end of function "findLetterPosition" --------------------------------------




//---------  function "substring_u" --------------------------------------
char *substring_u(char* longString, char* resultSubstring, int readStartPos, int readLength)  {

	// readStartPos, readLength:  position and number of meaningful characters, not Bytes
    // And also the following:
	int readStopPos = readStartPos + readLength-1;
    if (readStopPos > strlen_u(longString)-1)  readStopPos = strlen_u(longString)-1;

    // But this is about the bytes (not necessarily meaningful characters):
    int byteReadStartPos = findBytePosition(longString, readStartPos);
    int byteReadStopPos  = findBytePosition(longString, readStopPos);

    // The byte at byteReadStopPos might be the beginning of a sequence;
    // if this is the case, we must also take all those bytes.
    byteReadStopPos = byteReadStopPos + findBytesequenceLength((unsigned char)longString[byteReadStopPos]) -1;

    // don't do anything if the given parameter is nonsense:
	if (readStartPos > strlen_u(longString)-1) {
		return "";
    }

	int b;

	strcpy(resultSubstring, "");

	for (b=byteReadStartPos; b<=byteReadStopPos; b++) {
		sprintf(resultSubstring, "%s%c", resultSubstring, longString[b]);
	}

    return resultSubstring;
}
// ---------------   end of function "substring_u" ----------------------------------




//---------  function "find_u" --------------------------------------
int find_u(char* substring, char* longString, int givenStartPos, int targetCount)  {
    // Tries to find a substring (1st arg) in a long string (2nd arg).
    // Search begins at a given position (0-based) (3rd argument).
    // Returns the position (0-based) of n-th appearance (4th argument).
    // Returns -1 if not found.
    // (Substring must be < 400 char.)

    // givenStartPos, targetCount, and the result value are meant in "letters" (meaningfull characters, not bytes).


	// don't do anything if the given parameters are nonsense:
	if (targetCount<1 || givenStartPos<0) {
		return -1;
    }

	int appearanceCount=0;
	char testString[400]="";

	int byteReadStartPos = findBytePosition(longString, givenStartPos);
	int byteReadStopPos  = (byteReadStartPos + strlen(substring)-1);
	int i;


	// This loop reads (bytes) through longString
	for(;;) {

		// Do not read bytes beyond the end of longString
		if (byteReadStopPos > strlen(longString)-1) {
			break;
		}

		// Read a piece of longString that is the same byte length as substring, ...
		strcpy(testString,"");
		for (i = byteReadStartPos; i <= byteReadStopPos; i++) {
			sprintf(testString, "%s%c", testString, longString[i]);
		}

		// ...compare them; count upward, if you just found the substring
		if (strcmp(testString, substring)==0)  appearanceCount++;

		// Check if you found the substring often enough
		if (appearanceCount == targetCount) break;

		// Otherwise, prepare to read one more piece of longString
		byteReadStartPos++;
		byteReadStopPos++;

	} // end of loop that reads through longString


	// Now let's see what we have found:
	if (appearanceCount == targetCount) {
	    // Success, we found the position of the n-th appearance of subString.
		// Don't return the byte position, but the corresponding letter position:
		return findLetterPosition(longString, byteReadStartPos);
	}
	else {
		// no success, substring doesn't appear often enough.
		return -1;
	}
}
// ---------------   end of function "find_u" ----------------------------------
