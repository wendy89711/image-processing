#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

#define pi 3.1415926

struct Color {
    int R;
    int G;
    int B;
};


bool Write1Img(const Mat& DataImg, const char* fileName) {
    const int headerSize = 54;

    char header[headerSize] = {
        0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
           54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 0
    };
    
    int ysize = 384;
    int xsize = 720;
    int x1, y1;
    int xc = 512, yc = 384;
    
    vector<vector<Color> > DataImg1(384, vector<Color>(720));

    for (int y = 0; y !=720; ++y) {
        for (int x = 0; x !=384; ++x) {
            
            x1 =x * cos(y* 2 * pi / 720) + xc;
            y1 =x * sin(y* 2 * pi / 720) + yc;

            DataImg1[x][y].B = DataImg.at<Vec3b>(y1, x1)[0];
            DataImg1[x][y].G = DataImg.at<Vec3b>(y1, x1)[1];
            DataImg1[x][y].R = DataImg.at<Vec3b>(y1, x1)[2];
        }
    }

    long img_size = (long)ysize * xsize * 3 + 54;
    header[2] = (unsigned char)(img_size & 0x000000ff);
    header[3] = (img_size >> 8) & 0x000000ff;
    header[4] = (img_size >> 16) & 0x000000ff;
    header[5] = (img_size >> 24) & 0x000000ff;

    long width = xsize;
    header[18] = width & 0x000000ff;
    header[19] = (width >> 8) & 0x000000ff;
    header[20] = (width >> 16) & 0x000000ff;
    header[21] = (width >> 24) & 0x000000ff;

    long height = ysize;
    header[22] = height & 0x000000ff;
    header[23] = (height >> 8) & 0x000000ff;
    header[24] = (height >> 16) & 0x000000ff;
    header[25] = (height >> 24) & 0x000000ff;

    ofstream file(fileName, ios::out | ios::binary);
    if (!file)
        return false;


    file.write(header, headerSize);
    
    for (size_t y = 0; y !=384; ++y) {
        for (size_t x = 0; x !=720; ++x) {

            int chB =  DataImg1[y][720-x].B;
            int chG =  DataImg1[y][720-x].G;
            int chR =  DataImg1[y][720-x].R;

            file.put(chB).put(chG).put(chR);
        }
    }

    file.close();

    return true;
}

bool FuncNo2(const Mat& left, const Mat& right) {
    Mat result2(480, 900, CV_8UC3, Scalar(0, 0, 0)); //生成900*480的圖片
    int xm = 0, ym = 80;
    for (int x = 0; x < 510; x++) {
        for (int y = 0; y <370 ; y++) {

            result2.at<Vec3b>(y + ym, x + xm)[2] = left.at<Vec3b>(y, x)[2];
            result2.at<Vec3b>(y + ym, x + xm)[1] = left.at<Vec3b>(y, x)[1];
            result2.at<Vec3b>(y + ym, x + xm)[0] = left.at<Vec3b>(y, x)[0];
        }
    }

    //right黏上left的四個點
    // 0:(0,0) 1:(1,0) 2:(1,1) 3:(0,1)
    float x0 = 250, x3 = 310, x2= 900, x1 = 770;
    float y0 = 130, y3 = 470, y2 = 475, y1 = 0;

    float a, b, c, d, e, f, g, h;
    float dx1, dx2, sigx, dy1, dy2, sigy;
    float A, B, C, D, E, F, G, H, I;

    dx1 = x1 - x2;
    dx2 = x3 - x2;
    sigx = x0 - x1 + x2 - x3;
    dy1= y1 - y2;
    dy2 = y3 - y2;
    sigy = y0 - y1 + y2 - y3;

    g = (sigx * dy2 - sigy * dx2) / (dx1 * dy2 - dy1 * dx2);
    h = (dx1 * sigy - dy1 * sigx) / (dx1 * dy2 - dy1 * dx2);

    a = x1 - x0 + g * x1;
    b = x3 - x0 + h * x3;
    c = x0;
    d = y1 - y0 + g * y1;
    e = y3 - y0 + h * y3;
    f = y0;
    float i = 1;
    float detM = (a * e * i) + (b * f * g) + (c * d * h) - (c * e * g) - (b * d * i) - (a * f * h);
    
    A = e * i - f * h;
    B = c * h - b * i;
    C = b * f - c * e;
    D = f * g - d * i;
    E = a * i - c * g;
    F = c * d - a * f;
    G = d * h - e * g;
    H = b * g - a * h;
    I = a * e - b * d;

    A = A / detM;
    B = B / detM;
    C = C / detM;
    D = D / detM;
    E = E / detM;
    F = F / detM;
    G = G / detM;
    H = H / detM;
    I = I / detM;

    printf("matrix M: \n");
    printf("%lf, %lf, %lf\n", A, D, G);
    printf("%lf, %lf, %lf\n", B, E, H);
    printf("%lf, %lf, %lf\n", C, F, I);


    float u, v;
    
    for (float xx = 250; xx < 900; xx++) {
        for (float yy = 0; yy < 480; yy++) {
            //反矩證
            u = (A * xx + B * yy + C) * 510 / (G * xx + H * yy + I);
            v = (D * xx + E * yy + F) * 370 / (G * xx + H * yy + I);

            //cresult2 << u << "," << v << endl;
            if (u<510 and v<370 and u>-1 and v>-1) {
                result2.at<Vec3b>(yy, xx)[2] = right.at<Vec3b>(int(v), int(u))[2];
                result2.at<Vec3b>(yy, xx)[1] = right.at<Vec3b>(int(v), int(u))[1];
                result2.at<Vec3b>(yy, xx)[0] = right.at<Vec3b>(int(v), int(u))[0];
            }
        }
    }

    imwrite("result2.bmp", result2);

    return true;
}

int main() {

    // No.1: Rectify the image of omnidirectional camera to a panoramic image
    Mat DataImg = imread("../data/360degree.bmp");

    if (!Write1Img(DataImg, "result1.bmp")) {
        cout << "error" << endl;
        return -1;
    }

    cout << "No.1 success" << endl;


    // No.2: Image stitching with the projective transform
    Mat left = imread("../data/left.bmp");
    Mat right = imread("../data/right.bmp");

    FuncNo2(left, right);
    cout << "No.2 success" << endl;


    // NO.3: Image stitching by OpenCV
    Stitcher::Mode mode = Stitcher::PANORAMA;
    vector<Mat> imgs;

    imgs.push_back(left);
    imgs.push_back(right);

    string result3_name = "result3.bmp";
    Mat result3Img;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, result3Img);
    imshow(result3_name, result3Img);
    imwrite(result3_name, result3Img);

    cout << "No.3 success" << endl;

    waitKey(0);
    return 0;
}
