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

//获得图像周围的所有像素均值
bool Func_GetSurround(unsigned char** pBuffer, const long width, const long height, double &AVG) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;

	long long pixGraySum = 0;
	long pixCount = 0;

	for (unsigned long x = 1; x < width - 2; ++x) {
		pixGraySum += pBuffer[1][x];
		++pixCount;

		pixGraySum += pBuffer[height - 2][x];
		++pixCount;
	}

	for (unsigned long y = 1; y < height - 2; ++y) {
		pixGraySum += pBuffer[y][1];
		++pixCount;

		pixGraySum += pBuffer[y][width - 2];
		++pixCount;
	}

	AVG = pixGraySum*1.0 / pixCount*1.0;

	return true;
}

//纠正图像黑白类型
bool Func_Correction(unsigned char** pBuffer, double GT, double AVG, const long width, const long height) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;

	if (AVG > GT)
		return true;
	else {
		for (long y = 0; y < height; ++y) {
			for (long x = 0; x < width; ++x) {
				pBuffer[y][x] = 0xFF - pBuffer[y][x];
			}
		}

		return true;
	}
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
	double AVG = 0;
	Func_GetSurround(source, width, height, AVG);
	Func_Correction(source, GT, AVG, width, height);
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
	BOOL rt = pImg->AttachFromFile("..//imgs//code-test-15.bmp");
	if (!rt)
		return -1;

	CImg* DstImg = create_image();
	if (!DstImg)
		return -2;
	bool rt1 = true;
	rt1 = Func(pImg, DstImg);

	string filepath = NowTimeToFileName("..//results//cctc//DST", ".bmp");
	DstImg->SaveToFile(filepath.c_str());

	return 0;
}