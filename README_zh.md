# Image Processing Practice

- 本專案由一系列 **影像處理** 的練習程式組成，每個練習以 practice_X 形式存在，包含輸入資料、程式碼與輸出結果。
- 此專案使用 CMake 進行建置管理，並搭配 C++ 進行實作。
- 每個練習題，分別以純 C++（不使用 OpenCV）及使用 OpenCV 兩種方式實現。

## 環境需求
- CMake (版本 ≥ 3.10)
- C++ 編譯器 (g++ / clang++)
- OpenCV


## 專案結構
```bash
image_process/
│── practice_1/        # 練習一：BMP 影像的讀寫，並完成顏色負片轉換與色彩通道分離重組
│   ├── data/          # 輸入影像資料
│   ├── output/        # 輸出結果 (處理後影像)
│   ├── src/           # 原始程式碼
│   ├── CMakeLists.txt # 編譯設定
│   └── README.md      # 子專案說明
│
│── practice_2/        # 練習二：對影像二值化並進行 4-連通元件標記與森林區域偵測質心與面積等
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
│── practice_3/        # 練習三：全向攝影機影像展平為全景圖，並利用投影轉換與 OpenCV 分別進行影像拼接
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
│── practice_4/        # 練習四：透過影像處理與比對方法，尋找圖片中的 Mango 與 Wally
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
└── .gitignore         # Git 忽略規則
```
