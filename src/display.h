#pragma once
#include <Wire.h>
#include <U8g2lib.h>

void display_init();
void display_clear();
void display_render();
void display_draw_border();
void display_draw_box(int x, int y, int w, int h);
void display_draw_frame(int x, int y, int w, int h);
void display_draw_pixel(int x, int y);
void display_draw_text(int x, int y, const char *msg);
void display_draw_score(int score);
void display_draw_waiting();
void display_draw_gameover(int score);
void display_show_ip(const char *ip);
void display_invert_box(int x, int y, int w, int h);