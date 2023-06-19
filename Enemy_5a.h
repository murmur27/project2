#ifndef ENEMY_5A_H
#define ENEMY_5A_H

#include "Enemy_1n.h"

class Enemy_5a: public Enemy_1n{//shot 없음. buffer. 다른 enemy buff해서 capital size type으로 변경.
    public:
        Enemy_5a(int y_value, int x_value, int frame_value, int can_shot=true, char type_shape='a', int hp_value=8, int speed_value=0, int is_buffed, int buff_speed=6, int score_value=5, int live_state=true)
        :Enemy_1n(y_value, x_value, frame_value, can_shot, type_shape, hp_value, speed_value, is_buffed, buff_speed, score_value, live_state) // a, 8, 0, 6, 5, true 각각 type, hp, speed, buff_speed, score, live_state
        {};
};
#endif