/*--------------------
�ļ�˵����
  ���ļ�ʵ�ֻҶ�ͼ��ĸ�ʴ(Erosion)������(Dilition)
����ʵ��˵����
  Ҫ��ʹ��ָ����С��ģ���Ŀ��Ҷ�ͼ�����ģ��������
  ģ���С����Ϊ���������Ե�����Ĳ���Ϊż��ʱ�������Զ�����ȡ��
  �ڽ��и�ʴ����ʱ��ÿ��ģ��ԭ����ø���ģ���лҶ���Сֵ����
  �������Ͳ������������Ҷ�ֵ
  ����ʵ�ʲ�����Ϊ�ҵ�MxN�ľ����е���Сֵ�����ֵ
  �����˽�ά������ͨ��SetHorizontalMin(Max)ȡ��������M��Ԫ���е���С����ֵ
  �ٶ��м�������SetVerticalMin(Max)���������ϵ�N��Ԫ�ؽ��в���ȥ��ֵ
  �������ս����ΪMxN�ľ����е���Сֵ�����ֵ
  ��ͼƬ�Ķ�ȡʹ����hawkvis.dll�е�CImg��
ʱ�䣺
  07.31.2016
���ߣ�
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


//����˵����
//	�Ҷȸ�ʴ/����
//����˵����
//	���룺
//	img Ҫ���в�����Դͼ��BUFFER������֮���ͼ������Ҳ��ͨ���ò�������
//	width Դͼ����
//	height Դͼ��߶�
//	mask_width ģ����
//	mask_height ģ��߶�
//  MINorMAX ��ģ��ȡ��Сֵ�������ֵ����Сֵ��Ӧ��ʴ�����ֵ��Ӧ���Ͳ���
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

