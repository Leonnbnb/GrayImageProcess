#include "gray_image_processing.h"
#include <iostream>

#include <ctime>

using namespace std;


//void __declspec(naked) __stdcall SinCos(const double Angle, double& sina, double& cosa)
//{
//	__asm
//	{
//		fld  qword ptr[esp + 4]//Angle   
//		mov  esi, [esp + 12]//&sina
//		mov  edi, [esp + 16]//&cosa
//		fsincos
//		fstp qword ptr[edi]
//		fstp qword ptr[esi]
//		ret 16
//	}
//}

int main() {

	Gray_Image_Processing action;
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//1.bmp");
	//BOOL rt = pImg->AttachFromFile("..//results//binaryzation//DST.bmp");
	if (!rt)
		return -1;

	CImg* img = NULL;
	CImg* himg = NULL;

	clock_t begin, end;

	unsigned long* hitgram = NULL;

	begin = clock();

	//bool ret = action.Scaling(pImg, himg, 1.0 / 4.0, Gray_Image_Processing::SCALING_METHOD::SC_NONE);
	//bool ret = action.FlipVertical(pImg, himg);
	bool ret = action.ClipRectangle(pImg, himg, 20, 30, 500, 800);

	end = clock();

	double a = 2,s = 0, c = 0;
	action.SinCos(a, s, c);

	cout << s << " " << c << endl;

	cout << (unsigned int)(end - begin) << endl;

	if (ret)
		rt = himg->SaveToFile("..//results//gip//DST5.bmp");
	if (rt == false)
		return false;

	getchar();
	return 0;
}