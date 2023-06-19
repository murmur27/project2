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
    manager.item_push(num_event);
    
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

    int gameover=false;//default.
    int ch = 0;
    while(ch !='x')
    {
        if(ch=='p') {//press p when intend to pause.
            while(true){
                if(_kbhit()){
                        break;
                }
            }
        }
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
        if(manager.my_plane.hp_my_plane<=0){//my_plane hp 0이면 gameover.
            gameover=true;
            break;
        }
        int flag1=false;//남아 있는 enemy가 없다면 true로 바뀜.
        for(auto iter=manager.enemy.begin();iter<manager.enemy.end();iter++){
            if((*iter)->live==true){
                flag1=false;
                break;
            }
            else{
                flag1=true;
            }
        }
        if(manager.enemy.begin()==manager.enemy.end()){
            flag1=true;
        }
        int flag2=false;//남아 있는 event가 없다면 true로 바뀜.
        if(manager.curr_frame>=manager.frame_event[num_event-1]){
            flag2=true;
        }
        if(flag1==true&&flag2==true){//flag1,2 둘다 true이면 gameover
            gameover=true;
            break;
        }
    }
    system("cls");
    std::cout<<"Start game~!"<<endl;
    if(gameover==true){
        int total_score=manager.my_plane.my_score;
        int n_kill=manager.my_plane.n_kill;
        int r_kill=manager.my_plane.r_kill;
        int s_kill=manager.my_plane.s_kill;
        int d_kill=manager.my_plane.d_kill;
        int a_kill=manager.my_plane.a_kill;
        std::cout<<"Your score is "<<total_score<<" (n : "<<n_kill<<" , r : "<<r_kill<<" , s : "<<s_kill<<" , d : "<<d_kill<<" , a : "<<a_kill<<")"<<endl;
    }
    return 0;
}