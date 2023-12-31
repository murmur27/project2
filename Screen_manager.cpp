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
    while ((curr_frame-create_frame)/shot_frame - check_frame > 0){
        //item 부터 생성하고 처리하여 bullet 효과 이후에 바로 적용.
        for(auto iter=this->item.begin();iter<this->item.end(); ){//item vector 돌며 생성. *iter는 &item_obj 지칭.
            int item_create_frame = (*iter)->create_frame_item;
            int item_check_frame = (*iter)->check_frame_item;

            if(curr_frame==item_create_frame){//처음 생성.
                board[(*iter)->y][(*iter)->x]=(*iter)->type;
                iter++;
            }// iter++ 가 잘못 배정되거나 빠지면 오류. 아래 줄부터 에러 !!.
            else if(curr_frame-item_create_frame >= 0){//생성 이후.
                if(board[(*iter)->y][(*iter)->x]=='M'){//item의 현재 위치에 my_plane 있으면 삭제하고 효과 적용.
                    if((*iter)->type=='L'){
                        if(this->my_plane.level<3) this->my_plane.level++;
                    }
                    if((*iter)->type=='P'){
                        if(this->my_plane.power<2) this->my_plane.power++;
                    }
                    item.erase(iter);
                    continue;
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                iter++;
            }
            else {
                iter++;
            }
        }

        Bullet *bullet_obj = new Bullet(this->my_plane.y-1+shot_frame, this->my_plane.x, check_frame, this->my_plane.level);//bullet create
        this->my_plane.bullet.push_back(bullet_obj);//contact by reference
        if(this->my_plane.power==2){
            if(this->my_plane.x>1){
            Bullet *bullet_obj1 = new Bullet(this->my_plane.y-1+shot_frame, this->my_plane.x-1, check_frame, this->my_plane.level);//bullet create
            this->my_plane.bullet.push_back(bullet_obj1);//contact by reference
            }
            if(this->my_plane.x<width-2){
            Bullet *bullet_obj2 = new Bullet(this->my_plane.y-1+shot_frame, this->my_plane.x+1, check_frame, this->my_plane.level);//bullet create
            this->my_plane.bullet.push_back(bullet_obj2);//contact by reference
            }
        }
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
            
            if(board[(*iter)->y][(*iter)->x]=='M'){//my_plane과 enemy 상호작용.
                this->my_plane.hp_my_plane--;
            }

            // enemy 총알의 상호작용란. 이 범위 밖에 enemy 총알의 생성 파트가 있다.
            for(auto iter_EB=(*iter)->bullet.begin(); iter_EB<(*iter)->bullet.end(); ){//enemy_bullet의 원소는 live_state==false이면 더 이상 생성 x. 다만 원래 생성된 것들은 계속 상호작용.
                if(board[(*iter_EB)->y][(*iter_EB)->x]=='M'){//enemy_bullet conflict with my_plane
                    this->my_plane.hp_my_plane-=(*iter_EB)->damage;
                }
                int outside = false;
                if((*iter_EB)->x<=0||(*iter_EB)->x>=(width-1)){
                    outside = true;
                }
                if((*iter_EB)->cell_swift!=0&&outside==true){
                    board[(*iter_EB)->y][(*iter_EB)->x]=' ';
                    (*iter)->bullet.erase(iter_EB);
                     continue;
                }
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
                        (*iter_EB)->x += (*iter_EB)->cell_swift;
                        board[(*iter_EB)->y][(*iter_EB)->x]=(*iter_EB)->type;
                        iter_EB++;
                    }
                }
            }
            // enemy 총알의 상호작용란 위까지 ^^

            if((*iter)->live==false){//enemy가 죽은 상태이면 뒤 부분 모두 뛰어넘기.
                if((*iter)->bullet.begin()==(*iter)->bullet.end()){//enemy가 죽은 상태이고 enemy 총알마저 모두 없어지면 enemy 삭제하기.
                    this->enemy.erase(iter);
                }
                else{
                iter++;
                }
                continue;
            }
            if(curr_frame==enemy_create_frame){//처음 생성.
                if ((*iter)->buff_speed==0) {
                    if(this->buff_board[(*iter)->y][(*iter)->x]=='b'){
                        if((*iter)->type >= 97 && (*iter)->type <=122) {
                            (*iter)->type-=32;//대문자화.
                            (*iter)->buffed=true;
                        }
                    }
                }
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if(board[(*iter)->y][(*iter)->x]=='!'){//enemy가 현재 위치에 level_3_bullet 있으면 life 3까기.
                    (*iter)->hp_enemy-=3;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'||(*iter)->type=='N'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
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
                    if((*iter)->type=='r'||(*iter)->type=='R') this->my_plane.r_kill++;
                    if((*iter)->type=='s'||(*iter)->type=='S') this->my_plane.s_kill++;
                    if((*iter)->type=='d'||(*iter)->type=='D') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;
                iter++;
            }// iter++ 가 잘못 배정되거나 빠지면 오류. 아래 줄부터 에러 !!.
            else if((enemy_speed==0)&&(curr_frame-enemy_create_frame >= 0)){//~n, ~a
                if ((*iter)->buff_speed==0) {//~n
                    if(this->buff_board[(*iter)->y][(*iter)->x]=='b'){
                        if((*iter)->type >= 97 && (*iter)->type <=122) {
                            (*iter)->type-=32;//대문자화.
                            (*iter)->buffed=true;
                        }
                    }
                }
                else if((curr_frame-enemy_create_frame)/(*iter)->buff_speed - enemy_check_frame > 0){//buff 먼저 걸 기회가 있음. buff.
                //buff_range 생성.
                    int up=-3;
                    int down=3;
                    int left=-3;
                    int right=3;
                    if((*iter)->y<3){
                        up=0-(*iter)->y;
                    }
                    if((*iter)->y>height-4){
                        down=height-1-(*iter)->y;
                    }
                    if((*iter)->x<4){
                        left=1-(*iter)->x;
                    }
                    if((*iter)->x>width-4){
                        right=width-1-(*iter)->x;
                    }
                    for(int i=up;i<=down;i++){
                        for(int j=left;j<=right;j++){
                            this->buff_board[i+(*iter)->y][j+(*iter)->x]='b';
                        }
                    }
                }
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if(board[(*iter)->y][(*iter)->x]=='!'){//enemy가 현재 위치에 level_3_bullet 있으면 life 3까기.
                    (*iter)->hp_enemy-=3;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'||(*iter)->type=='N'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
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
                    if((*iter)->type=='r'||(*iter)->type=='R') this->my_plane.r_kill++;
                    if((*iter)->type=='s'||(*iter)->type=='S') this->my_plane.s_kill++;
                    if((*iter)->type=='d'||(*iter)->type=='D') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                iter++;
            }
            else if((curr_frame-enemy_create_frame >= 0)&&((curr_frame-enemy_create_frame)/enemy_speed - enemy_check_frame > 0)) {//이동 조건과 이동.
                if ((*iter)->buff_speed==0) {//
                    if(this->buff_board[(*iter)->y][(*iter)->x]=='b'){
                        if((*iter)->type >= 97 && (*iter)->type <=122) {
                            (*iter)->type-=32;//대문자화.
                            (*iter)->buffed=true;
                        }
                    }
                }
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
                    if(board[(*iter)->y][(*iter)->x]=='!'){//enemy가 현재 위치에 level_3_bullet 있으면 life 3까기.
                        (*iter)->hp_enemy-=3;
                    }
                    if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                        this->my_plane.my_score+=(*iter)->score;
                        if((*iter)->type=='n'||(*iter)->type=='N'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
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
                        if((*iter)->type=='r'||(*iter)->type=='R') this->my_plane.r_kill++;
                        if((*iter)->type=='s'||(*iter)->type=='S') this->my_plane.s_kill++;
                        if((*iter)->type=='d'||(*iter)->type=='D') this->my_plane.d_kill++;
                        (*iter)->type=' ';
                    }
                    if(iter!=this->enemy.end() && curr_frame!=1){//curr_frame==2 부터 enemy 이동.
                        board[(*iter)->y][(*iter)->x]=' ';
                    }

                    (*iter)->y += 1;//y position 이동.
                    //move!!! 이후에는 이동하려는 자리와 상호작용한다.

                    //enemy_bullet create!!!!!!
                    if((*iter)->can_shoot==true){
                        if((*iter)->y>=height-2){
                        }
                        else {
                            if((*iter)->type=='d'){//diagonal.
                                int swift=0;
                                if((*iter)->x>1&&(*iter)->x<(width/2)) {
                                    swift=-1;
                                    Enemy_bullet *bullet_obj = new Enemy_bullet((*iter)->y+1, (*iter)->x+swift, (*iter)->buffed, check_frame, swift);
                                    //enemy_bullet 생성 직후 충돌 여부 따지기.
                                    (*iter)->bullet.push_back(bullet_obj);
                                    board[bullet_obj->y][bullet_obj->x]=bullet_obj->type;//enemy_bullet 생성. 중요. 여기서 생성되는 enemy_bullet의 class variable을 reinitialize 할 수 있음.
                                }
                                else if((*iter)->x>=(width/2)&&(*iter)->x<(width-2)) {
                                    swift=1;
                                    Enemy_bullet *bullet_obj = new Enemy_bullet((*iter)->y+1, (*iter)->x+swift, (*iter)->buffed, check_frame, swift);
                                    //enemy_bullet 생성 직후 충돌 여부 따지기.
                                    (*iter)->bullet.push_back(bullet_obj);
                                    board[bullet_obj->y][bullet_obj->x]=bullet_obj->type;//enemy_bullet 생성. 중요. 여기서 생성되는 enemy_bullet의 class variable을 reinitialize 할 수 있음.
                                }
                            }
                            else{//~s
                                Enemy_bullet *bullet_obj = new Enemy_bullet((*iter)->y+1, (*iter)->x, (*iter)->buffed, check_frame);
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
                    if(board[(*iter)->y][(*iter)->x]=='!'){//enemy가 현재 위치에 level_3_bullet 있으면 life 3까기.
                        (*iter)->hp_enemy-=3;
                    }
                    if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                        this->my_plane.my_score+=(*iter)->score;
                        if((*iter)->type=='n'||(*iter)->type=='N'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
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
                        if((*iter)->type=='r'||(*iter)->type=='R') this->my_plane.r_kill++;
                        if((*iter)->type=='s'||(*iter)->type=='S') this->my_plane.s_kill++;
                        if((*iter)->type=='d'||(*iter)->type=='D') this->my_plane.d_kill++;
                        (*iter)->type=' ';
                    }
                    board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                    (*iter)->check_frame_enemy+=1;
                    enemy_check_frame++;
                    iter++;
                }
            }
            else if (curr_frame-enemy_create_frame >= 0){//이동하지 않을때.
                if ((*iter)->buff_speed==0) {
                    if(this->buff_board[(*iter)->y][(*iter)->x]=='b'){
                        if((*iter)->type >= 97 && (*iter)->type <=122) {
                            (*iter)->type-=32;//대문자화.
                            (*iter)->buffed=true;
                        }
                    }
                }
                if(board[(*iter)->y][(*iter)->x]=='\''){//enemy가 현재 위치에 level_1_bullet 있으면 life 1까기.
                    (*iter)->hp_enemy-=1;
                }
                if(board[(*iter)->y][(*iter)->x]=='^'){//enemy가 현재 위치에 level_2_bullet 있으면 life 2까기.
                    (*iter)->hp_enemy-=2;
                }
                if(board[(*iter)->y][(*iter)->x]=='!'){//enemy가 현재 위치에 level_3_bullet 있으면 life 3까기.
                    (*iter)->hp_enemy-=3;
                }
                if((*iter)->hp_enemy<=0){//hp 0이면 제거할 수는 없다. 다만 상호작용 못하도록 처리해야함. Enemy.live = false 처리.
                    this->my_plane.my_score+=(*iter)->score;
                    if((*iter)->type=='n'||(*iter)->type=='N'){//안 움직이면서 총알 생성도 안하는 것들은 erase 해줘도 됨.
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
                    if((*iter)->type=='r'||(*iter)->type=='R') this->my_plane.r_kill++;
                    if((*iter)->type=='s'||(*iter)->type=='S') this->my_plane.s_kill++;
                    if((*iter)->type=='d'||(*iter)->type=='D') this->my_plane.d_kill++;
                    (*iter)->type=' ';
                }
                board[(*iter)->y][(*iter)->x]=(*iter)->type;//enemy 이동. !~enemy_bullet
                iter++;
            }
            else {
                iter++;
            }
            board[my_plane.y][my_plane.x]='M';
        }
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            this->buff_board[i][j]='a';//initialize;
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
            case 'd':
                enemy_obj_d = new Enemy_4d(y_event[i],x_event[i],frame_event[i]);
                enemy.push_back(enemy_obj_d);
                break;
            case 'a':
                enemy_obj_a = new Enemy_5a(y_event[i],x_event[i],frame_event[i]);
                enemy.insert(enemy.begin(),enemy_obj_a);//맨 앞에 추가해서 buff field를 가장 먼저 생성하도록
                break;
        }
    }
}

void Screen_manager::item_push(int num_event){
    for(int i=0;i<num_event;i++){
        Item_L *item_obj_L = nullptr;
        Item_P *item_obj_P = nullptr;
        switch(type_event[i]){
            case 'L':
                item_obj_L = new Item_L(y_event[i],x_event[i],frame_event[i]);
                item.push_back(item_obj_L);
                break;
            case 'P':
                item_obj_P = new Item_P(y_event[i],x_event[i],frame_event[i]);
                item.push_back(item_obj_P);
                break;
        }
    }
}