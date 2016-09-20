#include "gray_image_processing.h"
#include <iostream>

#include <ctime>

using namespace std;

int main() {

	Gray_Image_Processing action;
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//1.bmp");
	//BOOL rt = pImg->AttachFromFile("..//results//binaryzation//DST.bmp");
	if (!rt)
		return -1;

	CImg* img = NULL;
	CImg* himg = NULL;
	CImg* limg = NULL;

	clock_t begin, end;

	unsigned long* hitgram = NULL;

	begin = clock();

	//bool ret = action.Scaling(pImg, himg, 1.0 / 4.0, Gray_Image_Processing::SCALING_METHOD::SC_NONE);
	//bool ret = action.FlipVertical(pImg, himg);
	bool ret = action.ClipRectangle(pImg, himg, 10, 10, 3500, 1800);
	/*bool ret = action.Rotate(pImg, himg, 2, pImg->GetWidthPixel()*0.5, pImg->GetHeight()*0.5,
		1, 1, 0, 0, 0xFF, Gray_Image_Processing::ROTATE_METHOD::RO_BILINEAR, Gray_Image_Processing::ROTATE_CLIP_METHOD::RC_ORIG);*/
	//ret = action.Scaling(himg, limg, 1.0 / 4.0, Gray_Image_Processing::SCALING_METHOD::SC_NONE);

	end = clock();

	cout << (unsigned int)(end - begin) << endl;

	if (ret)
		rt = himg->SaveToFile("..//results//gip//DST5.bmp");
	if (rt == false)
		return false;

	getchar();
	return 0;
}