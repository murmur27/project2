#ifndef ENEMY_2R_H
#define ENEMY_2R_H

#include "Enemy_1n.h"

class Enemy_2r: public Enemy_1n{//shot 없음.
    public:
        Enemy_2r(int y_value, int x_value, int frame_value, int can_shot=false, char type_shape='r', int hp_value=5, int speed_value=3, int is_buffed=false, int buff_speed_value=0, int score_value=2, int live_state=true)
        :Enemy_1n(y_value, x_value, frame_value, can_shot, type_shape, hp_value, speed_value, is_buffed, buff_speed_value, score_value, live_state) // r, 5, 3, 0, 2 각각 type, hp, speed, buff_speed, score, live_state
        {
            if(is_buffed==true){
                type='R';
            }
        };
};
#endif