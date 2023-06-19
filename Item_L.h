#ifndef ITEM_L_H
#define ITEM_L_H

#include "Item.h"

class Item_L: public Item{
    public:
        Item_L(int y_value, int x_value, int frame_value, char type_shape='L')
        :Item(y_value, x_value, frame_value, type_shape)
        {};
};
#endif