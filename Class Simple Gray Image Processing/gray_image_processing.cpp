#include "gray_image_processing.h"

#ifdef CALC_TIME
#include <iostream>
#include <ctime>
#endif

/*------------------------------------------------------------------------------------------------------------
------------------------------------------------Interface-----------------------------------------------------
------------------------------------------------------------------------------------------------------------*/

//bool Gray_Image_Processing::TestGetSurroundPixel(CImg* pSrcImg, unsigned long x, unsigned long y, unsigned long mask_width, unsigned long mask_height) {
//
//	unsigned char** pBuffer = NULL;
//
//	unsigned long WIDTH = pSrcImg->GetWidthPixel();
//	unsigned long HEIGHT = pSrcImg->GetHeight();
//
//
//	bool ret = false;
//	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
//	if (ret == false || pBuffer == NULL) {
//		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Erosion - 0\n");
//		return false;
//	}
//
//	//for (long y = 0; y < HEIGHT; ++y) {
//	//	for (long x = 0; x < WIDTH; ++x) {
//	//		std::cout << (unsigned int)pBuffer[y][x] << " ";
//	//	}
//	//	std::cout << std::endl;
//	//}
//
//	std::vector<unsigned char > pixelset;
//
//	ret = __getSurroundPixelsGrayValue(pBuffer, x, y, WIDTH, HEIGHT, mask_width, mask_height, pixelset);
//	if (ret) {
//		for each(auto var in pixelset) {
//			std::cout << (unsigned int)var << " ";
//		}
//	}
//	else {
//		OutputDebugString("\nERROR: Get surround pixels failed! --- Erosion - 1\n");
//		return false;
//	}
//
//
//	for (unsigned long i = 0; i < HEIGHT; ++i) {
//		delete[] pBuffer[i];
//	}
//	delete[] pBuffer;
//	pBuffer = NULL;
//
//	return true;
//
//}

bool Gray_Image_Processing::SetRegion(Hrlregion *region) {
	if (region == NULL) {
		OutputDebugString("\nERROR: Region noexist!\n");
		return false;
	}
	m_rl = region;
	m_bRegion = true;
	return true;
}

void Gray_Image_Processing::ClearRegion() {
	m_rl = NULL;
	m_bRegion = false;
}

bool Gray_Image_Processing::Binaryzation(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Binaryzation - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Binaryzation - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Binaryzation --- Binaryzation - 0\n");
		ret = _binaryzation(pBuffer, pDstBuffer, WIDTH, HEIGHT, threshold);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Binaryzation FAILED! --- Binaryzation - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Binaryzation --- Binaryzation - 0\n");
		ret = _binaryzation_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, threshold);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Binaryzation FAILED! --- Binaryzation - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Binaryzation - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::Erosion(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Erosion - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- Erosion - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- Erosion - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- Erosion - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- Erosion - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- Erosion - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Erosion - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Erosion --- Erosion - 0\n");
		ret = _erosion(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Erosion FAILED! --- Erosion - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Erosion --- Erosion - 0\n");
		ret = _erosion_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Erosion FAILED! --- Erosion - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Erosion - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::Dilition(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Dilition - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- Dilition - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- Dilition - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- Dilition - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- Dilition - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- Dilition - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Dilition - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Dilition --- Dilition - 0\n");
		ret = _dilition(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Dilition FAILED! --- Dilition - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Dilition --- Dilition - 0\n");
		ret = _dilition_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Dilition FAILED! --- Dilition - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Dilition - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::MedianFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- MedianFilter - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- MedianFilter - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- MedianFilter - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- MedianFilter - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- MedianFilter - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- MedianFilter - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- MedianFilter - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image MedianFilter --- MedianFilter - 0\n");
		ret = _medianfilter(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: MedianFilter FAILED! --- MedianFilter - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region MedianFilter --- MedianFilter - 0\n");
		ret = _medianfilter_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region MedianFilter FAILED! --- MedianFilter - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- MedianFilter - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::MeanFilter(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- MeanFilter - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- MeanFilter - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- MeanFilter - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- MeanFilter - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- MeanFilter - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- MeanFilter - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- MeanFilter - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image MeanFilter --- MeanFilter - 0\n");
		ret = _meanfilter_increment(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		//ret = _meanfilter_normal(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: MeanFilter FAILED! --- MeanFilter - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region MeanFilter --- MeanFilter - 0\n");
		ret = _meanfilter_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region MeanFilter FAILED! --- MeanFilter - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- MeanFilter - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::Contrast(CImg* pSrcImg, CImg* &pDstImg, double M) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Contrast - 0\n");
		return false;
	}

	if (M < 0) {
		OutputDebugString("\nERROR: M must greater than zero! --- Contrast - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Contrast - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image MedianFilter --- Contrast - 0\n");
		ret = _contrast(pBuffer, pDstBuffer, WIDTH, HEIGHT, M);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: MedianFilter FAILED! --- Contrast - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region MedianFilter --- Contrast - 0\n");
		ret = _contrast_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, M);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region MedianFilter FAILED! --- Contrast - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Contrast - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::Sharpen(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height, double M) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Sharpen - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Sharpen - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Sharpen --- Sharpen - 0\n");
		ret = _sharpen(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height, M);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Sharpen FAILED! --- Sharpen - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Sharpen --- Sharpen - 0\n");
		ret = _sharpen_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height, M);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Sharpen FAILED! --- Sharpen - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Sharpen - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

/*------------------------------------------------------------------------------------------------------------
--------------------------------------------------Private-----------------------------------------------------
------------------------------------------------------------------------------------------------------------*/

bool Gray_Image_Processing::_trans_Gray_CImg_to_Buffer(CImg* pImg, unsigned char** &pBuffer) {
	if (pImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- TGCTB\n");
		return false;
	}

	if (pBuffer != NULL) {
		OutputDebugString("\nWARNING: Src Img not NULL! --- TGCTB\n");
		delete pBuffer;
		pBuffer = NULL;
	}

	unsigned long WIDTH = pImg->GetWidthPixel();
	unsigned long HEIGHT = pImg->GetHeight();

	pBuffer = new unsigned char*[HEIGHT];
	for (unsigned long i = 0; i < HEIGHT; ++i) {
		pBuffer[i] = new unsigned char[WIDTH];
	}

	for (long y = 0; y < HEIGHT; ++y) {
		for (long x = 0; x < WIDTH; ++x) {
			BYTE* pBuff = pImg->GetPixelAddressRow(y);
			pBuffer[y][x] = pBuff[x];
		}
	}

	OutputDebugString("\nINFO: Src Img trans to Buffer success! --- TGCTB\n");
	return true;
}

bool Gray_Image_Processing::__getSurroundPixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, std::vector<unsigned char> &pixelset) {
	if (pBuffer == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- GSPGV\n");
		return false;
	}

	if (pos_x >= width || pos_y >= height) {
		OutputDebugString("\nERROR: Position out of image! --- GSPGV\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size must not be zero! --- GSPGV\n");
		return false;
	}

	pixelset.clear();

	long hmask_width = mask_width / 2;
	long hmask_height = mask_height / 2;

	long x_begin = 0;
	long x_end = width - 1;
	if (pos_x - hmask_width > 0)
		x_begin = pos_x - hmask_width;
	if (pos_x + hmask_width < width)
		x_end = pos_x + hmask_width;

	x_begin = x_begin < 0 ? 0 : x_begin;

	long y_begin = 0;
	long y_end = height - 1;
	if (pos_y - hmask_height > 0)
		y_begin = pos_y - hmask_height;
	if (pos_y + hmask_height < height)
		y_end = pos_y + hmask_height;

	y_begin = y_begin < 0 ? 0 : y_begin;

	for (unsigned long y = y_begin; y <= y_end; ++y) {
		for (unsigned long x = x_begin; x <= x_end; ++x) {
			pixelset.push_back(pBuffer[y][x]);
		}
	}

	return true;
}

bool Gray_Image_Processing::__getAdjacent_LR_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_width, std::vector<unsigned char> &pixelset) {
	if (pBuffer == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- GALRPGV\n");
		return false;
	}

	if (pos_x >= width || pos_y >= height) {
		OutputDebugString("\nERROR: Position out of image! --- GALRPGV\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size must not be zero! --- GALRPGV\n");
		return false;
	}

	pixelset.clear();

	long hmask_width = mask_width / 2;

	long x_begin = 0;
	long x_end = width - 1;
	if (pos_x - hmask_width > 0)
		x_begin = pos_x - hmask_width;
	if (pos_x + hmask_width < width)
		x_end = pos_x + hmask_width;

	x_begin = x_begin < 0 ? 0 : x_begin;

	for (unsigned long x = x_begin; x <= x_end; ++x) {
		pixelset.push_back(pBuffer[pos_y][x]);
	}

	return true;
}

bool Gray_Image_Processing::__getAdjacent_AB_PixelsGrayValue(unsigned char** pBuffer, unsigned long pos_x, unsigned long pos_y, unsigned long width, unsigned long height, unsigned long mask_height, std::vector<unsigned char> &pixelset) {
	if (pBuffer == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- GAABPGV\n");
		return false;
	}

	if (pos_x >= width || pos_y >= height) {
		OutputDebugString("\nERROR: Position out of image! --- GAABPGV\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size must not be zero! --- GAABPGV\n");
		return false;
	}

	pixelset.clear();

	long hmask_height = mask_height / 2;

	long y_begin = 0;
	long y_end = height - 1;
	if (pos_y - hmask_height > 0)
		y_begin = pos_y - hmask_height;
	if (pos_y + hmask_height < height)
		y_end = pos_y + hmask_height;

	y_begin = y_begin < 0 ? 0 : y_begin;

	for (unsigned long y = y_begin; y <= y_end; ++y) {
		pixelset.push_back(pBuffer[y][pos_x]);
	}

	return true;
}

bool Gray_Image_Processing::__getAvrageGrayValue(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg) {
	if (pBuffer == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- GAGVR\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nWARNING: Image size equal zero! --- GAGVR\n");
		avg = 0;
		return true;
	}

	long long sum = 0;
	long count = 0;

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			sum += pBuffer[y][x];
			++count;
		}
	}
	if (count)
		avg = sum*1.0 / count*1.0;
	else {
		avg = 0;
		OutputDebugString("\nERROR: Pixel count equal zero! --- GAGVR\n");
		return false;
	}

	return true;
}

bool Gray_Image_Processing::__getAvrageGrayValue_region(unsigned char** pBuffer, unsigned long width, unsigned long height, double &avg) {
	if (pBuffer == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- GAGVR\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nWARNING: Image size equal zero! --- GAGVR\n");
		avg = 0;
		return true;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- GAGVR\n");
		return false;
	}

	long long sum = 0;
	long count = 0;
	long sumlines = m_rl->num;

	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			sum += pBuffer[y][x];
			++count;
		}
	}

	if (count)
		avg = sum*1.0 / count*1.0;
	else {
		avg = 0;
		OutputDebugString("\nERROR: Pixel count equal zero! --- GAGVR\n");
		return false;
	}

	return true;
}

bool Gray_Image_Processing::__subtract(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height) {
	if (pMinuend == NULL || pSubtrahend == NULL || pDiff == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- SUB\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size equal zero! --- SUB\n");
		return false;
	}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			pDiff[y][x] = pMinuend[y][x] - pSubtrahend[y][x];
		}
	}

	return true;
}

bool Gray_Image_Processing::__subtract_region(unsigned char** pMinuend, unsigned char** pSubtrahend, short** &pDiff, unsigned long width, unsigned long height) {
	if (pMinuend == NULL || pSubtrahend == NULL || pDiff == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- SUB\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size equal zero! --- SUB\n");
		return false;
	}

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			pDiff[y][x] = pMinuend[y][x] - pSubtrahend[y][x];
		}
	}

	return true;
}

bool Gray_Image_Processing::__addition(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M) {
	if (pSrc == NULL || pAddend == NULL || pDst == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- ADD\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size equal zero! --- ADD\n");
		return false;
	}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			short value = pSrc[y][x] + (pAddend[y][x] * M);
			if (value > 0xFF)
				value = 0xFF;
			else if (value < 0)
				value = 0;
			pDst[y][x] = (unsigned char)value;
		}
	}

	return true;
}

bool Gray_Image_Processing::__addition_region(unsigned char** pSrc, short** pAddend, unsigned char** &pDst, unsigned long width, unsigned long height, double M) {
	if (pSrc == NULL || pAddend == NULL || pDst == NULL) {
		OutputDebugString("\nERROR: Image noexist! --- ADD\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: Image size equal zero! --- ADD\n");
		return false;
	}

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			short value = pSrc[y][x] + (pAddend[y][x] * M);
			if (value > 0xFF)
				value = 0xFF;
			else if (value < 0)
				value = 0;
			pDst[y][x] = (unsigned char)value;
		}
	}

	return true;
}

bool Gray_Image_Processing::_binaryzation(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Buffer noexist! --- Binaryzation - 1\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: WIDTH or HEIGHT must not be ZERO! --- Binaryzation - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	//bool valueTable[256];
	//memset(valueTable, 1, sizeof(bool) * 256);
	//for (short i = 0; i < threshold; ++i) {
	//	valueTable[i] = 0;
	//}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			if (pSrc[y][x] >= threshold)
				/*if (valueTable[pSrc[y][x]])*/
				pDst[y][x] = 0xFF;
			else
				pDst[y][x] = 0;
		}
	}
	return true;
}

bool Gray_Image_Processing::_binaryzation_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned char threshold) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Buffer noexist! --- Binaryzation - 1\n");
		return false;
	}

	if (width == 0 || height == 0) {
		OutputDebugString("\nERROR: WIDTH or HEIGHT must not be ZERO! --- Binaryzation - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- Binaryzation - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			if (pSrc[y][x] >= threshold)
				pDst[y][x] = 0xFF;
			else
				pDst[y][x] = 0;
		}
	}

	return true;
}

bool Gray_Image_Processing::_erosion(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Erosion - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned long hmask_width = mask_width / 2;
	unsigned long hmask_height = mask_height / 2;

	//当模板高度为1时
	if (mask_height == 1) {

		for (unsigned long rownum = 0; rownum < height; ++rownum) {
			//最小灰度值
			unsigned char current_min = 0xFF;
			//坐标值
			unsigned long x = 0, y = rownum;
			//当前行已记录的最小灰度值的位置
			unsigned long min_pos = -1;

			//当滑动窗口在左端
			//******-----------------
			for (x = 0; x <= hmask_width; ++x) {

				current_min = 0xFF;
				if (min_pos == -1) {//当没有记录最小灰度值坐标位置时,遍历窗口内全部元素
					for (long i = 0; i <= x + hmask_width; ++i) {
						unsigned char local_min = pSrc[y][i];
						if (local_min < current_min) {
							min_pos = i;
							current_min = local_min;
						}
					}
				}
				else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
					unsigned char local_min = pSrc[y][x + hmask_width];//新加入窗口的像素值
					if (local_min <= pSrc[y][min_pos]) {
						current_min = local_min;
						min_pos = x + hmask_width;
					}
					else {
						current_min = pSrc[y][min_pos];
					}
				}

				pDst[y][x] = current_min;
			}

			//当滑动窗口在当前行中部
			//-----**********-----
			min_pos = -1;
			for (; x <= (width - 1) - hmask_width; ++x) {

				current_min = 0xFF;
				if (min_pos == -1 || min_pos < (x - hmask_width)) {//当没有记录最小灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
					for (long i = x - hmask_width; i <= x + hmask_width; ++i) {
						unsigned char local_min = pSrc[y][i];
						if (local_min < current_min) {
							min_pos = i;
							current_min = local_min;
						}
					}
				}
				else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
					unsigned char local_min = pSrc[y][x + hmask_width];//新加入窗口的像素值
					if (local_min <= pSrc[y][min_pos]) {
						current_min = local_min;
						min_pos = x + hmask_width;
					}
					else {
						current_min = pSrc[y][min_pos];
					}
				}

				pDst[y][x] = current_min;
			}

			//当滑动窗口在右端
			//-------------******
			for (; x <= width - 1; ++x) {//尾部不需再记录最小值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小

				current_min = 0xFF;
				for (long i = x - hmask_width; i <= width - 1; ++i) {
					unsigned char local_min = pSrc[y][i];
					if (local_min < current_min)
						current_min = local_min;
				}

				pDst[y][x] = current_min;
			}

		}

		return true;
	}

	//当模板宽度为1时
	if (mask_width == 1) {

		for (long colnum = 0; colnum < width; ++colnum) {
			//最小灰度值
			unsigned char current_min = 0xFF;
			//坐标值
			unsigned long x = colnum, y = 0;
			//当前列已记录的最小灰度值的位置
			unsigned long min_pos = -1;

			//当滑动窗口在上部
			//******-----------------
			for (y = 0; y <= hmask_height; ++y) {
				current_min = 0xFF;
				if (min_pos == -1) {//当没有记录最小灰度值坐标位置时,遍历窗口内全部元素
					for (long i = 0; i <= y + hmask_height; ++i) {
						unsigned char local_min = pSrc[i][x];
						if (local_min < current_min) {
							current_min = local_min;
							min_pos = i;
						}
					}
				}
				else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
					unsigned char local_min = pSrc[y + hmask_height][x];
					if (local_min <= pSrc[min_pos][x]) {//
						current_min = local_min;
						min_pos = y + hmask_height;
					}
					else {
						current_min = pSrc[min_pos][x];
					}
				}
				pDst[y][x] = current_min;
			}

			//当滑动窗口在当前列中部
			//-----**********-----

			min_pos = -1;
			for (; y <= height - 1 - hmask_height; ++y) {//当没有记录最小灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
				current_min = 0xFF;
				if (min_pos == -1 || min_pos < (y - hmask_height)) {
					for (long i = y - hmask_height; i <= y + hmask_height; ++i) {
						unsigned char local_min = pSrc[i][x];
						if (local_min < current_min) {
							current_min = local_min;
							min_pos = i;
						}
					}
				}
				else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
					unsigned char local_min = pSrc[y + hmask_height][x];
					if (local_min <= pSrc[min_pos][x]) {
						current_min = local_min;
						min_pos = y + hmask_height;
					}
					else {
						current_min = pSrc[min_pos][x];
					}
				}
				pDst[y][x] = current_min;
			}

			//当滑动窗口在下部
			//-------------******

			for (; y <= height - 1; ++y) {//尾部不需再记录最小值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小
				current_min = 0xFF;
				for (long i = y - hmask_height; i <= height - 1; ++i) {
					unsigned char local_min = pSrc[i][x];
					if (local_min < current_min)
						current_min = local_min;
				}

				pDst[y][x] = current_min;
			}

		}

		return true;
	}

	//当模板宽度高度都大于1时

	unsigned char** pMid = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		pMid[i] = new unsigned char[width];
	}

	//先求得行中的窗口最小值
	for (unsigned long rownum = 0; rownum < height; ++rownum) {
		//最小灰度值
		unsigned char current_min = 0xFF;
		//坐标值
		unsigned long x = 0, y = rownum;
		//当前行已记录的最小灰度值的位置
		unsigned long min_pos = -1;

		//当滑动窗口在左端
		//******-----------------
		for (x = 0; x <= hmask_width; ++x) {

			current_min = 0xFF;
			if (min_pos == -1) {//当没有记录最小灰度值坐标位置时,遍历窗口内全部元素
				for (long i = 0; i <= x + hmask_width; ++i) {
					unsigned char local_min = pSrc[y][i];
					if (local_min < current_min) {
						min_pos = i;
						current_min = local_min;
					}
				}
			}
			else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
				unsigned char local_min = pSrc[y][x + hmask_width];//新加入窗口的像素值
				if (local_min <= pSrc[y][min_pos]) {
					current_min = local_min;
					min_pos = x + hmask_width;
				}
				else {
					current_min = pSrc[y][min_pos];
				}
			}

			pMid[y][x] = current_min;
		}

		//当滑动窗口在当前行中部
		//-----**********-----
		min_pos = -1;
		for (; x <= (width - 1) - hmask_width; ++x) {

			current_min = 0xFF;
			if (min_pos == -1 || min_pos < (x - hmask_width)) {//当没有记录最小灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
				for (long i = x - hmask_width; i <= x + hmask_width; ++i) {
					unsigned char local_min = pSrc[y][i];
					if (local_min < current_min) {
						min_pos = i;
						current_min = local_min;
					}
				}
			}
			else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
				unsigned char local_min = pSrc[y][x + hmask_width];//新加入窗口的像素值
				if (local_min <= pSrc[y][min_pos]) {
					current_min = local_min;
					min_pos = x + hmask_width;
				}
				else {
					current_min = pSrc[y][min_pos];
				}
			}

			pMid[y][x] = current_min;
		}

		//当滑动窗口在右端
		//-------------******
		for (; x <= width - 1; ++x) {//尾部不需再记录最小值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小

			current_min = 0xFF;
			for (long i = x - hmask_width; i <= width - 1; ++i) {
				unsigned char local_min = pSrc[y][i];
				if (local_min < current_min)
					current_min = local_min;
			}

			pMid[y][x] = current_min;
		}

	}

	//再从行的最小值中得到整个窗口的最小值
	for (long colnum = 0; colnum < width; ++colnum) {
		//最小灰度值
		unsigned char current_min = 0xFF;
		//坐标值
		unsigned long x = colnum, y = 0;
		//当前列已记录的最小灰度值的位置
		unsigned long min_pos = -1;

		//当滑动窗口在上部
		//******-----------------
		for (y = 0; y <= hmask_height; ++y) {
			current_min = 0xFF;
			if (min_pos == -1) {//当没有记录最小灰度值坐标位置时,遍历窗口内全部元素
				for (long i = 0; i <= y + hmask_height; ++i) {
					unsigned char local_min = pMid[i][x];
					if (local_min < current_min) {
						current_min = local_min;
						min_pos = i;
					}
				}
			}
			else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
				unsigned char local_min = pMid[y + hmask_height][x];
				if (local_min <= pMid[min_pos][x]) {//
					current_min = local_min;
					min_pos = y + hmask_height;
				}
				else {
					current_min = pMid[min_pos][x];
				}
			}
			pDst[y][x] = current_min;
		}

		//当滑动窗口在当前列中部
		//-----**********-----

		min_pos = -1;
		for (; y <= height - 1 - hmask_height; ++y) {//当没有记录最小灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
			current_min = 0xFF;
			if (min_pos == -1 || min_pos < (y - hmask_height)) {
				for (long i = y - hmask_height; i <= y + hmask_height; ++i) {
					unsigned char local_min = pMid[i][x];
					if (local_min < current_min) {
						current_min = local_min;
						min_pos = i;
					}
				}
			}
			else {//如果最小灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最小值比对即可
				unsigned char local_min = pMid[y + hmask_height][x];
				if (local_min <= pMid[min_pos][x]) {
					current_min = local_min;
					min_pos = y + hmask_height;
				}
				else {
					current_min = pMid[min_pos][x];
				}
			}
			pDst[y][x] = current_min;
		}

		//当滑动窗口在下部
		//-------------******

		for (; y <= height - 1; ++y) {//尾部不需再记录最小值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小
			current_min = 0xFF;
			for (long i = y - hmask_height; i <= height - 1; ++i) {
				unsigned char local_min = pMid[i][x];
				if (local_min < current_min)
					current_min = local_min;
			}

			pDst[y][x] = current_min;
		}

	}

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMid[i];
	}
	delete[] pMid;

	return true;
}

bool Gray_Image_Processing::_erosion_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Erosion - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- Erosion - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	std::vector<unsigned char > pixelset;

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			bool ret = __getSurroundPixelsGrayValue(pSrc, x, y, width, height, mask_width, mask_height, pixelset);
			if (ret) {
				std::sort(pixelset.begin(), pixelset.end());
				pDst[y][x] = *pixelset.begin();
				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get surround pixels failed! --- Erosion - 1\n");
				return false;
			}
		}
	}

	return true;
}

bool Gray_Image_Processing::_dilition(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Dilition - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned long hmask_width = mask_width / 2;
	unsigned long hmask_height = mask_height / 2;

	//当模板高度为1时
	if (mask_height == 1) {

		for (unsigned long rownum = 0; rownum < height; ++rownum) {
			//最大灰度值
			unsigned char current_max = 0;
			//坐标值
			unsigned long x = 0, y = rownum;
			//当前行已记录的最大灰度值的位置
			unsigned long max_pos = -1;

			//当滑动窗口在左端
			//******-----------------
			for (x = 0; x <= hmask_width; ++x) {

				current_max = 0;
				if (max_pos == -1) {//当没有记录最大灰度值坐标位置时,遍历窗口内全部元素
					for (long i = 0; i <= x + hmask_width; ++i) {
						unsigned char local_max = pSrc[y][i];
						if (local_max > current_max) {
							max_pos = i;
							current_max = local_max;
						}
					}
				}
				else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
					unsigned char local_max = pSrc[y][x + hmask_width];//新加入窗口的像素值
					if (local_max >= pSrc[y][max_pos]) {
						current_max = local_max;
						max_pos = x + hmask_width;
					}
					else {
						current_max = pSrc[y][max_pos];
					}
				}

				pDst[y][x] = current_max;
			}

			//当滑动窗口在当前行中部
			//-----**********-----
			max_pos = -1;
			for (; x <= (width - 1) - hmask_width; ++x) {

				current_max = 0;
				if (max_pos == -1 || max_pos < (x - hmask_width)) {//当没有记录最大灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
					for (long i = x - hmask_width; i <= x + hmask_width; ++i) {
						unsigned char local_max = pSrc[y][i];
						if (local_max > current_max) {
							max_pos = i;
							current_max = local_max;
						}
					}
				}
				else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
					unsigned char local_max = pSrc[y][x + hmask_width];//新加入窗口的像素值
					if (local_max >= pSrc[y][max_pos]) {
						current_max = local_max;
						max_pos = x + hmask_width;
					}
					else {
						current_max = pSrc[y][max_pos];
					}
				}

				pDst[y][x] = current_max;
			}

			//当滑动窗口在右端
			//-------------******
			for (; x <= width - 1; ++x) {//尾部不需再记录最大值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小

				current_max = 0;
				for (long i = x - hmask_width; i <= width - 1; ++i) {
					unsigned char local_max = pSrc[y][i];
					if (local_max > current_max)
						current_max = local_max;
				}

				pDst[y][x] = current_max;
			}

		}

		return true;
	}

	//当模板宽度为1时
	if (mask_width == 1) {

		for (long colnum = 0; colnum < width; ++colnum) {
			//最大灰度值
			unsigned char current_max = 0;
			//坐标值
			unsigned long x = colnum, y = 0;
			//当前列已记录的最大灰度值的位置
			unsigned long max_pos = -1;

			//当滑动窗口在上部
			//******-----------------
			for (y = 0; y <= hmask_height; ++y) {
				current_max = 0;
				if (max_pos == -1) {//当没有记录最大灰度值坐标位置时,遍历窗口内全部元素
					for (long i = 0; i <= y + hmask_height; ++i) {
						unsigned char local_max = pSrc[i][x];
						if (local_max > current_max) {
							current_max = local_max;
							max_pos = i;
						}
					}
				}
				else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
					unsigned char local_max = pSrc[y + hmask_height][x];
					if (local_max >= pSrc[max_pos][x]) {
						current_max = local_max;
						max_pos = y + hmask_height;
					}
					else {
						current_max = pSrc[max_pos][x];
					}
				}
				pDst[y][x] = current_max;
			}

			//当滑动窗口在当前列中部
			//-----**********-----

			max_pos = -1;
			for (; y <= height - 1 - hmask_height; ++y) {//当没有记录最大灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
				current_max = 0;
				if (max_pos == -1 || max_pos < (y - hmask_height)) {
					for (long i = y - hmask_height; i <= y + hmask_height; ++i) {
						unsigned char local_max = pSrc[i][x];
						if (local_max > current_max) {
							current_max = local_max;
							max_pos = i;
						}
					}
				}
				else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
					unsigned char local_max = pSrc[y + hmask_height][x];
					if (local_max >= pSrc[max_pos][x]) {
						current_max = local_max;
						max_pos = y + hmask_height;
					}
					else {
						current_max = pSrc[max_pos][x];
					}
				}
				pDst[y][x] = current_max;
			}

			//当滑动窗口在下部
			//-------------******

			for (; y <= height - 1; ++y) {//尾部不需再记录最大值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小
				current_max = 0;
				for (long i = y - hmask_height; i <= height - 1; ++i) {
					unsigned char local_max = pSrc[i][x];
					if (local_max > current_max)
						current_max = local_max;
				}

				pDst[y][x] = current_max;
			}

		}

		return true;
	}

	//当模板宽度高度都大于1时

	unsigned char** pMid = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		pMid[i] = new unsigned char[width];
	}

	//先求得行中的窗口最大值
	for (unsigned long rownum = 0; rownum < height; ++rownum) {
		//最大灰度值
		unsigned char current_max = 0;
		//坐标值
		unsigned long x = 0, y = rownum;
		//当前行已记录的最大灰度值的位置
		unsigned long max_pos = -1;

		//当滑动窗口在左端
		//******-----------------
		for (x = 0; x <= hmask_width; ++x) {

			current_max = 0;
			if (max_pos == -1) {//当没有记录最大灰度值坐标位置时,遍历窗口内全部元素
				for (long i = 0; i <= x + hmask_width; ++i) {
					unsigned char local_max = pSrc[y][i];
					if (local_max > current_max) {
						max_pos = i;
						current_max = local_max;
					}
				}
			}
			else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
				unsigned char local_max = pSrc[y][x + hmask_width];//新加入窗口的像素值
				if (local_max >= pSrc[y][max_pos]) {
					current_max = local_max;
					max_pos = x + hmask_width;
				}
				else {
					current_max = pSrc[y][max_pos];
				}
			}

			pMid[y][x] = current_max;
		}

		//当滑动窗口在当前行中部
		//-----**********-----
		max_pos = -1;
		for (; x <= (width - 1) - hmask_width; ++x) {

			current_max = 0;
			if (max_pos == -1 || max_pos < (x - hmask_width)) {//当没有记录最大灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
				for (long i = x - hmask_width; i <= x + hmask_width; ++i) {
					unsigned char local_max = pSrc[y][i];
					if (local_max > current_max) {
						max_pos = i;
						current_max = local_max;
					}
				}
			}
			else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
				unsigned char local_max = pSrc[y][x + hmask_width];//新加入窗口的像素值
				if (local_max >= pSrc[y][max_pos]) {
					current_max = local_max;
					max_pos = x + hmask_width;
				}
				else {
					current_max = pSrc[y][max_pos];
				}
			}

			pMid[y][x] = current_max;
		}

		//当滑动窗口在右端
		//-------------******
		for (; x <= width - 1; ++x) {//尾部不需再记录最大值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小

			current_max = 0;
			for (long i = x - hmask_width; i <= width - 1; ++i) {
				unsigned char local_max = pSrc[y][i];
				if (local_max > current_max)
					current_max = local_max;
			}

			pMid[y][x] = current_max;
		}

	}

	//再从行的最大值中得到整个窗口的最大值
	for (long colnum = 0; colnum < width; ++colnum) {
		//最大灰度值
		unsigned char current_max = 0;
		//坐标值
		unsigned long x = colnum, y = 0;
		//当前列已记录的最大灰度值的位置
		unsigned long max_pos = -1;

		//当滑动窗口在上部
		//******-----------------
		for (y = 0; y <= hmask_height; ++y) {
			current_max = 0;
			if (max_pos == -1) {//当没有记录最大灰度值坐标位置时,遍历窗口内全部元素
				for (long i = 0; i <= y + hmask_height; ++i) {
					unsigned char local_max = pMid[i][x];
					if (local_max > current_max) {
						current_max = local_max;
						max_pos = i;
					}
				}
			}
			else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
				unsigned char local_max = pMid[y + hmask_height][x];
				if (local_max >= pMid[max_pos][x]) {//
					current_max = local_max;
					max_pos = y + hmask_height;
				}
				else {
					current_max = pMid[max_pos][x];
				}
			}
			pDst[y][x] = current_max;
		}

		//当滑动窗口在当前列中部
		//-----**********-----

		max_pos = -1;
		for (; y <= height - 1 - hmask_height; ++y) {//当没有记录最大灰度值坐标位置 或者 其位置已经不在窗口内时,遍历窗口内全部元素
			current_max = 0;
			if (max_pos == -1 || max_pos < (y - hmask_height)) {
				for (long i = y - hmask_height; i <= y + hmask_height; ++i) {
					unsigned char local_max = pMid[i][x];
					if (local_max > current_max) {
						current_max = local_max;
						max_pos = i;
					}
				}
			}
			else {//如果最大灰度值坐标位置有记录,那么只需要将新加入窗口的像素值与最大值比对即可
				unsigned char local_max = pMid[y + hmask_height][x];
				if (local_max >= pMid[max_pos][x]) {
					current_max = local_max;
					max_pos = y + hmask_height;
				}
				else {
					current_max = pMid[max_pos][x];
				}
			}
			pDst[y][x] = current_max;
		}

		//当滑动窗口在下部
		//-------------******

		for (; y <= height - 1; ++y) {//尾部不需再记录最大值位置,因为没有新像素增加,只有像素离开窗口,对其遍历开销很小
			current_max = 0;
			for (long i = y - hmask_height; i <= height - 1; ++i) {
				unsigned char local_max = pMid[i][x];
				if (local_max > current_max)
					current_max = local_max;
			}

			pDst[y][x] = current_max;
		}

	}

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMid[i];
	}
	delete[] pMid;

	return true;
}

bool Gray_Image_Processing::_dilition_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Dilition - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- Dilition - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	std::vector<unsigned char > pixelset;

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			bool ret = __getSurroundPixelsGrayValue(pSrc, x, y, width, height, mask_width, mask_height, pixelset);
			if (ret) {
				std::sort(pixelset.begin(), pixelset.end());
				pDst[y][x] = *prev(pixelset.end());
				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get surround pixels failed! --- Dilition - 1\n");
				return false;
			}
		}
	}

	return true;
}

bool Gray_Image_Processing::_medianfilter(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- MedianFilter - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	std::vector<unsigned char > pixelset;

	for (unsigned long y = 0; y < height; ++y) {
		for (unsigned long x = 0; x < width; ++x) {
			bool ret = __getSurroundPixelsGrayValue(pSrc, x, y, width, height, mask_width, mask_height, pixelset);
			if (ret) {
				std::sort(pixelset.begin(), pixelset.end());
				if (pixelset.size() >= 1) {
					unsigned long mid = pixelset.size() / 2;
					if (pixelset.size() % 2 == 0) {
						pDst[y][x] = (pixelset.at(mid) + pixelset.at(mid + 1)) / 2;
					}
					else
						pDst[y][x] = pixelset.at(mid);
				}

				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get surround pixels failed! --- MedianFilter - 1\n");
				return false;
			}
		}
	}

	return true;
}

bool Gray_Image_Processing::_medianfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- MedianFilter - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- MedianFilter - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	std::vector<unsigned char > pixelset;

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			bool ret = __getSurroundPixelsGrayValue(pSrc, x, y, width, height, mask_width, mask_height, pixelset);
			if (ret) {
				std::sort(pixelset.begin(), pixelset.end());
				if (pixelset.size() >= 1) {
					unsigned long mid = pixelset.size() / 2;
					if (pixelset.size() % 2 == 0) {
						pDst[y][x] = (pixelset.at(mid) + pixelset.at(mid + 1)) / 2;
					}
					else
						pDst[y][x] = pixelset.at(mid);
				}

				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get surround pixels failed! --- MedianFilter - 1\n");
				return false;
			}
		}
	}

	return true;
}

bool Gray_Image_Processing::_meanfilter_normal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- MeanFilter - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned long hmask_width = mask_width / 2;
	unsigned long hmask_height = mask_height / 2;

	//当模板高度为1时
	if (mask_height == 1) {
		std::vector<unsigned char > pixelset;
		for (unsigned long rownum = 0; rownum < height; ++rownum) {
			for (unsigned long x = 0; x < width; ++x) {
				bool ret = __getAdjacent_LR_PixelsGrayValue(pSrc, x, rownum, width, height, mask_width, pixelset);
				if (ret) {
					if (pixelset.size() > 0) {
						unsigned long long sum = 0;
						for (std::vector<unsigned char >::iterator pvar = pixelset.begin(); pvar < pixelset.end(); ++pvar) {
							sum += *pvar;
						}
						pDst[rownum][x] = sum*1.0 / pixelset.size();//不会为0 
					}
					else {
						pDst[rownum][x] = 0;
					}
					pixelset.clear();
				}
				else {
					OutputDebugString("\nERROR: Get left and right pixels failed! --- MeanFilter - 1\n");
					return false;
				}
				pixelset.clear();
			}
		}
		return true;
	}

	//当模板宽度为1时
	if (mask_width == 1) {
		std::vector<unsigned char > pixelset;
		for (long colnum = 0; colnum < width; ++colnum) {
			for (unsigned long y = 0; y < height; ++y) {
				bool ret = __getAdjacent_LR_PixelsGrayValue(pSrc, colnum, y, width, height, mask_width, pixelset);
				if (ret) {
					if (pixelset.size() > 0) {
						unsigned long long sum = 0;
						for (std::vector<unsigned char >::iterator pvar = pixelset.begin(); pvar < pixelset.end(); ++pvar) {
							sum += *pvar;
						}
						pDst[y][colnum] = sum*1.0 / pixelset.size();//不会为0 
					}
					else {
						pDst[y][colnum] = 0;
					}
					pixelset.clear();
				}
				else {
					OutputDebugString("\nERROR: Get above and below pixels failed! --- MeanFilter - 1\n");
					return false;
				}

				pixelset.clear();
			}
		}

		return true;
	}

	//当模板宽度高度都大于1时

	std::vector<unsigned char > pixelset;

	unsigned char** pMid = new unsigned char*[height];
	for (int i = 0; i < height; ++i) {
		pMid[i] = new unsigned char[width];
	}

	//先求得行中的窗口均值
	for (unsigned long rownum = 0; rownum < height; ++rownum) {
		for (unsigned long x = 0; x < width; ++x) {
			bool ret = __getAdjacent_LR_PixelsGrayValue(pSrc, x, rownum, width, height, mask_width, pixelset);
			if (ret) {
				if (pixelset.size() > 0) {
					unsigned long long sum = 0;
					for (std::vector<unsigned char >::iterator pvar = pixelset.begin(); pvar < pixelset.end(); ++pvar) {
						sum += *pvar;
					}
					pMid[rownum][x] = sum*1.0 / pixelset.size();//不会为0 
				}
				else {
					pMid[rownum][x] = 0;
				}
				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get left and right pixels failed! --- MeanFilter - 1\n");
				return false;
			}

			pixelset.clear();
		}
	}

	//再从行的均值中得到整个窗口的均值
	for (long colnum = 0; colnum < width; ++colnum) {
		for (unsigned long y = 0; y < height; ++y) {
			bool ret = __getAdjacent_LR_PixelsGrayValue(pMid, colnum, y, width, height, mask_width, pixelset);
			if (ret) {
				if (pixelset.size() > 0) {
					unsigned long long sum = 0;
					for (std::vector<unsigned char >::iterator pvar = pixelset.begin(); pvar < pixelset.end(); ++pvar) {
						sum += *pvar;
					}
					pDst[y][colnum] = sum*1.0 / pixelset.size();//不会为0 
				}
				else {
					pDst[y][colnum] = 0;
				}
				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get above and below pixels failed! --- MeanFilter - 1\n");
				return false;
			}
			pixelset.clear();
		}
	}

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMid[i];
	}
	delete[] pMid;

	return true;
}

bool Gray_Image_Processing::_meanfilter_increment(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- MeanFilter - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned long hmask_width = mask_width / 2;
	unsigned long hmask_height = mask_height / 2;

	unsigned long maskNum = mask_width*mask_height;

	//当模板高度为1时
	if (mask_height == 1) {
		for (unsigned long rownum = 0; rownum < height; ++rownum) {
			unsigned long sum = 0;
			for (unsigned long i = 0; i <= hmask_width; ++i) {
				sum += pSrc[rownum][i];
			}
			pDst[rownum][0] = sum;
			for (unsigned long x = 1; x <= hmask_width; ++x) {
				sum += pSrc[rownum][x + hmask_width];
				pDst[rownum][x] = (unsigned char)(1.0*sum / maskNum);
			}

			for (unsigned long x = hmask_width + 1; x <= (width - 1) - hmask_width; ++x) {
				sum -= pSrc[rownum][x - 1 - hmask_width];
				sum += pSrc[rownum][x + hmask_width];
				pDst[rownum][x] = (unsigned char)(1.0*sum / maskNum);
			}

			for (unsigned long x = width - hmask_width; x < width; ++x) {
				sum -= pSrc[rownum][x - 1 - hmask_width];
				pDst[rownum][x] = (unsigned char)(1.0*sum / maskNum);
			}
		}
		return true;
	}

	//当模板宽度为1时
	if (mask_width == 1) {
		for (long colnum = 0; colnum < width; ++colnum) {
			unsigned long sum = 0;
			for (unsigned long i = 0; i <= hmask_height; ++i) {
				sum += pSrc[i][colnum];
			}
			pDst[0][colnum] = sum;
			for (unsigned long y = 1; y <= hmask_height; ++y) {
				sum += pSrc[y + hmask_height][colnum];
				pDst[y][colnum] = sum / maskNum;
			}

			for (unsigned long y = hmask_height + 1; y <= (height - 1) - hmask_height; ++y) {
				sum -= pSrc[y - 1 - hmask_height][colnum];
				sum += pSrc[y + hmask_height][colnum];
				pDst[y][colnum] = sum / maskNum;
			}

			for (unsigned long y = height - hmask_height; y < height; ++y) {
				sum -= pSrc[y - 1 - hmask_height][colnum];
				pDst[y][colnum] = sum / maskNum;
			}
		}

		return true;
	}

	//当模板宽度高度都大于1时

	//存放行的累加和的中间值
	unsigned long** pMid = new unsigned long*[height];
	for (int i = 0; i < height; ++i) {
		pMid[i] = new unsigned long[width];
	}

	//先求得行中的窗口均值
	for (unsigned long rownum = 0; rownum < height; ++rownum) {
		unsigned long sum = 0;
		for (unsigned long i = 0; i <= hmask_width; ++i) {
			sum += pSrc[rownum][i];
		}
		pDst[rownum][0] = sum;
		for (unsigned long x = 1; x <= hmask_width; ++x) {
			sum += pSrc[rownum][x + hmask_width];
			pMid[rownum][x] = sum;
		}

		for (unsigned long x = hmask_width + 1; x <= (width - 1) - hmask_width; ++x) {
			sum -= pSrc[rownum][x - 1 - hmask_width];
			sum += pSrc[rownum][x + hmask_width];
			pMid[rownum][x] = sum;
		}

		for (unsigned long x = width - hmask_width; x < width; ++x) {
			sum -= pSrc[rownum][x - 1 - hmask_width];
			pMid[rownum][x] = sum;
		}
	}

	//再从行的均值中得到整个窗口的均值
	for (long colnum = 0; colnum < width; ++colnum) {
		unsigned long sum = 0;
		for (unsigned long i = 0; i <= hmask_height; ++i) {
			sum += pMid[i][colnum];
		}
		pDst[0][colnum] = sum;
		for (unsigned long y = 1; y <= hmask_height; ++y) {
			sum += pMid[y + hmask_height][colnum];
			pDst[y][colnum] = (unsigned char)(1.0*sum / maskNum);
		}

		for (unsigned long y = hmask_height + 1; y <= (height - 1) - hmask_height; ++y) {
			sum -= pMid[y - 1 - hmask_height][colnum];
			sum += pMid[y + hmask_height][colnum];
			pDst[y][colnum] = (unsigned char)(1.0*sum / maskNum);
		}

		for (unsigned long y = height - hmask_height; y < height; ++y) {
			sum -= pMid[y - 1 - hmask_height][colnum];
			pDst[y][colnum] = (unsigned char)(1.0*sum / maskNum);
		}
	}

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMid[i];
	}
	delete[] pMid;

	return true;
}

bool Gray_Image_Processing::_meanfilter_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- MeanFilter - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- MeanFilter - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	std::vector<unsigned char > pixelset;

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			bool ret = __getSurroundPixelsGrayValue(pSrc, x, y, width, height, mask_width, mask_height, pixelset);
			if (ret) {
				if (pixelset.size() > 0) {
					unsigned long long sum = 0;
					for (std::vector<unsigned char >::iterator pvar = pixelset.begin(); pvar < pixelset.end(); ++pvar) {
						sum += *pvar;
					}
					pDst[y][x] = sum*1.0 / pixelset.size();//不会为0 
				}
				else {
					pDst[y][x] = 0;
				}
				pixelset.clear();
			}
			else {
				OutputDebugString("\nERROR: Get surround pixels failed! --- MeanFilter - 1\n");
				return false;
			}
		}
	}

	return true;
}

bool Gray_Image_Processing::_contrast(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Contrast - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	double global_avg = 0;
	__getAvrageGrayValue(pSrc, width, height, global_avg);

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			double value = ((double)pSrc[y][x] - global_avg) * M + 0x7F;
			if (value > 0xFF)
				value = 0xFF;
			if (value < 0)
				value = 0;
			pDst[y][x] = (unsigned char)value;
		}
	}

	return true;
}

bool Gray_Image_Processing::_contrast_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, double M) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Contrast - 1\n");
		return false;
	}

	if (m_bRegion == false || m_rl == NULL) {
		OutputDebugString("\nERROR: Region noexist! --- Contrast - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	double region_avg = 0;
	__getAvrageGrayValue(pSrc, width, height, region_avg);

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			double value = ((double)pSrc[y][x] - region_avg) * M + 0x7F;
			if (value > 0xFF)
				value = 0xFF;
			if (value < 0)
				value = 0;
			pDst[y][x] = (unsigned char)value;
		}
	}

	return true;
}

bool Gray_Image_Processing::_sharpen(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Sharpen - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned char** pMean = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pMean[i] = new unsigned char[width];
	}

	short** pDiffer = new short*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDiffer[i] = new short[width];
	}

	//_meanfilter_normal(pSrc, pMean, width, height, mask_width, mask_height);
	_meanfilter_increment(pSrc, pMean, width, height, mask_width, mask_height);
	__subtract(pSrc, pMean, pDiffer, width, height);
	__addition(pSrc, pDiffer, pDst, width, height, M);

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMean[i];
	}
	delete[] pMean;
	pMean = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pDiffer[i];
	}
	delete[] pDiffer;
	pDiffer = NULL;


	return true;
}

bool Gray_Image_Processing::_sharpen_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height, unsigned long mask_width, unsigned long mask_height, double M) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Sharpen - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	unsigned char** pMean = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pMean[i] = new unsigned char[width];
	}

	short** pDiffer = new short*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDiffer[i] = new short[width];
	}

	_meanfilter_region(pSrc, pMean, width, height, mask_width, mask_height);
	__subtract_region(pSrc, pMean, pDiffer, width, height);
	__addition_region(pSrc, pDiffer, pDst, width, height, M);

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pMean[i];
	}
	delete[] pMean;
	pMean = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] pDiffer[i];
	}
	delete[] pDiffer;
	pDiffer = NULL;

	return true;
}