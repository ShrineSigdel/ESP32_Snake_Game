#include "display.h"

static U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void display_init()
{
    Wire.begin(21, 22);
    u8g2.begin();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void display_clear()
{
    u8g2.clearBuffer();
}

void display_render()
{
    u8g2.sendBuffer();
}

void display_draw_border()
{
    u8g2.setDrawColor(1);
    u8g2.drawFrame(0, 0, 128, 64);
}

void display_draw_box(int x, int y, int w, int h)
{
    u8g2.setDrawColor(1);
    u8g2.drawBox(x, y, w, h);
}

void display_draw_frame(int x, int y, int w, int h)
{
    u8g2.setDrawColor(1);
    u8g2.drawFrame(x, y, w, h);
}

void display_draw_pixel(int x, int y)
{
    u8g2.setDrawColor(1);
    u8g2.drawPixel(x, y);
}

void display_draw_text(int x, int y, const char *msg)
{
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setDrawColor(1);
    u8g2.drawStr(x, y, msg);
}

void display_draw_score(int score)
{
    char buf[8];
    sprintf(buf, "%d", score);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setDrawColor(1);
    u8g2.drawStr(90, 10, buf);
}

void display_draw_waiting()
{
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setDrawColor(1);
    u8g2.drawStr(16, 28, "press any key");
    u8g2.drawStr(28, 42, "to start");
}

void display_draw_gameover(int score)
{
    // filled box background
    u8g2.setDrawColor(1);
    u8g2.drawBox(24, 20, 80, 28);
    // text in black over it
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(30, 32, "GAME OVER");
    char buf[16];
    sprintf(buf, "score: %d", score);
    u8g2.drawStr(30, 44, buf);
    u8g2.setDrawColor(1);
}

void display_show_ip(const char *ip)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setDrawColor(1);
    u8g2.drawStr(0, 10, "Connected!");
    u8g2.drawStr(0, 26, "Open browser:");
    u8g2.drawStr(0, 42, ip);
    u8g2.sendBuffer();
}

// draws a filled box in black — used to erase a region
void display_invert_box(int x, int y, int w, int h)
{
    u8g2.setDrawColor(0);
    u8g2.drawBox(x, y, w, h);
    u8g2.setDrawColor(1);
}