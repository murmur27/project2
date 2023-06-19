#ifndef BULLET_H
#define BULLET_H

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

class Bullet{
    public:
        Bullet(int y_value, int x_value, int frame_value, int level) \
        : y(y_value), x(x_value), create_frame_bullet(frame_value) {
            damage=level;
            if(level==1) type='\'';
            if(level==2) type='^';
            if(level==3) type='!';
        };
        int damage=1;
        int y, x;
        int create_frame_bullet; //initialize when it creates
        int check_frame_bullet; //update
        int level=1;
        int cell_speed_bullet=1;
        char type='\'';//종류 ',
};
#endif