#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
using namespace cv;

#define __DEBUG__

typedef struct {
	int r_max;
	int r_min;
	int c_max;
	int c_min;
	int TL[2];
	int TR[2];
	int BL[2];
	int BR[2];
	int LT[2];
	int LB[2];
	int RT[2];
	int RB[2];
	int area;
	int r_centroid;
	int c_centroid;
	double max_len;
	double max_len_tan;
} PR;

Mat gray(Mat src);
Mat binary(Mat src);
Mat binary_inv(Mat src);
float var(int* arr, int left,int right);
Mat DILATE(Mat src, int iter, int num);
Mat ERODE(Mat src, int iter, int num);
Mat Connected(Mat src);
Mat check(Mat src, Mat m0);
int partition(int* arr, int start, int end, int* idx);
void quickSort(int* arr, int start, int end, int* idx);
PR property(Mat src, int top);
Mat drawing(Mat src, PR p, int* BGR);

clock_t t0; clock_t t1;
clock_t t2; clock_t t3;
clock_t t4; clock_t t5;

double binarizing_time;
double cc_time;
double drawing_time;
double property_time;
double morphology_time;

int main()
{
	// Image Read
	Mat origin = imread("../data/island.bmp");

	clock_t t0 = clock();

	Mat gray_result = gray(origin);
	imshow("gray", gray_result);

	Mat binary_result = binary(gray_result);
	imshow("binary", binary_result);

	Mat binaryInv_result = binary_inv(binary_result);
	imshow("binaryInv", binaryInv_result);

	binarizing_time = clock() - t0;

	
	clock_t t1 = clock();
	Mat map = Connected(binary_result);
	double cc_time = clock() - t1;

	int BGR[] = { 255,0,0 };

	clock_t t2 = clock();
	PR p = property(map, 1);
	double property_time = clock() - t2;

	clock_t t3 = clock();
	Mat box = drawing(origin, p, BGR);
	double drawing_time = clock() - t3;

	for (int i = 2; i <= 3; i++) {
		int temp = BGR[i - 2];
		BGR[i - 2] = BGR[i - 1];
		BGR[i - 1] = temp;
		p = property(map, i);
		box = drawing(box, p, BGR);
	}
	imshow("drawing", box);

	clock_t t4 = clock();
	binary_result = ERODE(binary_result, 1, 5);
	binary_result = DILATE(binary_result, 1, 5);
	binary_result = DILATE(binary_result, 2, 5);
	binary_result = ERODE(binary_result, 1, 5);
	imshow("morphology", binary_result);
	double morphology_time = clock() - t4;

	// time analysis
	printf("\nbinarizing time = %.2f ms", binarizing_time);
	printf("\nconnected component time = %.2f ms", cc_time);
	printf("\nproperty time = %.2f ms", property_time);
	printf("\ndrawing time = %.2f ms", drawing_time);
	printf("\nmorphology time = %.2f ms", morphology_time);

	waitKey(0);
	return 0;
}

Mat gray(Mat src) {
	Mat gray_img = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    
	int rows = src.rows;
    int cols = src.cols;
    int channel = src.channels();

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (channel == 3) {
                //opencv 圖片的顏色順序為 B G R ，如下
                int b = src.at<Vec3b>(i, j)[0];
                int g = src.at<Vec3b>(i, j)[1];
                int r = src.at<Vec3b>(i, j)[2];

                gray_img.at<uchar>(i, j) = 0.299 * r + 0.587 * g + 0.114 * b;
            }
        }
    }
	return(gray_img);
}

Mat binary(Mat src) {
	Mat binary_img = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
	
	int rows = src.rows;
    int cols = src.cols;
    int channel = src.channels();

	for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
			int grayPixels = src.at<uchar>(i, j);
            if (grayPixels >= 105) {
                binary_img.at<uchar>(i, j) = 255;
            } else {
				binary_img.at<uchar>(i, j) = 0;
			}

        }
    }
	return(binary_img);
}

Mat binary_inv(Mat src) {
	Mat binaryInv_img = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	int rows = src.rows;
    int cols = src.cols;
    int channel = src.channels();

	for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
			binaryInv_img.at<uchar>(i, j) = ~src.at<uchar>(i, j);
        }
    }
	return(binaryInv_img);
}

Mat DILATE(Mat src, int iter, int num) {
	if (iter == 0) {
		// imshow("dilate", src);
		// waitKey(0);
		// destroyAllWindows();
		return src;
	}
	int half = num / 2.;
	Mat dst = src.clone();
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			dst.at<uchar>(r, c) = src.at<uchar>(r, c);
			for (int i = 0; i < num; i++) {
				for (int j = 0; j < num; j++) {
					int c0 = r - half + i;
					int c1 = c - half + j;
					int t = c0 < 0 || c1 < 0 || c0 >= src.rows || c1 >= src.cols ? 0 : src.at<uchar>(c0, c1);
					dst.at<uchar>(r, c) |= t;
				}
			}
		}
	}
	return DILATE(dst, iter - 1, num);
}

Mat ERODE(Mat src, int iter, int num) {
	if (iter == 0) {
		// imshow("erode", src);
		// waitKey(0);
		// destroyAllWindows();
		return src;
	}
	int half = num / 2.;
	Mat dst = src.clone();
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			dst.at<uchar>(r, c) = src.at<uchar>(r, c);
			for (int i = 0; i < num; i++) {
				for (int j = 0; j < num; j++) {
					int c0 = r - half + i;
					int c1 = c - half + j;
					int t = c0 < 0 || c1 < 0 || c0 >= src.rows || c1 >= src.cols ? 255 : src.at<uchar>(c0, c1);
					dst.at<uchar>(r, c) &= t;
				}
			}
		}
	}
	return ERODE(dst, iter - 1, num);
}

float var(int* hist, int left, int right) {
	int s = 0;
	for (int i = left; i < right + 1; i++) {
		s += hist[i];
	}
	if (s == 0) {
		return 0;
	}
	float u = 0;
	for (int i = left; i < right + 1; i++) {
		u += i * hist[i];
	}
	u /= s;
	float v = 0;
	for (int i = left; i < right + 1; i++) {
		v += (i - u) * (i - u) * hist[i];
	}
	v /= s;
	return v;
}

Mat check(Mat src, Mat m0) {
	Mat map = m0.clone();
	int label = 0;
	int r_start = 0;
	int c_start = 0;
	int r_end = src.rows - 1;
	int c_end = src.cols - 1;
	for (int r = r_start; r <= r_end; r++) {
		for (int c = c_start; c <= c_end; c++) {
			int label_up = r - 1 < r_start ? 0 : map.at<int>(r - 1, c);
			int label_left = c - 1 < c_start ? 0 : map.at<int>(r, c - 1);
			if (src.at<uchar>(r, c) == 255) { // background
				map.at<int>(r, c) = 0;
			}
			else if (label_up == 0 && label_left == 0 && map.at<int>(r, c) == 0) { // only run once
				map.at<int>(r, c) = ++label;
			}
			else if (label_up == 0 && label_left == 0) {
				map.at<int>(r, c) = map.at<int>(r, c);
			}
			else if (label_up == 0) {
				map.at<int>(r, c) = label_left;
			}
			else if (label_left == 0) {
				map.at<int>(r, c) = label_up;
			}
			else if (label_up == label_left) {
				map.at<int>(r, c) = label_up;
			}
			else if (label_up > label_left) {
				map.at<int>(r, c) = label_left;
			}
			else {
				map.at<int>(r, c) = label_up;
			}
		}
	}
	for (int r = r_end; r >= r_start; r--) {
		for (int c = c_end; c >= c_start; c--) {
			int label_down = r + 1 > r_end ? 0 : map.at<int>(r + 1, c);
			int label_right = c + 1 > c_end ? 0 : map.at<int>(r, c + 1);
			if (src.at<uchar>(r, c) == 255) { // background
				map.at<int>(r, c) = 0;
			}
			else if (label_down == 0 && label_right == 0) {
				map.at<int>(r, c) = map.at<int>(r, c);
			}
			else if (label_down == 0) {
				map.at<int>(r, c) = label_right;
			}
			else if (label_right == 0) {
				map.at<int>(r, c) = label_down;
			}
			else if (label_down == label_right) {
				map.at<int>(r, c) = label_down;
			}
			else if (label_down > label_right) {
				map.at<int>(r, c) = label_right;
			}
			else {
				map.at<int>(r, c) = label_down;
			}
		}
	}
	bool k = true;
	for (int r = r_start; r <= r_end; r++) {
		for (int c = c_start; c <= c_end; c++) {
			if (map.at<int>(r, c) != m0.at<int>(r, c)) {
				k = false;
			}
		}
	}
	if (k) {
#ifndef __DEBUG__
		printf("2\n");
#endif // !__DEBUG__
		return map;
	}
	else {
#ifndef __DEBUG__
		printf("1\n");
#endif // !__DEBUG__
		return check(src, map);
	}
}

Mat Connected(Mat src) {
	Mat map = Mat(src.rows, src.cols, CV_32SC1);
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			map.at<int>(r, c) = 0;
		}
	}
	map = check(src, map);
	int max = 0;
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			max = max < map.at<int>(r, c) ? map.at<int>(r, c) : max;
		}
	}
	int* R = (int*)malloc(sizeof(int) * max);
	int* G = (int*)malloc(sizeof(int) * max);
	int* B = (int*)malloc(sizeof(int) * max);
	srand(time(NULL));
	for (int i = 0; i < max; i++) {
		R[i] = rand() % 255 + 1;
		G[i] = rand() % 255 + 1;
		B[i] = rand() % 255 + 1;
	}
	Mat dst = Mat(src.rows, src.cols, CV_8UC3);
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			int k = map.at<int>(r, c);
			if (k == 0) {
				dst.at<Vec3b>(r, c)[0] = 0;
				dst.at<Vec3b>(r, c)[1] = 0;
				dst.at<Vec3b>(r, c)[2] = 0;
			}
			else {
				dst.at<Vec3b>(r, c)[0] = B[k-1];
				dst.at<Vec3b>(r, c)[1] = G[k-1];
				dst.at<Vec3b>(r, c)[2] = R[k-1];
			}
		}
	}
	imshow("CC", dst);
	waitKey(0);
	destroyAllWindows();
	return map;
}

Mat drawing(Mat src, PR p, int* BGR) {
	Mat dst = src.clone();
	int r_max = p.r_max;
	int r_min = p.r_min;
	int c_max = p.c_max;
	int c_min = p.c_min;
	for (int r = 0; r < dst.rows; r++) {
		for (int i = 0; i < 10; i++) {
			if (r >= r_min && r <= r_max) {
				dst.at<Vec3b>(r, c_max - i)[0] = BGR[0];
				dst.at<Vec3b>(r, c_max - i)[1] = BGR[1];
				dst.at<Vec3b>(r, c_max - i)[2] = BGR[2];
				dst.at<Vec3b>(r, c_min + i)[0] = BGR[0];
				dst.at<Vec3b>(r, c_min + i)[1] = BGR[1];
				dst.at<Vec3b>(r, c_min + i)[2] = BGR[2];
			}
		}
	}
	for (int c = 0; c < dst.cols; c++) {
		for (int i = 0; i < 10; i++) {
			if (c >= c_min && c <= c_max) {
				dst.at<Vec3b>(r_max - i, c)[0] = BGR[0];
				dst.at<Vec3b>(r_max - i, c)[1] = BGR[1];
				dst.at<Vec3b>(r_max - i, c)[2] = BGR[2];
				dst.at<Vec3b>(r_min + i, c)[0] = BGR[0];
				dst.at<Vec3b>(r_min + i, c)[1] = BGR[1];
				dst.at<Vec3b>(r_min + i, c)[2] = BGR[2];
			}
		}
	}
	return dst;
}

int partition(int* arr, int start, int end, int* idx) {
	int pivot = start;
	for (int i = start; i <= end; i++) {
		if (arr[i] > arr[end]) {
			int temp = arr[i];
			arr[i] = arr[pivot];
			arr[pivot] = temp;
			int temp_idx = idx[i];
			idx[i] = idx[pivot];
			idx[pivot] = temp_idx;
			pivot++;
		}
	}
	int temp = arr[end];
	arr[end] = arr[pivot];
	arr[pivot] = temp;
	int temp_idx = idx[end];
	idx[end] = idx[pivot];
	idx[pivot] = temp_idx;
	return pivot;
}

void quickSort(int* arr, int start, int end, int* idx) {
	if (start>=end||end<0) {
		return;
	}
	int pivot = partition(arr, start, end, idx);
	quickSort(arr, start, pivot-1, idx);
	quickSort(arr, pivot+1, end, idx);
}

PR property(Mat src, int top) {
	PR p;
	int max = 0;
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			max = max < src.at<int>(r, c) ? src.at<int>(r, c) : max;
		}
	}
	int* list = (int*)malloc(sizeof(int) * max);
	int* idx = (int*)malloc(sizeof(int) * max);
	for (int i = 0; i < max; i++) {
		list[i] = 0;
		idx[i] = i + 1;
	}
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			//list[src.at<int>(r, c) - 1]++;
			if (src.at<int>(r, c) != 0) {
				list[src.at<int>(r, c)-1]++;
			}
		}
	}
	int len = 0;
	for (int i = 0; i < max; i++) {
		if (list[i] != 0) {
			len++;
		}
	}
	int* list_n0 = (int*)malloc(sizeof(int) * len);
	int* idx_n0 = (int*)malloc(sizeof(int) * len);
	int j = 0;
	for (int i = 0; i < max; i++) {
		if (list[i] != 0) {
			list_n0[j] = list[i];
			idx_n0[j] = idx[i];
			j++;
		}
	}
	quickSort(list_n0, 0, len - 1, idx_n0);
	int* Pv = (int*)malloc(sizeof(int) * src.cols);
	for (int i = 0; i < src.cols; i++) {
		Pv[i] = 0;
	}
	int* Ph = (int*)malloc(sizeof(int) * src.rows);
	for (int i = 0; i < src.rows; i++) {
		Ph[i] = 0;
	}
	for (int r = 0; r < src.rows; r++) {
		for (int c = 0; c < src.cols; c++) {
			if (src.at<int>(r, c) == idx_n0[top - 1]) {
				Pv[c]++;
				Ph[r]++;
			}
		}
	}
	int r_max = 0;
	int r_min = src.rows - 1;
	for (int i = 0; i < src.rows; i++) {
		if (Ph[i] != 0) {
			r_max = r_max < i ? i : r_max;
			r_min = r_min > i ? i : r_min;
		}
	}
	int c_max = 0;
	int c_min = src.cols - 1;
	for (int i = 0; i < src.cols; i++) {
		if (Pv[i] != 0) {
			c_max = c_max < i ? i : c_max;
			c_min = c_min > i ? i : c_min;
		}
	}
	int area = 0;
	for (int i = 0; i < src.cols; i++) {
		area += Pv[i];
	}
	int r_centroid = 0;
	for (int i = 0; i < src.rows; i++) {
		r_centroid += (i * Ph[i]);
	}
	r_centroid /= area;
	int c_centroid = 0;
	for (int i = 0; i < src.cols; i++) {
		c_centroid += (i * Pv[i]);
	}
	c_centroid /= area;
	
	p.c_max = c_max;
	p.c_min = c_min;
	p.r_max = r_max;
	p.r_min = r_min;
	p.area = area;
	p.r_centroid = r_centroid;
	p.c_centroid = c_centroid;

	int TL[] = { r_min,src.cols - 1 };
	int TR[] = { r_min,0 };
	int BL[] = { r_max,src.cols - 1 };
	int BR[] = { r_max,0 };
	int LT[] = { src.rows - 1,c_min };
	int LB[] = { 0,c_min };
	int RT[] = { src.rows - 1,c_max };
	int RB[] = { 0,c_max };
	for (int r = 0; r < src.rows; r++) {
		if (src.at<int>(r, c_min) == idx_n0[top - 1]) {
			LT[0] = LT[0] > r ? r : LT[0];
			LB[0] = LB[0] < r ? r : LB[0];
		}
		if (src.at<int>(r, c_max) == idx_n0[top - 1]) {
			RT[0] = RT[0] > r ? r : RT[0];
			RB[0] = RB[0] < r ? r : RB[0];
		}
	}
	for (int c = 0; c < src.cols; c++) {
		if (src.at<int>(r_min, c) == idx_n0[top - 1]) {
			TL[1] = TL[1] > c ? c : TL[1];
			TR[1] = TR[1] < c ? c : TR[1];
		}
		if (src.at<int>(r_max, c) == idx_n0[top - 1]) {
			BL[1] = BL[1] > c ? c : BL[1];
			BR[1] = BR[1] < c ? c : BR[1];
		}
	}
	for (int i = 0; i < 2; i++) {
		p.TL[i] = TL[i];
		p.TR[i] = TR[i];
		p.BL[i] = BL[i];
		p.BR[i] = BR[i];
		p.LT[i] = LT[i];
		p.LB[i] = LB[i];
		p.RT[i] = RT[i];
		p.RB[i] = RB[i];
	}
	int L[4];
	double tan[4];
	int max_L = 0;
	int max_L_idx = 0;
	L[0] = (TL[0] + TR[0] - BL[0] - BR[0]) * (TL[0] + TR[0] - BL[0] - BR[0]) + (TL[1] + TR[1] - BL[1] - BR[1]) * (TL[1] + TR[1] - BL[1] - BR[1]);
	L[1] = (LT[0] + LB[0] - RT[0] - RB[0]) * (LT[0] + LB[0] - RT[0] - RB[0]) + (LT[1] + LB[1] - RT[1] - RB[1]) * (LT[1] + LB[1] - RT[1] - RB[1]);
	L[2] = (TL[0] + LT[0] - RB[0] - BR[0]) * (TL[0] + LT[0] - RB[0] - BR[0]) + (TL[1] + LT[1] - RB[1] - BR[1]) * (TL[1] + LT[1] - RB[1] - BR[1]);
	L[3] = (TR[0] + LT[0] - LB[0] - BL[0]) * (TR[0] + LT[0] - LB[0] - BL[0]) + (TR[1] + LT[1] - LB[1] - BL[1]) * (TR[1] + LT[1] - LB[1] - BL[1]);
	tan[0] = -(double)(TL[0] + TR[0] - BL[0] - BR[0]) / (TL[1] + TR[1] - BL[1] - BR[1]);
	tan[1] = -(double)(LT[0] + LB[0] - RT[0] - RB[0]) / (LT[1] + LB[1] - RT[1] - RB[1]);
	tan[2] = -(double)(TL[0] + LT[0] - RB[0] - BR[0]) / (TL[1] + LT[1] - RB[1] - BR[1]);
	tan[3] = -(double)(TR[0] + LT[0] - LB[0] - BL[0]) / (TR[1] + LT[1] - LB[1] - BL[1]);
	for (int i = 0; i < 4; i++) {
		max_L = max_L < L[i] ? L[i] : max_L;
		max_L_idx = max_L < L[i] ? i : max_L_idx;
	}
	p.max_len = sqrt(max_L/4.);
	p.max_len_tan = tan[max_L_idx];


	// Report the length and orientation of the longest axis
	printf("\ntop: %d", top);
	printf("\n(r_max, c_max) = (%d, %d)", r_max, c_max);
	printf("\n(r_min, c_min) = (%d, %d)", r_min, c_min);
	printf("\n(r_centroid, c_centroid) = (%d, %d)", r_centroid, c_centroid);
	printf("\narea = %d", area);
	printf("\nmax_length = %f", sqrt(max_L / 4.));
	printf("\nmax_degree = %f\n", atan(tan[max_L_idx]));
	free(list);
	free(idx);
	free(Pv);
	free(Ph);
	return p;
}
