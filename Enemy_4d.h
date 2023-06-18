#ifndef ENEMY_4D_H
#define ENEMY_4D_H

#include "Enemy_2r.h"

class Enemy_4d: public Enemy_2r{//shot 있음. diagonally shot.
    public:
        Enemy_4d(int y_value, int x_value, int frame_value, int can_shot=true, char type_shape='d', int hp_value=5, int speed_value=3, int buff_speed_value=0, int score_value=4, int live_state=true)
        :Enemy_2r(y_value, x_value, frame_value, can_shot, type_shape, hp_value, speed_value, buff_speed_value, score_value, live_state) // s, 5, 3, 0, 4, true 각각 type, hp, speed, buff_speed,  score, live_state
        {};
};
#endif