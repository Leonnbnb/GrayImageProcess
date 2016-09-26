#include <iostream>
#include <vector>
#include <cmath>

#include <opencv2\opencv.hpp>

#include <ctime>

using namespace cv;
using namespace std;

#define PREVIEW
#define SIZE_THRESHOLD 300000.0
#define BLUR_SIZE Size(11, 11)
#define DILATE_ELE_SIZE Size(3, 3)
#define BINARYZATION_THRESHOLD 128
#define ERODE_ELE_SIZE Size(9, 9)

#ifdef PREVIEW
void show(string Wndname, Mat img) {
	namedWindow(Wndname.c_str(), 0);
	cvResizeWindow(Wndname.c_str(), 800, 600);
	imshow(Wndname.c_str(), img);
}
#endif

void FillInternalContours(Mat &img, double dAreaThre)
{
	double dConArea;
	vector<vector<Point > > Contours;
	CvSeq *pConInner = NULL;
	Mat Storage;
	// 查找所有轮廓   
	findContours(img, Contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	int cmin = 40;   // 最小轮廓长度  
	vector<vector<Point>>::const_iterator itc = Contours.begin();
	while (itc != Contours.end())
	{
		if ((itc->size()) < cmin)
		{
			itc = Contours.erase(itc);
		}
		else ++itc;
	}

	// 填充所有轮廓   
	drawContours(img, Contours, -1, Scalar(255, 255, 255), CV_FILLED);
}

int main(int argc, char *argv[]) {
	clock_t begin, end;

	begin = clock();
	//Load
	Mat src = imread("..//imgs//barcodes//4_rotate.bmp", CV_LOAD_IMAGE_COLOR);
	if (!src.data) {
		cout << "Read image error" << endl;
		return -1;
	}

#ifdef PREVIEW
	show("Src", src);
#endif

	//Resize
	Mat img;
	double resize_scale = 1;
	if ((src.cols*src.rows*0.8) > SIZE_THRESHOLD) {
		resize_scale = std::sqrt(SIZE_THRESHOLD / (src.cols*src.rows*1.0));
		resize(src, img, Size(0, 0), resize_scale, resize_scale, CV_INTER_NN);
	}
	else {
		src.copyTo(img);
	}

#ifdef PREVIEW
	show("After Resize", img);
#endif

	cvtColor(img, img, CV_RGB2GRAY);

	//Sobel
	Mat gx, gy;
	Sobel(img, gx, CV_32F, 1, 0, -1);
	Sobel(img, gy, CV_32F, 0, 1, -1);

	Mat angleMat, gradient;
	cartToPolar(gx, gy, gradient, angleMat);
	convertScaleAbs(gradient, gradient);

#ifdef PREVIEW
	show("Gradient", gradient);
#endif

	//Blur
	Mat after_blur;
	blur(gradient, after_blur, BLUR_SIZE);
#ifdef PREVIEW
	show("Blur", after_blur);
#endif

	//Dilate
	Mat first_dilate;
	Mat dilate_element = getStructuringElement(MORPH_RECT, DILATE_ELE_SIZE);
	dilate(after_blur, first_dilate, dilate_element, Point(-1, -1), 1);
#ifdef PREVIEW
	show("Dilate 1", first_dilate);
#endif

	//Binaryzation
	Mat after_binaryzation;
	threshold(first_dilate, after_binaryzation, BINARYZATION_THRESHOLD, 255, 0);
#ifdef PREVIEW
	show("Binaryzation", after_binaryzation);
#endif

	//	//Erode
	//	Mat after_erode;
	//	Mat erode_element = getStructuringElement(MORPH_ELLIPSE, ERODE_ELE_SIZE);
	//	erode(after_binaryzation, after_erode, erode_element, Point(-1, -1), 1);
	//#ifdef PREVIEW
	//	show("Erode", after_erode);
	//#endif


	FillInternalContours(after_binaryzation, 100);
#ifdef PREVIEW
	show("FillInternalContours", after_binaryzation);
#endif

	//Erode
	Mat after_erode;
	Mat erode_element = getStructuringElement(MORPH_ELLIPSE, ERODE_ELE_SIZE);
	erode(after_binaryzation, after_erode, erode_element, Point(-1, -1), 1);
#ifdef PREVIEW
	show("Erode", after_erode);
#endif

	//Close
	Mat after_open;
	Mat close_element = getStructuringElement(MORPH_RECT, Size(11, 11));
	morphologyEx(after_erode, after_open, MORPH_OPEN, close_element);
#ifdef PREVIEW
	show("Opened", after_open);
#endif



	Mat after_canny;
	Canny(after_open, after_canny, 250, 240, 3);
#ifdef PREVIEW
	show("Canny", after_canny);
#endif

	Mat img_cpy;
	after_canny.copyTo(img_cpy);

	vector<Vec2f> lines;
	HoughLines(after_canny, lines, 1, CV_PI / 360, 80);
	size_t size = lines.size();
	size = size > 5 ? 5 : size;
	for (size_t i = 0; i < size; i++) {
		float rho = lines[i][0];
		float theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta);
		double b = sin(theta);
		double x0 = rho*a;
		double y0 = rho*b;
		pt1.x = cvRound(x0 + 800 * (-b));
		pt1.y = cvRound(y0 + 800 * a);
		pt2.x = cvRound(x0 - 800 * (-b));
		pt2.y = cvRound(y0 - 800 * a);
		line(img_cpy, pt1, pt2, Scalar(255, 255, 255), 1, CV_AA);
	}
#ifdef PREVIEW
	show("HoughLine", img_cpy);
#endif

	//	//Close
	//	Mat after_close;
	//	Mat close_element = getStructuringElement(MORPH_RECT, Size(11, 11));
	//	morphologyEx(after_erode, after_close, MORPH_CLOSE, close_element);
	//#ifdef PREVIEW
	//	show("Closed", after_close);
	//#endif
	//
	//	//Dilate
	//	Mat second_dilate;
	//	//Mat dilate_element = getStructuringElement(MORPH_RECT, DILATE_ELE_SIZE);
	//	dilate(after_close, second_dilate, dilate_element, Point(-1, -1), 1);
	//#ifdef PREVIEW
	//	show("Dilate 2", second_dilate);
	//#endif

	FillInternalContours(img_cpy, 100);
#ifdef PREVIEW
	show("FillInternalContours 2", img_cpy);
#endif

	//Dilate
	dilate(img_cpy, img_cpy, dilate_element, Point(-1, -1), 1);
#ifdef PREVIEW
	show("Dilate L", img_cpy);
#endif

//	Mat erode_element_L = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
//	erode(img_cpy, img_cpy, erode_element_L, Point(-1, -1), 1);
//#ifdef PREVIEW
//	show("Erode L", img_cpy);
//#endif

	vector<vector<Point>> con;
	findContours(img_cpy, con, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	double max_area = 0;
	int max_index = 0;

	size_t i = 0;
	for (; i < con.size(); ++i) {
		if (contourArea(con.at(i)) > max_area) {
			max_area = contourArea(con.at(i));
			max_index = i;
		}
	}

	RotatedRect rc = minAreaRect(con.at(max_index));
	Point2f vertices[4];
	rc.points(vertices);

	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 0));
	/*Rect brect = rc.boundingRect();
	rectangle(img, brect, Scalar(255, 255, 255));*/

#ifdef PREVIEW
	show("Result in Small", img);
#endif


	//	for (int i = 0; i < 4; i++)
	//		line(src, Point(vertices[i].x / resize_scale, vertices[i].y / resize_scale),
	//			Point(vertices[(i + 1) % 4].x / resize_scale, vertices[(i + 1) % 4].y / resize_scale), Scalar(255, 255, 255));
	//#ifdef PREVIEW
	//	show("Result in Src", src);
	//#endif
	//

	end = clock();

	//cout << end - begin << endl;

	waitKey(0);

	return 0;

}