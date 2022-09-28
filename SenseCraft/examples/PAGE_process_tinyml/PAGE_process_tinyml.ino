#include <TFT_eSPI.h>
#include "Free_Fonts.h"

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

#define SCREEN_WIDTH 320                        // Wio Terminal Maximum Width
#define SCREEN_HIGH 240                         // Wio Terminal Maximum Height
#define PIXEL 4                                 // Width of one letter
const static unsigned int FONT_ROW_HEIGHT = 22; // The height of a letter

void DISPLAY_INIT() // Display initialization, black background rotation
{
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
}

void Process_main(){
    spr.drawString("Vision AI real-time analysis", 24, 71, GFXFF);

    spr.drawString("TinyML Example", 24, 71, GFXFF);

    spr.drawString("Vision AI real-time analysis", 24, 71, GFXFF);
}
void Sense_Display() // Sense interface display
{
    spr.createSprite(SCREEN_WIDTH, SCREEN_HIGH);

    // put your main code here

    Process_main();
    
    spr.setFreeFont(FSSB9);
    spr.setTextColor(TFT_BLACK, TFT_WHITE);

    spr.fillRect(4 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, TFT_WHITE);
    spr.fillRect(30 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, TFT_WHITE);
    spr.fillRect(56 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, TFT_WHITE);

    spr.drawString("Sense", 32, 11, GFXFF);
    spr.drawString("Process", 127, 11, GFXFF);
    spr.drawString("Network", 231, 11, GFXFF);

    spr.drawLine(0, 2 * FONT_ROW_HEIGHT, SCREEN_WIDTH, 2 * FONT_ROW_HEIGHT, TFT_WHITE);

    spr.setFreeFont(FSS9);
    spr.setTextColor(TFT_BLACK, tft.color565(220, 220, 220));
    spr.fillRect(127, 53.5, 16 * PIXEL, FONT_ROW_HEIGHT, tft.color565(220, 220, 220));
    spr.drawString("Sensor", 130, 56, GFXFF);

    spr.pushSprite(0, 0);
    spr.deleteSprite();
}



void setup()
{
    DISPLAY_INIT();
}

void loop()
{
    Sense_Display();
}
