#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>

//filename
#include <ctime>
#include <sstream>
#include <iomanip>

#include <windows.h>//BOOL etc.
//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

#ifdef _DEBUG
//获取当前的系统时间以创建文件名（仅供调试输出使用）
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
#endif

//计算GT值
bool Func_GT(unsigned char** pBuffer, const long width, const long height, double &GT) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));

	for (long i = 0; i < height; ++i) {
		for (long j = 0; j < width; ++j) {
			++tab[(unsigned int)pBuffer[i][j]];
		}
	}

	unsigned long long limit = 0.1*width*height;
	unsigned long long count = 0;
	unsigned long long sum = 0;

	for (int i = 0; i < 256; ++i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avglower = (double)sum / count;

	count = 0;
	sum = 0;
	for (int i = 255; i >= 0; --i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avggreater = (double)sum / count;
	GT = (avglower + avggreater) / 2;

	return true;
}

bool Func_Binaryzation(unsigned char** pBuffer, double GT, const long width, const long height) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {

			if (pBuffer[y][x] >= GT)
				pBuffer[y][x] = 0xFF;
			else
				pBuffer[y][x] = 0;
		}
	}

	return true;
}

//-----功能入口-----
bool Func(CImg* SrcImg, CImg* DstImg) {
	if (SrcImg == NULL)
		return false;

	long width;
	long height;

	width = SrcImg->GetWidthPixel();
	height = SrcImg->GetHeight();

	//new

	unsigned char** source = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		source[i] = new unsigned char[width];
	}

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = SrcImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//action
	double GT = 0;
	Func_GT(source, width, height, GT);
	Func_Binaryzation(source, GT, width, height);

	DstImg->InitArray8(source, height, width);

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	return true;
}

//-----程序入口-----
int main() {
	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//TEST//origin.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//TEST//Decodability_3__0_51_4.bmp");
	if (!rt)
		return -1;

	CImg* DstImg = create_image();
	if (!DstImg)
		return -2;
	bool rt1 = true;
	rt1 = Func(pImg, DstImg);

	string filepath = NowTimeToFileName("..//results//gib//DST", ".bmp");
	DstImg->SaveToFile(filepath.c_str());

	return 0;
}