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
---------------------------------------------------�ӿں���-----------------------------------------------------
------------------------------------------------------------------------------------------------------------*/
public:
	//��������: ����ִ��ͼ���������
	//����:
	//Hrlregion *region: ����
	//����ֵ:�Ƿ�ɹ�ִ��
	bool SetRegion(Hrlregion *region);

	//��������:���ִ��ͼ���������
	void ClearRegion();

	//��������: ��ֵ��
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned char threshold: ��ֵ����ֵ(Ĭ��Ϊ127)
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Binaryzation(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold = 0x7F/*127*/);

	//��������: �Ҷȸ�ʴ
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: ���и�ʴ��ģ����
	//unsigned long mask_height: ���и�ʴ��ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Erosion(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//��������: �Ҷ�����
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: �������͵�ģ����
	//unsigned long mask_height: �������͵�ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Dilition(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//��������: ������(�ȸ�ʴ������)
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: ģ����
	//unsigned long mask_height: ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	//˵��:
	//����С����,����ϸ�㴦��������,ƽ���ϴ�����ı߽��ͬʱ�������Ըı������
	bool Opening(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width = 3, unsigned long mask_height = 3);

	//��������: ������(�������ٸ�ʴ)
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: ģ����
	//unsigned long mask_height: ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	//˵��:
	//���������ϸС�ն�,�����ڽ�����,ƽ����߽��ͬʱ�������Ըı������
	bool Closing(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width = 3, unsigned long mask_height = 3);

	//��������: ��ֵ�˲�
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: �����˲���ģ����
	//unsigned long mask_height: �����˲���ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	bool MedianFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//��������: ��ֵ�˲�
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: �����˲���ģ����
	//unsigned long mask_height: �����˲���ģ��߶�
	//����ֵ:�Ƿ�ɹ�ִ��
	bool MeanFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height);

	//��������: �Աȶȵ���
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//double M: �Աȶȵ���ϵ��(Ĭ��Ϊ1)
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Contrast(CImg* pSrcImg, CImg* &pDstImg, double M = 1);

	//��������: ��
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//unsigned long mask_width: �о�ֵ�˲���ģ����
	//unsigned long mask_height: ���о�ֵ�˲���ģ��߶�
	//double M: �񻯵���ϵ��(Ĭ��Ϊ1)
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Sharpen(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height, double M = 0);

	//��������: ��ȫͼֱ��ͼ
	//����:
	//CImg* pSrcImg: Դͼ��
	//unsigned long* &Histogram: ֱ��ͼ
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Histogram(CImg* pSrcImg, unsigned long* &Histogram);

	//��������: ��ֱ��ͼ����ͼƬ
	//����:
	//unsigned long* Histogram: ֱ��ͼ
	//CImg* pCreateImg: Ŀ��ͼ��
	//const unsigned  char bkgcolor: ����ɫ(Ĭ��Ϊ��)
	//const unsigned  char forecolor: ����ɫ(Ĭ��Ϊ��)
	//bool divided: ����֮���Ƿ��м��(Ĭ���м��)
	//����ֵ:�Ƿ�ɹ�ִ��
	bool HistogramToImage(unsigned long* Histogram, CImg* &pCreateImg, const unsigned  char bkgcolor = 0xFF, const unsigned  char forecolor = 0, bool divided = true);

	//��������: ������ͼ����ʱ����ת90��
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//����ֵ:�Ƿ�ɹ�ִ��
	bool RotateLeft(CImg* pSrcImg, CImg* &pDstImg);

	//��������: ������ͼ��˳ʱ����ת90��
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//����ֵ:�Ƿ�ɹ�ִ��
	bool RotateRight(CImg* pSrcImg, CImg* &pDstImg);

	//��������: ����ͼ��
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//double rate: ͼ�����ű���
	//SCALING_METHOD method: ��ֵ�㷨
	//����ֵ:�Ƿ�ɹ�ִ��
	bool Scaling(CImg* pSrcImg, CImg* &pDstImg, double rate, SCALING_METHOD method);

	//��������: ������ͼ��ˮƽ��ת
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//����ֵ:�Ƿ�ɹ�ִ��
	bool FlipHorizontal(CImg* pSrcImg, CImg* &pDstImg);

	//��������: ������ͼ��ֱ��ת
	//����:
	//CImg* pSrcImg: Դͼ��
	//CImg* &pDstImg: Ŀ��ͼ��
	//����ֵ:�Ƿ�ɹ�ִ��
	bool FlipVertical(CImg* pSrcImg, CImg* &pDstImg);

	/*------------------------------------------------------------------------------------------------------------
	---------------------------------------------------����ʵ��-----------------------------------------------------
	------------------------------------------------------------------------------------------------------------*/
private:
	//��CImg����ת��ΪBuffer����
	bool _trans_Gray_CImg_to_Buffer(CImg* pImg, unsigned char** &pBuffer);

	//ȫͼ��ֵ��
	bool _binaryzation(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold = 0x7F/*127*/);

	//ָ�������ֵ��
	bool _binaryzation_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold = 0x7F/*127*/);

	//ȫͼ��ʴ
	bool _erosion(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ָ������ʴ
	bool _erosion_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ȫͼ����
	bool _dilition(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ָ����������
	bool _dilition_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ȫͼ��ֵ�˲�
	bool _medianfilter(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ָ��������ֵ�˲�
	bool _medianfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ȫͼ��ֵ�˲�(��ÿ�����ص���getAdjacentȡ��Χ����ֵ)
	bool _meanfilter_normal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ȫͼ��ֵ�˲�(������getAdjacent,�ں�������ʵ�����������һ����Ҫ������ֵ��ͬʱ��Ҫ����ȥ������ֵ)
	bool _meanfilter_increment(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ָ�������ֵ�˲�
	bool _meanfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height);

	//ȫͼ�Աȶȵ���
	bool _contrast(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M);

	//ָ������Աȶȵ���
	bool _contrast_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M);

	//ȫͼ��
	bool _sharpen(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M);

	//ָ��������
	bool _sharpen_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M);

	//��ʱ����ת90��
	bool _rotate_left(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//˳ʱ����ת90��
	bool _rotate_right(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//�޲�ֵ��Сͼ��
	bool _scaling_none(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, int piece_size, int selected);

	//ˮƽ��תͼ��
	bool _flip_horizontal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	//��ֱ��תͼ��
	bool _flip_vertical(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height);

	/*------------------------------------------------------------------------------------------------------------
	---------------------------------------------------ʵ�����-----------------------------------------------------
	------------------------------------------------------------------------------------------------------------*/
private:
	//�����Χ����ֵ
	//�˺���������ִ��Ч�ʵ�����»�ȡ���ģ����������ֵ��ͨ����
	//������ٵ�ִ��Ч�ʣ��벻Ҫ���øú���
	bool __getSurroundPixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, std::vector<unsigned char> &pixelset);

	//����������ڵ�����ֵ
	bool __getAdjacent_LR_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, std::vector<unsigned char> &pixelset);

	//����������ڵ�����ֵ
	bool __getAdjacent_AB_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_height, std::vector<unsigned char> &pixelset);

	//���ȫͼ����ֵ��ֵ
	bool __getAvrageGrayValue(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg);

	//���ָ����������ֵ��ֵ
	bool __getAvrageGrayValue_region(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg);

	//������ͼ�����(pMinuend-pSubtrahend)
	bool __subtract(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height);

	//������ͼ��ָ���������(pMinuend-pSubtrahend)
	bool __subtract_region(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height);

	//��ͼ�����ָ��ϵ�����Ե�����(pSrc+pAddend*M)
	bool __addition(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M = 0);

	//��ͼ��ָ���������ָ��ϵ�����Ե�����(pSrc+pAddend*M)
	bool __addition_region(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M = 0);
};