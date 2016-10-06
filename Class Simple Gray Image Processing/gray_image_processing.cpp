#include "gray_image_processing.h"

/*------------------------------------------------------------------------------------------------------------
--------------------------------------------------Global------------------------------------------------------
------------------------------------------------------------------------------------------------------------*/

void __declspec(naked) __stdcall __sincos(const double Angle, double& sina, double& cosa)
{
	__asm
	{
		fld  qword ptr[esp + 4] //Angle   
		mov  esi, [esp + 12] //&sina
		mov  edi, [esp + 16] //&cosa
		fsincos
		fstp qword ptr[edi]
		fstp qword ptr[esi]
		ret 16
	}
}

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

bool Gray_Image_Processing::Binaryzation(CImg* pSrcImg, CImg* &pDstImg, unsigned char threshold, BINARYZATION_METHOD method) {
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

	switch (method) {

	case BINARYZATION_METHOD::BI_NONE: {

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

		break;
	}

	default: {
		break;
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

bool Gray_Image_Processing::Opening(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Opening - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pMidBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- Opening - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- Opening - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- Opening - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- Opening - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- Opening - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Opening - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Opening --- Opening - 0\n");
		ret = _erosion(pBuffer, pMidBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		ret = _dilition(pMidBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Opening FAILED! --- Opening - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Opening --- Erosion - 0\n");
		ret = _erosion_region(pBuffer, pMidBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		ret = _dilition_region(pMidBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Opening FAILED! --- Opening - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Opening - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pMidBuffer[i];
	}
	delete[] pMidBuffer;
	pMidBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	m_rl = NULL;
	m_bRegion = false;

	return true;
}

bool Gray_Image_Processing::Closing(CImg* pSrcImg, CImg* &pDstImg, unsigned long mask_width, unsigned long mask_height) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Closing - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pMidBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	if (mask_width == 0 || mask_height == 0) {
		OutputDebugString("\nERROR: Mask size must not be zero! --- Closing - 0\n");
		return false;
	}

	if ((mask_width > mask_height ? mask_width : mask_height) >= (WIDTH > HEIGHT ? HEIGHT : WIDTH)) {
		OutputDebugString("\nERROR: Mask size must smaller than size of image! --- Closing - 0\n");
		return false;
	}

	if (mask_width == 1 && mask_height == 1) {
		OutputDebugString("\nINFO: Mask size is 1 --- Closing - 0\n");
		pDstImg = create_image(pSrcImg);
		return true;
	}

	if (mask_width % 2 == 0) {
		OutputDebugString("\nINFO: Mask width should be odd --- Closing - 0\n");
		mask_width = mask_width + 1;
	}
	if (mask_height % 2 == 0) {
		OutputDebugString("\nINFO: Mask height should be odd --- Closing - 0\n");
		mask_height = mask_height + 1;
	}


	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Closing - 0\n");
		return false;
	}

	if (!m_bRegion) {
		OutputDebugString("\nINFO: Whole image Closing --- Closing - 0\n");
		ret = _dilition(pBuffer, pMidBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		ret = _erosion(pMidBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Closing FAILED! --- Closing - 0\n");
			return false;
		}
	}
	else {
		OutputDebugString("\nINFO: Region Closing --- Closing - 0\n");
		ret = _dilition_region(pBuffer, pMidBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		ret = _erosion_region(pMidBuffer, pDstBuffer, WIDTH, HEIGHT, mask_width, mask_height);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Region Closing FAILED! --- Closing - 0\n");
			return false;
		}
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Closing - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pMidBuffer[i];
	}
	delete[] pMidBuffer;
	pMidBuffer = NULL;

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

bool Gray_Image_Processing::Histogram(CImg* pSrcImg, unsigned long* &Histogram) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Histogram - 0\n");
		return false;
	}

	unsigned long height = pSrcImg->GetHeight();
	unsigned long width = pSrcImg->GetWidthPixel();

	Histogram = new unsigned long[256];
	memset(Histogram, 0, sizeof(unsigned long) * 256);

	unsigned char** pBuffer = NULL;

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Histogram - 0\n");
		return false;
	}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			++Histogram[pBuffer[y][x]];
		}
	}

	return false;
}

bool Gray_Image_Processing::HistogramToImage(unsigned long* Histogram, CImg* &pCreateImg, const unsigned  char bkgcolor, const unsigned  char forecolor, bool divided) {
	if (Histogram == NULL) {
		OutputDebugString("\nERROR: Histogram noexist! --- HistogramToImage - 0\n");
		return false;
	}

	//柱体之间的距离
	const unsigned long stripe_distance = divided;
	//柱体的宽度
	const unsigned long stripe_width = 5;

	//整张图片的高度
	const unsigned long width = 768;

	//图片的宽度
	const unsigned long height = (stripe_distance + stripe_width) * 256 + 10;

	//每个灰度值柱体的高度
	unsigned int striplen[256];

	//像素数最大值
	long pix_max = 0;
	for (int i = 0; i < 256; ++i) {
		if (Histogram[i] > pix_max)
			pix_max = Histogram[i];
	}

	if (pix_max == 0) {
		return false;
	}

	//计算每个像素柱体的高度
	for (int i = 0; i < 256; ++i) {
		double rate = Histogram[i] * 1.0 / pix_max*1.0;
		striplen[i] = (unsigned int)(1.0 * width * rate * 0.95 - 0.5);
	}

	unsigned char** pBuffer = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pBuffer[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memset(pBuffer[i], bkgcolor, sizeof(unsigned char)*width);
	}

	unsigned char no = 0;
	unsigned char flag = 0;
	for (unsigned long i = 5; i < height - 5; ++i) {

		//if (flag < stripe_width) {
		if (flag >= stripe_width) {
			++no;
			if (no == 256)
				break;
			flag = 0;
			if (stripe_distance != 0)
				continue;
			//}
		}
		int len = striplen[no] > width ? width : striplen[no];
		memset(pBuffer[i], forecolor, sizeof(unsigned char)*len);
		++flag;
		//}
		//else {
	}

	unsigned char** pRotateBuffer = NULL;
	_rotate_left(pBuffer, pRotateBuffer, width, height);

	pCreateImg = create_image();
	pCreateImg->InitArray8(pRotateBuffer, width, height);//图像旋转后SrcBuffer的宽度为DstBuffer的高度

	return true;
}

bool Gray_Image_Processing::RotateLeft(CImg* pSrcImg, CImg* &pDstImg) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- RotateLeft - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- RotateLeft - 0\n");
		return false;
	}

	ret = _rotate_left(pBuffer, pDstBuffer, WIDTH, HEIGHT);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: RotateLeft FAILED! --- RotateLeft - 0\n");
		return false;
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, WIDTH, HEIGHT);//图像旋转后必须将目标图像的宽高互换
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- RotateLeft - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < WIDTH; ++i) {//图像旋转后SrcBuffer的宽度为DstBuffer的高度
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	return true;
}

bool Gray_Image_Processing::RotateRight(CImg* pSrcImg, CImg* &pDstImg) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- RotateRight - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- RotateRight - 0\n");
		return false;
	}

	ret = _rotate_right(pBuffer, pDstBuffer, WIDTH, HEIGHT);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: RotateLeft FAILED! --- RotateRight - 0\n");
		return false;
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, WIDTH, HEIGHT);//图像旋转后必须将目标图像的宽高互换
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- RotateRight - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < WIDTH; ++i) {//图像旋转后SrcBuffer的宽度为DstBuffer的高度
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	return true;
}

bool Gray_Image_Processing::FlipHorizontal(CImg* pSrcImg, CImg* &pDstImg) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- FlipHorizontal - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- FlipHorizontal - 0\n");
		return false;
	}

	ret = _flip_horizontal(pBuffer, pDstBuffer, WIDTH, HEIGHT);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Flip Horizontal FAILED! --- FlipHorizontal - 0\n");
		return false;
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- FlipHorizontal - 0\n");
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

	return true;
}

bool Gray_Image_Processing::FlipVertical(CImg* pSrcImg, CImg* &pDstImg) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- FlipVertical - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- FlipVertical - 0\n");
		return false;
	}

	ret = _flip_vertical(pBuffer, pDstBuffer, WIDTH, HEIGHT);

	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Flip Vertical FAILED! --- FlipVertical - 0\n");
		return false;
	}

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT, WIDTH);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- FlipVertical - 0\n");
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

	return true;
}

bool Gray_Image_Processing::Scaling(CImg* pSrcImg, CImg* &pDstImg, double rate, SCALING_METHOD method) {
	if (rate == 0) {
		OutputDebugString("\nERROR: Rate cannot equal 0! --- Scaling - 0\n");
		return false;
	}

	if (method == SCALING_METHOD::SC_NONE && rate > 1) {
		OutputDebugString("\nERROR: Rate cannot greater than 1 when use NONE method! --- Scaling - 0\n");
		return false;
	}

	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Scaling - 0\n");
		return false;
	}

	switch (method) {
	case SCALING_METHOD::SC_NONE: {
		unsigned char** pBuffer = NULL;
		unsigned char** pDstBuffer = NULL;

		unsigned long WIDTH = pSrcImg->GetWidthPixel();
		unsigned long HEIGHT = pSrcImg->GetHeight();


		unsigned int piece = (int)(1 / rate);	//should not be 0

		if (piece == 0) {
			OutputDebugString("\nERROR: Rate lower than 1! --- Scaling - 0\n");
			return false;
		}

		int WIDTH_NEW = WIDTH / piece;
		int HEIGHT_NEW = HEIGHT / piece;

		bool ret = false;
		ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Scaling - 0\n");
			return false;
		}

		_scaling_none(pBuffer, pDstBuffer, WIDTH, HEIGHT, WIDTH_NEW, HEIGHT_NEW, piece, piece - 1);

		pDstImg = create_image();
		if (pDstBuffer)
			pDstImg->InitArray8(pDstBuffer, HEIGHT_NEW, WIDTH_NEW);
		else {
			OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Scaling - 0\n");
			return false;
		}

		for (unsigned long i = 0; i < HEIGHT; ++i) {
			delete[] pBuffer[i];
		}
		delete[] pBuffer;
		pBuffer = NULL;

		for (unsigned long i = 0; i < HEIGHT_NEW; ++i) {
			delete[] pDstBuffer[i];
		}
		delete[] pDstBuffer;
		pDstBuffer = NULL;

		return true;
	}
	case SCALING_METHOD::SC_NEAREST_NEIGHBOR: {
		unsigned char** pBuffer = NULL;
		unsigned char** pDstBuffer = NULL;

		unsigned long WIDTH = pSrcImg->GetWidthPixel();
		unsigned long HEIGHT = pSrcImg->GetHeight();

		int WIDTH_NEW = WIDTH * rate;
		int HEIGHT_NEW = HEIGHT * rate;

		bool ret = false;
		ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Scaling - 0\n");
			return false;
		}

		//TODO:

		pDstImg = create_image();
		if (pDstBuffer)
			pDstImg->InitArray8(pDstBuffer, HEIGHT_NEW, WIDTH_NEW);
		else {
			OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Scaling - 0\n");
			return false;
		}

		for (unsigned long i = 0; i < HEIGHT; ++i) {
			delete[] pBuffer[i];
		}
		delete[] pBuffer;
		pBuffer = NULL;

		for (unsigned long i = 0; i < HEIGHT_NEW; ++i) {
			delete[] pDstBuffer[i];
		}
		delete[] pDstBuffer;
		pDstBuffer = NULL;

		return true;
	}
	default: {
		return false;
	}
	}
}

bool Gray_Image_Processing::Scaling(CImg* pSrcImg, CImg* &pDstImg, unsigned long width_dst, unsigned long height_dst, SCALING_METHOD method) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Scaling - 0\n");
		return false;
	}

	switch (method) {
	case SCALING_METHOD::SC_NONE: {
		OutputDebugString("\nERROR: This overload function is not support this method param! --- Scaling - 0\n");
		return false;
	}
	case SCALING_METHOD::SC_NEAREST_NEIGHBOR: {
		unsigned char** pBuffer = NULL;
		unsigned char** pDstBuffer = NULL;

		unsigned long WIDTH = pSrcImg->GetWidthPixel();
		unsigned long HEIGHT = pSrcImg->GetHeight();

		int WIDTH_NEW = width_dst;
		int HEIGHT_NEW = height_dst;

		bool ret = false;
		ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
		if (ret == false || pBuffer == NULL) {
			OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Scaling - 0\n");
			return false;
		}

		//TODO:

		pDstImg = create_image();
		if (pDstBuffer)
			pDstImg->InitArray8(pDstBuffer, HEIGHT_NEW, WIDTH_NEW);
		else {
			OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Scaling - 0\n");
			return false;
		}

		for (unsigned long i = 0; i < HEIGHT; ++i) {
			delete[] pBuffer[i];
		}
		delete[] pBuffer;
		pBuffer = NULL;

		for (unsigned long i = 0; i < HEIGHT_NEW; ++i) {
			delete[] pDstBuffer[i];
		}
		delete[] pDstBuffer;
		pDstBuffer = NULL;

		return true;
	}
	default: {
		return false;
	}
	}
}

bool Gray_Image_Processing::ClipRectangle(CImg* pSrcImg, CImg* &pDstImg, long leftTop_x, long leftTop_y, unsigned long clip_width, unsigned long clip_height) {
	if (clip_height == 0 || clip_width == 0) {
		OutputDebugString("\nERROR: Rectangle Region size cannot be zero! --- ClipRectangle - 0\n");
		return false;
	}

	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- ClipRectangle - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	unsigned long WIDTH_NEW = clip_width;
	unsigned long HEIGHT_NEW = clip_height;

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- ClipRectangle - 0\n");
		return false;
	}

	//TODO:
	_clip_rectangle(pBuffer, pDstBuffer, leftTop_x, leftTop_y, WIDTH, HEIGHT, WIDTH_NEW, HEIGHT_NEW);
	//_scaling_none(pBuffer, pDstBuffer, WIDTH, HEIGHT, WIDTH_NEW, HEIGHT_NEW, piece, piece - 1);

	pDstImg = create_image();
	if (pDstBuffer)
		pDstImg->InitArray8(pDstBuffer, HEIGHT_NEW, WIDTH_NEW);
	else {
		OutputDebugString("\nERROR: Dst Buffer create FAILED! --- ClipRectangle - 0\n");
		return false;
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT_NEW; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	return true;


}

bool Gray_Image_Processing::ClipRegion(CImg* pSrcImg, CImg* &pDstImg, const unsigned char fillup_color) {
	if (m_bRegion == false && m_rl == NULL) {
		OutputDebugString("\nERROR: Region donot exist! --- ClipRegion - 0\n");
		return false;
	}

	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img donot exist! --- ClipRegion - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	unsigned long HEIGHT_NEW = m_rl->num;
	unsigned long WIDTH_NEW = 0;
	for (long n = 0; n < HEIGHT_NEW; ++n) {
		long tmp = m_rl->rl[n].ce - m_rl->rl[n].cb;
		if (tmp > WIDTH_NEW)
			WIDTH_NEW = tmp;
	}

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- ClipRegion - 0\n");
		return false;
	}

	ret = _clip_region(pBuffer, pDstBuffer, WIDTH, HEIGHT, WIDTH_NEW, HEIGHT_NEW, fillup_color);

	if (ret) {
		pDstImg = create_image();
		if (pDstBuffer)
			pDstImg->InitArray8(pDstBuffer, HEIGHT_NEW, WIDTH_NEW);
		else {
			OutputDebugString("\nERROR: Dst Buffer create FAILED! --- ClipRectangle - 0\n");
			return false;
		}
	}

	for (unsigned long i = 0; i < HEIGHT; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < HEIGHT_NEW; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	return true;


	return false;
}

bool Gray_Image_Processing::Rotate(CImg* pSrcImg, CImg* &pDstImg, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color, ROTATE_METHOD method, ROTATE_CLIP_METHOD clip_method) {
	if (pSrcImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- Rotate - 0\n");
		return false;
	}

	const unsigned long width_src = pSrcImg->GetWidthPixel();
	const unsigned long height_src = pSrcImg->GetHeight();

	if ((fabs(zoom_x*width_src) < 1.0e-4) || (fabs(zoom_y*height_src) < 1.0e-4)) {
		OutputDebugString("\nERROR: Zoom Rate is too small! --- Rotate - 0\n");
		return false;
	}

	unsigned long width_dst = 0;
	unsigned long height_dst = 0;

	switch (clip_method) {
	case ROTATE_CLIP_METHOD::RC_ORIG: {
		width_dst = width_src;
		height_dst = height_src;
		break;
	}
	case ROTATE_CLIP_METHOD::RC_FIT: {
		//TODO: Calc the new size of image after rotate

		//TEMP
		return false;//delete this after complete

		break;
	}
	}

	if (width_dst == 0 || height_dst == 0) {
		OutputDebugString("\nERROR: Dst img is too small! --- Rotate - 0\n");
		return false;
	}

	unsigned char** pBuffer = NULL;
	unsigned char** pDstBuffer = NULL;

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- Rotate - 0\n");
		return false;
	}

	switch (method) {
	case ROTATE_METHOD::RO_NEAREST_NEIGHBOR: {
		ret = _rotate_common_nearest_neighbor(pBuffer, pDstBuffer, width_src, height_src, width_dst, height_dst, angle, rotarycentre_x, rotarycentre_y, zoom_x, zoom_y, move_x, move_y, fill_color);
		break;
	}
	case ROTATE_METHOD::RO_BILINEAR: {
		ret = _rotate_common_bilinear(pBuffer, pDstBuffer, width_src, height_src, width_dst, height_dst, angle, rotarycentre_x, rotarycentre_y, zoom_x, zoom_y, move_x, move_y, fill_color);
		break;
	}
	default: {
		//TODO:

	}

	}

	if (ret) {
		pDstImg = create_image();
		if (pDstBuffer)
			pDstImg->InitArray8(pDstBuffer, height_dst, width_dst);
		else {
			OutputDebugString("\nERROR: Dst Buffer create FAILED! --- Erosion - 0\n");
			return false;
		}
	}

	for (unsigned long i = 0; i < height_src; ++i) {
		delete[] pBuffer[i];
	}
	delete[] pBuffer;
	pBuffer = NULL;

	for (unsigned long i = 0; i < height_dst; ++i) {
		delete[] pDstBuffer[i];
	}
	delete[] pDstBuffer;
	pDstBuffer = NULL;

	return true;
}

#ifdef LOCAL_FUNCS_EXTEND

void Gray_Image_Processing::SinCos(const double Angle, double& sina, double& cosa) {
	double sinvalue = 0, cosvalue = 0;
	__sincos(Angle, sinvalue, cosvalue);
	sina = sinvalue;
	cosa = cosvalue;
}

#endif

#ifdef DEBUG_TEST
void Gray_Image_Processing::CV_Test(CImg* pSrcImg) {

	//clock_t begin = clock();

	unsigned char* pBuffer = NULL;

	unsigned long WIDTH = pSrcImg->GetWidthPixel();
	unsigned long HEIGHT = pSrcImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_1D_Buffer(pSrcImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- CV TEST - 0\n");
		return;
	}

	int bits = pSrcImg->GetBitCount();

	cv::Mat Img;
	if (bits == 8)
		Img = cv::Mat(HEIGHT, WIDTH, CV_8U, pBuffer).clone();
	else {
		Img = cv::Mat(HEIGHT, WIDTH, CV_8UC3, pBuffer).clone();
		cvtColor(Img, Img, CV_RGB2GRAY);
	}

	//clock_t end = clock();

	cv::imshow("TEST", Img);

}
#endif

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
		BYTE* pBuff = pImg->GetPixelAddressRow(y);
		for (long x = 0; x < WIDTH; ++x) {
			pBuffer[y][x] = pBuff[x];
		}
	}

	OutputDebugString("\nINFO: Src Img trans to Buffer success! --- TGCTB\n");
	return true;
}

bool Gray_Image_Processing::_trans_Gray_CImg_to_1D_Buffer(CImg* pImg, unsigned char* &pBuffer) {
	if (pImg == NULL)
	{
		OutputDebugString("\nERROR: Src Img noexist! --- TGCTODB\n");
		return false;
	}

	if (pBuffer != NULL) {
		OutputDebugString("\nWARNING: Src Img not NULL! --- TGCTODB\n");
		delete pBuffer;
		pBuffer = NULL;
	}

	unsigned long WIDTH = pImg->GetWidthPixel();
	unsigned long HEIGHT = pImg->GetHeight();

	pBuffer = new unsigned char[HEIGHT * WIDTH];

	for (long y = 0; y < HEIGHT; ++y) {
		BYTE* pBuff = pImg->GetPixelAddressRow(y);
		memcpy(&pBuffer[y*WIDTH], pBuff, sizeof(unsigned char)*WIDTH);
	}

	OutputDebugString("\nINFO: Src Img trans to Buffer success! --- TGCTODB\n");
	return true;
}

bool Gray_Image_Processing::_trans_Gray_CImg_to_Mat(CImg* pImg, cv::Mat &mat) {
	unsigned char* pBuffer = NULL;
	unsigned long WIDTH = pImg->GetWidthPixel();
	unsigned long HEIGHT = pImg->GetHeight();

	bool ret = false;
	ret = this->_trans_Gray_CImg_to_1D_Buffer(pImg, pBuffer);
	if (ret == false || pBuffer == NULL) {
		OutputDebugString("\nERROR: Trans CImg to Buffer FAILED! --- TGCTMAT - 0\n");
		return false;
	}
	int bits = pImg->GetBitCount();
	if (bits == 8)
		mat = cv::Mat(HEIGHT, WIDTH, CV_8U, pBuffer).clone();
	else {
		mat = cv::Mat(HEIGHT, WIDTH, CV_8UC3, pBuffer).clone();
		cvtColor(mat, mat, CV_RGB2GRAY);
	}
	return true;
}

bool Gray_Image_Processing::_trans_Mat_to_Gray_CImg(cv::Mat mat, CImg* &pImg) {
	if (pImg) {
		OutputDebugString("\nINFO: Img Exist! --- TGCTODB\n");
		delete pImg;
		pImg = NULL;
	}
	pImg = create_image();
	pImg->InitArray8(mat.data, mat.rows, mat.cols);
}

inline bool Gray_Image_Processing::__pixels_is_in_img(const unsigned long width, const unsigned long height, const long x, const long y) {
	return ((x >= 0) && (x < width) && (y >= 0) && (y < height));
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

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(pDst[i], pSrc[i], sizeof(unsigned char)*width);
	}

	//_meanfilter_region(pSrc, pMean, width, height, mask_width, mask_height);
	_meanfilter_increment(pSrc, pMean, width, height, mask_width, mask_height);
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

bool Gray_Image_Processing::_rotate_left(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- TrunLeft - 1\n");
		return false;
	}

	pDst = new unsigned char*[width];
	for (unsigned long i = 0; i < width; ++i) {
		pDst[i] = new unsigned char[height];
	}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			pDst[(width - 1) - x][y] = pSrc[y][x];
		}
	}

	return true;
}

bool Gray_Image_Processing::_rotate_right(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- TrunRight - 1\n");
		return false;
	}

	pDst = new unsigned char*[width];
	for (unsigned long i = 0; i < width; ++i) {
		pDst[i] = new unsigned char[height];
	}

	for (long y = 0; y < height; ++y) {
		for (long x = 0; x < width; ++x) {
			pDst[x][(height - 1) - y] = pSrc[y][x];
		}
	}

	return true;
}

bool Gray_Image_Processing::_scaling_none(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, int piece_size, int selected) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Scaling - 1\n");
		return false;
	}

	if (piece_size > width_src || piece_size > height_src) {
		OutputDebugString("\nERROR: Piece size is greater than width or height! --- Scaling - 1\n");
		return false;
	}

	if (selected > piece_size) {
		OutputDebugString("\nERROR: Selected index is greater than the most! --- Scaling - 1\n");
		return false;
	}

	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
	}

	unsigned char** pMid = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pMid[i] = new unsigned char[width_src];
	}

	unsigned long mid_row_index = 0;
	for (unsigned long row_index = 0; row_index < height_src; ++row_index) {
		if (row_index % piece_size == selected) {
			if (mid_row_index >= height_dst)
				break;
			memcpy(pMid[mid_row_index++], pSrc[row_index], width_src * sizeof(unsigned char));
		}
	}

	unsigned long dst_col_index = 0;
	for (unsigned long col_index = 0; col_index < width_src; ++col_index) {
		if (col_index % piece_size == selected) {
			if (dst_col_index >= width_dst)
				break;
			for (unsigned long tmp_index = 0; tmp_index < height_dst; ++tmp_index) {
				pDst[tmp_index][dst_col_index] = pMid[tmp_index][col_index];
			}
			++dst_col_index;
		}
	}

	for (unsigned long i = 0; i < height_dst; ++i) {
		delete[] pMid[i];
	}
	delete[] pMid;
	pMid = NULL;

	return true;
}

bool Gray_Image_Processing::_scaling_nearest_neighbor(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Scaling - 1\n");
		return false;
	}


	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
	}

	//TODO:


	return true;
}

bool Gray_Image_Processing::_flip_horizontal(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- FlipHorizontal - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long row_index = 0; row_index < height; ++row_index) {
		for (unsigned long col_index = 0; col_index < width; ++col_index) {
			pDst[row_index][col_index] = pSrc[row_index][width - 1 - col_index];
		}
	}

	return true;
}

bool Gray_Image_Processing::_flip_vertical(unsigned char** pSrc, unsigned char** &pDst, unsigned long width, unsigned long height) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- FlipVertical - 1\n");
		return false;
	}

	pDst = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		pDst[i] = new unsigned char[width];
	}

	for (unsigned long col_index = 0; col_index < width; ++col_index) {
		for (unsigned long row_index = 0; row_index < height; ++row_index) {
			pDst[row_index][col_index] = pSrc[height - 1 - row_index][col_index];
		}
	}

	return true;

}

bool Gray_Image_Processing::_clip_rectangle(unsigned char** pSrc, unsigned char** &pDst, long leftTop_x, long leftTop_y, unsigned long width_src, unsigned long height_src, unsigned long &width_dst, unsigned long &height_dst) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- ClipRectangle - 1\n");
		return false;
	}

	if (leftTop_x > width_src || leftTop_y > height_src) {
		OutputDebugString("\nERROR: Rectangle position should be inside of Image Area! --- ClipRectangle - 1\n");
		return false;
	}

	long start_row = leftTop_y < 0 ? 0 : leftTop_y;
	long start_col = leftTop_x < 0 ? 0 : leftTop_x;
	long end_row = leftTop_y + height_dst - 1;
	long end_col = leftTop_x + width_dst - 1;

	end_row = end_row >= height_src ? height_src - 1 : end_row;
	end_col = end_col >= width_src ? width_src - 1 : end_col;

	height_dst = end_row - start_row + 1;
	width_dst = end_col - start_col + 1;

	if (height_dst == 0 || width_dst == 0) {//should not go into,but for safe
		OutputDebugString("\nERROR: Destination Rectangle size cannot be zero! --- ClipRectangle - 1\n");
		return false;
	}

	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
	}

	for (unsigned long row_index = start_row; start_row <= end_row; row_index++) {
		if (row_index > end_row) break;
		memcpy(&pDst[row_index - start_row][0], &pSrc[row_index][start_col], sizeof(unsigned char)*width_dst);
	}

	return true;
}

bool Gray_Image_Processing::_rotate_common_nearest_neighbor(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Rotate - 1\n");
		return false;
	}

	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
	}

	double sinVal = 0, cosVal = 0;
	__sincos(angle, sinVal, cosVal);

	//double rzoomx = 1 / zoom_x, rzoomy = 1 / zoom_y;
	double tmprzoomxy = 1.0 / (zoom_x*zoom_y);
	double rzoomx = tmprzoomxy*zoom_y;
	double rzoomy = tmprzoomxy*zoom_x;

	double rzxcv = rzoomx * cosVal;
	double rzxsv = rzoomx * sinVal;
	double rzycv = rzoomy * cosVal;
	double rzysv = rzoomy * sinVal;

	double mxAddrcx = move_x + rotarycentre_x;
	double myAddrcy = move_y + rotarycentre_y;

	for (long y = 0; y < height_src; ++y) {
		double ySegSv = (y - myAddrcy) * rzysv - rotarycentre_x;
		double ySegCv = (y - myAddrcy) * rzycv + rotarycentre_y;
		for (long x = 0; x < width_src; ++x) {
			long srcx = (long)((x - mxAddrcx) * rzxcv - ySegSv);
			long srcy = (long)((x - mxAddrcx) * rzxsv + ySegCv);
			if (__pixels_is_in_img(width_src, height_src, srcx, srcy))
				pDst[y][x] = pSrc[srcy][srcx];
			else
				pDst[y][x] = fill_color;
		}
	}

	return true;
}

bool Gray_Image_Processing::_rotate_common_bilinear(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long width_dst, unsigned long height_dst, double angle, double rotarycentre_x, double rotarycentre_y, double zoom_x, double zoom_y, double move_x, double move_y, unsigned char fill_color) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- Rotate - 1\n");
		return false;
	}

	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
	}

	double sinVal = 0, cosVal = 0;
	__sincos(angle, sinVal, cosVal);

	//double rzoomx = 1 / zoom_x, rzoomy = 1 / zoom_y;
	double tmprzoomxy = 1.0 / (zoom_x*zoom_y);
	double rzoomx = tmprzoomxy*zoom_y;
	double rzoomy = tmprzoomxy*zoom_x;

	double rzxcv = rzoomx * cosVal;
	double rzxsv = rzoomx * sinVal;
	double rzycv = rzoomy * cosVal;
	double rzysv = rzoomy * sinVal;

	double mxAddrcx = move_x + rotarycentre_x;
	double myAddrcy = move_y + rotarycentre_y;

	for (long y = 0; y < height_src; ++y) {
		double ySegSv = (y - myAddrcy) * rzysv - rotarycentre_x;
		double ySegCv = (y - myAddrcy) * rzycv + rotarycentre_y;

		for (long x = 0; x < width_src; ++x) {
			double srcx = (x - mxAddrcx) * rzxcv - ySegSv;
			double srcy = (x - mxAddrcx) * rzxsv + ySegCv;

			long lx = (long)srcx;
			long ly = (long)srcy;

			double u = srcx - lx;
			double v = srcy - ly;

			long lx_1 = (lx + 1) >= width_src ? width_src - 1 : (lx + 1);
			long ly_1 = (ly + 1) >= height_src ? height_src - 1 : (ly + 1);

			if (__pixels_is_in_img(width_src, height_src, srcx, srcy)) {
				pDst[y][x] = (1 - u) * (1 - v) *		pSrc[ly][lx] +
					(1 - u) * v *			pSrc[ly][lx_1] +
					u * (1 - v) *			pSrc[ly_1][lx] +
					u * v *					pSrc[ly_1][lx_1];
			}
			else
				pDst[y][x] = fill_color;


			/*if (__pixels_is_in_img(width_src, height_src, srcx, srcy))
				pDst[y][x] = pSrc[srcy][srcx];
			else
				pDst[y][x] = fill_color;*/

		}
	}

	return true;
}

bool Gray_Image_Processing::_clip_region(unsigned char** pSrc, unsigned char** &pDst, unsigned long width_src, unsigned long height_src, unsigned long &width_dst, unsigned long &height_dst, const unsigned char fillup_color) {
	if (pSrc == NULL) {
		OutputDebugString("\nERROR: Src Img noexist! --- ClipRegion - 1\n");
		return false;
	}

	if (height_dst == 0 || width_dst == 0) {//should not go into,but for safe
		OutputDebugString("\nERROR: Destination Region size cannot be null! --- ClipRegion - 1\n");
		return false;
	}

	pDst = new unsigned char*[height_dst];
	for (unsigned long i = 0; i < height_dst; ++i) {
		pDst[i] = new unsigned char[width_dst];
		memset(pDst[i], fillup_color, sizeof(unsigned char)*width_dst);
	}

	unsigned long str_x = m_rl->rl[0].cb;
	unsigned long str_y = m_rl->rl[0].l;

	long sumlines = m_rl->num;
	for (long n = 0; n < sumlines; ++n) {
		long y = m_rl->rl[n].l;
		for (long x = m_rl->rl[n].cb; x < m_rl->rl[n].ce; ++x) {
			pDst[y - str_y][x - str_x] = pSrc[y][x];
		}
	}

	return true;
}