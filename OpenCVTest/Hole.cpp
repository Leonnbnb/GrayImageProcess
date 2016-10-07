#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2\opencv.hpp>

#include <ctime>

//#define FILE_SAVE

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
	clock_t begin, end;

	begin = clock();

	Mat src = imread("..//imgs//hole//Hole-121.bmp", CV_LOAD_IMAGE_COLOR);
	if (!src.data) {
		cout << "Read image error" << endl;
		return -1;
	}

	cvtColor(src, src, CV_RGB2GRAY);

	Mat src_cpy;
	src.copyTo(src_cpy);

	blur(src_cpy, src_cpy, Size(9, 9));

	cvResizeWindow("Src", 800, 600);
	imshow("Src", src_cpy);

	//adaptiveThreshold(src_cpy, src_cpy, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 300, 0);
	//CV_THRESH_OTSU
	threshold(src_cpy, src_cpy, 0, 255, CV_THRESH_OTSU);

	cvResizeWindow("Threshold", 800, 600);
	imshow("Threshold", src_cpy);

	//Mat element0 = getStructuringElement(MORPH_RECT, Size(7, 7));
	//dilate(src_cpy, src_cpy, element0, Point(-1, -1), 1);
	//cvResizeWindow("Dilate", 800, 600);
	//imshow("Dilate", src_cpy);


	//Canny(src_cpy, src_cpy, 200, 128);
	//cvResizeWindow("Canny", 800, 600);
	//imshow("Canny", src_cpy);
	
	waitKey(0);



	return 0;
}