#include"player.h"
#include"util.h"

void move_player(Player* p, int dir){
    p->y+=dir*P_HEIGHT/4;
    if(p->y > MAX_P_Y)
        p->y = MAX_P_Y;
    if(p->y < MIN_P_Y)
        p->y = MIN_P_Y;
}

void move_PC(Player* p, int dir, double *last_PC_move){
    double curr_time = getCurrentTimeInSeconds();
    if(curr_time - (*last_PC_move) < DELAY_PC)
        return;
    *last_PC_move = curr_time;
    p->y+=dir*P_HEIGHT/10;
    if(p->y > MAX_P_Y)
        p->y = MAX_P_Y;
    if(p->y < MIN_P_Y)
        p->y = MIN_P_Y;
}

void benchmark(Fatores fatores, Player*player, double *last_PC_move){
    int dir = 0;
    if(fatores.pos_P2 < fatores.ball_y)
        dir = UP;
    if(fatores.pos_P2 > fatores.ball_y)
        dir = DOWN;
    move_PC(player, dir, last_PC_move);
}

void update_score(Player *p1, Player *p2, int*point){
    switch (*point)
    {
    case 1:
        p1->points+=1;
        *point = 0;
        break;
    case -1:
        p2->points+=1;
        *point = 0;
        break;
    default:
        *point = 0;
        break;
    }
}
