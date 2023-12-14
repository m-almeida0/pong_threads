#ifndef PLAYER_H
#define PLAYER_H

#include "util.h"
#define P_WIDTH 0.06
#define P_HEIGHT 0.4
#define XP_1 0.97//1-P_WIDTH/2
#define XP_2 -0.97//-1+P_WIDTH/2
#define MAX_P_Y 1 - P_HEIGHT/2
#define MIN_P_Y -1 + P_HEIGHT/2
#define UP 1
#define DOWN -1
#define DELAY_PC 0.08

typedef struct _player{
    double width;
    double height;
    double x;
    double y;
    int points;
}Player;


void move_player(Player* p, int dir);
void move_PC(Player* p, int dir, double *last_PC_move);
void benchmark(Fatores fatores, Player*player, double *last_PC_move);
void update_score(Player *p1, Player *p2, int*point);

#endif