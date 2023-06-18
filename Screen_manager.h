#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

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
#include "My_plane.h"
#include "Enemy_1n.h"
#include "Enemy_2r.h"
#include "Enemy_3s.h"
#include "Enemy_4d.h"
#include "Enemy_5a.h"

using namespace std;

//Screen manager.
class Screen_manager{
    public:
        int frame_length=100000; //each frame's length == 0.1sec(10,000 microseconds)
        int curr_frame=0; //1frame=0.1sec

        int y, x;
        int width=60;
        int height=30;
        int frame_event[300]={0,}, y_event[300]={0,}, x_event[300]={0,};//screen 상에 나타나는 변화 기록.
        char type_event[300] = {' ',};

        void print(int ascii);
        void print();
        void print_share();
        void render();
        void enemy_push(int num_event);
        char board[30][60]={'a'};
        char before_board[30][60]={'a'};
        My_plane my_plane = My_plane(height-2, 1, 0);
        vector<Enemy*> enemy;//existing enemy list.
};
#endif