#ifndef BALL_H
#define BALL_H

#include "player.h"
#define BALL_LEN 0.04
#define C_TRESHOLD_X 0.92//1-P_WIDTH-BALL_LEN/2
#define MAX_X_BALL 1-BALL_LEN/2
//eu sei que 3 desses defines sao redundantes. Mantendo eles por legibilidade
#define MIN_X_BALL -1+BALL_LEN/2
#define MAX_Y_BALL 1-BALL_LEN/2
#define MIN_Y_BALL -1+BALL_LEN/2
#define X_BIAS 1.8
#define DELAY_RESET 1
typedef struct _ball{
    double x;
    double y;
    double len;
    double s_abs;
    double speed[2];
}Ball;

int detect_collision(double*pos, double*new_v, Player player_1, Player player_2, int*point);
void move_ball(Ball*b, Player player_1, Player player_2, int*point, bool*reset, double*score_time);

#endif