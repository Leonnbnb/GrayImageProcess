#pragma once

#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <algorithm>
#include <opencv2\opencv.hpp>//OpenCV

#include <windows.h>//BOOL etc.

//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

//#define CALC_TIME
//#include <ctime>
//#define LOCAL_FUNCS_EXTEND
#define DEBUG_TEST

#ifdef CALC_TIME
#include <ctime>//clock_t
#endif

//class begin here:

class Gray_Image_Processing
{
private:
	Hrlregion *m_rl;
	bool m_bRegion;

public:
	Gray_Image_Processing() { m_bRegion = false; };
	~Gray_Image_Processing() {};

	enum BINARYZATION_METHOD {
		//自定义阈值
		BI_NONE
	};

	enum SCALING_METHOD {
		//无插值(抽取)
		SC_NONE,
		//最邻近插值
		SC_NEAREST_NEIGHBOR,
		//双线性插值
		SC_BILINEAR,
		//双三次插值
		SC_BICUBIC,
		//基于图像配准的傅立叶变换
		SC_FOURIER_BASED,
	};

	enum ROTATE_METHOD {
		//最邻近插值
		RO_NEAREST_NEIGHBOR,
		//双线性插值
		RO_BILINEAR,
		//双三次插值
		RO_BICUBIC
	};

	enum ROTATE_CLIP_METHOD {
		//原图大小
		RC_ORIG,
		//适应旋转缩放
		RC_FIT
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
	//BINARYZATION_METHOD method: 其他自适应阈值计算算法(当参数不为BINARYZATION_METHOD::BI_NONE时,指定的阈值无效)
	//返回值:是否成功执行
	bool Binaryzation(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold = 0x7F/*127*/, BINARYZATION_METHOD method = BINARYZATION_METHOD::BI_NONE);

	//TODO:
	//函数功能: 二值化(OpenCV)
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned char threshold: 二值化阈值(默认为127)
	//BINARYZATION_METHOD method: 其他自适应阈值计算算法(当参数不为BINARYZATION_METHOD::BI_NONE时,指定的阈值无效)
	//返回值:是否成功执行
	//bool Binaryzation_CV(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold = 0x7F/*127*/, CVBINARYZATION_METHOD method = CVBINARYZATION_METHOD::BI_NONE);

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

	//函数功能: 缩放图像
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long width_dst: 目标图像的宽度
	//unsigned long height_dst: 目标图像的高度
	//SCALING_METHOD method: 插值算法
	//返回值:是否成功执行
	bool Scaling(CImg* pSrcImg, CImg* &pDstImg, unsigned long width_dst, unsigned long height_dst, SCALING_METHOD method);

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

	//函数功能: 裁切指定矩形区域
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//unsigned long leftTop_x: 矩形的左上角顶点X坐标
	//unsigned long leftTop_y: 矩形的左上角顶点Y坐标
	//unsigned long clip_width: 矩形的宽度
	//unsigned long clip_height: 矩形的高度
	//返回值:是否成功执行
	bool ClipRectangle(CImg* pSrcImg, CImg* &pDstImg, long leftTop_x, long leftTop_y, unsigned long clip_width, unsigned long clip_height);

	//函数功能: 裁切指定区域
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//const unsigned char fillup_color: 填充颜色
	//返回值:是否成功执行
	bool ClipRegion(CImg* pSrcImg, CImg* &pDstImg, const unsigned char fillup_color);

	//函数功能: 将整幅图像旋转
	//参数:
	//CImg* pSrcImg: 源图像
	//CImg* &pDstImg: 目标图像
	//double angle: 旋转角度
	//double rotarycentre_x: 旋转中心X坐标
	//double rotarycentre_y: 旋转中心Y坐标
	//double zoom_x: X方向缩放比率
	//double zoom_y: Y方向缩放比率
	//double move_x: X方向偏移量
	//double move_y: Y方向偏移量
	//unsigned char fill_color: 旋转后不在源图像中的点的填充颜色
	//ROTATE_METHOD method: 插值方式(默认为无插值 ROTATE_METHOD::RO_NONE)
	//ROTATE_CLIP_METHOD clip_method: 图片裁切缩放方式(默认为原图大小 ROTATE_CLIP_METHOD::RC_ORIG)
	bool Rotate(CImg* pSrcImg, CImg* &pDstImg, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color, ROTATE_METHOD method = ROTATE_METHOD::RO_NEAREST_NEIGHBOR, ROTATE_CLIP_METHOD clip_method = ROTATE_CLIP_METHOD::RC_ORIG);


#ifdef LOCAL_FUNCS_EXTEND
	//函数功能: 使用FPU一次性得到Sin与Cos值
	//参数:
	//const double Angle: 角度(弧度制)
	//double& sina: 返回的Sin值
	//double& cosa: 返回的Cos值
	void SinCos(const double Angle, double& sina, double& cosa);

#endif

#ifdef DEBUG_TEST
	void CV_Test(CImg* pSrcImg);

#endif

	/*------------------------------------------------------------------------------------------------------------
	---------------------------------------------------功能实现-----------------------------------------------------
	------------------------------------------------------------------------------------------------------------*/
private:
	//将CImg对象转换为Buffer数组
	bool _trans_Gray_CImg_to_Buffer(CImg* pImg, unsigned char** &pBuffer);

	//将CImg对象转换为Buffer一维数组
	bool _trans_Gray_CImg_to_1D_Buffer(CImg* pImg, unsigned char* &pBuffer);

	//将CImg对象转换为OpenCV Mat对象
	bool _trans_Gray_CImg_to_Mat(CImg* pImg, cv::Mat &mat);

	//将OpenCV Mat对象转换为CImg对象
	bool _trans_Mat_to_Gray_CImg(cv::Mat mat, CImg* &pImg);

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

	//抽取像素缩小图像
	bool _scaling_none(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, int piece_size, int selected);

	//邻近点取样缩放图像
	bool _scaling_nearest_neighbor(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst);

	//水平翻转图像
	bool _flip_horizontal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//垂直翻转图像
	bool _flip_vertical(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//裁切矩形区域
	bool _clip_rectangle(unsigned char** pSrc, unsigned char** &pDst, long leftTop_x, long leftTop_y, unsigned long width_src, unsigned long height_src, unsigned long &width_dst, unsigned long &height_dst);

	//非特殊角度的临近插值旋转
	bool _rotate_common_nearest_neighbor(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color);

	//非特殊角度的双线性插值旋转
	bool _rotate_common_bilinear(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color);

	//裁切区域
	bool _clip_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long &width_dst, unsigned long &height_dst, const unsigned char fillup_color);

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

	//判断一个点是否在图片中
	inline bool __pixels_is_in_img(const unsigned long width, const unsigned long height, const long x, const long y);
};