#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

void toNegative(const Mat& input, Mat& output) {

    output = input.clone();
    int height = output.rows; //高度
	int width = output.cols; //寬度

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int b = output.at<Vec3b>(row, col)[0]; //顯示圖片像素
			int g = output.at<Vec3b>(row, col)[1]; //顯示圖片像素
			int r = output.at<Vec3b>(row, col)[2]; //顯示圖片像素
			output.at<Vec3b>(row, col)[0] = 255 - b ; //將圖片像素反轉
			output.at<Vec3b>(row, col)[1] = 255 - g;
			output.at<Vec3b>(row, col)[2] = 255 - r;
		}
	}
}

int main(int argc, char** argv)
{
    Mat lena;
    Mat negative_lena;
    
    // 1. Image Read/Write
    lena = imread("../data/InputImage1.bmp");
    imshow("lena", lena);
    imwrite("../data/lena.bmp", lena);

    // 2. Generate a color negative image 
    toNegative(lena, negative_lena);
    imshow("negative_lena", negative_lena);
    imwrite("../output/negative_lena.bmp", negative_lena);

    // 3. Split and reassemble into new image 

    Point corners[4][3];

    int cols = lena.cols;
    int rows = lena.rows;

    corners[0][0] = Point(0, 0);
    corners[0][1] = Point(cols/2, rows/2);
    corners[0][2] = Point(cols, 0);

    corners[1][0] = Point(0, 0);
    corners[1][1] = Point(cols/2, rows/2);
    corners[1][2] = Point(0, rows);

    corners[2][0] = Point(0, rows);
    corners[2][1] = Point(cols/2, rows/2);
    corners[2][2] = Point(cols, rows);

    corners[3][0] = Point(cols, 0);
    corners[3][1] = Point(cols/2, rows/2);
    corners[3][2] = Point(cols, rows);

    const Point* corner_list0[1] = {corners[0]};
    const Point* corner_list1[1] = {corners[1]};
    const Point* corner_list2[1] = {corners[2]};
    const Point* corner_list3[1] = {corners[3]};

    int num_points = 3;
    int num_polygons = 1;
    int line_type = 8;

    // 0 --> 3 向右轉90度
    Mat mask0 = cv::Mat::zeros(lena.rows, lena.cols, CV_8UC1);
    fillPoly(mask0, corner_list0, &num_points, num_polygons, Scalar(255, 255, 255), line_type);
    Mat result0(lena.size(), lena.type(), Scalar::all(0));
    lena.copyTo(result0, mask0);
    rotate(result0, result0, ROTATE_90_CLOCKWISE);
    imshow("result0", result0);
    // print(result0);

    // 1 --> 2 向左轉90度
    Mat mask1 = cv::Mat::zeros(lena.rows, lena.cols, CV_8UC1); 
    fillPoly(mask1, corner_list1, &num_points, num_polygons, Scalar(255, 255, 255), line_type);
    Mat result1(lena.size(), lena.type(), Scalar(0, 0, 0));
    lena.copyTo(result1, mask1);
    rotate(result1, result1, ROTATE_90_COUNTERCLOCKWISE);
    imshow("result1", result1);

    // 2 --> 1 向右轉90度
    Mat mask2 = cv::Mat::zeros(lena.rows, lena.cols, CV_8UC1); 
    fillPoly(mask2, corner_list2, &num_points, num_polygons, Scalar(255, 255, 255), line_type);    
    Mat result2(lena.size(), lena.type(), Scalar(0, 0, 0));
    lena.copyTo(result2, mask2);
    rotate(result2, result2, ROTATE_90_CLOCKWISE);
    imshow("result2", result2);

    // 3 --> 0 向左轉90度
    Mat mask3 = cv::Mat::zeros(lena.rows, lena.cols, CV_8UC1); 
    fillPoly(mask3, corner_list3, &num_points, num_polygons, Scalar(255, 255, 255), line_type);
    Mat result3(lena.size(), lena.type(), Scalar(0, 0, 0));
    lena.copyTo(result3, mask3);
    rotate(result3, result3, ROTATE_90_COUNTERCLOCKWISE);
    imshow("result3", result3);

    // reassemble
    Mat resultImg = Mat(512, 512, CV_8UC3, Scalar::all(0));
    addWeighted(result0, 1, result1, 1, 0, resultImg);
    addWeighted(resultImg, 1, result2, 1, 0, resultImg);
    addWeighted(resultImg, 1, result3, 1, 0, resultImg);
    imshow("reassemble_lena", resultImg);
    imwrite("../output/reassemble_lena.bmp", resultImg);


    waitKey(0);
    return 0;
}