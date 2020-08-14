#define CNFG_IMPLEMENTATION

#include "CNFG.h"
#include <stdio.h>



void HandleDestroy() { }

size_t const num_chars = 256;
unsigned char** charArray; 



short selectedSquareX, selectedSquareY;
short scale = 100;
short drawnPoints = 0;

int selectedChar = 48;
unsigned char* charData;

char testText[100];
const uint32_t colorContinuing = 0xff0000;
const uint32_t colorNotContinuing = 0xff00ff;
const uint32_t* currentColor = &colorContinuing;

int segmentLength = 0;

char coordToPos(int x, int y) {
	x = (x > 7 * scale ? 7 * scale : x) / scale;
	y = (y > 7 * scale ? 7 * scale : y) / scale;
	return (x << 3) + y;
}

void HandleButton(int x, int y, int button, int bDown) 
{
	
	if (bDown && button == 1 && selectedSquareX >= 0 && selectedSquareY >= 0)
	{
		char coords = coordToPos(x, y);

		//lines[drawnPoints] = coordToPos(x, y)+0b10000000;
		if (drawnPoints == 0 || coords != (charData[drawnPoints - 1] & 0b00111111))
		{
			if (drawnPoints >= 8 && ((drawnPoints & (drawnPoints - 1)) == 0))
			{
				printf("Allocating %d\n", sizeof(char) * (drawnPoints << 1));
				
				unsigned char* tmp =(unsigned char *) realloc(charArray[selectedChar], sizeof(char) * (drawnPoints << 1));
				if (tmp != NULL && tmp != charArray[selectedChar]) {
					free(charArray[selectedChar]);
					charArray[selectedChar] = tmp;
					charData = tmp;
				}
			}

			charData[drawnPoints] = coords + 0b10000000;
			
			if (drawnPoints > 0)
			{
				
				currentColor = &colorContinuing;
				memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] - 0b10000000, 1);
				segmentLength++;
			}
			drawnPoints++;
		}
		else if (coords == (charData[drawnPoints - 1] & 0b00111111)) 
		{

			
			memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] ^ 0b01000000, 1);
			if (charData[drawnPoints - 1] & 0b01000000)
			{
				currentColor = &colorNotContinuing;
			}
			else {
				currentColor = &colorContinuing;
				
			}
			

		
		}
	}

	else if (bDown && button == 2)
	{
		if (drawnPoints > 0)
		{
			drawnPoints--;
			memset(&charData[drawnPoints], 0b00000000, 1);
			if (drawnPoints > 0)
			{
				memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] + 0b10000000, 1);
				if (drawnPoints >= 7 && (((drawnPoints+1) & (drawnPoints)) == 0))
				{
					printf("Allocating %d\n", sizeof(char) * ((drawnPoints+1)));

					unsigned char* tmp = (unsigned char*)realloc(charArray[selectedChar], sizeof(char) * (drawnPoints+1));
					if (tmp != NULL) {
						charArray[selectedChar] = tmp;
						charData = tmp;
					}
					//printf("new size %d\n", drawnPoints + 1);
				}
			}
		}
	}
	
}

void changeChar(int difference) {
	selectedChar += difference;
	printf("%c\n", selectedChar);
	charData = charArray[selectedChar];
	segmentLength = 0;
	drawnPoints = 0;
	printf("0x%x\n", charData[0]);
	int c;
	if ((charData[0] & 0b10000000) != 0b10000000)
	{
		drawnPoints++;
		for (c = 0; (charData[c] & 0b10000000) != 0b10000000; c++) {

			if (charData[c] & 0b01000000 != 0b01000000) {
				segmentLength = 0;
			}
			else {
				segmentLength++;
			}
			drawnPoints++;
		}
	}
	
}


void HandleKey( int keycode, int bDown )
{
	if (bDown)
	{
		switch (keycode)
		{
		case 27:	//esc
			if (drawnPoints > 1)
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
			printf("\n");
			exit(0);
		case 38:	//up
			if (selectedChar - 16 >= 0) {
				changeChar(-16);
			}
			break;
		case 40:	//down
			if (selectedChar + 16 < 256) {
				changeChar(16);
			}
			break;
		case 37:	//left
			if (selectedChar - 1 >= 0) {
				changeChar(-1);
			}
			break;
		case 39:	//right
			if (selectedChar + 1 < 256) {
				changeChar(1);
			}
			break;
		}
	}
	//printf( "Key: %d -> %d\n", keycode, bDown );
}




void HandleMotion(int x, int y, int mask) 
{

	if (x < 8 * scale && y < 8 * scale)
	{
		selectedSquareX = (x > 7 * scale ? 7 * scale : x) / scale;
		selectedSquareY = (y > 7 * scale ? 7 * scale : y) / scale;
	}
	else
	{
		selectedSquareX = -1;
		selectedSquareY = -1;
	}
	
}

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
			if (c == selectedChar)
			{
				CNFGColor(0x444444);
				CNFGTackRectangle(CNFGPenX - 4 * scale / 2, CNFGPenY - 4 * scale / 2, CNFGPenX + 8 * scale / 2, CNFGPenY + 12 * scale / 2);
				CNFGColor(0xffffff);
			}
			CNFGDrawText(tw, scale);
		}
	}
}

void DrawTestText(int offsetX, int offsetY, int scale) 
{

	CNFGPenX = offsetX;
	CNFGPenY = offsetY;
	CNFGDrawText(testText, scale);
}


int main()
{
	charArray = malloc(sizeof(char*) * num_chars);

	for (int i = 0; i < num_chars; i++) {
		charArray[i] = malloc(8*sizeof(char));
		memset(charArray[i], 0b10000000, 1);
	}

	charData = charArray[48];

	sprintf(testText, "The quick brown fox jumped over the lazy dog");

	CNFGSetup("Example App", 1024, 1200);
	while (1)
	{
		short w, h;
		CNFGClearFrame(); //Clearing the buffer
		CNFGHandleInput(); //Handling all the buffer functions
		CNFGGetDimensions(&w, &h); //Getting the current window size
		
		CNFGPenX = 1; CNFGPenY = 1; //Position of the Pen, text will be made from here
		
		// Preparing the grid values, we use 3 bites for x and 3 for y so 8x8
		short gridW = 8;
		short gridH = 8;
		
		short centerSize = 10;	//Size of the center squares

		
		CNFGColor(0x444444);	//Color for the background rectangle
		
		CNFGTackRectangle(0, 0, gridW * scale, gridH * scale);	//Drawing the background rectangle


		CNFGColor(0x0);
		//current position for iterations
		short gx;
		short gy;
		for (gy = 0; gy < gridW; gy++) 
		{
			for (gx = 0; gx < gridH; gx++)
			{
				//Drawing all the rectangles that signal the center of a grid position
				CNFGTackRectangle(gx * scale + (scale - centerSize) / 2, gy * scale + (scale - centerSize) / 2, gx * scale + (scale - centerSize) / 2 + centerSize, gy * scale + (scale - centerSize) / 2 + centerSize);
			}

		}
		
		

		CNFGColor(0xffffff); //Setting the color white for the lines
		
		//If we have already drawn a point
		if (drawnPoints && selectedSquareX >= 0 && selectedSquareY >= 0 && (charData[drawnPoints - 1] & 0b01000000) != 0b01000000)
		{
			
			short x = (short)(((charData[drawnPoints-1] & 0b00111000) >> 3) * scale + (scale - centerSize) / 2);
			short y = (short)(((charData[drawnPoints-1] & 0b00000111)) * scale + (scale - centerSize) / 2);
			CNFGTackSegment(x + centerSize / 2, y + centerSize / 2, selectedSquareX * scale + scale / 2, selectedSquareY * scale + scale / 2);	//draw line from last point to selected point
		}


		int bQuit = 0;
		const unsigned char* lmap;
		
		if (drawnPoints)
		{
			int length = 0;
			lmap = &charData[0];
			do
			{

				short x1 = (short)((((*lmap) & 0b00111000) >> 3) * scale + (scale) / 2);
				short y1 = (short)(((*lmap) & 0b00000111) * scale + (scale) / 2);


				bQuit = *(lmap) & 0x80;
				lmap++;
				if ((*(lmap - 1) & 0b01000000))
				{
					if (length == 0) {
						CNFGTackRectangle(x1 - centerSize*2, y1 - centerSize*2, x1 + centerSize*2, y1 + centerSize*2);
					}
					length = 0;
					continue;
				}
				else {
					length++;
				}
				if (bQuit)continue;
				if (drawnPoints > 1)
				{
					short x2 = (short)((((*(lmap)) & 0b00111000) >> 3) * scale + (scale) / 2);
					short y2 = (short)(((*(lmap)) & 0b00000111) * scale + (scale) / 2);
					CNFGTackSegment(x1, y1, x2, y2);
				}
				
			} while (!bQuit);
		}
		
		
		CNFGColor(*currentColor);
		CNFGTackRectangle(selectedSquareX * scale + (scale - centerSize*2) / 2, selectedSquareY * scale + (scale - centerSize*2) / 2, selectedSquareX * scale + (scale - centerSize*2) / 2 + centerSize*2, selectedSquareY * scale + (scale - centerSize*2) / 2 + centerSize*2);

		//CNFGTackPixel(50, 50);
		
		/*unsigned char* point;
		for (point = lines[0]; (*point); point++)
		{
			printf("%c\n", point);
		}*/
		int yOff = 20 + (gridH)*scale ;
		makeText(10,yOff,2);
		yOff += 10+16 * 8 * 2;
		DrawTestText(10, yOff, 5);
		CNFGSwapBuffers();
	}
}
