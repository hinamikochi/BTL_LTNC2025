# ⛏️ Gold Miner
Đây là bản game Gold Miner (đào vàng) mà mọi người chắc đã từng chơi khi còn đi học, được code đầu bằng C++ có sử dụng thư viện SDL2.




#
![Ảnh chụp màn hình game Gold Miner](https://github.com/user-attachments/assets/5549cc78-5545-49f3-bbd4-4812857cbc16)
<br>
<br>
![Ảnh chụp màn hình game Gold Miner](https://github.com/user-attachments/assets/ac690127-971a-4e2e-99c6-406c48d7cfb6)


## 📜 Mô tả

Luật chơi đơn giản: người chơi sẽ vào vai một thợ mỏ cố gắng gắp đủ số vàng mục tiêu trước khi hết thời gian để qua màn.

## ✨ Các tính năng chính

* **Cách chơi:** Thả móc để gắp vàng, đá, và các vật phẩm giá trị.
* **Hệ thống màn chơi:** Game có 5 màn chơi với độ khó và mục tiêu tăng dần.
* **Vật phẩm đa dạng:** Vàng (nhỏ, vừa, to), Đá (nhỏ, vừa, to), Kim cương, và Túi quà may mắn.
* **Âm thanh:** Có nhạc nền lúc chơi, nhạc menu, và các hiệu ứng âm thanh (gắp trúng, thắng, thua).
* **Hệ thống UI:** Hiển thị Điểm, Mục tiêu, Thời gian, và Màn chơi.
* **Chức năng:** Tạm dừng (Pause), Tắt/Mở âm thanh (Mute).

## 🎮 Cách chơi

* Nhấn phím **SPACE** để thả móc.
* Móc sẽ tự động kéo vật thể lên khi chạm vào.
* Cố gắng đạt được số tiền **Mục tiêu (Goal)** trước khi **Thời gian (Time)** chạy về 0.

## 🛠️ Môi trường và thư viện

* **Ngôn ngữ:** C++
* **Thư viện:** [SDL2](https://www.libsdl.org/) (để xử lý đồ họa, cửa sổ và sự kiện)
* **Thư viện phụ (SDL):**
    * [SDL2_image](https://www.libsdl.org/projects/SDL_image/) (để tải ảnh .png)
    * [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) (để hiển thị text)
    * [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/) (để xử lý âm thanh)
* **Môi trường:** Visual Studio 2022

## ⚙️ Hướng dẫn Cài đặt & Chạy (Tùy chọn)

1.  Clone repository này về máy.
2.  Mở project bằng Visual Studio 2022.
3.  Bạn cần [thiết lập thư viện SDL2, SDL_image, SDL_ttf, và SDL_mixer] cho project (thiết lập Include Directories và Linker).
4.  Nhấn F5 để Build và Chạy.
