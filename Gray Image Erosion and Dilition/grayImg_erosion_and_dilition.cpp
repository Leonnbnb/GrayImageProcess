/*--------------------
文件说明：
  该文件实现灰度图像的腐蚀(Erosion)和膨胀(Dilition)
基本实现说明：
  要求使用指定大小的模板对目标灰度图像进行模板卷积运算
  模板大小必须为奇数，所以当传入的参数为偶数时，将会自动向上取整
  在进行腐蚀操作时对每个模板原点采用赋予模板中灰度最小值处理
  进行膨胀操作即赋予最大灰度值
  所以实际操作即为找到MxN的矩阵中的最小值与最大值
  进行了降维处理，先通过SetHorizontalMin(Max)取横向方向上M个元素中的最小（大）值
  再对中间结果进行SetVerticalMin(Max)进行纵向上的N个元素进行操作去最值
  这样最终结果即为MxN的矩阵中的最小值与最大值
  对图片的读取使用了hawkvis.dll中的CImg类
时间：
  07.31.2016
作者：
  XD_G
--------------------*/
#include <iostream>
#include <windows.h>

#include <ctime>//

#include "..//shared//ExtPatternRecognize.h"


#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

void SetHorizontalMin(unsigned char** src,
	const unsigned long width,
	const unsigned long mask_width,
	unsigned char** &dst,
	const unsigned long line_num
)
{
	const unsigned long half_mask_width = mask_width / 2;
	long x = 0, y = line_num;
	unsigned char min = 255;

	long min_pos = -1;

	for (x = 0; x <= half_mask_width; ++x) {
		min = 255;
		if (min_pos == -1) {
			for (long i = 0; i <= x + half_mask_width; ++i) {
				unsigned char temp = src[y][i];
				if (temp < min) {
					min_pos = i;
					min = temp;
				}
			}
		}
		else {
			unsigned char temp = src[y][x + half_mask_width];
			if (temp <= src[y][min_pos]) {//
				min = temp;
				min_pos = x + half_mask_width;
			}
			else {
				min = src[y][min_pos];
			}
		}
		dst[y][x] = min;
	}

	min_pos = -1;

	for (; x <= width - 1 - half_mask_width; ++x) {
		min = 255;
		if (min_pos == -1 || min_pos < (x - half_mask_width)) {
			for (long i = x - half_mask_width; i <= x + half_mask_width; ++i) {
				unsigned char temp = src[y][i];
				if (temp < min) {
					min_pos = i;
					min = temp;
				}
			}
		}
		else {
			unsigned char temp = src[y][x + half_mask_width];
			if (temp <= src[y][min_pos]) {//
				min = temp;
				min_pos = x + half_mask_width;
			}
			else {
				min = src[y][min_pos];
			}
		}
		dst[y][x] = min;
	}

	for (; x <= width - 1; ++x) {
		min = 255;
		for (long i = x - half_mask_width; i <= width - 1; ++i) {
			unsigned char temp = src[y][i];
			if (temp < min)
				min = temp;
		}
		dst[y][x] = min;
	}
}

void SetVerticalMin(
	unsigned char** src,
	const unsigned long height,
	const unsigned long mask_height,
	unsigned char** &dst,
	const unsigned long row_num
)
{

	const unsigned long half_mask_height = mask_height / 2;
	long x = row_num, y = 0;

	unsigned char min = 255;
	//for (y = 0; y <= half_mask_height; ++y) {
	//	min = 255;
	//	for (long i = 0; i <= y + half_mask_height; ++i) {
	//		unsigned char temp = src[i][x];
	//		if (temp < min)
	//			min = temp;
	//	}
	//	dst[y][x] = min;
	//}
	long min_pos = -1;
	for (y = 0; y <= half_mask_height; ++y) {
		min = 255;
		if (min_pos == -1) {
			for (long i = 0; i <= y + half_mask_height; ++i) {
				unsigned char temp = src[i][x];
				if (temp < min) {
					min = temp;
					min_pos = i;
				}
			}
		}
		else {
			unsigned char temp = src[y + half_mask_height][x];
			if (temp <= src[min_pos][x]) {//
				min = temp;
				min_pos = y + half_mask_height;
			}
			else {
				min = src[min_pos][x];
			}
		}
		dst[y][x] = min;
	}

	//for (; y <= height - 1 - half_mask_height; ++y) {
	//	min = 255;
	//	for (long i = y - half_mask_height; i <= y + half_mask_height; ++i) {
	//		unsigned char temp = src[i][x];
	//		if (temp < min)
	//			min = temp;
	//	}
	//	dst[y][x] = min;
	//}

	min_pos = -1;
	for (; y <= height - 1 - half_mask_height; ++y) {
		min = 255;
		if (min_pos == -1 || min_pos < (y - half_mask_height)) {
			for (long i = y - half_mask_height; i <= y + half_mask_height; ++i) {
				unsigned char temp = src[i][x];
				if (temp < min) {
					min = temp;
					min_pos = i;
				}
			}
		}
		else {
			unsigned char temp = src[y + half_mask_height][x];
			if (temp <= src[min_pos][x]) {//
				min = temp;
				min_pos = y + half_mask_height;
			}
			else {
				min = src[min_pos][x];
			}
		}
		dst[y][x] = min;
	}

	for (; y <= height - 1; ++y) {
		min = 255;
		for (long i = y - half_mask_height; i <= height - 1; ++i) {
			unsigned char temp = src[i][x];
			if (temp < min)
				min = temp;
		}
		dst[y][x] = min;
	}

}

void SetHorizontalMax(unsigned char** src,
	const unsigned long width,
	const unsigned long mask_width,
	unsigned char** &dst,
	const unsigned long line_num
)
{
	const unsigned long half_mask_width = mask_width / 2;
	long x = 0, y = line_num;
	unsigned char max = 0;

	long max_pos = -1;

	for (x = 0; x <= half_mask_width; ++x) {
		max = 0;
		if (max_pos == -1) {
			for (long i = 0; i <= x + half_mask_width; ++i) {
				unsigned char temp = src[y][i];
				if (temp > max) {
					max_pos = i;
					max = temp;
				}
			}
		}
		else {
			unsigned char temp = src[y][x + half_mask_width];
			if (temp >= src[y][max_pos]) {//
				max = temp;
				max_pos = x + half_mask_width;
			}
			else {
				max = src[y][max_pos];
			}
		}
		dst[y][x] = max;
	}


	//for (; x <= width - 1 - half_mask_width; ++x) {
	//	max = 0;
	//	for (long i = x - half_mask_width; i <= x + half_mask_width; ++i) {
	//		unsigned char temp = src[y][i];
	//		if (temp < max)
	//			max = temp;
	//	}
	//	dst[y][x] = max;
	//}

	max_pos = -1;

	for (; x <= width - 1 - half_mask_width; ++x) {
		max = 0;
		if (max_pos == -1 || max_pos < (x - half_mask_width)) {
			for (long i = x - half_mask_width; i <= x + half_mask_width; ++i) {
				unsigned char temp = src[y][i];
				if (temp > max) {
					max_pos = i;
					max = temp;
				}
			}
		}
		else {
			unsigned char temp = src[y][x + half_mask_width];
			if (temp >= src[y][max_pos]) {//
				max = temp;
				max_pos = x + half_mask_width;
			}
			else {
				max = src[y][max_pos];
			}
		}
		dst[y][x] = max;
	}

	for (; x <= width - 1; ++x) {
		max = 0;
		for (long i = x - half_mask_width; i <= width - 1; ++i) {
			unsigned char temp = src[y][i];
			if (temp > max)
				max = temp;
		}
		dst[y][x] = max;
	}
}

void SetVerticalMax(
	unsigned char** src,
	const unsigned long height,
	const unsigned long mask_height,
	unsigned char** &dst,
	const unsigned long row_num
)
{

	const unsigned long half_mask_height = mask_height / 2;
	long x = row_num, y = 0;

	unsigned char max = 0;

	long max_pos = -1;
	for (y = 0; y <= half_mask_height; ++y) {
		max = 0;
		if (max_pos == -1) {
			for (long i = 0; i <= y + half_mask_height; ++i) {
				unsigned char temp = src[i][x];
				if (temp > max) {
					max = temp;
					max_pos = i;
				}
			}
		}
		else {
			unsigned char temp = src[y + half_mask_height][x];
			if (temp >= src[max_pos][x]) {//
				max = temp;
				max_pos = y + half_mask_height;
			}
			else {
				max = src[max_pos][x];
			}
		}
		dst[y][x] = max;
	}

	max_pos = -1;
	for (; y <= height - 1 - half_mask_height; ++y) {
		max = 0;
		if (max_pos == -1 || max_pos < (y - half_mask_height)) {
			for (long i = y - half_mask_height; i <= y + half_mask_height; ++i) {
				unsigned char temp = src[i][x];
				if (temp > max) {
					max = temp;
					max_pos = i;
				}
			}
		}
		else {
			unsigned char temp = src[y + half_mask_height][x];
			if (temp >= src[max_pos][x]) {//
				max = temp;
				max_pos = y + half_mask_height;
			}
			else {
				max = src[max_pos][x];
			}
		}
		dst[y][x] = max;
	}

	for (; y <= height - 1; ++y) {
		max = 0;
		for (long i = y - half_mask_height; i <= height - 1; ++i) {
			unsigned char temp = src[i][x];
			if (temp > max)
				max = temp;
		}
		dst[y][x] = max;
	}

}


//函数说明：
//	灰度腐蚀/膨胀
//参数说明：
//	输入：
//	img 要进行操作的源图像BUFFER，处理之后的图像数据也会通过该参数传出
//	width 源图像宽度
//	height 源图像高度
//	mask_width 模板宽度
//	mask_height 模板高度
//  MINorMAX 对模板取最小值还是最大值，最小值对应腐蚀，最大值对应膨胀操作
bool Func(unsigned char** &img,
	const unsigned long width,
	const unsigned long height,
	unsigned long mask_width,
	unsigned long mask_height,
	bool MINorMAX = false
)
{
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

	//new
	unsigned char** mid = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		mid[i] = new unsigned char[width];
	}

	if (mask_height == 1) {
		if (!MINorMAX) {
			//action
			for (long i = 0; i < height; ++i) {
				SetHorizontalMin(img, width, mask_width, mid, i);
			}
		}
		else {
			//action
			for (long i = 0; i < height; ++i) {
				SetHorizontalMax(img, width, mask_width, mid, i);
			}
		}

		//memcpy
		for (unsigned long i = 0; i < height; ++i) {
			memcpy(img[i], mid[i], width);
		}

		//delete
		for (unsigned long i = 0; i < height; ++i) {
			delete[] mid[i];
		}
		delete[] mid;

		return true;
	}

	if (mask_width == 1) {
		if (!MINorMAX) {
			//action
			for (long i = 0; i < width; ++i) {
				SetVerticalMin(img, height, mask_height, mid, i);
			}
		}
		else {
			//action
			for (long i = 0; i < width; ++i) {
				SetVerticalMax(img, height, mask_height, mid, i);
			}
		}

		//memcpy
		for (unsigned long i = 0; i < height; ++i) {
			memcpy(img[i], mid[i], width);
		}

		//delete
		for (unsigned long i = 0; i < height; ++i) {
			delete[] mid[i];
		}
		delete[] mid;

		return true;
	}
	if (!MINorMAX) {
		//action
		for (long i = 0; i < height; ++i) {
			SetHorizontalMin(img, width, mask_width, mid, i);
		}

		for (long i = 0; i < width; ++i) {
			SetVerticalMin(mid, height, mask_height, img, i);
		}
	}
	else {
		//action
		for (long i = 0; i < height; ++i) {
			SetHorizontalMax(img, width, mask_width, mid, i);
		}

		for (long i = 0; i < width; ++i) {
			SetVerticalMax(mid, height, mask_height, img, i);
		}
	}

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] mid[i];
	}
	delete[] mid;

	return true;
}

int main() {
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//code-test-26.bmp");
	if (!rt)
		return -1;

	long WIDTH;
	long HEIGHT;

	WIDTH = pImg->GetWidthPixel();
	HEIGHT = pImg->GetHeight();

	//new
	unsigned char** source = new unsigned char*[HEIGHT];
	for (unsigned long i = 0; i < HEIGHT; ++i) {
		source[i] = new unsigned char[WIDTH];
	}

	//trans
	for (long j = 0; j < HEIGHT; ++j) {
		for (long i = 0; i < WIDTH; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//mask
	const unsigned long MASK_WIDTH = 3;
	const unsigned long MASK_HEIGHT = 3;

	Func(source, WIDTH, HEIGHT, MASK_WIDTH, MASK_HEIGHT, 1);

	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(source, HEIGHT, WIDTH);
	pPreviewImg->SaveToFile("..//results//giead//Preview.bmp");

	//delete
	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] source[i];
	}
	delete[] source;

	getchar();

	return 0;
}

