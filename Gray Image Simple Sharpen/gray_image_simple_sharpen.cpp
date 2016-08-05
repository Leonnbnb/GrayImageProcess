#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

//filename
#include <ctime>
#include <sstream>
#include <iomanip>

//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

//获取当前的系统时间以创建文件名
string NowTimeToFileName(const string preStr, const string suffixalNameStr) {
	static string pastTime;
	time_t t = time(NULL);
	tm tm = *localtime(&t);
	string nowTime;
	stringstream os;
	os.clear();
	os << put_time(&tm, "%y%m%d_%H%M%S");
	nowTime = os.str();
	string fileName;
	static short k = 0;
	if (nowTime != pastTime && (!nowTime.empty())) {//因为获取时间只精确到秒，但是程序可以在一秒之内创建数百个文件，所以要对文件名进行区分
		fileName = preStr + " - " + nowTime + "_0000" + suffixalNameStr;
		k = 0;
	}
	else {
		char extra[5];
		sprintf_s(extra, sizeof(extra), "%04d", ++k);//
		fileName = preStr + " - " + nowTime + "_" + extra + suffixalNameStr;
	}
	pastTime = nowTime;
	return fileName;
}

void SetHorizontalAverage(unsigned char** src,
	const unsigned long width,
	const unsigned long mask_width,
	unsigned char** &dst,
	const unsigned long line_num
)
{
	const unsigned long half_mask_width = mask_width / 2;
	long x = 0, y = line_num;

	for (x = 0; x <= half_mask_width; ++x) {
		long avg;
		long sum = 0, count = 0;
		for (long i = 0; i <= x + half_mask_width; ++i) {
			sum += src[y][i];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}

	for (; x <= width - 1 - half_mask_width; ++x) {
		long avg;
		long sum = 0, count = 0;
		for (long i = x - half_mask_width; i <= x + half_mask_width; ++i) {
			sum += src[y][i];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}

	for (; x <= width - 1; ++x) {
		long avg;
		long sum = 0, count = 0;
		for (long i = x - half_mask_width; i <= width - 1; ++i) {
			sum += src[y][i];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}
}

void SetVerticalAverage(
	unsigned char** src,
	const unsigned long height,
	const unsigned long mask_height,
	unsigned char** &dst,
	const unsigned long row_num
)
{
	const unsigned long half_mask_height = mask_height / 2;
	long x = row_num, y = 0;

	for (y = 0; y <= half_mask_height; ++y) {
		long avg;
		long sum = 0, count = 0;
		for (long i = 0; i <= y + half_mask_height; ++i) {
			sum += src[i][x];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}

	for (; y <= height - 1 - half_mask_height; ++y) {
		long avg;
		long sum = 0, count = 0;
		for (long i = y - half_mask_height; i <= y + half_mask_height; ++i) {
			sum += src[i][x];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}

	for (; y <= height - 1; ++y) {
		long avg;
		long sum = 0, count = 0;
		for (long i = y - half_mask_height; i <= height - 1; ++i) {
			sum += src[i][x];
			++count;
			avg = sum / count;
		}
		dst[y][x] = avg;
	}
}

bool Func_AverageValueFiltering(unsigned char** img,
	unsigned char** &dst,
	const unsigned long width,
	const unsigned long height,
	unsigned long mask_width,
	unsigned long mask_height) {
	//non_img
	if (img == NULL) {
		return false;
	}

	//non_mask
	if (mask_width == 0 || mask_height == 0) {
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (width > height ? height : width))
		return false;

	//1*1
	if (mask_width == 1 && mask_height == 1) {
		return true;
	}

	//even2odd
	if (mask_width % 2 == 0)
		mask_width = mask_width + 1;
	if (mask_height % 2 == 0)
		mask_height = mask_height + 1;


	if (mask_height == 1) {
		for (long i = 0; i < height; ++i) {
			SetHorizontalAverage(img, width, mask_width, dst, i);
		}
		return true;
	}

	if (mask_width == 1) {
		for (long i = 0; i < width; ++i) {
			SetVerticalAverage(img, height, mask_height, dst, i);
		}

		return true;
	}

	//new
	unsigned char** mid = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		mid[i] = new unsigned char[width];
	}

	//action
	for (long i = 0; i < height; ++i) {
		SetHorizontalAverage(img, width, mask_width, mid, i);
	}

	for (long i = 0; i < width; ++i) {
		SetVerticalAverage(mid, height, mask_height, dst, i);
	}

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] mid[i];
	}
	delete[] mid;

	return true;
}

bool Func_Subtract(
	unsigned char** minuend,
	unsigned char** subtrahend,
	char** &dst,
	const unsigned long width,
	const unsigned long height
) {
	if (minuend == NULL || subtrahend == NULL || dst == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			dst[j][i] = minuend[j][i] - subtrahend[j][i];
		}
	}
	return true;
}

bool Func_Addition(
	unsigned char** src,
	char** addend,
	const double m,
	unsigned char** &dst,
	const unsigned long width,
	const unsigned long height
) {
	if (src == NULL || addend == NULL || dst == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			double result = (src[j][i] + addend[j][i])*m;
			if (result < 0)
				result = 0;
			else if (result > 255)
				result = 255;
			dst[j][i] = result;
		}
	}
	return true;
}

bool Func(CImg* img) {
	long width;
	long height;

	width = img->GetWidthPixel();
	height = img->GetHeight();

	unsigned char** source = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		source[i] = new unsigned char[width];
	}

	unsigned char** afteravg = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		afteravg[i] = new unsigned char[width];
	}

	char** aftersubtract = new char*[height];
	for (int i = 0; i < height; ++i) {
		aftersubtract[i] = new char[width];
	}

	unsigned char** destination = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		destination[i] = new unsigned char[width];
	}

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = img->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//mask
	const unsigned long MASK_WIDTH = 7;
	const unsigned long MASK_HEIGHT = 7;

	Func_AverageValueFiltering(source, afteravg, width, height, MASK_WIDTH, MASK_HEIGHT);
	Func_Subtract(source, afteravg, aftersubtract, width, height);
	Func_Addition(source, aftersubtract, 1.5, destination, width, height);


	//CImg * pPreviewImg1 = create_image();
	//pPreviewImg1->InitArray8((unsigned char**)aftersubtract, height, width);
	//string filepath1 = NowTimeToFileName("..//results//giss//afterAVF", ".bmp");
	//pPreviewImg1->SaveToFile(filepath1.c_str());


	CImg * pPreviewImg2 = create_image();
	pPreviewImg2->InitArray8(destination, height, width);
	string filepath2 = NowTimeToFileName("..//results//giss//rafterAdd", ".bmp");
	pPreviewImg2->SaveToFile(filepath2.c_str());


	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] afteravg[i];
	}
	delete[] afteravg;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] aftersubtract[i];
	}
	delete[] aftersubtract;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] destination[i];
	}
	delete[] destination;

	return true;
}

int main() {
	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//2-1-0.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//code-test-23.bmp");
	if (!rt)
		return -1;

	Func(pImg);

	getchar();
	return 0;
}