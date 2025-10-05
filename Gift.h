#pragma once
#include "Item.h"
#include <random>

class Gift : public Item {
public:
    Gift(int x, int y)
        // <<< SỬA ĐỔI Ở ĐÂY: Gọi hàm Item() với đúng 7 tham số >>>
        : Item(x, y, 40, 40, "gift_bag", getRandomScore(), 1.0f)
    {
        // Gán giá trị 'type' ở bên trong thân hàm
        this->type = TYPE_GIFT;
    }

private:
    // Hàm tĩnh để tạo điểm ngẫu nhiên
    static int getRandomScore() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(50, 400);
        return distrib(gen);
    }
};