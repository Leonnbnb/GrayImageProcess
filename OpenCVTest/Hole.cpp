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

	Mat src = imread("..//imgs//barcodes//5.bmp", CV_LOAD_IMAGE_COLOR);
	if (!src.data) {
		cout << "Read image error" << endl;
		return -1;
	}

	cvtColor(src, src, CV_RGB2GRAY);

	Mat src_cpy;
	src.copyTo(src_cpy);

	blur(src_cpy, src_cpy, Size(9, 9));

	
	waitKey(0);



	return 0;
}