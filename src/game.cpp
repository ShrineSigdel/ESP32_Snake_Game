#include "game.h"
#include "display.h"
#include "wifi_server.h"

static Point snake[MAX_LENGTH];
static int snakeLen = 3;
static int dirX = 1;
static int dirY = 0;
static int nextDirX = 1;
static int nextDirY = 0;
static Point food;
static int score = 0;
static GameState state = WAITING;

static void place_food()
{
    bool onSnake;
    do
    {
        onSnake = false;
        food.x = random(0, GRID_W);
        food.y = random(0, GRID_H);
        for (int i = 0; i < snakeLen; i++)
        {
            if (snake[i].x == food.x && snake[i].y == food.y)
            {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void game_init()
{
    randomSeed(analogRead(0));
    game_reset();
}

void game_reset()
{
    snakeLen = 3;
    dirX = 1;
    dirY = 0;
    nextDirX = 1;
    nextDirY = 0;
    score = 0;
    state = WAITING;

    snake[0] = {GRID_W / 2, GRID_H / 2};
    snake[1] = {GRID_W / 2 - 1, GRID_H / 2};
    snake[2] = {GRID_W / 2 - 2, GRID_H / 2};

    place_food();
}

void game_start()
{
    if (state == WAITING)
        state = PLAYING;
}

bool game_is_over()
{
    return state == DEAD;
}

GameState game_get_state()
{
    return state;
}

void game_set_direction(int dx, int dy)
{
    if (dx == -dirX && dy == -dirY)
        return;
    nextDirX = dx;
    nextDirY = dy;
}

void game_update()
{
    if (state != PLAYING)
        return;

    dirX = nextDirX;
    dirY = nextDirY;

    Point newHead = {snake[0].x + dirX, snake[0].y + dirY};

    // wall collision
    if (newHead.x < 0 || newHead.x >= GRID_W ||
        newHead.y < 0 || newHead.y >= GRID_H)
    {
        state = DEAD;
        ws_broadcast("gameover");
        return;
    }

    // self collision
    for (int i = 0; i < snakeLen - 1; i++)
    {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y)
        {
            state = DEAD;
            ws_broadcast("gameover");
            return;
        }
    }

    bool ate = (newHead.x == food.x && newHead.y == food.y);
    if (ate && snakeLen < MAX_LENGTH)
        snakeLen++;

    for (int i = snakeLen - 1; i > 0; i--)
        snake[i] = snake[i - 1];
    snake[0] = newHead;

    if (ate)
    {
        score++;
        place_food();
        ws_broadcast("score");
    }
}

void game_draw()
{
    display_clear();
    display_draw_border();

    if (state == WAITING)
    {
        display_draw_waiting();
        display_render();
        return;
    }

    // food
    display_draw_box(
        food.x * GRID_SIZE + 1,
        food.y * GRID_SIZE + 1,
        GRID_SIZE - 2,
        GRID_SIZE - 2);

    // snake — head filled, body outlined
    for (int i = 0; i < snakeLen; i++)
    {
        int px = snake[i].x * GRID_SIZE;
        int py = snake[i].y * GRID_SIZE;
        if (i == 0)
            display_draw_box(px, py, GRID_SIZE, GRID_SIZE);
        else
            display_draw_frame(px, py, GRID_SIZE, GRID_SIZE);
    }

    display_draw_score(score);

    if (state == DEAD)
        display_draw_gameover(score);

    display_render();
}