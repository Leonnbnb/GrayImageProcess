{
//namedWindow("Source", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Source", 800, 600);
//imshow("Source", src);



Mat img;
cvtColor(src, img, CV_RGB2GRAY);

begin = clock();


Canny(img, img, 200, 200, 3);
namedWindow("Canny", 0/*CV_WINDOW_AUTOSIZE*/);
cvResizeWindow("Canny", 800, 600);
imshow("Canny", img);

//Mat gx, gy;
//Sobel(img, gx, CV_32F, 1, 0, -1);
//Sobel(img, gy, CV_32F, 0, 1, -1);

//Mat gradient;
//subtract(gx, gy, gradient);
//convertScaleAbs(gradient, gradient);
//namedWindow("Gradient", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Gradient", 800, 600);
//imshow("Gradient", gradient);

//Mat after_blur;
//blur(gradient, after_blur, Size(9, 9));
//namedWindow("Blur", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Blur", 800, 600);
//imshow("Blur", after_blur);

//---

//Mat first_dilate;
//Mat element0 = getStructuringElement(MORPH_RECT, Size(3, 3));
//dilate(after_blur, first_dilate, element0, Point(-1, -1), 1);
//namedWindow("F-Dilate", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("F-Dilate", 800, 600);
//imshow("F-Dilate", first_dilate);

//Mat after_threshold;
//threshold(after_blur, after_threshold, 128, 255, 0);


//Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
//dilate(after_threshold, after_threshold, element1, Point(-1, -1), 1);

//----

//namedWindow("Threshold", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Threshold", 800, 600);
//imshow("Threshold", after_threshold);

//------
//Mat after_dilate;
//Mat element0 = getStructuringElement(MORPH_RECT, Size(9, 9));
//dilate(after_threshold, after_dilate, element0, Point(-1, -1), 1);
//namedWindow("Dilate", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Dilate", 800, 600);
//imshow("Dilate", after_dilate);
//------

//Mat dst;
//dst = Mat::zeros(src.size(), CV_32FC1);
////src.copyTo(dst);

//cornerHarris(after_threshold, dst, 1, 3, 0.04, BORDER_DEFAULT);

//normalize(dst, dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
//convertScaleAbs(dst, dst);

//threshold(dst, dst, 10, 255, 0);
//--------

///// Drawing a circle around corners  
//for (int j = 0; j < dst.rows; j++)
//{
//	for (int i = 0; i < dst.cols; i++)
//	{
//		if ((int)dst.at<float>(j, i) > 200)
//		{
//			circle(dst, Point(i, j), 5, Scalar(0), 2, 8, 0);
//			//circle(src, Point(i, j), 5, Scalar(255, 0, 0), -1, 8, 0);
//		}
//	}
//}

//--------

//namedWindow("Corner", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Corner", 800, 600);
//imshow("Corner", dst);


//-------


//Mat opened;
//Mat ele = getStructuringElement(MORPH_RECT, Size(7, 7));
//morphologyEx(dst, opened, MORPH_OPEN, ele);
//namedWindow("Opened", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Opened", 800, 600);
//imshow("Opened", opened);

//Mat after_dilate;
//Mat element0 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
//dilate(opened, after_dilate, element0, Point(-1, -1), 1);
//namedWindow("Dilate", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Dilate", 800, 600);
//imshow("Dilate", after_dilate);


//Mat opened1;
//Mat ele1 = getStructuringElement(MORPH_RECT, Size(50, 50));
//morphologyEx(after_dilate, opened1, MORPH_OPEN, ele1);
//namedWindow("ReOpened", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("ReOpened", 800, 600);
//imshow("ReOpened", opened1);

//namedWindow("Source", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Source", 800, 600);
//imshow("Source", src);


//------


Mat src_cpy;
src.copyTo(src_cpy);

vector<Vec4i> lines;
HoughLinesP(img, lines, 1, CV_PI / 90, 80, 5, 5);
for (size_t i = 0; i < lines.size(); i++)
{
	line(src_cpy, Point(lines[i][0], lines[i][1]),
		Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 2, 8);
	//cout << "X1: " << lines[i][0] << " \tY1: " << lines[i][1] << " ----> " << " X2: " << lines[i][2] << " \tY2:" << lines[i][3] << endl;
}


//vector<Vec2f> lines;
//HoughLines(after_threshold, lines, 1, CV_PI / 180, 100);
//for (size_t i = 0; i<lines.size() * 0.1; i++) {
//	float rho = lines[i][0];
//	float theta = lines[i][1];
//	Point pt1, pt2;
//	double a = cos(theta);
//	double b = sin(theta);
//	double x0 = rho*a;
//	double y0 = rho*b;
//	pt1.x = cvRound(x0 + 50 * (-b));
//	pt1.y = cvRound(y0 + 50 * a);
//	pt2.x = cvRound(x0 - 50 * (-b));
//	pt2.y = cvRound(y0 - 50 * a);
//	line(src_cpy, pt1, pt2, Scalar(0, 0, 255), 0.5, CV_AA);
//}

namedWindow("Result", 0/*CV_WINDOW_AUTOSIZE*/);
cvResizeWindow("Result", 800, 600);
imshow("Result", src_cpy);


//----------------------------------------------

//vector<vector<Point>> con;
//findContours(after_threshold, con, RETR_EXTERNAL, CHAIN_APPROX_NONE);

//for(size_t i = 0; i < con.size(); ++i) {
//	rectangle(src_cpy, boundingRect(con.at(i)) , Scalar(0, 0, 255), CV_FILLED);
//}

//namedWindow("Result", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Result", 800, 600);
//imshow("Result", src_cpy);


//-----------------------------------------------
//Mat closed;
//Mat ele = getStructuringElement(MORPH_RECT, Size(15, 15));
//morphologyEx(after_threshold, closed, MORPH_CLOSE, ele);
//namedWindow("Closed", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Closed", 800, 600);
//imshow("Closed", closed);

//Mat after_erode;
//Mat element0 = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
//erode(closed, after_erode, element0, Point(-1, -1), 1);
//namedWindow("Erode", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Erode", 800, 600);
//imshow("Erode", after_erode);

//Mat after_dilate;
//dilate(after_erode, after_dilate, element0, Point(-1, -1), 1);
//namedWindow("Dilate", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Dilate", 800, 600);
//imshow("Dilate", after_dilate);

//vector<vector<Point>> con;
//findContours(after_dilate, con, RETR_EXTERNAL, CHAIN_APPROX_NONE);

//double max_area = 0;
//int max_index = 0;
//
////for each(auto area in con) {
////	if (contourArea(area) > max_area)
////		max_area = contourArea(area);
////}

//size_t i = 0;
//for(; i < con.size(); ++i) {
//	if (contourArea(con.at(i)) > max_area) {
//		max_area = contourArea(con.at(i));
//		max_index = i;
//	}
//}

////Rect rc = boundingRect(con.at(max_index));
//RotatedRect rc = minAreaRect(con.at(max_index));
//Point2f vertices[4];
//rc.points(vertices);

//for (int i = 0; i < 4; i++)
//	line(src, vertices[i], vertices[(i + 1)%4], Scalar(0, 255, 0));
//Rect brect = rc.boundingRect();
//rectangle(src, brect, Scalar(0, 0, 255));

////rectangle
////rotatedRectangle(src, rc, Scalar(0, 0, 255), 2);
//namedWindow("Result", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Result", 800, 600);
//imshow("Result", src);



//------------------------------------------------

//GaussianBlur(img, img, Size(3, 3), 0, 0);

//threshold(img, img, 127, 255, 0);
//namedWindow("Threshold", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Threshold", 800, 600);
//imshow("Threshold", img);

//Mat element0 = getStructuringElement(MORPH_RECT, Size(3, 3));
//erode(img, img, element0);
//namedWindow("Erode", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Erode", 800, 600);
//imshow("Erode", img);


//Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
//dilate(img, img, element);
//namedWindow("Dilate", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Dilate", 800, 600);
//imshow("Dilate", img);


//Canny(img, img, 100, 80, 3);
//namedWindow("Canny", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Canny", 800, 600);
//imshow("Canny", img);

//vector<Vec2f> lines;
//HoughLines(img, lines, 1, CV_PI / 360, 130);
//for (size_t i = 0; i<lines.size(); i++) {
//	float rho = lines[i][0];
//	float theta = lines[i][1];
//	Point pt1, pt2;
//	double a = cos(theta);
//	double b = sin(theta);
//	double x0 = rho*a;
//	double y0 = rho*b;
//	pt1.x = cvRound(x0 + 3000 * (-b));
//	pt1.y = cvRound(y0 + 3000 * a);
//	pt2.x = cvRound(x0 - 3000 * (-b));
//	pt2.y = cvRound(y0 - 3000 * a);
//	line(src, pt1, pt2, Scalar(0, 0, 255), 2, CV_AA);
//}
//namedWindow("Hough", 0/*CV_WINDOW_AUTOSIZE*/);
//cvResizeWindow("Hough", 800, 600);
//imshow("Hough", src);

}