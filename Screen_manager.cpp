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

using namespace std;

//print 'M' 이후 print_share에서 bullet.
//move cursor
void cursorYX(int y, int x)
{
    COORD pos;
    pos.X = x;        
    pos.Y = y;            
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Screen_manager::render(){//print_board()..., in c.
    for(int j=0; j<height; j++){
        for(int i=0; i<width; i++){
            cursorYX(j, i);
            printf("%c", board[j][i]);
        }
    }
}

//print sharing things. every single char has to deal with.
void Screen_manager::print_share(){
    //wall
    for(int i=0; i<width; i++){board[height-1][i]='w';}
    for(int j=0; j<height; j++){board[j][0]='w'; board[j][width-1]='w';}//board[0][1]~board[0][width-2];board[height-2][1]~board[height-2][width-2] can available.

    int shot_frame, create_frame, check_frame;

    //enemy_existing vector 필요. hp가 0이 되면 delete. iter로 돌면서 vector의 원소에서 cell_speed와 shot_frame 고려해서 아래 while문 돌기.
    //my_plane.bullet part
    shot_frame = this->my_plane.shot_frame_my_plane;
    create_frame = this->my_plane.create_frame_my_plane;//기준값 변하지 않음 const.
    check_frame = this->my_plane.check_frame_my_plane;//frame 값 1씩 증가.
    while ((curr_frame-create_frame)/shot_frame - check_frame > 0){ //bullet create
        Bullet *bullet_obj = new Bullet(this->my_plane.y-1+shot_frame, this->my_plane.x, check_frame);
        this->my_plane.bullet.push_back(bullet_obj);//contact by reference
        for(auto iter=this->my_plane.bullet.begin(); iter<this->my_plane.bullet.end(); ){//iter == bullet reference의 reference. *iter == bullet_obj
            if((*iter)->y<=0){//position
                board[(*iter)->y][(*iter)->x]=' ';//erase
                this->my_plane.bullet.erase(iter);//erase하면 순서 당겨지므로 iter++ X. memory 반환 필요?
                continue;
            }
            else{
                if(iter!=(this->my_plane.bullet.end()-1) && curr_frame!=1){//총알이 한박자 쉬고 나감. end()-1 때문에.
                    board[(*iter)->y][(*iter)->x]=' ';
                }
                (*iter)->y -= 1;
                board[(*iter)->y][(*iter)->x]=(*iter)->type;
                iter++;
            }
        }
        for(auto iter=this->enemy.begin();iter<this->enemy.end(); ){//enemy vector 돌며 생성. *iter는 &enemy_obj 지칭.
            int enemy_create_frame = (*iter)->create_frame_enemy;
            int enemy_speed = (*iter)->cell_speed;
            int enemy_check_frame = (*iter)->check_frame_enemy;
            //enemy_bullet move per cell_speed=1==shot_frame
            // enemy 총알의 상호작용란. 이 범위 밖에 enemy 총알의 생성 파트가 있다.
            for(auto iter_EB=(*iter)->bullet.begin(); iter_EB<(*iter)->bullet.end(); ){//enemy_bullet의 원소는 live_state =false이면 더 이상 생성 x. 다만 원래 생성된 것들은 계속 상호작용.
                if((*iter_EB)->y>=height-1){
                    board[(*iter_EB)->y][(*iter_EB)->x]=' ';
                    (*iter)->bullet.erase(iter_EB);
                     continue;
                }
                else{
                    int flag=1;
                    if(iter_EB!=((*iter)->bullet.end()) && curr_frame!=1){//end()-1 인지 아닌지가 중요.
                        if(board[(*iter_EB)->y][(*iter_EB)->x]==(*iter_EB)->type){
                            board[(*iter_EB)->y][(*iter_EB)->x]=' ';
                        }
                        (*iter_EB)->y += 1;
                        board[(*iter_EB)->y][(*iter_EB)->x]=(*iter_EB)->type;
                        iter_EB++;
                    }
                }
            }
            if((*iter)->live==false){//enemy가 죽은 상태이면 뒤 부분 모두 뛰어넘기.
                iter++;
                continue;
            }
            if(curr_frame==enemy_create_frame){//처음 생성.
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.n_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    if((*iter)->type=='a'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.a_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    (*iter)->live=false;
                    if((*iter)->type=='r') this->my_plane.r_kill++;
                    if((*iter)->type=='s') this->my_plane.s_kill++;
                    if((*iter)->type=='d') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;
                iter++;
            }// iter++ 가 잘못 배정되거나 빠지면 오류. 아래 줄부터 에러 !!.
            else if((enemy_speed==0)&&(curr_frame-enemy_create_frame >= 0)){//~n
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.n_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    if((*iter)->type=='a'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.a_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    (*iter)->live=false;
                    if((*iter)->type=='r') this->my_plane.r_kill++;
                    if((*iter)->type=='s') this->my_plane.s_kill++;
                    if((*iter)->type=='d') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                iter++;
            }
            else if((curr_frame-enemy_create_frame >= 0)&&((curr_frame-enemy_create_frame)/enemy_speed - enemy_check_frame > 0)) {//이동 조건과 이동.
                if((*iter)->y>=height-1){//position
                    board[(*iter)->y][(*iter)->x]=' ';//erase
                    this->enemy.erase(iter);//이 때 무조건 제거되야 함.
                    continue;
                }
                else{
                    if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                        (*iter)->hp_enemy-=1;
                    }
                    if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                        (*iter)->hp_enemy-=2;
                    }
                    if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                        this->my_plane.my_score+=(*iter)->score;
                        if((*iter)->type=='n'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                            this->my_plane.n_kill++;
                            this->enemy.erase(iter);
                            continue;
                        }
                        if((*iter)->type=='a'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                            this->my_plane.a_kill++;
                            this->enemy.erase(iter);
                            continue;
                        }
                        (*iter)->live=false;
                        if((*iter)->type=='r') this->my_plane.r_kill++;
                        if((*iter)->type=='s') this->my_plane.s_kill++;
                        if((*iter)->type=='d') this->my_plane.d_kill++;
                        (*iter)->type=' ';
                    }
                    if(iter!=this->enemy.end() && curr_frame!=1){//curr_frame==2 부터 enemy 이동.
                        board[(*iter)->y][(*iter)->x]=' ';
                    }
                    (*iter)->y += 1;//y position 이동. !!!!!!!!move!!!!!!!!
                    //enemy_bullet create!!!!!!
                    if((*iter)->can_shoot==true){
                        if((*iter)->y>=height-2){
                        }
                        else {
                            if((*iter)->type=='d'){//diagonal.
                                int swift=0;
                                if((*iter)->x<15) swift=-1;
                                else if((*iter)->x>=15) swift=1;
                                Enemy_bullet *bullet_obj = new Enemy_bullet((*iter)->y+1, (*iter)->x+swift, check_frame);
                                (*iter)->bullet.push_back(bullet_obj);
                                board[bullet_obj->y][bullet_obj->x]=bullet_obj->type;//enemy_bullet 생성. 중요. 여기서 생성되는 enemy_bullet의 class variable을 reinitialize 할 수 있음.
                            }
                            else{                              
                                Enemy_bullet *bullet_obj = new Enemy_bullet((*iter)->y+1, (*iter)->x, check_frame);
                                (*iter)->bullet.push_back(bullet_obj);
                                board[bullet_obj->y][bullet_obj->x]=bullet_obj->type;//enemy_bullet 생성. 중요. 여기서 생성되는 enemy_bullet의 class variable을 reinitialize 할 수 있음.
                            }
                        }
                    }
                    //enemy_bullet *
                    if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 이동하려는 자리에 level_1_bullet 있으면 life 1까기.
                        (*iter)->hp_enemy-=1;
                    }
                    if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 이동하려는 자리에 level_2_bullet 있으면 life 2까기.
                        (*iter)->hp_enemy-=2;
                    }
                    if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                        this->my_plane.my_score+=(*iter)->score;
                        if((*iter)->type=='n'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                            this->my_plane.n_kill++;
                            this->enemy.erase(iter);
                            continue;
                        }
                        if((*iter)->type=='a'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                            this->my_plane.a_kill++;
                            this->enemy.erase(iter);
                            continue;
                        }
                        (*iter)->live=false;
                        if((*iter)->type=='r') this->my_plane.r_kill++;
                        if((*iter)->type=='s') this->my_plane.s_kill++;
                        if((*iter)->type=='d') this->my_plane.d_kill++;
                        (*iter)->type=' ';
                    }
                    board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                    (*iter)->check_frame_enemy+=1;
                    enemy_check_frame++;
                    iter++;
                }
            }
            else if (curr_frame-enemy_create_frame >= 0){//이동하지 않을때.
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.n_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    if((*iter)->type=='a'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
                        this->my_plane.a_kill++;
                        this->enemy.erase(iter);
                        continue;
                    }
                    (*iter)->live=false;
                    if((*iter)->type=='r') this->my_plane.r_kill++;
                    if((*iter)->type=='s') this->my_plane.s_kill++;
                    if((*iter)->type=='d') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                iter++;
            }
            else {
                iter++;
            }
        }
    this->my_plane.check_frame_my_plane+=1;
    check_frame++;
    }//위까지는 정상 작동 enemy 포인터의 enemy_bullet에 대한 access 필요.
}
    //my_plane.bullet part ends

//print when key didn't pressed
void Screen_manager::print(){
    board[this->my_plane.y][this->my_plane.x]='M';

    print_share();
}

//print when key pressed
void Screen_manager::print(int ch){ //ascii
    if(ch=='d'){ //right
        if(this->my_plane.y >=0 && this->my_plane.y <(height-1) && this->my_plane.x >0 && this->my_plane.x < (width-2)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.x +=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='a'){ //left
        if(this->my_plane.y >=0 && this->my_plane.y <(height-1) && this->my_plane.x >1 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.x -=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='w'){ //up
        if(this->my_plane.y >0 && this->my_plane.y <(height-1) && this->my_plane.x >0 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.y -=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='s'){ //down
        if(this->my_plane.y >=0 && this->my_plane.y <(height-2) && this->my_plane.x >0 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.y +=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }    
    }
    print_share();
}
//inheritance 와 polymorphoism 중요.
void Screen_manager::enemy_push(int num_event){
    for(int i=0;i<num_event;i++){
        Enemy_1n *enemy_obj_n = nullptr;
        Enemy_2r *enemy_obj_r = nullptr;
        Enemy_3s *enemy_obj_s = nullptr;
        Enemy_4d *enemy_obj_d = nullptr;
        Enemy_5a *enemy_obj_a = nullptr;
        Enemy_1n *enemy_obj_L = nullptr;
        Enemy_1n *enemy_obj_P = nullptr;
        switch(type_event[i]){
            case 'n':
                enemy_obj_n = new Enemy_1n(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_n);
                break;
            case 'r':
                enemy_obj_r = new Enemy_2r(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_r);
                break;
            case 's':
                enemy_obj_s = new Enemy_3s(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_s);
                break;
                /*
            case 'd':
                enemy_obj_d = new Enemy_4d(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_d);
                break;
            case 'a':
                enemy_obj_a = new Enemy_5a(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_a);
                break;
            case 'L':
                enemy_obj_L = new Enemy_1n(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_L);
                break;
            case 'P':
                enemy_obj_P = new Enemy_1n(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_P);
                break;
            */
        }
    }
}