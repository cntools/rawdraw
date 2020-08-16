#define CNFG_IMPLEMENTATION

#include "CNFG.h"
#include <stdio.h>
#include "FontData.h"

//unsigned char* FontData = { 'a','b' };


size_t const num_chars = 256;
unsigned char** charArray; 



short selectedSquareX, selectedSquareY;		// Coords of the chosen point
short scale = 50;	//scale for the grid
short drawnPoints = 0; //Number of points in this character

int selectedChar = 48;	//Selected character 
unsigned char* charData;	//Array of points for this character

char testText[100]; //The quick brown fox...
const uint32_t colorContinuing = 0xff0000;	//color for the selected square when continuing a line
const uint32_t colorNotContinuing = 0xff00ff;	//color for the selected square when not continuing a line
const uint32_t* currentColor = &colorContinuing;	//current color

int segmentLength = 0;	//how many points since the last segment cut

char coordToPos(int x, int y) { //Screen coordinates to grid position
	x = (x > 7 * scale ? 7 * scale : x) / scale;
	y = (y > 7 * scale ? 7 * scale : y) / scale;
	return (x << 3) + y;
}

void HandleButton(int x, int y, int button, int bDown) 
{
	
	if (bDown && button == 1 && selectedSquareX >= 0 && selectedSquareY >= 0) //If we left click inside the grid
	{
		char coords = coordToPos(x, y); //Get the selected point

		
		if (drawnPoints == 0 || coords != (charData[drawnPoints - 1] & 0b00111111)) //If we have no points or the last point was different from the current one, add a new point
		{
			if (drawnPoints >= 8 && ((drawnPoints & (drawnPoints - 1)) == 0)) //If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this character
			{
				//printf("Allocating %d\n", sizeof(char) * (drawnPoints << 1));
				
				unsigned char* tmp =(unsigned char *) realloc(charArray[selectedChar], sizeof(char) * (drawnPoints << 1)); 
				if (tmp != NULL && tmp != charArray[selectedChar]) { //If the memory was reallocated properly and we get a new pointer
					free(charArray[selectedChar]); //Free the old address
					charArray[selectedChar] = tmp; //Get the new Address
					charData = tmp;
				}
			}

			charData[drawnPoints] = coords + 0b10000000; //Set the current point's position to the selected point's and add the "end of character flag"
			//printf("coords: %x, %x\N", coords, coords + 0b10000000);
			if (drawnPoints > 0) //and if we have another point from before
			{
				
				currentColor = &colorContinuing;
				memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] - 0b10000000, 1); //remove the previous byte's end of character flag
				segmentLength++;
			}
			drawnPoints++;
		}
		else if (coords == (charData[drawnPoints - 1] & 0b00111111))  //else if we are clicking the same point again
		{

			
			memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] ^ 0b01000000, 1); //change the continuity flag
			if (charData[drawnPoints - 1] & 0b01000000)
			{
				currentColor = &colorNotContinuing;
			}
			else {
				currentColor = &colorContinuing;
				
			}
			

		
		}
	}

	else if (bDown && button == 2) //If we are right clicking
	{
		if (drawnPoints > 0) //And there are points drawn
		{
			drawnPoints--;
			memset(&charData[drawnPoints], 0b00000000, 1); //reset the last point's value
			if (drawnPoints > 0)
			{
				memset(&charData[drawnPoints - 1], charData[drawnPoints - 1] + 0b10000000, 1); //set the previous point's end of character flag to 1
				if (drawnPoints >= 7 && (((drawnPoints+1) & (drawnPoints)) == 0)) //If we have allocated more ram than neccesary now, halve it, same formula as before
				{
				//	printf("Allocating %d\n", sizeof(char) * ((drawnPoints+1)));

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



#ifdef FONTINIT
void LoadFont()
{


	int totalPoints = 0;

	int characterToLoad;

	for (characterToLoad = 0; characterToLoad < 256; characterToLoad++) {

		charArray[characterToLoad] = malloc(8 * sizeof(char)); //allocating 8 points (bytes/characters) per character
		int index = CharIndex[characterToLoad];
		
		if (!index) {

			memset(charArray[characterToLoad], 0b10000000, 1);
		}
		else {

			//If the character has data, add it
			
			unsigned char* characterData = &FontData[index];

			unsigned char* characterDestination = charArray[characterToLoad];

			int c = -1;
			do
			{
				c++;
				totalPoints++;

				if (totalPoints >= 8 && ((totalPoints & (totalPoints - 1)) == 0)) //If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this array
				{

					unsigned char* tmp = (unsigned char*)realloc(charArray[characterToLoad], sizeof(char) * (totalPoints << 1));
					if (tmp != NULL && tmp != charArray[characterToLoad]) { //If the memory was reallocated properly and we get a new pointer
						free(charArray[characterToLoad]); //Free the old address
						charArray[characterToLoad] = tmp; //Get the new Address
						characterDestination = tmp;
					}
				}
				memset(&characterDestination[c], characterData[c], 1);
				printf("%x\n", characterData[c]);
				

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
		if ((characterData[0] & 0b10000000) != 0b10000000)
		{
			characterIndex[characterToSave] = totalPoints + 1;

			do
			{
				c++;
				totalPoints++;

				if (totalPoints >= 7 && ((totalPoints & (totalPoints - 1)) == 0)) //If we have used 8 or more bytes and the current count is a power of 2 (8,16,32...) double the amount of memory for this array
				{

					unsigned char* tmp = (unsigned char*)realloc(AllCharacterData, sizeof(char) * (totalPoints << 1));
					if (tmp != NULL && tmp != AllCharacterData) { //If the memory was reallocated properly and we get a new pointer
						free(AllCharacterData); //Free the old address
						AllCharacterData = tmp; //Get the new Address
					}
				}
				AllCharacterData[totalPoints] = characterData[c];

				//printf("%x\n", characterData[c]);

			} while ((characterData[c] & 0b10000000) != 0b10000000);
		}
		else {
			characterIndex[characterToSave] = 0;
		}
	}
	fprintf(f,"%s\n%s\n", "#ifndef FONTINIT","#define FONTINIT");
	fprintf(f, "int CharIndex[] = {\n\t");

	for (int i = 0; i < 256; i++)
		fprintf(f, "%4d,%s", characterIndex[i], (((i+1) % 16) == 0) ? "\n\t" : " ");
	fprintf(f, "};\n\n");

	fprintf(f, "unsigned char FontData[] = {\n\t");
	for (int i = 0; i <= totalPoints; i++)
		fprintf(f, "0x%02x,%s", AllCharacterData[i], (((i+1) % 16)==0) ? "\n\t" : " ");
	fprintf(f, "};\n\n");

	fprintf(f, "%s\n", "#endif");
	fclose(f);

}


//Selecting a different character from the grid below to redraw
void changeChar(int difference) {
	SaveFont("FontBackup.c");
	selectedChar += difference;
	printf("Selected character: %c, %d\n", selectedChar, selectedChar);
	charData = charArray[selectedChar];
	segmentLength = 0;
	drawnPoints = 0;
	//printf("0x%x\n", charData[0]);
	int c;

	//If the character has lines from previously, count the amount of points and segments
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

void HandleDestroy() { 
	SaveFont("FontData.c");
}

void HandleKey( int keycode, int bDown )
{
	if (bDown)
	{
		printf("kc: %d\n",keycode);
		switch (keycode)
		{
		case 27:	//esc
			SaveFont("FontData.c");
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
		case 119:
		case 38:	//up
			if (selectedChar - 16 >= 0) {
				changeChar(-16);
			}
			break;
		case 115:
		case 40:	//down
			if (selectedChar + 16 < 256) {
				changeChar(16);
			}
			break;
		case 97:
		case 37:	//left
			if (selectedChar - 1 >= 0) {
				changeChar(-1);
			}
			break;
		case 100:
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
	//Update the selected point if the mouse is in the drawing grid.
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


//Here we will try the new font	with the testText
void DrawTestText(int offsetX, int offsetY, int scale) 
{

	CNFGPenX = offsetX;
	CNFGPenY = offsetY;
	CNFGDrawText(testText, scale);
	CNFGPenY = offsetY+5*scale;
	CNFGDrawBigText(testText, scale);
}





int main()
{
	charArray = malloc(sizeof(char*) * num_chars);//Allocating the memory for the array of pointers that'll store every character's info
	LoadFont();


	charData = charArray[48]; //Selecting one random character

	changeChar(0);
	sprintf(testText, "'The quick brown fox jumped over the lazy dog' \"05+6=B\""); //Setting up the test text

	CNFGSetup("Font Creation by https://github.com/efrenmanuel for rawdraw.", 800, 1200);

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


		int bQuit = 0; //flag in case we reach the last point of the character
		const unsigned char* lmap; //Current point
		
		if (drawnPoints)
		{
			int length = 0; //total character's points
			lmap = &charData[0]; //start by the first point... of course.
			do
			{

				short x1 = (short)((((*lmap) & 0b00111000) >> 3) * scale + (scale) / 2); //get the first point's coordinates
				short y1 = (short)(((*lmap) & 0b00000111) * scale + (scale) / 2); 


				bQuit = *(lmap) & 0x80; //Check if this is the last point
				lmap++; 
				if ((*(lmap - 1) & 0b01000000)) //if it was the last point of the segment
				{
					if (length == 0) { //and it had no length, aka it was a single point segment
						CNFGTackRectangle(x1 - centerSize*2, y1 - centerSize*2, x1 + centerSize*2, y1 + centerSize*2); //draw a rectangle 
					}
					length = 0; //reset the lenght
					continue;
				}
				else {	// if it wasnt the last point of the segment
					length++; //add one to the segment's length
				}
				if (bQuit)continue; //if it was the last point of the character, skip everything else
				if (drawnPoints > 1) //if we had already drawn more points
				{
					//Draw a line from the previous point to this one
					short x2 = (short)((((*(lmap)) & 0b00111000) >> 3) * scale + (scale) / 2);
					short y2 = (short)(((*(lmap)) & 0b00000111) * scale + (scale) / 2);
					CNFGTackSegment(x1, y1, x2, y2);
				}
				
			} while (!bQuit);
		}
		
		//Draw a bigger rectangle where our selection is
		CNFGColor(*currentColor);
		CNFGTackRectangle(selectedSquareX * scale + (scale - centerSize*2) / 2, selectedSquareY * scale + (scale - centerSize*2) / 2, selectedSquareX * scale + (scale - centerSize*2) / 2 + centerSize*2, selectedSquareY * scale + (scale - centerSize*2) / 2 + centerSize*2);

		//CNFGTackPixel(50, 50);
		
		/*unsigned char* point;
		for (point = lines[0]; (*point); point++)
		{
			printf("%c\n", point);
		}*/

		//draw the bottom text grid
		int yOff = 20 + (gridH)*scale ;
		makeText(10,yOff,2);
		CNFGPenX = 10+16*16;
		CNFGPenY = yOff;
		char characterIndex[32];
		sprintf(characterIndex,"Character code: %d\nCharacter: %c\n", selectedChar,selectedChar);
		CNFGDrawText(characterIndex,5);
		yOff += 10+16 * 8 * 2;
		//draw the bottom test text
		DrawTestText(10, yOff, 3);

		//swap the buffer
		CNFGSwapBuffers();
	}
}
