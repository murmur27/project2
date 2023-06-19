#ifndef ENEMY_1N_H
#define ENEMY_1N_H

#include "Enemy.h"

class Enemy_1n: public Enemy{//shot 없음.
    public:
        Enemy_1n(int y_value, int x_value, int frame_value, int can_shot=false, char type_shape='n', int hp_value=10, int speed_value=0, int is_buffed=false, int buff_speed_value=0, int score_value=1, int live_state=true)
        :Enemy(y_value, x_value, frame_value, can_shot, type_shape, hp_value, speed_value, is_buffed, buff_speed_value, score_value, live_state) // n, 5, 0, 0, 1 각각 type, hp, speed, buff_speed, score, live_state
        {
            if(is_buffed==true){
                type='N';
            }
        };
};
#endif