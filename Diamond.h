#pragma once
#include "Item.h"

class Diamond : public Item {
public:
    Diamond(int x, int y)
        // Gọi hàm khởi tạo của Item với đúng 7 tham số
        : Item(x, y, 25, 21, "diamond", 600, 0.8f)
    {
        // Gán giá trị 'type' ở bên trong thân hàm
        this->type = TYPE_DIAMOND;
    }
};