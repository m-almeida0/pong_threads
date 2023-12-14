#include "ball.h"
#include <tgmath.h>
#include "player.h"
#include "util.h"

int detect_collision(double*pos, double*new_v, Player player_1, Player player_2, int*point){
    int c_player = 0, c_wall = 0;
    if(pos[0] >= C_TRESHOLD_X){
        if((pos[1] >= player_1.y && pos[1] <= player_1.y+P_HEIGHT/2) || (pos[1] <= player_1.y && pos[1] >= player_1.y-P_HEIGHT/2)){
            c_player = 1;
            pos[0] = C_TRESHOLD_X;
            double delta_x = X_BIAS*(pos[0]-player_1.x), delta_y = pos[1]-player_1.y, dist;
            dist = sqrt(delta_x*delta_x+delta_y*delta_y);
            new_v[0] = delta_x/dist;
            new_v[1] = delta_y/dist;
        }else{
            if(pos[0] >= MAX_X_BALL){
                pos[0] = MAX_X_BALL;
                *point = -1;
                return -1;
            }
        }
    }else
    if(pos[0] <= -1*C_TRESHOLD_X){
        if((pos[1] >= player_2.y && pos[1] <= player_2.y+P_HEIGHT/2) || (pos[1] <= player_2.y && pos[1] >= player_2.y-P_HEIGHT/2)){
            c_player = 1;
            pos[0] = -1*C_TRESHOLD_X;
            double delta_x = X_BIAS*(pos[0]-player_2.x), delta_y = pos[1]-player_2.y, dist;
            dist = sqrt(delta_x*delta_x+delta_y*delta_y);
            new_v[0] = delta_x/dist;
            new_v[1] = delta_y/dist;
        }else{
            if(pos[0] <= MIN_X_BALL){
                pos[0] = MIN_X_BALL;
                *point = 1;
                return -1;
            }
        }
    }
    if(pos[1] >= MAX_Y_BALL || pos[1] <= MIN_Y_BALL){
        c_wall = 2;
    }
    return c_player + c_wall;
}

void move_ball(Ball*b, Player player_1, Player player_2, int*point, bool*reset, double*score_time){
    if(*reset){
        double curr_time = getCurrentTimeInSeconds();
        if(curr_time-*score_time < DELAY_RESET)
            return;
        *reset = false;
    }
    double new_pos[2], new_v[2];
    new_pos[0] = b->x+b->speed[0]*b->s_abs;
    new_pos[1] = b->y+b->speed[1]*b->s_abs;
    int collision = detect_collision(new_pos, new_v, player_1, player_2, point);
    switch (collision)
    {
    case -1:
        b->speed[0] = -1*(*point);
        b->speed[1] = 0;
        b->x = 0;
        b->y = 0;
        *reset = true;
        *score_time = getCurrentTimeInSeconds();
        return;
    case 1:
        b->speed[0] = new_v[0];
        b->speed[1] = new_v[1];
        break;
    case 2:
        b->speed[1] = -1*b->speed[1];
        break;
    case 3:
        b->speed[0] = -1*b->speed[0];
        b->speed[1] = -1*b->speed[1];
    }
    b->x = new_pos[0];
    b->y = new_pos[1];
}