#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#define clear() printf("\033[2J")
#define gotopos(x,y) printf("\033[%d;%dH", (y), (x))
#define textattr(attr) printf("\x1b[%dm", (attr))

float rawxpos, rawypos, touchx, touchy;
int xpos, ypos;

int RESET = 0;
int FG_BLACK = 30;
int BG_BLACK = 40;
int FG_RED = 31;
int BG_RED = 41;
int FG_GREEN = 32;
int BG_GREEN = 42;
int FG_YELLOW = 33;
int BG_YELLOW = 43;
int FG_BLUE = 34;
int BG_BLUE = 44;
int FG_MAGENTA = 35;
int BG_MAGENTA = 45;
int FG_CYAN = 36;
int BG_CYAN = 46;
int FG_WHITE = 37;
int BG_WHITE = 47;

int cookies = 0;
int upperclick = 100;
int uppersecond = 200;
int addcookies = 1;
int cookiespersec = 0;
int selector = 1;
int shopselector = 0;
int shop = 0;
int frames = 0;

int main()
{	
	gfxInitDefault();
	//gfxSet3D(true); // uncomment if using stereoscopic 3D
	
	PrintConsole topScreen;
	PrintConsole bottomScreen;
	
	consoleInit(GFX_BOTTOM, &bottomScreen);
	consoleInit(GFX_TOP, &topScreen);
	
	// Options menu
	consoleInit(GFX_BOTTOM, NULL); // Selecting the bottom screen
	textattr(FG_CYAN);
	printf(
		"Instructions:\n"
		"\n"
		"Use the left and rigth keys on the D-Padto move the cursor above around!\n"
		"\n"
		"Press A to select an option\n"
		"\n"
		"And have fun!"
	
	);
	gotopos(10,9);
	printf("     ÛÛÛÛÛÛÛÛÛÛ");
	gotopos(10,10);
	printf("     ÛÛÛÛÛÛÛÛÛÛ");
	gotopos(10,11);
	printf("   ÛÛ    Û     ÛÛ");
	gotopos(10,12);
	printf("   ÛÛ    Û     ÛÛ");
	gotopos(10,13);
	printf(" ÛÛ      ÛÛ      ÛÛ");
	gotopos(10,14);
	printf(" ÛÛ      ÛÛ      ÛÛ");
	gotopos(10,15);
	printf(" Û   Û     Û      Û");
	gotopos(10,16);
	printf(" Û   Û     Û      Û");
	gotopos(10,17);
	printf("ÛÛ   ÛÛ   ÛÛÛ     ÛÛ");
	gotopos(10,18);
	printf("ÛÛ   ÛÛ   ÛÛÛ     ÛÛ");
	gotopos(10,19);
	printf("ÛÛ   Û         ÛÛ ÛÛ");
	gotopos(10,20);
	printf("ÛÛ   Û         ÛÛ ÛÛ");
	gotopos(10,21);
	printf(" Û       ÛÛÛ    ÛÛÛ");
	gotopos(10,22);
	printf(" Û       ÛÛÛ    ÛÛÛ");
	gotopos(10,23);
	printf(" ÛÛ     ÛÛÛ      ÛÛ");
	gotopos(10,24);
	printf(" ÛÛ     ÛÛÛ      ÛÛ");
	gotopos(10,25);
	printf("   ÛÛ          ÛÛ");
	gotopos(10,26);
	printf("   ÛÛ          ÛÛ");
	gotopos(10,27);
	printf("     ÛÛÛÛÛÛÛÛÛÛ");
	gotopos(10,28);
	printf("     ÛÛÛÛÛÛÛÛÛÛ");

	textattr(RESET);
	consoleInit(GFX_TOP, NULL); // Selecting the top screen
	
	// Main loop
	while (aptMainLoop())
	{
		frames++;

		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		
		touchPosition touch;
		
		hidTouchRead(&touch);
		touchx = touch.px;
		touchy = touch.py;
		
		rawxpos = ((touchx) / 320) * 40;
		rawypos = ((touchy) / 240) * 30;
		xpos = (int)rawxpos;
		ypos = (int)rawypos;
		
		// Ouf of shop display
		if(shop == 0){
			shopselector = 0;
			textattr(FG_YELLOW);
			gotopos(9,0);
			printf("0                                       ");
			gotopos(0,2);
			printf("                                                 ");
			gotopos(0,4);
			printf("                                                 ");
			gotopos(0,6);
			printf("                                                 ");
			gotopos(0,8);
			printf("                                                 ");
			gotopos(0,0);
			printf("Cookies: %i", cookies);
			textattr(RESET);
		}
		
		// Inside shop display
		if(shop == 1){
			textattr(FG_CYAN);
			gotopos(29,0);
			printf("0                   ");
			gotopos(1,0);
			printf(" Upgrade cookies per click: %i", upperclick);
			textattr(FG_GREEN);
			gotopos(30,2);
			printf("0                  ");
			gotopos(1,2);
			printf(" Upgrade cookies per second: %i", uppersecond);
			textattr(FG_YELLOW);
			gotopos(27,4);
			printf("0                     ");
			gotopos(0,4);
			printf("Current cookies per click: %i", addcookies);
			gotopos(28,6);
			printf("0                    ");
			gotopos(0,6);
			printf("Current cookies per second: %i", cookiespersec);
			gotopos(9,8);
			printf("0                                       ");
			gotopos(0,8);
			printf("Cookies: %i", cookies);
			textattr(RESET);
			
		}
		
		// Add cookies per second
		if(frames % 60 == 0)
			cookies += cookiespersec;
		
		// Out of shop selections
		if(kDown & KEY_A & (selector == 0) & (shop == 0))
			shop = 2;
		if(kDown & KEY_TOUCH)
			if((xpos >= 10) & (xpos <= 30) & (ypos >= 9) & (ypos <= 28) & (shop == 0))
				cookies += addcookies;
		if(kDown & KEY_A & (selector == 2) & (shop == 0))
			break; // Break to return to HBL
		// Inside shop selections
		if(kDown & KEY_A & (selector == 0) & (shop == 1))
			shop = 0;
		if(kDown & KEY_A & (selector == 1) & (shop == 1)){
			if((shopselector == 0) & (cookies >= upperclick)){
				cookies -= upperclick;
				addcookies *= 2;
				upperclick += (upperclick / 2);
			}
			if((shopselector == 1) & (cookies >= uppersecond)){
				cookies -= uppersecond;
				cookiespersec += 1;
				cookiespersec *= 2;
				uppersecond += (uppersecond / 2);
			}
		}
		if((selector == 2) & (shop == 1)){
			if(kDown & KEY_DUP)
				shopselector--;
			if(kDown & KEY_DDOWN)
				shopselector++;
		}
		
		if(shopselector > 1)
			shopselector = 1;
		if(shopselector < 0)
			shopselector = 0;
		
		if(shop == 1){
			if(shopselector == 0){
				gotopos(0,0);
				printf(">");
				gotopos(0,2);
				printf(" ");
			}
			if(shopselector == 1){
				gotopos(0,0);
				printf(" ");
				gotopos(0,2);
				printf(">");
			}
		}
		
		// Prevent player from instantly returning from shop
		if(shop == 2)
			shop = 1;
		
		// Update selector position
		if(kDown & KEY_DLEFT)
			selector--;
		if(kDown & KEY_DRIGHT)
			selector++;
		if(selector > 2)
			selector = 0;
		if(selector < 0)
			selector = 2;
		
		// Selector
		if(selector == 0){
			gotopos(8,23);
			printf("\\/");
			gotopos(24,23);
			printf("  ");
			gotopos(40,23);
			printf("  ");
		}
		if(selector == 1){
			gotopos(8,23);
			printf("  ");
			gotopos(24,23);
			printf("\\/");
			gotopos(40,23);
			printf("  ");
		}
		if(selector == 2){
			gotopos(8,23);
			printf("  ");
			gotopos(24,23);
			printf("  ");
			gotopos(40,23);
			printf("\\/");
		}
		
		// Buttons (out of shop)
		if(shop == 0){
			gotopos(2,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(2,25);
			printf("º            º");
			gotopos(2,26);
			printf("º    Shop    º");
			gotopos(2,27);
			printf("º            º");
			gotopos(2,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
			gotopos(18,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(18,25);
			printf("º            º");
			gotopos(18,26);
			printf("º  Cookies!  º");
			gotopos(18,27);
			printf("º            º");
			gotopos(18,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
			gotopos(34,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(34,25);
			printf("º            º");
			gotopos(34,26);
			printf("º    Exit    º");
			gotopos(34,27);
			printf("º            º");
			gotopos(34,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
		}
		
		// Buttons (in shop)
		if(shop == 1){
			gotopos(2,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(2,25);
			printf("º            º");
			gotopos(2,26);
			printf("º   Return   º");
			gotopos(2,27);
			printf("º            º");
			gotopos(2,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
			if(shopselector == 0){
				if(cookies < upperclick)
					textattr(FG_RED);
			}
			if(shopselector == 1){
				if(cookies < uppersecond)
					textattr(FG_RED);
			}
			gotopos(18,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(18,25);
			printf("º            º");
			gotopos(18,26);
			printf("º  Upgrade!  º");
			gotopos(18,27);
			printf("º            º");
			gotopos(18,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
			textattr(RESET);
			gotopos(34,24);
			printf("ÉÍÍÍÍÍÍÍÍÍÍÍÍ»");
			gotopos(34,25);
			printf("º            º");
			gotopos(34,26);
			printf("º   Select   º");
			gotopos(34,27);
			printf("º            º");
			gotopos(34,28);
			printf("ÈÍÍÍÍÍÍÍÍÍÍÍÍ¼");
		}
		
		
		
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		
		
		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}

int string_length(char s[]) {
   int c = 0;
 
   while (s[c] != '\0')
      c++;
 
   return c;
}
