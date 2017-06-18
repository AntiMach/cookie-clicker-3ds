#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <math.h>
#include <sys\stat.h>
#include <time.h>

u64 cookies = 0;
int opt = 1;
int scr = 1;
const char* buildingNames[9] = {"Clicker", "Grandma", "Farm", "Mine", "???", "Bank", "Temple", "Wizard", "Ship"};
const u64  buildingPrices[9] = {150, 500, 2200, 18000, 130000, 1400000, 20000000, 330000000, 5100000000};
const u64 cookieMultiplier[9] = {1, 5, 16, 75, 260, 1400, 7800, 44000, 260000};
u64 buildingData[9] = {};
u32 buildings = 9;
u64 clickerMultiplier = 0;
u64 totalCps = 0;
u8 autoSaved = 0;


u64 frames = 0;

void selectOption(int *cursor, int keyS, int keyA, int min, int max){
	u32 altKeyDown = hidKeysDown();
	if(altKeyDown & keyS){
		*cursor -= 1;
	}
	if(altKeyDown & keyA){
		*cursor += 1;
	}
	
	if(*cursor > max){
		*cursor = min;
	}
	if(*cursor < min){
		*cursor = max;
	}
	return;
}

void printOption(int cursor, int option, char text[],int notSelected, int selected, int x, int y){
	if(cursor == option){
		printf("\e[%d;%dH\e[%d;1m%s\e[K\e[0m", y, x, selected, text);
	} else {
		printf("\e[%d;%dH\e[%d;1m%s\e[K\e[0m", y, x, notSelected, text);
	}
	return;
}

void changeScreen(int *scr, int toChange, int *cursor, PrintConsole top, PrintConsole bottom){
	consoleSelect(&top);
	consoleClear();
	consoleSelect(&bottom);
	consoleClear();
	*scr = toChange;
	*cursor = 1;
	return;
}

void resetData(){
	memset(buildingData, 0, sizeof(buildingData));
	cookies = 0;
	totalCps = 0;
	clickerMultiplier = 0;
	return;
}

void cookiesPerSecond(){
	totalCps = 0;
	for (int i = 0; i < buildings; i++)
		totalCps += buildingData[i] * cookieMultiplier[i];
}

void buyBuilding(int cursor, int buildingNum, int notSelected, int affordable, int notAffordable, int x, int y){
	if(cursor == buildingNum + 1){
		if(cookies >= (u64)(buildingPrices[buildingNum] * pow(1.2, buildingData[buildingNum]))){
			printf("\e[%d;%dH\e[%d;1mBuy %s\e[K\e[0m", y, x, affordable, buildingNames[buildingNum]);
			
			u32 altKeyDown = hidKeysDown(); 
			if (altKeyDown & KEY_A) {
				cookies -= (u64)(buildingPrices[buildingNum] * pow(1.2, buildingData[buildingNum]));
				buildingData[buildingNum]++;
			}
		}
		else
			printf("\e[%d;%dH\e[%d;1mBuy %s\e[K\e[0m", y, x, notAffordable, buildingNames[buildingNum]);
	} else {
		printf("\e[%d;%dH\e[%d;1mBuy %s\e[K\e[0m", y, x, notSelected, buildingNames[buildingNum]);
	}
	return;
}

u64 buildingPrice(int buildingNum){
	return (u64)(buildingPrices[buildingNum] * pow(1.2, buildingData[buildingNum]));
}

void saveData(char* filepath){
	FILE *file = fopen(filepath, "r+");
	
	if (!file)
		file = fopen(filepath, "w+");
	
	time_t curTime;
	
	time(&curTime);
	
	fwrite(&cookies, sizeof(u64), 1, file);
	fwrite(buildingData, sizeof(u64), buildings, file);
	fwrite(&clickerMultiplier, sizeof(u64), 1, file);
	fwrite(&curTime, sizeof(time_t), 1, file);
	fclose(file);
}

void loadData(char* filepath){
	FILE *file = fopen(filepath, "r+");
	
	if (!file)
		file = fopen(filepath, "w+");
	
	time_t lastTime;
	time_t curTime;
	
	time(&curTime);
	
	fread(&cookies, sizeof(u64), 1, file);
	fread(buildingData, sizeof(u64), buildings, file);
	fread(&clickerMultiplier, sizeof(u64), 1, file);
	fread(&lastTime, sizeof(u64), 1, file);
	fclose(file);
	
	if(lastTime != 0){
		cookiesPerSecond();
		cookies += totalCps * (curTime - lastTime);
	}
}

int main(int argc, char **argv) {
	gfxInitDefault();
	
	PrintConsole topScreen;
	PrintConsole bottomScreen;
	
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	mkdir("/3ds/", 0777);
	mkdir("/3ds/data", 0777);
	mkdir("/3ds/data/CookieClicker3DS", 0777);
	
	loadData("/3ds/data/CookieClicker3DS/userData.bin");

	// Main loop
	while (aptMainLoop()) {

		gspWaitForVBlank();
		hidScanInput();
		u32 kDown = hidKeysDown();
	
		// Top screen
		consoleSelect(&topScreen);
		
		printf("\e[1;1HCookies: %llu\e[K", cookies);
		
		cookiesPerSecond();
		
		if (scr == 1) {
			printf("\e[3;1HMain Menu\e[K");
			for (int i = 0; i < buildings; i++){
				printf("\e[%d;1H%ss: %llu\e[K", 5 + i, buildingNames[i], buildingData[i]);
			}
			printf("\e[%lu;1HCookies Per Click: %llu", 7 + buildings, (u64)pow(2, clickerMultiplier));
			printf("\e[%lu;1HCookies Per Second: %llu", 8 + buildings, totalCps);
		} else if (scr == 2) {
			printf("\e[3;1HShop\e[K");
			for (int i = 0; i < buildings; i++){
				printf("\e[%d;1H%s price: %llu \e[%d;30HAmount: %llu\e[K", 5 + i, buildingNames[i], buildingPrice(i), 5 + i, buildingData[i]);
			}
			printf("\e[%lu;1HClicker upgrade price: %llu\e[K", 7 + buildings, (u64)(500 * pow(3, clickerMultiplier)));
		} else if (scr == 3) {
			printf("\e[3;1HOptions\e[K");
		} else if (scr == 4) {
			printf("\e[3;1HAre you sure you want to reset?\e[K");
		}
		
		// Bottom screen & statements
		consoleSelect(&bottomScreen);
		
		if (scr == 1) {
			selectOption(&opt, KEY_UP, KEY_DOWN, 1, 4);
		
			printOption(opt, 1, "Cookies!", 37, 32, 1, 1);
			printOption(opt, 2, "Shop", 37, 32, 1, 2);
			printOption(opt, 3, "Options", 37, 32, 1, 3);
			printOption(opt, 4, "Exit", 37, 31, 1, 4);
			if (autoSaved > 0)
				printf("\e[6;1HAutosaved!");
			else
				printf("\e[6;1H\e[K");
			
			if(kDown & KEY_A & (opt == 1)){
				cookies += (u64)pow(2, clickerMultiplier);
			}
			if(kDown & KEY_A & (opt == 2)){
				changeScreen(&scr, 2, &opt, topScreen, bottomScreen);
			}
			if(kDown & KEY_A & (opt == 3)){
				changeScreen(&scr, 3, &opt, topScreen, bottomScreen);
			}
			if(kDown & KEY_A & (opt == 4)){
				break;
			}
		} else if (scr == 2) {
			selectOption(&opt, KEY_UP, KEY_DOWN, 1, 2 + buildings);
			
			for (int i = 0; i < buildings; i++){
				buyBuilding(opt, i, 37, 32, 31, 1, 1 + i);
			}
			if (cookies >= (u64)(500 * pow(3, clickerMultiplier)))
				printOption(opt, 1 + buildings, "Upgrade clicker", 37, 32, 1, 2 + buildings);
			else
				printOption(opt, 1 + buildings, "Upgrade clicker", 37, 31, 1, 2 + buildings);
			printOption(opt, 2 + buildings, "Return", 37, 32, 1, 4 + buildings);
			
			if (autoSaved > 0)
				printf("\e[%lu;1HAutosaved!", 6 + buildings);
			else
				printf("\e[%lu;1H\e[K", 6 + buildings);
			
			if(kDown & KEY_A & (opt == 1 + buildings)){
				if(cookies >= (u64)(500 * pow(3, clickerMultiplier))){
					cookies -= (u64)(500 * pow(3, clickerMultiplier));
					clickerMultiplier++;
				}
					
			}
			if(kDown & KEY_A & (opt == 2 + buildings)){
				changeScreen(&scr, 1, &opt, topScreen, bottomScreen);
			}
		} else if (scr == 3) {
			selectOption(&opt, KEY_UP, KEY_DOWN, 1, 3);
			
			printOption(opt, 1, "Save game", 37, 34, 1, 1);
			printOption(opt, 2, "Reset game", 37, 31, 1, 2);
			printOption(opt, 3, "Return", 37, 32, 1, 3);
			
			if (autoSaved > 0)
				printf("\e[5;1HAutosaved!");
			else
				printf("\e[5;1H\e[K");
			
			if(kDown & KEY_A & (opt == 1)){
				saveData("/3ds/data/CookieClicker3DS/userData.bin");
			}
			if(kDown & KEY_A & (opt == 2)){
				changeScreen(&scr, 4, &opt, topScreen, bottomScreen);
			}
			if(kDown & KEY_A & (opt == 3)){
				changeScreen(&scr, 1, &opt, topScreen, bottomScreen);
			}
		} else if (scr == 4) {
			selectOption(&opt, KEY_UP, KEY_DOWN, 1, 2);
			
			printOption(opt, 1, "Yes", 37, 31, 1, 1);
			printOption(opt, 2, "No", 37, 32, 1, 2);
			if(kDown & KEY_A & (opt == 1)){
				resetData();
				changeScreen(&scr, 1, &opt, topScreen, bottomScreen);
			}
			if(kDown & KEY_A & (opt == 2)){
				changeScreen(&scr, 3, &opt, topScreen, bottomScreen);
			}
		}
		
		if (scr != 4){
			frames++;
			
			if (frames % 60 == 0){
				cookies += totalCps; // Add n cookies per building every second
			}
			
			if (frames % 1200 == 0){
				saveData("/3ds/data/CookieClicker3DS/userData.bin"); // Add n cookies per building every second
				autoSaved = 180;
			}
			if (autoSaved > 0){
				autoSaved--;
			}
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}