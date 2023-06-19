#ifndef ENEMY_3S_H
#define ENEMY_3S_H

#include "Enemy_2r.h"

class Enemy_3s: public Enemy_2r{//shot 있음.
    public:
        Enemy_3s(int y_value, int x_value, int frame_value, int can_shot=true, char type_shape='s', int hp_value=4, int speed_value=9, int is_buffed=false, int buff_speed_value=0, int score_value=3, int live_state=true)
        :Enemy_2r(y_value, x_value, frame_value, can_shot, type_shape, hp_value, speed_value, is_buffed, buff_speed_value, score_value, live_state) // s, 4, 9, 0, 3 각각 type, hp, speed, buff_speed, score, live_state
        {
            if(is_buffed==true){
                type='S';
            }
        };
};
#endif