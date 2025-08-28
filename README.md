# Image Processing Practice

- This project consists of a series of **image processing** practice programs.  
- Each practice is organized in the form of `practice_X`, containing input data, source code, and output results.  
- The project uses **CMake** for build management and is implemented in **C++**.  
- Each exercise is implemented in two ways: pure C++ (without OpenCV) and with OpenCV.  

## Requirements
- CMake (version ≥ 3.10)  
- C++ compiler (g++ / clang++)  
- OpenCV  

## Project Structure
```bash
image_process/
│── practice_1/        # Practice 1: Read/Write BMP images, perform negative color transformation, and separate/recombine color channels
│   ├── data/          # Input image data
│   ├── output/        # Output results (processed images)
│   ├── src/           # Source code
│   ├── CMakeLists.txt # Build configuration
│   └── README.md      # Subproject description
│
│── practice_2/        # Practice 2: Image binarization, 4-connected component labeling, forest region detection, centroid and area calculation
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
│── practice_3/        # Practice 3: Rectify omnidirectional camera images into panoramic view, perform image stitching with projective transform and OpenCV
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
│── practice_4/        # Practice 4: Use image processing and matching techniques to find Mango and Wally in images
│   ├── data/
│   ├── output/
│   ├── src/
│   ├── CMakeLists.txt
│   └── README.md
│
└── .gitignore         # Git ignore rules
