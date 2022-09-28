#include <TFT_eSPI.h>
#include "Free_Fonts.h"

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

#define SCREEN_WIDTH 320                        // Wio Terminal Maximum Width
#define SCREEN_HIGH 240                         // Wio Terminal Maximum Height
#define PIXEL 4                                 // Width of one letter
#define MOVE_PIXEL_y 20                         // Virtual to Reality Moving Distance
const static unsigned int FONT_ROW_HEIGHT = 22; // The height of a letter
#include "RTC_SAMD51.h"
#include "DateTime.h"
#include <qrcode.h>

RTC_SAMD51 rtc;

DateTime now = DateTime(F(__DATE__), F(__TIME__));

void DISPLAY_INIT() // Display initialization, black background rotation
{
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
}

void Process_main(int page)
{
    //此处加入 page 0;  page3 为了方便测试 可以调用此函数 也可以独立调用 
    //case 0   Vision AI log 
    //case 1  Proccsee首页
    //case 2  TinyML示例 
    //case 3  TinyML已进入
    switch (page)
    {
    case 0:
        Vision_AI_real_time_analysis();
        return;
    case 1:
        spr.setTextColor(TFT_WHITE);
        spr.drawString("Vision AI real-time analysis", 63, 33 + MOVE_PIXEL_y, GFXFF);
        spr.fillRect(24, 71 + MOVE_PIXEL_y - 3, 221, FONT_ROW_HEIGHT, tft.color565(0, 204, 2));
        break;
    case 2:
        spr.setTextColor(TFT_WHITE);
        spr.drawString("TinyML Example", 107, 32 + MOVE_PIXEL_y, GFXFF);
        spr.fillRect(24, 100 + MOVE_PIXEL_y - 3, 135, FONT_ROW_HEIGHT, tft.color565(0, 204, 2));
        break;
    case 3:
        Process_TinyML_ENTER();
        return;
    default:;
    }
    spr.setFreeFont(FSS9);
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Vision AI real-time analysis", 24, 71 + MOVE_PIXEL_y, GFXFF);
    spr.drawString("TinyML Example", 24, 100 + MOVE_PIXEL_y, GFXFF);

    spr.setTextColor(tft.color565(169, 169, 169));
    spr.drawString("Data Filter(In Development)", 24, 129 + MOVE_PIXEL_y, GFXFF);
}

void Network_state(int s_key)
{
    switch (s_key)
    {
    case 0:
        spr.setTextColor(TFT_RED);
        spr.drawString("OFF", 80, 215, GFXFF);
        break;
    case 1:
        spr.setTextColor(TFT_GREEN);
        spr.drawString("ON", 80, 215, GFXFF);
        break;
    default:;
    }
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Network:", 7, 215, GFXFF);
}

void Process_TinyML_ENTER(void)
{
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Please scan the QR ", 135, 86, GFXFF);
    spr.drawString("code on the screen ", 135, 106, GFXFF);
    spr.drawString("to view the Github", 135, 126, GFXFF);
    spr.drawString("sample tutorial ", 135, 146, GFXFF);
    double PIXELL = 3; 

    spr.fillRect(13, 70, 115, 115, TFT_WHITE);
    QRCode qrcode;
    uint8_t *qrcodeData = (uint8_t *)malloc(qrcode_getBufferSize(5));
    qrcode_initText(&qrcode, qrcodeData, 5, 0, "https://wiki.seeedstudio.com/K1100-Getting-Started/#tinyml-section");
    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        // Each horizontal module
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
                spr.fillRect(x * PIXELL + 15, y * PIXELL + 70 + 2, PIXELL, PIXELL, TFT_BLACK);
        }
    }

    free(qrcodeData);
}

int i = 0;
void Vision_AI_real_time_analysis(void) // todo
{
    spr.setTextColor(TFT_WHITE);
    spr.drawString("Vision AI real-time analysis", 64, 52, GFXFF);
    spr.fillRect(40, 73, 216 + 20, 117 + 15, tft.color565(128, 128, 128));
    DateTime now = rtc.now();
    //  Serial.print(now.second(), DEC);
    spr.drawString("Time", 52, 80, GFXFF);
    spr.drawString("Data", 140, 80, GFXFF);

    char buf_T[4][20];

    uint8_t HH = now.hour();
    uint8_t MM = now.minute();
    uint8_t SS = now.second();

    int kk = i % 4;
    sprintf(buf_T[i % 4], "%02d:%02d:%02d ", HH, MM, SS);
    i++;
    if (i == 4)
    {
        i = 0;
    }

    for (int i = 0; i < 4; i++)
    {
        int MM_add = 0;
        if (SS + i > 59)
        {
            MM += 1;
            SS -= 60;
        }
        if (MM > 59)
        {
            HH += 1;
            MM -= 60;
        }
        sprintf(buf_T[i], " %02d:%02d:%02d ", HH, MM, SS + i);
    }
    for (int gg = 0; gg < 4; gg++)
    {
        spr.drawString(buf_T[gg], 45, 103 + gg * 26, GFXFF);
    }
}

void Process_Display(int G, int G_network) // Sense interface display
{
    spr.createSprite(SCREEN_WIDTH, SCREEN_HIGH);

    //  choose func  here
    Process_main(G);
    //    Process_TinyML_ENTER();
    //    Vision_AI_real_time_analysis();
    spr.setFreeFont(FSSB9);
    spr.setTextColor(TFT_BLACK, TFT_WHITE);

    spr.fillRect(4 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, TFT_WHITE);
    spr.fillRect(30 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, tft.color565(135, 206, 235));
    spr.fillRect(56 * PIXEL, 0, 21 * PIXEL, FONT_ROW_HEIGHT + 15, TFT_WHITE);

    spr.setFreeFont(FSS9);
    spr.setTextColor(TFT_BLACK);
    spr.drawString("Sense", 32, 11, GFXFF);
    spr.drawString("Process", 127, 11, GFXFF);
    spr.drawString("Network", 231, 11, GFXFF);

    spr.drawLine(0, 2 * FONT_ROW_HEIGHT, SCREEN_WIDTH, 2 * FONT_ROW_HEIGHT, TFT_WHITE);

    Network_state(G_network);

    spr.pushSprite(0, 0);
    spr.deleteSprite();
}

void setup()
{
    DISPLAY_INIT();
    rtc.begin();
    DateTime now = DateTime(F(__DATE__), F(__TIME__));
    Serial.println("adjust time!");
    rtc.adjust(now);

    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
}

int gg_switch = 0;
int gg_switch_flag = 0;
int gg_network_flag = 0;
void loop()
{
    // test block begin
    if (digitalRead(WIO_KEY_B) == LOW)
    {
        Serial.println("B Key pressed");
        gg_switch++;
        delay(200);
    }
    if (digitalRead(WIO_KEY_C) == LOW)
    {
        Serial.println("C Key pressed");
        gg_network_flag++;
        delay(200);
    }

    Process_Display(gg_switch % 4, gg_network_flag % 2);
}
