#ifndef MY_PLANE_H
#define MY_PLANE_H

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
#include "Bullet.h"

using namespace std;

class My_plane{
    public:
        My_plane(int y_value, int x_value, int frame_value) \

        : y(y_value), x(x_value), create_frame_my_plane(frame_value) {};
        int y, x;
        int shot_frame_my_plane=1;//1shot for 1 frame //매 프레임마다 bullet 생성.
        int create_frame_my_plane; //initialize when it creates//처음 값 0.
        int check_frame_my_plane=0;
        int hp_my_plane=5;
        int my_score=0;
        int n_kill=0;
        int r_kill=0;
        int s_kill=0;
        int d_kill=0;
        int a_kill=0;
        vector<Bullet*> bullet;//pointer 이용 권장. ex) vector<Bullet*> bullet
};
#endif