/****************************************************************************************************
 * Copyright (c) 2008 by Crop.
 ****************************************************************************************************
 * @file: CImg.h
 *
 * @description: ͼ����ຯ������������ͼ���ʽ���඼�Ӵ˻������������� BMP PNG �����ݲɼ�
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
	// ��������
	virtual ~CImg(){};

public:	

	// ��һά����ֵ��ʼ��ͼ������,nWidthҪ��4�ֽڶ���
	virtual void InitArray8(BYTE* pixArray, const int& nHieght, const int& nWidth) = 0;
	// �ö�ά����ֵ��ʼ��ͼ������
	virtual void InitArray8(PBYTE* pixArray, const int& nHieght, const int& nWidth) = 0;

	virtual void InitArray24(BYTE* pixArray, const int& nHieght, const int& nWidth) = 0;
	virtual BOOL ConvertTOGray(CImg*  pColorImg) = 0;

	// ��ȡ���ص�ֵ
	virtual COLORREF GetPixel(int x, int y) const = 0;
	// ��ȡ�Ҷ�ֵ
	virtual BYTE GetGray(int x, int y) const = 0;
	// ��ȡͼ����ĳһ�����ص���ʼ��ַ
	virtual BYTE* GetPixelAddressRow(const int& y) const = 0;

	// ��ȡһ�е��ֽ���
	virtual int GetWidthByte() const = 0;
	// ��ȡһ�е�������
	virtual int GetWidthPixel() const = 0;
	// ��ȡ�߶�
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
