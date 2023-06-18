#ifndef ENEMY_H
#define ENEMY_H

#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <Windows.h>
#include <conio.h>
#include <chrono>
#include <vector>
#include "Enemy_bullet.h"

using namespace std;

//inheritance Enemy_1~5
class Enemy{
    public:
        Enemy(int y_value, int x_value, int frame_value, int can_shot, char type_shape, int hp_value, int speed_value, int buff_speed_value, int score_value, int live_state) \

        : y(y_value), x(x_value), create_frame_enemy(frame_value), can_shoot(can_shot), type(type_shape), hp_enemy(hp_value), cell_speed(speed_value), buff_speed(buff_speed_value), score(score_value), live(live_state)
        {};//constructor with full condition. can_shoot
        int y, x;
        int create_frame_enemy; //initialize when it creates//처음 값 0.
        int check_frame_enemy=0;
        int hp_enemy;
        int cell_speed=0;
        int buff_speed=0;
        int score;
        char type;
        int can_shoot=false;//false
        int live=true;//enemy_bullet이 Enemy가 life가 0이 되어도 남아 있으므로, 상태를 따로 표기하고 다루기.
        vector<Enemy_bullet*> bullet;//pointer 이용 권장. ex) vector<Enemy_bullet*> bullet
};
#endif