#pragma once

#define GRID_SIZE 4
#define GRID_W 32
#define GRID_H 16
#define MAX_LENGTH 64

enum GameState
{
    WAITING,
    PLAYING,
    DEAD
};

struct Point
{
    int x;
    int y;
};

void game_init();
void game_update();
void game_draw();
void game_reset();
void game_start();
bool game_is_over();
GameState game_get_state();
void game_set_direction(int dx, int dy);    