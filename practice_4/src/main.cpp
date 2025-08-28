#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, char** argv)
{   

    // 1.1 Use template matching with given template to find Mangogo
    // 1.2 Label the bounding box of the best matching

    // img1
    Mat img_source1 = imread("../data/findmango1.jpg");
    Mat img_template1 = imread("../data/mangogo1.jpg");

    Mat img_match1;

    double min_Val1, max_Val1;
    Point min_Loc1, max_Loc1;

    for (int i = 0; i < img_source1.cols ; i++) {
        for (int j = 0; j < img_source1.rows ; j++) {
            img_source1.at<Vec3b>(j, i)[0] /= 2;
            img_source1.at<Vec3b>(j, i)[1] /= 2;
            img_source1.at<Vec3b>(j, i)[2] /= 2;
        }
    }

    matchTemplate(img_source1, img_template1, img_match1, cv::TM_CCOEFF_NORMED);

    minMaxLoc(img_match1, &min_Val1, &max_Val1, &min_Loc1, &max_Loc1);

    rectangle(img_source1,Point(max_Loc1.x, max_Loc1.y) , Point(max_Loc1.x + img_template1.cols, max_Loc1.y + img_template1.rows), Scalar(0, 255, 0), 2);

    for (int ai = max_Loc1.x; ai < max_Loc1.x + img_template1.cols; ai++) {
        for (int aj = max_Loc1.y; aj < max_Loc1.y + img_template1.rows; aj++) {
                img_source1.at<Vec3b>(aj, ai)[0] *= 2;
                img_source1.at<Vec3b>(aj, ai)[1] *= 2;
                img_source1.at<Vec3b>(aj, ai)[2] *= 2;
        }
    }

    imshow("result1", img_source1);
    imwrite("../output/result1.jpg", img_source1);

    //img2
    Mat img_source2 = imread("../data/findmango2.jpg");
    Mat img_template2 = imread("../data/mangogo2.jpg");

    Mat img_match2;

    for (int i = 0; i < img_source2.cols ; i++) {
        for (int j = 0; j < img_source2.rows ; j++) {
            img_source2.at<Vec3b>(j, i)[0] /= 2;
            img_source2.at<Vec3b>(j, i)[1] /= 2;
            img_source2.at<Vec3b>(j, i)[2] /= 2;
        }
    }

    double min_Val2, max_Val2;
    Point min_Loc2, max_Loc2;

    matchTemplate(img_source2, img_template2, img_match2, cv::TM_CCOEFF_NORMED);

    minMaxLoc(img_match2, &min_Val2, &max_Val2, &min_Loc2, &max_Loc2);

    rectangle(img_source2,Point(max_Loc2.x, max_Loc2.y) , Point(max_Loc2.x + img_template2.cols, max_Loc2.y + img_template2.rows), Scalar(0, 255, 0), 2);

    for (int ai = max_Loc2.x; ai < max_Loc2.x + img_template2.cols; ai++) {
        for (int aj = max_Loc2.y; aj < max_Loc2.y + img_template2.rows; aj++) {
                img_source2.at<Vec3b>(aj, ai)[0] *= 2;
                img_source2.at<Vec3b>(aj, ai)[1] *= 2;
                img_source2.at<Vec3b>(aj, ai)[2] *= 2;
        }
    }

    imshow("result2", img_source2);
    imwrite("../output/result2.jpg", img_source2);

    // 2.1 Use template matching with multiple given templates to find Wally.
    // 2.2 Consider the distances between these templates as a prior model.
    // 2.3 Label Wally by connecting the centers of four matching results

    Mat img_sourceWally = imread("../data/findwally1.jpg");
    Mat img_wallyHat = imread("../data/hat.jpg");
    Mat img_wallyEye = imread("../data/eye.jpg");
    Mat img_wallyShirt = imread("../data/shirt.jpg");
    Mat img_wally = imread("../data/wally.jpg");

    Mat img_matchHat;
    Mat img_matchEye;
    Mat img_matchShirt;

    matchTemplate(img_sourceWally, img_wallyHat, img_matchHat, 4); 
    normalize(img_matchHat, img_matchHat, 0, 1000, NORM_MINMAX, -1, Mat());

    matchTemplate(img_sourceWally, img_wallyEye, img_matchEye, 4);
    normalize(img_matchEye, img_matchEye, 0, 1000, NORM_MINMAX, -1, Mat());

    matchTemplate(img_sourceWally, img_wallyShirt, img_matchShirt, 4); 
    normalize(img_matchShirt, img_matchShirt, 0, 1000, NORM_MINMAX, -1, Mat());

    int eye2d = 0;

    for (int i = 0; i < img_sourceWally.cols ; i++) {
        for (int j = 0; j < img_sourceWally.rows ; j++) {
            img_sourceWally.at<Vec3b>(j, i)[0] /= 2;
            img_sourceWally.at<Vec3b>(j, i)[1] /= 2;
            img_sourceWally.at<Vec3b>(j, i)[2] /= 2;
        }
    }


    int resulti = 0;
    int resultj = 0;

    for (int i = 1; i < img_matchHat.cols - 1; i++) {
        for (int j = 1; j < img_matchHat.rows - 1; j++) {
            if (img_matchHat.at<float>(j, i) > 950) { //設閥值

                for (int eyei = i; eyei < i + 10; eyei++) {
                    for (int eyej = j; eyej < j + 20; eyej++) {
                        if (eyei > img_matchHat.cols or eyej > img_matchHat.rows) { break; }
                        if (img_matchEye.at<float>(eyej, eyei) > 800) {
                            for (int eye2i = 0; eye2i < 10; eye2i++) {
                                if (img_matchEye.at<float>(eyej, eyei + eye2i) > 800) {
                                    eye2d = eye2i;

                                    for (int shirti = eyei; shirti < eyei + 10; shirti++) {
                                        for (int shirtj = eyej; shirtj < eyej + 30; shirtj++) {
                                            if (shirti > img_matchHat.cols or shirtj > img_matchHat.rows) { break; }
                                            if (img_matchShirt.at<float>(shirtj, shirti) > 850) {
                                                
                                                resulti = i;
                                                resultj = j;

                                                rectangle(img_sourceWally, Point(i-10, j-10), Point(i + 30, j + 80), Scalar(0, 255, 0), 2, 8, 0);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }

    for (int ai = resulti - 10; ai < resulti + 31; ai++) {
        for (int aj = resultj - 10; aj < resultj + 81; aj++) {
            img_sourceWally.at<Vec3b>(aj, ai)[0] *= 2;
            img_sourceWally.at<Vec3b>(aj, ai)[1] *= 2;
            img_sourceWally.at<Vec3b>(aj, ai)[2] *= 2;
        }
    }

    imshow("result3", img_sourceWally);
    imwrite("../output/result3.jpg", img_sourceWally);

    waitKey(0);
    return 0;

}