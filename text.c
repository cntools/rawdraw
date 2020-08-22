#define FONT_CREATION_TOOL
#define CNFG_IMPLEMENTATION

#include "FontData.h"
#include "CNFG.h"
#include <stdio.h>


//unsigned char* FontData = { 'a','b' };


size_t const num_chars = 256;
unsigned char** charArray; 



short selectedSquareX, selectedSquareY;		// Coords of the chosen point
short scale = 50;	//scale for the grid
short bytesInCharacter = 1; //Number of bytes in this character

short selectedBaselineX, selectedBaselineY;

int selectedChar = 48;	//Selected character 
unsigned char* charData;	//Array of points for this character

char testText[100]; //The quick brown fox...
const uint32_t colorContinuing = 0xff0000;	//color for the selected square when continuing a line
const uint32_t colorNotContinuing = 0xff00ff;	//color for the selected square when not continuing a line
const uint32_t* currentColor = &colorContinuing;	//current color

int segmentLength = 0;	//how many points since the last segment cut

unsigned char* tempFontData;
int* tempCharIndex;

//Data for the character visualization
short centerSize = 10;	
short gridW = 8;
short gridH = 8;

//Data for the baseline
int gridRepX;
int gridRepY;

char coordToPos(int x, int y) { //Screen coordinates to grid position
	x = (x > 7 * scale ? 7 * scale : x) / scale;
	y = (y > 7 * scale ? 7 * scale : y) / scale;
	return (x << 3) + y;
}

void HandleButton(int x, int y, int button, int bDown) 
{
	if (bDown && button == 1){
		if (selectedBaselineX >= 0 && selectedBaselineY >= 0){
			memset(&charData[0],(selectedBaselineX << 2) + (selectedBaselineY),1);
		}else if (selectedSquareX >= 0 && selectedSquareY >= 0){
			//		printf("Drawn points: %d\n", drawnPoints);
			char coords = coordToPos(x, y); //Get the selected point


			 //If we have no points or the last point was different from the current one, add a new point
			if (bytesInCharacter == 1 || coords != (charData[bytesInCharacter - 1] & 0b00111111)){
				//If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this character
				if (bytesInCharacter >= 8 && ((bytesInCharacter & (bytesInCharacter - 1)) == 0)){
					unsigned char* tmp = (unsigned char*)realloc(charArray[selectedChar], sizeof(char) * (bytesInCharacter << 1));
					if (tmp != NULL && tmp != charArray[selectedChar]) { 
						charArray[selectedChar] = tmp; //Get the new Address
						charData = tmp;
					}
				}

				charData[bytesInCharacter] = coords + 0b10000000; //Set the current point's position to the selected point's and add the "end of character flag"
				//printf("coords: %x, %x\N", coords, coords + 0b10000000);
				if (bytesInCharacter > 1) //and if we have another point from before
				{

					currentColor = &colorContinuing;
					memset(&charData[bytesInCharacter - 1], charData[bytesInCharacter - 1] - 0b10000000, 1); //remove the previous byte's end of character flag
					segmentLength++;
				}
				bytesInCharacter++;
			} else if (coords == (charData[bytesInCharacter - 1] & 0b00111111)){


				memset(&charData[bytesInCharacter - 1], charData[bytesInCharacter - 1] ^ 0b01000000, 1); //change the continuity flag
				if (charData[bytesInCharacter - 1] & 0b01000000)
				{
					currentColor = &colorNotContinuing;
				}
				else {
					currentColor = &colorContinuing;

				}



			}
		}
	} else if (bDown && button == 2){
		if (bytesInCharacter > 1){
			bytesInCharacter--;
			memset(&charData[bytesInCharacter], 0b00000000, 1); //reset the last point's value
			if (bytesInCharacter > 1){
				memset(&charData[bytesInCharacter - 1], charData[bytesInCharacter - 1] + 0b10000000, 1); //set the previous point's end of character flag to 1
				//If we have allocated more ram than what's neccesary now, halve it, same formula as before
				if (bytesInCharacter >= 7 && (((bytesInCharacter+1) & (bytesInCharacter)) == 0)){

					unsigned char* tmp = (unsigned char*)realloc(charArray[selectedChar], sizeof(char) * (bytesInCharacter+1));
					if (tmp != NULL) {
						charArray[selectedChar] = tmp;
						charData = tmp;
					}
				}
			}
		}
	}
	
}


void CNFGDrawBigText(const char* text, short scale)
{
	//memset(tempCharIndex, 0, 256);
	const unsigned char* lmap;
	float iox = (float)CNFGPenX; //x offset
	float ioy = (float)CNFGPenY; //y offset

	int place = 0;
	unsigned short index;
	int bQuit = 0;
	int segmentEnd = 0;
	while (text[place]){
		unsigned char c = text[place];
		switch (c)
		{
		case 9: // tab
			iox += 16 * scale;
			break;
		case 10: // linefeed
			iox = (float)CNFGPenX;
			ioy += 6 * scale;
			break;
		default:
			index = tempCharIndex[c];
			//printf("%d\n", tempCharIndex[c]);
			
			if (index == 0){
				iox += 8 * scale;
				break;
			}

			lmap = &tempFontData[index];

			short xbase = ((*lmap) & 0b00001100) >> 2;
			short ybase = (*lmap) & 0b00000011;
			lmap++;
			do{

				int x1 = ((((*lmap) & 0b00111000) >> 3) * scale + iox + xbase*scale);
				int y1 = (((*lmap) & 0b00000111) * scale + ioy + ybase * scale);
				segmentEnd = *lmap & 0x40;
				int x2 = 0;
				int y2 = 0;
				lmap++;
				if (segmentEnd){
					x2 = x1;
					y2 = y1;
				}
				else{

					x2 = ((((*lmap) & 0b00111000) >> 3) * scale + iox + xbase * scale);
					y2 = (((*lmap) & 0b00000111) * scale + ioy + ybase * scale);

				}


				CNFGTackSegment(x1, y1, x2, y2);
				bQuit = *(lmap - 1) & 0x80;

			} while (!bQuit);

			iox += 8 * scale;
		}
		place++;
	}
}


#ifdef FONTINIT
void LoadFont()
{
	
	tempCharIndex =(int *) malloc(sizeof(CharIndex));
	//memcpy(tempCharIndex, CharIndex, sizeof(CharIndex));
	memset(&tempCharIndex[0], 0, sizeof(CharIndex));
	

	
	tempFontData = malloc(sizeof(FontData));
	memcpy(tempFontData, FontData, sizeof(FontData));

	int characterToLoad;

	for (characterToLoad = 0; characterToLoad < 256; characterToLoad++) {
		int characterDataBytes = 1;
		charArray[characterToLoad] = malloc(8 * sizeof(char)); //allocating 8 points (bytes/characters) per character
		int index = CharIndex[characterToLoad];
		
		if (!index) {

			memset(&(charArray[characterToLoad][0]), 0b00000101, 1);
			memset(&(charArray[characterToLoad][1]), 0b10000000, 1);
		} else{
			unsigned char* characterData = &FontData[index];

			unsigned char* characterDestination = charArray[characterToLoad];

			int c = -1;
			do{
				c++;
				characterDataBytes++;
				//If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this array
				if (characterDataBytes >= 8 && ((characterDataBytes & (characterDataBytes - 1)) == 0)) {

					unsigned char* tmp = (unsigned char*)realloc(charArray[characterToLoad], sizeof(char) * (characterDataBytes << 1));
					if (tmp != NULL && tmp != charArray[characterToLoad]) { 
						charArray[characterToLoad] = tmp;
						characterDestination = tmp;
					}
				}
				memset(&characterDestination[c], characterData[c], 1);			

			} while ((characterData[c] & 0b10000000) != 0b10000000);
		}
	}
	printf("Font Loaded\n");
}



	

#else
void LoadFont() 
{

	for (int i = 0; i < num_chars; i++) {
		charArray[i] = malloc(8 * sizeof(char)); //allocating 8 points (bytes/characters) per character
		memset(charArray[i], 0b10000000, 1);
	}
	printf("No Font Loaded\n");
}
#endif // !FONTINIT


void SaveFont(char* filename)
{
	FILE* f = fopen(filename, "wb");
	int characterIndex[256];
	unsigned char* AllCharacterData = malloc(sizeof(char) * 8);
	memset(&AllCharacterData[0], 0b10000000, 1);

	int totalPoints = 0;

	int characterToSave;

	for (characterToSave = 0; characterToSave < 256; characterToSave++) {

		unsigned char* characterData = charArray[characterToSave];
		int c = -1;
		
		//If the character has lines from previously, count the amount of points and segments
		if ((characterData[1] & 0b10000000) != 0b10000000){
			characterIndex[characterToSave] = totalPoints + 1;

			do{
				
				c++;
				totalPoints++;
				if (totalPoints >= 7 && ((totalPoints & (totalPoints - 1)) == 0)) //If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this array
				{
				//	printf("char %c too big, allocating more\n", characterToSave);
					unsigned char* tmp = (unsigned char*)realloc(AllCharacterData, sizeof(char) * (totalPoints << 1));
					if (tmp != NULL && tmp != AllCharacterData) { //If the memory was reallocated properly and we get a new pointer
						AllCharacterData = tmp; //Get the new Address
					}
				}
				AllCharacterData[totalPoints] = characterData[c];

				//printf("%x\n", characterData[c]);

			} while ((characterData[c] & 0b10000000) != 0b10000000);
		} else{
			characterIndex[characterToSave] = 0;
		}
	}
	fprintf(f,"%s\n%s\n", "#ifndef FONTINIT","#define FONTINIT");
	fprintf(f, "int CharIndex[] = {\n\t");

	for (int i = 0; i < 256; i++)
		fprintf(f, "%4d,%s", characterIndex[i], (((i+1) % 16) == 0) ? "\n\t" : " ");

	tempCharIndex = (int *) malloc(sizeof(characterIndex));
	memcpy(tempCharIndex, characterIndex, sizeof(characterIndex));

	fprintf(f, "};\n\n");

	fprintf(f, "unsigned char FontData[] = {\n\t");
	for (int i = 0; i <= totalPoints; i++)
		fprintf(f, "0x%02x,%s", AllCharacterData[i], (((i+1) % 16)==0) ? "\n\t" : " ");
	fprintf(f, "};\n\n");

	tempFontData = malloc(sizeof(AllCharacterData));
	memcpy(&tempFontData, &AllCharacterData, sizeof(AllCharacterData));

	fprintf(f, "%s\n", "#endif");
	fclose(f);

}


//Selecting a different character from the grid below to redraw
void changeChar(int difference) 
{
	SaveFont("FontBackup.c");
	selectedChar += difference;
	printf("Selected character: %c, %d\n", selectedChar, selectedChar);
	charData = charArray[selectedChar];
	segmentLength = 0;
	bytesInCharacter = 0;
	//printf("0x%x\n", charData[0]);
	int c;

	//If the character has lines from previously, count the amount of points and segments
	if ((charData[1] & 0b10000000) != 0b10000000){
		bytesInCharacter++;
		for (c = 0; (charData[c] & 0b10000000) != 0b10000000; c++) {

			if (charData[c] & 0b01000000 != 0b01000000) {
				segmentLength = 0;
			}
			else {
				segmentLength++;
			}
			bytesInCharacter++;
		}
	} else{	
		bytesInCharacter++;
		memset(&charData[0],0b00000101,1);
	}

}


void resetChar()
{
	free(charArray[selectedChar]);
	charArray[selectedChar] = malloc(8 * sizeof(char));
	charData = charArray[selectedChar];
	memset(&charData[0], 0b00000101, 1);
	memset(&charData[1], 0b10000000, 1);
	segmentLength = 0;
	bytesInCharacter = 1;


}
void HandleDestroy() { 
	SaveFont("FontData.c");
}

void HandleKey( int keycode, int bDown )
{
	if (bDown){
		//printf("kc: %d\n",keycode);
		switch (keycode)
		{
		case 65307:
		case 27:	//esc
			SaveFont("FontData.c");
			if (bytesInCharacter > 1)
			{

				int bQuit = 0;
				const unsigned char* lmap = &charData[0];
				do
				{
					printf("0x%x", *lmap);

					bQuit = *(lmap) & 0x80;
					lmap++;
					if (!bQuit)printf(", ");
				} while (!bQuit);
			}
//			printf("\n");
			exit(0);

		case 15:	//r x11
		case 114:	//r
			resetChar();
			break;

		case 65362:	//up linux
		case 119:	//w
		case 38:	//up
			if (selectedChar - 16 >= 0) {
				changeChar(-16);
			}
			break;


		case 65364:	//down linux
		case 115:	//s
		case 40:	//down
			if (selectedChar + 16 < 256) {
				changeChar(16);
			}
			break;

		case 65361:
		case 97:
		case 37:	//left
			if (selectedChar - 1 >= 0) {
				changeChar(-1);
			}
			break;

		case 65363:
		case 100:
		case 39:	//right
			if (selectedChar + 1 < 256) {
				changeChar(1);
			}
			break;
		}
	}
}




void HandleMotion(int x, int y, int mask) 
{
	int secondaryGridx = 8 * scale + 10;
	int secondaryGridy = 120;
	//Update the selected point if the mouse is in the drawing grid.
	if (x < 8 * scale && y < 8 * scale){
		selectedSquareX = (x > 7 * scale ? 7 * scale : x) / scale;
		selectedSquareY = (y > 7 * scale ? 7 * scale : y) / scale;
	}else{
		selectedSquareX = -1;
		selectedSquareY = -1;
	}
	
	if (x>(secondaryGridx) && x<(secondaryGridx+4*scale) && y>(secondaryGridy) && y < (secondaryGridy + 4 * scale)){	
		selectedBaselineX = ((x - (secondaryGridx)) / scale);
		selectedBaselineY = ((y - (secondaryGridy)) / scale);
	}else{
		selectedBaselineX = -1;
		selectedBaselineY = -1;
	}
	
}


//copied from rawdraw.c to show the characters grid
void makeText(int offsetX,int offsetY,int scale)
{
	int i;
	CNFGColor(0xffffff);
	for (i = 0; i < 1; i++)
	{
		int c;
		char tw[2] = { 0,0};
		for (c = 0; c < 256; c++)
		{
			
			tw[0] = c;

			CNFGPenX = offsetX + (c % 16) * 16 * scale / 2;
			CNFGPenY = offsetY + (c / 16) * 16 * scale / 2;

			//if this is the selected character, draw a grey rectangle behind
			if (c == selectedChar){
				CNFGColor(0x444444);
				CNFGTackRectangle(CNFGPenX - 4 * scale / 2, CNFGPenY - 4 * scale / 2, CNFGPenX + 8 * scale / 2, CNFGPenY + 12 * scale / 2);
				CNFGTackRectangle(CNFGPenX - 4 * scale / 2 + 17 * 16, CNFGPenY - 4 * scale / 2, CNFGPenX + 8 * scale / 2 + 17 * 16, CNFGPenY + 12 * scale / 2);
				CNFGColor(0xffffff);
			}
			CNFGDrawText(tw, scale);
			CNFGPenX = offsetX + (c % 16) * 16 * scale / 2 +17*16;
			CNFGDrawBigText(tw, scale/2);
		}
	}
	CNFGTackSegment(offsetX+ 16 * 16,offsetY, offsetX  + 16 * 16, offsetY+256/16*16*scale/2);
}


//Here we will try the new font	with the testText
void DrawTestText(int offsetX, int offsetY, int scale) 
{

	CNFGPenX = offsetX;
	CNFGPenY = offsetY;
	CNFGDrawText(testText, scale);
	CNFGPenY = offsetY+5*scale;
	CNFGDrawBigText(testText, scale/2);
}

void drawCharacterGrid() 
{
	CNFGColor(0x444444);	

	CNFGTackRectangle(0, 0, gridW * scale, gridH * scale);	

	//Draw the squares
	CNFGColor(0x0);
	short gx;
	short gy;
	for (gy = 0; gy < gridW; gy++) {
		for (gx = 0; gx < gridH; gx++) {
			CNFGTackRectangle(gx * scale + (scale - centerSize) / 2, gy * scale + (scale - centerSize) / 2, gx * scale + (scale - centerSize) / 2 + centerSize, gy * scale + (scale - centerSize) / 2 + centerSize);
		}
	}


}

void prepareFrame(short* w, short* h) 
{
	CNFGClearFrame(); //Clearing the buffer
	CNFGHandleInput(); //Handling all the buffer functions
	CNFGGetDimensions(w, h); //Getting the current window size
	CNFGPenX = 1; CNFGPenY = 1; //Position of the Pen, text will be made from here

	drawCharacterGrid();
}


void drawLineFromLastPoint() 
{
	short x = (short)(((charData[bytesInCharacter - 1] & 0b00111000) >> 3) * scale + (scale - centerSize) / 2);
	short y = (short)(((charData[bytesInCharacter - 1] & 0b00000111)) * scale + (scale - centerSize) / 2);
	CNFGTackSegment(x + centerSize / 2, y + centerSize / 2, selectedSquareX * scale + scale / 2, selectedSquareY * scale + scale / 2);	//draw line from last point to selected point
}

void drawBaselineGrid() 
{

	CNFGColor(0x444444);
	CNFGTackRectangle(gridRepX, gridRepY, gridRepX + 4 * scale, gridRepY + 4 * scale);
	CNFGColor(0x884444);
	CNFGTackRectangle(gridRepX + scale, gridRepY + 1 * scale, gridRepX + 2 * scale, gridRepY + 2 * scale);

	CNFGColor(0x000000);
	for (int linePos = 1; linePos < 4; linePos++) {

		CNFGTackSegment(gridRepX + linePos * scale, gridRepY, gridRepX + linePos * scale, gridRepY + 4 * scale);
		CNFGTackSegment(gridRepX, gridRepY + linePos * scale, gridRepX + 4 * scale, gridRepY + linePos * scale);
	}

	if (selectedBaselineX >= 0 && selectedBaselineY >= 0) {
		CNFGColor(0x444488);
		CNFGTackRectangle(gridRepX + selectedBaselineX * scale, gridRepY + selectedBaselineY * scale, gridRepX + (selectedBaselineX + 1) * scale, gridRepY + (selectedBaselineY + 1) * scale);
	}

}

void drawSelectedSlot() 
{
	CNFGColor(*currentColor);
	CNFGTackRectangle(selectedSquareX * scale + (scale - centerSize * 2) / 2, selectedSquareY * scale + (scale - centerSize * 2) / 2, selectedSquareX * scale + (scale - centerSize * 2) / 2 + centerSize * 2, selectedSquareY * scale + (scale - centerSize * 2) / 2 + centerSize * 2);


}

void drawCharacterInfo()
{
	int yOff = 20 + (gridH)*scale;
	makeText(10, yOff, 2);
	CNFGPenX = 8 * scale + 10;
	CNFGPenY = 10;

	char characterInfo[32];
	sprintf(characterInfo, "Character code: %d\nCharacter: %c\n", selectedChar, selectedChar);
	CNFGDrawText(characterInfo, 5);

	CNFGPenY = 90;
	sprintf(characterInfo, "New Character:  %c\n", selectedChar);



	CNFGDrawBigText(characterInfo, 2);
}

void drawCharacterTest()
{

	int yOff = 20 + (gridH)*scale + 10 + 16 * 8 * 2;
	DrawTestText(10, yOff, 4);

	CNFGPenY = yOff + 70;
	CNFGDrawBigText("Tool made by https://github.com/efrenmanuel", 2);
}

void drawCharacterLines(const unsigned char * lmap)
{
	int length = 0; //total character's points


	short xbase = ((*lmap) & 0b00001100) >> 2;
	short ybase = (*lmap) & 0b00000011;
	lmap++;


	CNFGColor(0x448844);
	CNFGTackRectangle(gridRepX + xbase * scale, gridRepY + ybase * scale, gridRepX + (xbase + 1) * scale, gridRepY + (ybase + 1) * scale);

	CNFGColor(0xffffff);

	int bQuit = 0;
	do {

		short x1 = (short)((((*lmap) & 0b00111000) >> 3) * scale + (scale) / 2); //get the first point's coordinates
		short y1 = (short)(((*lmap) & 0b00000111) * scale + (scale) / 2);


		bQuit = *(lmap) & 0x80; //Check if this is the last point
		lmap++;
		if ((*(lmap - 1) & 0b01000000)) {
			if (length == 0) { //and it had no length, aka it was a single point segment
				CNFGTackRectangle(x1 - centerSize * 2, y1 - centerSize * 2, x1 + centerSize * 2, y1 + centerSize * 2); //draw a rectangle 
			}
			length = 0; //reset the lenght
			continue;
		}
		else {	// if it wasnt the last point of the segment
			length++; //add one to the segment's length
		}
		if (bQuit)continue; //if it was the last point of the character, skip everything else
		if (bytesInCharacter > 1) {
			//Draw a line from the previous point to this one
			short x2 = (short)((((*(lmap)) & 0b00111000) >> 3) * scale + (scale) / 2);
			short y2 = (short)(((*(lmap)) & 0b00000111) * scale + (scale) / 2);
			CNFGTackSegment(x1, y1, x2, y2);
		}

	} while (!bQuit);

}

int main()
{
	charArray = malloc(sizeof(char*) * num_chars);//Allocating the memory for the array of pointers that'll store every character's info
	LoadFont();


	charData = charArray[48]; //Selecting one random character

	changeChar(0);
	sprintf(testText, "'The quick brown fox jumps over the lazy dog' \"05+6=B\""); //Setting up the test text

	CNFGSetup("Font Creation by https://github.com/efrenmanuel for rawdraw.", 1000, 800);

	gridRepX = 8 * scale + 10;
	gridRepY = 120;


	while (1)
	{
		short w, h;
		prepareFrame(&w, &h);
		
		
		CNFGColor(0xffffff); //Setting the color white for the lines
		
		//If we have already drawn a point
		if (bytesInCharacter > 1 && bytesInCharacter && selectedSquareX >= 0 && selectedSquareY >= 0 && (charData[bytesInCharacter - 1] & 0b01000000) != 0b01000000){
			drawLineFromLastPoint();
			
		}

		drawBaselineGrid();


		
		 //Current point
		
		if (bytesInCharacter){

			drawCharacterLines(&charData[0]);
			
		}
		
		drawSelectedSlot();

		drawCharacterInfo();
		
		drawCharacterTest();
		
		CNFGSwapBuffers();
	}
}
