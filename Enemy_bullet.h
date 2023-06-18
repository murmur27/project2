#ifndef ENEMY_BULLET_H
#define ENEMY_BULLET_H

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

class Enemy_bullet{
    public:
        Enemy_bullet(int y_value, int x_value, int buffed, int frame_value) \
        : y(y_value), x(x_value), is_buffed(buffed), create_frame_bullet(frame_value) {
            if(buffed==true){
                damage=2;
            }
        };
        Enemy_bullet(int y_value, int x_value, int buffed, int frame_value, int swift_value) \
        : y(y_value), x(x_value), is_buffed(buffed), create_frame_bullet(frame_value), cell_swift(swift_value) {
            if(buffed==true){
                damage=2;
            }
        };
        int damage=1;
        int y, x;
        int create_frame_bullet; //initialize when it creates
        int is_buffed=false;//if true increase damage by 1.
        int check_frame_bullet; //update
        int level=1;
        int cell_speed_bullet=1;//y increase
        int cell_swift=0;//x increase. to the left == 1, to the right == -1
        char type='*';
};
#endif