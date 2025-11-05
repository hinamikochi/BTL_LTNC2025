#pragma once
#include "Item.h"

class Diamond : public Item {
public:
    Diamond(int x, int y)
        // Các giá trị của diamond
        : Item(x, y, 25, 21, "diamond", 300, 0.8f)
    {
       
        this->type = TYPE_DIAMOND;
    }
};