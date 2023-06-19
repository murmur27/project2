#ifndef ITEM_P_H
#define ITEM_P_H

#include "Item.h"

class Item_P: public Item{
    public:
        Item_P(int y_value, int x_value, int frame_value, char type_shape='P')
        :Item(y_value, x_value, frame_value, type_shape)
        {};
};
#endif