#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif
#include <iostream> 
#include "game/player.h"
#include "game/ball.h"
#include "game/util.h"
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h> 

//g++ teste.cpp -o teste -lglut -lGLU -lGL -Wall
/*TODO:
    Base - criar threads separadas pra mover o jogador, mover a IA, mover a bola, e um semáforo de sincronização para cada, e uma função que joga o jogo sem threads.
        Base.1 - mudar "fatores" para "last_known_values". As funções de cada thread para pegarem valores dessa struct, e não diretamente.
        Base.2 - implementar uma thread que atualiza os valores de last_known_values e desperta as outras. Cada thread executa uma vez e dorme, e só depois que
            as outras duas executaram que essa terceira vai atualizar os valores e decrementar os semáforos.
    Extra - implementar um neurônio artificial básico, uma rede neural básica usando eles, e a execução da rede neural linearmente
        Extra.1 - criar uma função que roda a rede neural com multithreading e que recebe o máximo de threads a serem passadas
        Extra.2 - criar uma função "delay_move" e mudar o delay pra benchmark, não pra move
    */

#define windowWidth 600
#define windowHeight 600

Player player_1 = {P_WIDTH, P_HEIGHT, XP_1, 0, 0};
Player player_2 = {P_WIDTH, P_HEIGHT, XP_2, 0, 0};

Ball ball = {0, 0, BALL_LEN, BALL_LEN/2, {1, 0}};

Fatores fatores = {1, player_1.y, player_2.y, ball.x, ball.y, ball.speed[0], ball.speed[1], ball.s_abs, P_HEIGHT};

typedef struct _gameData{
    bool running;
    Player*player_1;
    Player*player_2;
    Ball*ball;
    Fatores*fatores;
    int*point;
    bool*reset;
    double*last_PC_move;
    double*score_time;
}GameData;

GameData gameData;
//Aqui, miguel
sem_t sync_sem[3];
int counter = 0;
sem_t sem_counter;

void draw();
void game_mtt(int);
void keyboard(unsigned char key, int x, int y);
void*benchmarkWrapper(void*dataPointer);
void*move_ballWrapper(void*dataPointer);
void*watchdog(void*);

int point = 0;
const double time_start = getCurrentTimeInSeconds();
double last_PC_move = time_start;
double score_time;
bool reset = false;

void pong_mtt(){
    gameData = {false, &player_1, &player_2, &ball, &fatores, &point, &reset, &last_PC_move, &score_time};
    pthread_t move_pc;
    pthread_create(&move_pc, NULL, benchmarkWrapper, (void*)&gameData);
    sem_init(&sync_sem[0], 0, 1);

    pthread_t move_ball;
    pthread_create(&move_ball, NULL, move_ballWrapper, (void*)&gameData);
    sem_init(&sync_sem[1], 0, 1);

    pthread_t watchdogThread;
    pthread_create(&watchdogThread, NULL, watchdog, NULL);
    sem_init(&sem_counter, 0, 1);

    sem_init(&sync_sem[2], 0, 1);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(120, 120);
    glutCreateWindow("nome da janela");
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glutDisplayFunc(draw);
    glutTimerFunc(0, game_mtt, 0);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    pong_mtt();
    return 0;
}

void draw_player(Player p){
    double player_num = p.x, y = p.y, w = p.width, h = p.height;
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON);
    glVertex2d(player_num+w/2, y+h/2);
    glVertex2d(player_num+w/2, y-h/2);
    glVertex2d(player_num-w/2, y-h/2);
    glVertex2d(player_num-w/2, y+h/2);
    glEnd();
}

void draw_ball(Ball ball){
    double x_center = ball.x, y_center = ball.y, h_len = ball.len/2;
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON);
    glVertex2d(x_center+h_len, y_center+h_len);
    glVertex2d(x_center+h_len, y_center-h_len);
    glVertex2d(x_center-h_len, y_center-h_len);
    glVertex2d(x_center-h_len, y_center+h_len);
    glEnd();
}

void keyboard(unsigned char key, int x, int y){
    switch (key)
    {
    case 'w':
        move_player(&player_1, UP);
        break;
    case 's':
        move_player(&player_1, DOWN);
        break;
    }
}

void draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    if(player_1.points < 10 && player_2.points < 10){
        draw_player(player_1);
        draw_player(player_2);
        draw_ball(ball);
        update_score(&player_1, &player_2, &point);
        std::string score_str = std::to_string(player_2.points) +" - "+std::to_string(player_1.points);
        glRasterPos2f(-0.01, 0);
        for (char c : score_str) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }else{
        glRasterPos2f(-0.2, 0);
        std::string end_message = "GAME OVER";
        for (char c : end_message) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        //TODO:matar threads e destruir semáforos
    }
    glutSwapBuffers();
}

void*benchmarkWrapper(void*dataPointer){
    GameData*gameData = (GameData*) dataPointer;
    while(1)
        if(gameData->running){
            sem_wait(&sync_sem[0]);
            benchmark(*(gameData->fatores), gameData->player_2, (gameData->last_PC_move));
            sem_wait(&sem_counter);
            counter++;
            sem_post(&sem_counter);
        }
    return NULL;
}

void*move_ballWrapper(void*dataPointer){
    GameData*gameData = (GameData*) dataPointer;
    while(1){
        if(gameData->running){
            sem_wait(&sync_sem[1]);
            move_ball(gameData->ball, *(gameData->player_1), *(gameData->player_2), gameData->point, gameData->reset, gameData->score_time);
            sem_wait(&sem_counter);
            counter++;
            sem_post(&sem_counter);
        }
    }
    return NULL;
}

void game_mtt(int){
    sem_wait(&sync_sem[2]);
    fatores = {1, player_1.y, player_2.y, ball.x, ball.y, ball.speed[0], ball.speed[1], ball.s_abs, P_HEIGHT};
    gameData.running = true;
    //sem_post(&sync_sem[0]);
    //sem_post(&sync_sem[1]);
    glutPostRedisplay();
    sem_wait(&sem_counter);
    counter++;
    sem_post(&sem_counter);
    glutTimerFunc(1000/60, game_mtt, 0);// Garante que esta função será chamada de novo
}

void*watchdog(void*){
    while(1){
        sem_wait(&sem_counter);
        if(counter == 3){
            counter = 0;
            sem_post(&sync_sem[0]);
            sem_post(&sync_sem[1]);
            sem_post(&sync_sem[2]);
            sem_post(&sem_counter);
        }else{
            sem_post(&sem_counter);
            sleep(0.2);
        }
    }
    return NULL;
}