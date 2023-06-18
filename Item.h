#ifndef ITEM_H
#define ITEM_H

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

using namespace std;

class Item{
    public:
        Item(int y_value, int x_value, int frame_value, char type_shape) \

        : y(y_value), x(x_value), create_frame_item(frame_value), type(type_shape)
        {};
        int y, x;
        int create_frame_item; //initialize when it creates//처음 값 0.
        int check_frame_item=0;
        int cell_speed=0;
        char type;
};
#endif