/****************************************************************************************************
 * Copyright (c) 2008 by Crop.
 ****************************************************************************************************
 * @file: CImg.h
 *
 * @description: 图像基类函数。其他各种图像格式的类都从此基类派生。例如 BMP PNG 和数据采集
 *
 * @date: May 9, 2013
 * @author: Ou Shile
 * @last modified by:
 *        $Author: Ou shile $  $DateTime: 2013/07/02 11:20:26 $
 ****************************************************************************************************
 */


#ifndef __GRAY_IMG_H__
#define __GRAY_IMG_H__
#include "math.h"
#include "../shared/dataType.h"
#include "../shared/List.h"

class PATTERN_RECOGNIZE_CLASS_DLL CImg
{
public:
	// 析构函数
	virtual ~CImg(){};

public:	

	// 用一维数组值初始化图像数据,nWidth要求4字节对齐
	virtual void InitArray8(BYTE* pixArray, const int& nHieght, const int& nWidth) = 0;
	// 用二维数组值初始化图像数据
	virtual void InitArray8(PBYTE* pixArray, const int& nHieght, const int& nWidth) = 0;

	virtual void InitArray24(BYTE* pixArray, const int& nHieght, const int& nWidth) = 0;
	virtual BOOL ConvertTOGray(CImg*  pColorImg) = 0;

	// 获取像素的值
	virtual COLORREF GetPixel(int x, int y) const = 0;
	// 获取灰度值
	virtual BYTE GetGray(int x, int y) const = 0;
	// 获取图像中某一行像素的起始地址
	virtual BYTE* GetPixelAddressRow(const int& y) const = 0;

	// 获取一行的字节数
	virtual int GetWidthByte() const = 0;
	// 获取一行的像素数
	virtual int GetWidthPixel() const = 0;
	// 获取高度
	virtual int GetHeight() const = 0;

	virtual int GetBitCount() = 0;

	virtual BITMAPINFOHEADER * getImgHeader() = 0;
	virtual LPBYTE getImgBuffer() = 0;

	virtual BOOL AttachFromFile(LPCTSTR lpcPathName) = 0;
	virtual BOOL SaveToFile(LPCTSTR lpcPathName) = 0;

	//domain
	virtual BOOL setdomain(const Hrun* prl, const HITEMCNT num) = 0;
	virtual void clearDomain() = 0;
	virtual void getDomain(Hrun*& prl, HITEMCNT& num) const = 0;

};


#endif // __GRAY_H__
