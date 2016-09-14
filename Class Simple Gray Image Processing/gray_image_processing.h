#pragma once

#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <algorithm>

#include <windows.h>//BOOL etc.
//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

class Gray_Image_Processing
{
private:
	Hrlregion *m_rl;
	bool m_bRegion;

public:
	Gray_Image_Processing() { m_bRegion = false; };
	~Gray_Image_Processing() {};

	//0x0010
	typedef enum BINARYZATION_METHOD {
		BI_NONE = 0x0010,

	};

	//0x0020
	typedef enum SCALING_METHOD {
		SC_NONE = 0x0020,
		SC_NEAREST_NEIGHBOR,
		SC_BILINEAR,
		SC_BICUBIC,
		SC_FOURIER_BASED,
		SC_EDGE_DIRECTED,
		SC_HQX,
		SC_VECTORIZATION
	};

	//bool TestGetSurroundPixel(CImg* pSrcImg, unsigned long x, unsigned long y, unsigned long mask_width, unsigned long mask_height);

/*------------------------------------------------------------------------------------------------------------
---------------------------------------------------接口函数-----------------------------------------------------
------------------------------------------------------------------------------------------------------------*/
public:
	//函数功能: 设置执行图像处理的区域
	//参数:
	//Hrlregion *region: 区域
	//返回值:是否成功执行
	bool SetRegion(Hrlregion *region);

	//函数功能:清除执行图像处理的区域
	void ClearRegion();

	//函数功能: 二值化
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned char threshold: 二值化阈值(默认为127)
	//返回值:是否成功执行
	bool Binaryzation(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold = 0x7F/*127*/);

	//函数功能: 灰度腐蚀
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 进行腐蚀的模板宽度
	//unsigned long mask_height: 进行腐蚀的模板高度
	//返回值:是否成功执行
	bool Erosion(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//函数功能: 灰度膨胀
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 进行膨胀的模板宽度
	//unsigned long mask_height: 进行膨胀的模板高度
	//返回值:是否成功执行
	bool Dilition(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//函数功能: 开运算(先腐蚀再膨胀)
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 模板宽度
	//unsigned long mask_height: 模板高度
	//返回值:是否成功执行
	//说明:
	//消除小物体,在纤细点处分离物体,平滑较大物体的边界的同时并不明显改变其面积
	bool Opening(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width = 3, unsigned long mask_height = 3);

	//函数功能: 闭运算(先膨胀再腐蚀)
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 模板宽度
	//unsigned long mask_height: 模板高度
	//返回值:是否成功执行
	//说明:
	//填充物体内细小空洞,连接邻近物体,平滑其边界的同时并不明显改变其面积
	bool Closing(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width = 3, unsigned long mask_height = 3);

	//函数功能: 中值滤波
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 进行滤波的模板宽度
	//unsigned long mask_height: 进行滤波的模板高度
	//返回值:是否成功执行
	bool MedianFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//函数功能: 均值滤波
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 进行滤波的模板宽度
	//unsigned long mask_height: 进行滤波的模板高度
	//返回值:是否成功执行
	bool MeanFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//函数功能: 对比度调整
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//double M: 对比度调整系数(默认为1)
	//返回值:是否成功执行
	bool Contrast(CImg* pSrcImg, CImg* &pDstImg, double M = 1);

	//函数功能: 锐化
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long mask_width: 行均值滤波的模板宽度
	//unsigned long mask_height: 进行均值滤波的模板高度
	//double M: 锐化调整系数(默认为1)
	//返回值:是否成功执行
	bool Sharpen(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height, double M = 0);

	//函数功能: 求全图直方图
	//参数:
	//CImg* pSrcImg: 源图像
	//unsigned long* &Histogram: 直方图
	//返回值:是否成功执行
	bool Histogram(CImg* pSrcImg, unsigned long* &Histogram);

	//函数功能: 由直方图生成图片
	//参数:
	//unsigned long* Histogram: 直方图
	//CImg* pCreateImg: 目标图像
	//const unsigned  char bkgcolor: 背景色(默认为白)
	//const unsigned  char forecolor: 柱体色(默认为黑)
	//bool divided: 柱体之间是否有间隔(默认有间隔)
	//返回值:是否成功执行
	bool HistogramToImage(unsigned long* Histogram, CImg* &pCreateImg, const unsigned  char bkgcolor = 0xFF, const unsigned  char forecolor = 0, bool divided = true);

	//函数功能: 将整幅图像逆时针旋转90°
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//返回值:是否成功执行
	bool RotateLeft(CImg* pSrcImg, CImg* &pDstImg);

	//函数功能: 将整幅图像顺时针旋转90°
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//返回值:是否成功执行
	bool RotateRight(CImg* pSrcImg, CImg* &pDstImg);

	//函数功能: 缩放图像
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//double rate: 图像缩放比率
	//SCALING_METHOD method: 插值算法
	//返回值:是否成功执行
	bool Scaling(CImg* pSrcImg, CImg* &pDstImg, double rate, SCALING_METHOD method);

	//函数功能: 将整幅图像水平翻转
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//返回值:是否成功执行
	bool FlipHorizontal(CImg* pSrcImg, CImg* &pDstImg);

	//函数功能: 将整幅图像垂直翻转
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//返回值:是否成功执行
	bool FlipVertical(CImg* pSrcImg, CImg* &pDstImg);

	/*------------------------------------------------------------------------------------------------------------
	---------------------------------------------------功能实现-----------------------------------------------------
	------------------------------------------------------------------------------------------------------------*/
private:
	//将CImg对象转换为Buffer数组
	bool _trans_Gray_CImg_to_Buffer(CImg* pImg, unsigned char** &pBuffer);

	//全图二值化
	bool _binaryzation(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold = 0x7F/*127*/);

	//指定区域二值化
	bool _binaryzation_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold = 0x7F/*127*/);

	//全图腐蚀
	bool _erosion(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//指定区域腐蚀
	bool _erosion_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//全图膨胀
	bool _dilition(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//指定区域膨胀
	bool _dilition_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//全图中值滤波
	bool _medianfilter(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//指定区域中值滤波
	bool _medianfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//全图均值滤波(对每个像素调用getAdjacent取周围像素值)
	bool _meanfilter_normal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//全图均值滤波(不调用getAdjacent,在函数体内实现增量获得下一个需要的像素值与同时需要被减去的像素值)
	bool _meanfilter_increment(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//指定区域均值滤波
	bool _meanfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//全图对比度调整
	bool _contrast(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M);

	//指定区域对比度调整
	bool _contrast_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M);

	//全图锐化
	bool _sharpen(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M);

	//指定区域锐化
	bool _sharpen_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M);

	//逆时针旋转90度
	bool _rotate_left(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//顺时针旋转90度
	bool _rotate_right(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//无插值缩小图像
	bool _scaling_none(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, int piece_size, int selected);

	//水平翻转图像
	bool _flip_horizontal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//垂直翻转图像
	bool _flip_vertical(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	/*------------------------------------------------------------------------------------------------------------
	---------------------------------------------------实现组件-----------------------------------------------------
	------------------------------------------------------------------------------------------------------------*/
private:
	//获得周围像素值
	//此函数在牺牲执行效率的情况下换取获得模板区域像素值的通用性
	//若需快速的执行效率，请不要调用该函数
	bool __getSurroundPixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, std::vector<unsigned char> &pixelset);

	//获得左右相邻的像素值
	bool __getAdjacent_LR_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, std::vector<unsigned char> &pixelset);

	//获得上下相邻的像素值
	bool __getAdjacent_AB_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_height, std::vector<unsigned char> &pixelset);

	//获得全图像素值均值
	bool __getAvrageGrayValue(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg);

	//获得指定区域像素值均值
	bool __getAvrageGrayValue_region(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg);

	//将两幅图像相减(pMinuend-pSubtrahend)
	bool __subtract(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height);

	//将两幅图像指定区域相减(pMinuend-pSubtrahend)
	bool __subtract_region(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height);

	//将图像加上指定系数乘以的增量(pSrc+pAddend*M)
	bool __addition(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M = 0);

	//将图像指定区域加上指定系数乘以的增量(pSrc+pAddend*M)
	bool __addition_region(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M = 0);
};