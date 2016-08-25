#include "gray_image_processing.h"
#include <iostream>

#include <ctime>

using namespace std;

int main() {

	Gray_Image_Processing action;
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//8KW.bmp");
	//BOOL rt = pImg->AttachFromFile("..//results//binaryzation//DST.bmp");
	if (!rt)
		return -1;

	CImg* img = NULL;

	clock_t begin, end;

	begin = clock();
	//bool ret = action.MedianFilter(pImg, img, 3, 3);
	bool ret = action.Contrast(pImg, img,3);
	//bool ret =mean_image(*pImg, 0, 0, pImg->GetWidthByte(), pImg->GetHeight(), 3, 3);
	//bool ret = action.Erosion(pImg, img, 3, 3);
	end = clock();

	cout << (unsigned int)(end - begin) << endl;

	if (ret)
		rt = img->SaveToFile("..//results//gip//DST2.bmp");
	if (rt == false)
		return false;

	getchar();
	return 0;
}