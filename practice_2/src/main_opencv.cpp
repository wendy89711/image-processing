#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <time.h>

using namespace cv;
using namespace std;

Mat img; //island_gray
Mat island;
int threshval = 107;

clock_t t0; clock_t t1;
clock_t t2; clock_t t3;
clock_t t4; clock_t t5;

double binarizing_time;
double cc_time;
double drawing_time;
double p1_time; double p2_time;
double property_time;
double morphology_time;

static void on_trackbar(int, void*)
{
    Mat bw = threshval < 128 ? (img < threshval) : (img > threshval);
    Mat labelImage(img.size(), CV_32S);
    int nLabels = connectedComponents(bw, labelImage, 4);
    std::vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);//background
    for(int label = 1; label < nLabels; ++label){
        colors[label] = Vec3b( (rand()&255), (rand()&255), (240&255) );
    }
    Mat dst(img.size(), CV_8UC3);
    for(int r = 0; r < dst.rows; ++r){
        for(int c = 0; c < dst.cols; ++c){

            int label = labelImage.at<int>(r, c);
            Vec3b &pixel = dst.at<Vec3b>(r, c);
            pixel = colors[label];
         }
    }

    imshow( "colors", dst );

    cvtColor(dst, dst, COLOR_BGR2GRAY);
    threshold(dst, dst, 107, 255, THRESH_BINARY);

    cc_time = clock() - t1;
    
    t2 = clock();
    // the centroid and area
    cv::Mat labels, stats, centroid;
    int connectivity = 4; 
    int label_count = cv::connectedComponentsWithStats(dst, labels, stats, centroid, connectivity);
    
    int r1; int c1;
    int r2; int c2;
    int r3; int c3;
    int r4; int c4;
    int r5; int c5;
    int r6; int c6;

    int M1; int M2; int M3; int M4;
    double tan1; double tan2; double tan3; double tan4;

    for (int i = 0; i < label_count; i++)
    {
        int x = stats.at<int>(i, cv::CC_STAT_LEFT);
        int y = stats.at<int>(i, cv::CC_STAT_TOP);
        int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        int area = stats.at<int>(i, cv::CC_STAT_AREA);
        double cx = centroid.at<double>(i, 0);
        double cy = centroid.at<double>(i, 1);

        p1_time = clock() - t2; // p1 = 重心、面積

        t3 = clock();
        // RGB bounding box
        if(area > 100000) {

            if (x == 0 && y == 0) {
                rectangle(island, Point(x,y), Point(w, h), Scalar(0, 255, 0));
                c2 = x+w; r2 = y;
                c5 = x; r5 = y+h;
                // cout << "success";
            } else if (y == 0) {
                rectangle(island, Point(x,y), Point(x+w, y+h), Scalar(255, 0, 0));
                c1 = x; r1 = y;
                c4 = x+w; r4 = y+h;
            } else if (x == 0) {
                rectangle(island, Point(x,y), Point(x+w, y+h), Scalar(0, 0, 255));
                c3 = x+w; r3 = y;
                c6 = x; r6 = y;
            }

            printf("\nx,y,w,h = (%d,%d,%d,%d) \n", x, y, w, h);
            printf("area = %d \n", area);
            printf("centroid = (%f,%f) \n", cx, cy);            
        }
    }
    imshow( "rectangle", island );

    drawing_time = clock() - t3;

    printf("\n(r1, c1) = (%d, %d) \n", r1, c1);
    printf("(r2, c2) = (%d, %d) \n", r2, c2);
    printf("(r3, c3) = (%d, %d) \n", r3, c3);
    printf("(r4, c4) = (%d, %d) \n", r4, c4);
    printf("(r5, c5) = (%d, %d) \n", r5, c5);
    printf("(r6, c6) = (%d, %d) \n", r6, c6);


    t4 = clock();
    M1 = sqrt(pow(r1-r4, 2) + pow(c1-c4, 2)); // 平方開根號
    M2 = sqrt(pow(r2-r4, 2) + pow(c2-c4, 2));
    M3 = sqrt(pow(r2-r6, 2) + pow(c2-c6, 2));
    M4 = sqrt(pow(r2-r5, 2) + pow(c2-c5, 2));

    tan1 = atan((r1-r4)/-(c1-c4));
    tan2 = atan((r2-r4)/-(c2-c4));
    tan3 = atan((r2-r6)/-(c2-c6));
    tan4 = atan((r2-r5)/-(c2-c5));

    // Report the length and orientation of the longest axis
    printf("\nM1 = %d\n", M1);
    printf("M2 = %d\n", M2);
    printf("M3 = %d\n", M3);
    printf("M4 = %d\n", M4);

    printf("\ntan1 = %f\n", tan1);
    printf("tan2 = %f\n", tan2);
    printf("tan3 = %f\n", tan3);
    printf("tan4 = %f\n", tan4);

    p2_time = clock() - t4;
    property_time = p1_time + p2_time;
    
}


int main(int argc, char** argv)
{
    
    Mat island_gray;
    Mat island_threshold;

    // 1. Generate a binarized image
    island = imread("../data/island.bmp");

    t0  = clock();

    cvtColor(island, img, COLOR_BGR2GRAY);

    threshold(img, island_threshold, 107, 255, THRESH_BINARY); //(input, output, 數值頭, 數值尾)
    imshow("island_threshold", island_threshold);
    // imwrite("../island_threshold.bmp", island_threshold);

    binarizing_time = clock() - t0;
    
    
    // 2. Label the forest regions with 4-connected neighbor
    t1 = clock();
    on_trackbar(threshval, 0);

    // 3. Use the morphology algorithms to reserve the road
    t5 = clock();
    Mat dst1;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kernel1 = getStructuringElement(MORPH_RECT, Size(8, 8));

    erode(island_threshold, dst1, kernel); // 侵蝕
    dilate(dst1, dst1, kernel); // 膨脹（用長方形）
    dilate(dst1, dst1, kernel);
    dilate(dst1, dst1, kernel);
    dilate(dst1, dst1, kernel);
    erode(dst1, dst1, kernel);
    imshow( "result", dst1);

    morphology_time = clock() - t5;

    // 4. Analyze and print the computational time of your program
    printf("\nbinarizing time = %.2f ms", binarizing_time);
    printf("\nconnected component time = %.2f ms", cc_time);
    printf("\ndrawing time = %.2f ms", drawing_time);
    printf("\nproperty time = %.2f ms", property_time);
    printf("\nmorphology time = %.2f ms", morphology_time);

    waitKey(0);
    return 0;

}