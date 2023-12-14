#ifndef UTILS_H
#define UTILS_H

//9 fatores incluindo o bias
typedef struct _fatores{
    double bias;
    double pos_P1;
    double pos_P2;
    double ball_x;
    double ball_y;
    double ball_speed_x;
    double ball_speed_y;
    double ball_speed_abs;
    double p_height;
}Fatores;

double getCurrentTimeInSeconds();

#endif