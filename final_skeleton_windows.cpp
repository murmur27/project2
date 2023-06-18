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
#include "Screen_manager.h"
#include "keyboard_controller.cpp"

using namespace std;

//main function
int main(int argc, char *argv[])
{   
    ifstream input;
    string input_file = string(argv[1]);
    if(argc!=2){
        cerr<<"Wrong arguments"<<endl;
    }
    input.open(input_file); //an input_file
    if(!(input.is_open())){
        cerr<<"Input missing"<<endl;
        return 0;
    }

    Screen_manager manager;
    int num_event, frame_event, y_event, x_event;
    char type_event;
    input >> num_event;
    for(int i=0; i<num_event; i++){
        input >> frame_event >> type_event >> y_event >> x_event;
        manager.frame_event[i] = frame_event;
        manager.type_event[i] = type_event;
        manager.y_event[i] = y_event;
        manager.x_event[i] = x_event;
    }
    input.close();
    manager.enemy_push(num_event);
    
    double operation_time=0; //microseconds
    double frame_length = manager.frame_length;

    HANDLE consolehandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO consolecursor;
    consolecursor.bVisible=0;
    consolecursor.dwSize=1;
    SetConsoleCursorInfo(consolehandle, &consolecursor);
    system("cls");
    system("mode con:cols=62 lines=32");
    manager.print();
    manager.render();

    int ch = 0;
    while(ch !='x')
    {
        auto start = std::chrono::system_clock::now();
        if(_kbhit()) {
            ch = getKeyDown();
            manager.print(ch);
            if(ch=='x') break;
        }
        else{
            manager.print();
        }
        auto end = std::chrono::system_clock::now();
        auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
        int prev_frame = operation_time / frame_length; //int 값 return.
        //operation_time += (end-start);
        operation_time += microsec.count();
        manager.curr_frame = operation_time / frame_length;//integer
        
        /*Objects operate every 0.1 seconds.
        Call refresh() every 0.1 seconds(or every 0.1*n seconds if manager.print() takes a long time)
        ex) if operation_time changes to 0.25->0.30, refresh()

        You can modify your manager.print() using multi-threading or not.
        If you use multi-threading, you may print more frequently.
        You can reduce the execution time of manager.print() using multi-threading*/
        if(manager.curr_frame-prev_frame>0){
            manager.render();
        }
    }
    system("cls");
    std::cout<<"Start game~!"<<endl;
    return 0;
}