#include <iostream>
#include <string>

#define PI 3.1415926535898

#include <windows.h>//BOOL etc.
//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#pragma  comment(lib, "..//libdmtx-0.7.4//project//visualc9//Debug//libdmtx.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#pragma  comment(lib, "..//libdmtx-0.7.4//project//visualc9//Release//libdmtx.lib")
#endif

#include "..//Class Simple Gray Image Processing//gray_image_processing.h"

#include "..//libdmtx-0.7.4//dmtx.h"

#include <ctime>

using namespace std;

bool DecodeAndClipRegion(CImg* pImg, CImg* pRegion, string &code) {
	Gray_Image_Processing action;
	CImg* pMidImg = NULL;
	action.Scaling(pImg, pMidImg, 1.0 / 4.0, Gray_Image_Processing::SCALING_METHOD::SC_NONE);

	long width, height;
	width = pMidImg->GetWidthPixel();
	height = pMidImg->GetHeight();

	//new
	unsigned char* source = new unsigned char[height*width];

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pMidImg->GetPixelAddressRow(j);
			source[j*width + i] = pBuff[i];
		}
	}


	DmtxEncode     *enc = NULL;
	DmtxImage      *img = NULL;
	DmtxDecode     *dec = NULL;
	DmtxRegion     *reg = NULL;
	DmtxMessage    *msg = NULL;

	img = dmtxImageCreate(source, width, height, DmtxPack8bppK);
	assert(img != NULL);

	dec = dmtxDecodeCreate(img, 1);
	assert(dec != NULL);

	reg = dmtxRegionFindNext(dec, NULL);
	if (reg != NULL) {
		msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
		if (msg != NULL) {
			code = (const char*)msg->output;
			/*fputs("output: \"", stdout);
			fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
			fputs("\"\n", stdout);*/
			dmtxMessageDestroy(&msg);
		}
		//dmtxRegionDestroy(&reg);
	}
	else {
		dmtxDecodeDestroy(&dec);
		dmtxImageDestroy(&img);
		return -1;
	}

	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&img);

	//cout << "LeftLine: (" << "LocBeg_X:" << reg->leftLine.locBeg.X << " LocBeg_Y:" << reg->leftLine.locBeg.Y << " - "
	//	<< "LocNeg_X:" << reg->leftLine.locNeg.X << " LocNeg_Y:" << reg->leftLine.locNeg.Y << " - "
	//	<< "LocPos_X:" << reg->leftLine.locPos.X << " LocPos_Y:" << reg->leftLine.locPos.Y << " ) " << endl;

	//cout << "BottomLine: (" << "LocBeg_X:" << reg->bottomLine.locBeg.X << " LocBeg_Y:" << reg->bottomLine.locBeg.Y << " - "
	//	<< "LocNeg_X:" << reg->bottomLine.locNeg.X << " LocNeg_Y:" << reg->bottomLine.locNeg.Y << " - "
	//	<< "LocPos_X:" << reg->bottomLine.locPos.X << " LocPos_Y:" << reg->bottomLine.locPos.Y << " ) " << endl;

	float left_pos_x[3], left_pos_y[3], bottom_pos_x[3], bottom_pos_y[3];
	left_pos_x[0] = reg->leftLine.locPos.X;
	left_pos_x[1] = reg->leftLine.locBeg.X;
	left_pos_x[2] = reg->leftLine.locNeg.X;
	left_pos_y[0] = reg->leftLine.locPos.Y;
	left_pos_y[1] = reg->leftLine.locBeg.Y;
	left_pos_y[2] = reg->leftLine.locNeg.Y;

	bottom_pos_x[0] = reg->bottomLine.locPos.X;
	bottom_pos_x[1] = reg->bottomLine.locBeg.X;
	bottom_pos_x[2] = reg->bottomLine.locNeg.X;
	bottom_pos_y[0] = reg->bottomLine.locPos.Y;
	bottom_pos_y[1] = reg->bottomLine.locBeg.Y;
	bottom_pos_y[2] = reg->bottomLine.locNeg.Y;

	float leftLine_a = 0, leftLine_b = 0, bottomLine_a = 0, bottomLine_b = 0;
	//y = a * x + b;
	leftLine_b = (((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))*(left_pos_y[0] + left_pos_y[1] + left_pos_y[2])
		- (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*((left_pos_x[0] * left_pos_y[0]) + (left_pos_x[1] * left_pos_y[1]) + (left_pos_x[2] * left_pos_y[2]))) /
		(3.0*(((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))) - (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_x[0] + left_pos_x[1] + left_pos_x[2]));

	leftLine_a = (3.0*((left_pos_x[0] * left_pos_y[0]) + (left_pos_x[1] * left_pos_y[1]) + (left_pos_x[2] * left_pos_y[2])) - ((left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_y[0] + left_pos_y[1] + left_pos_y[2]))) /
		(3.0*(((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))) - (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_x[0] + left_pos_x[1] + left_pos_x[2]));

	bottomLine_b = (((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))*(bottom_pos_y[0] + bottom_pos_y[1] + bottom_pos_y[2])
		- (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*((bottom_pos_x[0] * bottom_pos_y[0]) + (bottom_pos_x[1] * bottom_pos_y[1]) + (bottom_pos_x[2] * bottom_pos_y[2]))) /
		(3.0*(((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))) - (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2]));

	bottomLine_a = (3.0*((bottom_pos_x[0] * bottom_pos_y[0]) + (bottom_pos_x[1] * bottom_pos_y[1]) + (bottom_pos_x[2] * bottom_pos_y[2])) - ((bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_y[0] + bottom_pos_y[1] + bottom_pos_y[2]))) /
		(3.0*(((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))) - (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2]));

	float cx = (bottomLine_b - leftLine_b) / (leftLine_a - bottomLine_a);
	float cy = leftLine_a * cx + leftLine_b;

	DmtxVector2 topLeft, topRight, bottomLeft, bottomRight;
	DmtxPixelLoc pxTopLeft, pxTopRight, pxBottomLeft, pxBottomRight;

	topLeft.X = bottomLeft.X = topLeft.Y = topRight.Y = -0.1;
	topRight.X = bottomRight.X = bottomLeft.Y = bottomRight.Y = 1.1;

	dmtxMatrix3VMultiplyBy(&topLeft, reg->fit2raw);
	dmtxMatrix3VMultiplyBy(&topRight, reg->fit2raw);
	dmtxMatrix3VMultiplyBy(&bottomLeft, reg->fit2raw);
	dmtxMatrix3VMultiplyBy(&bottomRight, reg->fit2raw);

	pxTopLeft.X = (int)(0.5 + topLeft.X);
	pxTopLeft.Y = (int)(0.5 + topLeft.Y);
	pxBottomLeft.X = (int)(0.5 + bottomLeft.X);
	pxBottomLeft.Y = (int)(0.5 + bottomLeft.Y);
	pxTopRight.X = (int)(0.5 + topRight.X);
	pxTopRight.Y = (int)(0.5 + topRight.Y);
	pxBottomRight.X = (int)(0.5 + bottomRight.X);
	pxBottomRight.Y = (int)(0.5 + bottomRight.Y);

	vector<DmtxPixelLoc > region_point_set;
	region_point_set.push_back(pxTopLeft);
	region_point_set.push_back(pxBottomLeft);
	region_point_set.push_back(pxTopRight);
	region_point_set.push_back(pxBottomRight);

	vector<int > x_set, y_set;
	x_set.push_back(pxTopLeft.X);
	x_set.push_back(pxBottomLeft.X);
	x_set.push_back(pxTopRight.X);
	x_set.push_back(pxBottomRight.X);

	y_set.push_back(abs(height - pxTopLeft.Y));
	y_set.push_back(abs(height - pxBottomLeft.Y));
	y_set.push_back(abs(height - pxTopRight.Y));
	y_set.push_back(abs(height - pxBottomRight.Y));

	sort(x_set.begin(), x_set.end());
	sort(y_set.begin(), y_set.end());

	for (vector<int >::iterator p = x_set.begin(); p != x_set.end(); ++p) {
		*p = 4 * *p;
		if (*p < 0)
			*p = 0;
	}
	for (vector<int >::iterator p = y_set.begin(); p != y_set.end(); ++p) {
		*p = 4 * *p;
		if (*p < 0)
			*p = 0;
	}

	float flipcx = cx, flipcy = abs(height - cy);
	float dx = reg->bottomLine.locBeg.X - flipcx;
	float dy = flipcy - abs(height - reg->bottomLine.locBeg.Y);

	vector<DmtxPixelLoc > region_flip_point_set;
	for each(auto point in region_point_set) {
		DmtxPixelLoc pTmp;
		pTmp.X = point.X;
		pTmp.Y = abs(height - point.Y);
		region_flip_point_set.push_back(pTmp);
	}

	float angle = 0;

	if (dy >= 0)
		angle = -reg->bottomLine.angle;
	else
		angle = 180 - reg->bottomLine.angle;

	angle = PI * angle / 180;

	CImg* himg = NULL;
	CImg* rimg = NULL;

	bool ret = action.ClipRectangle(pImg, himg, *(x_set.begin()), *(y_set.begin()), *prev(x_set.end()) - *(x_set.begin()) + 1, *prev(y_set.end()) - *(y_set.begin()) + 1);

	DmtxPixelLoc pxTopLeft_a, pxTopRight_a, pxBottomLeft_a, pxBottomRight_a;

	pxTopLeft_a.X = (pxTopLeft.X < 0 ? 0 : pxTopLeft.X) * 4 - *(x_set.begin());
	pxTopRight_a.X = (pxTopRight.X < 0 ? 0 : pxTopRight.X) * 4 - *(x_set.begin());
	pxBottomLeft_a.X = (pxBottomLeft.X < 0 ? 0 : pxBottomLeft.X) * 4 - *(x_set.begin());
	pxBottomRight_a.X = (pxBottomRight.X < 0 ? 0 : pxBottomRight.X) * 4 - *(x_set.begin());

	pxTopLeft_a.Y = fabs(height - pxTopLeft.Y) * 4 - *(y_set.begin());
	pxTopRight_a.Y = fabs(height - pxTopRight.Y) * 4 - *(y_set.begin());
	pxBottomLeft_a.Y = fabs(height - pxBottomLeft.Y) * 4 - *(y_set.begin());
	pxBottomRight_a.Y = fabs(height - pxBottomRight.Y) * 4 - *(y_set.begin());

	vector<DmtxPixelLoc > point_set_after_clip;
	point_set_after_clip.push_back(pxTopLeft_a);
	point_set_after_clip.push_back(pxTopRight_a);
	point_set_after_clip.push_back(pxBottomLeft_a);
	point_set_after_clip.push_back(pxBottomRight_a);

	long rwidth = himg->GetWidthPixel();
	long rheight = himg->GetHeight();

	ret = action.Rotate(himg, rimg, angle, rwidth*0.5, rheight*0.5, 1, 1, 0, 0, 0x00, Gray_Image_Processing::ROTATE_METHOD::RO_BILINEAR, Gray_Image_Processing::ROTATE_CLIP_METHOD::RC_ORIG);

	vector<int > x_a_set, y_a_set;
	for (vector<DmtxPixelLoc >::iterator p = point_set_after_clip.begin(); p < point_set_after_clip.end(); ++p) {
		int x, y;
		x = (p->X - rwidth*0.5)*cos(angle) + (p->Y - rheight*0.5)*sin(angle) + rwidth*0.5;
		if (x < 0)
			x = 0;
		x_a_set.push_back(x);
		y = -(p->X - rwidth*0.5)*sin(angle) + (p->Y - rheight*0.5)*cos(angle) + rheight*0.5;
		if (y < 0)
			y = 0;
		y_a_set.push_back(y);
	}

	sort(x_a_set.begin(), x_a_set.end());
	sort(y_a_set.begin(), y_a_set.end());

	DmtxPixelLoc str_point;
	str_point.X = x_a_set.at(1);
	str_point.Y = y_a_set.at(1);

	int width_a = *(x_a_set.end() - 2) - str_point.X;
	int	height_a = *(y_a_set.end() - 2) - str_point.Y;

	if (width_a >= 0 && height_a >= 0) {
		CImg* rcimg = NULL;
		ret = action.ClipRectangle(rimg, rcimg, str_point.X, str_point.Y, width_a, height_a);
		pRegion = rcimg;
	}
	else {
		pRegion = rimg;
	}

	delete source;

	if (reg != NULL) {
		dmtxRegionDestroy(&reg);
	}
}

int main() {

	clock_t begin, end;

	CImg* pImg = create_image();
	
	CImg* rImg = NULL;
	string code;


	//BOOL rt = pImg->AttachFromFile("..//imgs//3_rotate.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//micro//clip_9_contrast_binary.bmp");
	if (!rt)
		return -1;

	//------

	long width, height;
	width = pImg->GetWidthPixel();
	height = pImg->GetHeight();

	//new
	unsigned char* source = new unsigned char[height*width];

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j*width + i] = pBuff[i];
		}
	}

	begin = clock();

	DmtxEncode     *enc = NULL;
	DmtxImage      *img = NULL;
	DmtxDecode     *dec = NULL;
	DmtxRegion     *reg = NULL;
	DmtxMessage    *msg = NULL;

	img = dmtxImageCreate(source, width, height, DmtxPack8bppK);
	assert(img != NULL);

	dec = dmtxDecodeCreate(img, 1);
	assert(dec != NULL);

	reg = dmtxRegionFindNext(dec, NULL);
	if (reg != NULL) {
		msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
		if (msg != NULL) {
			fputs("output: \"", stdout);
			fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
			fputs("\"\n", stdout);
			dmtxMessageDestroy(&msg);
		}
		dmtxRegionDestroy(&reg);
	}
	else {
		dmtxDecodeDestroy(&dec);
		dmtxImageDestroy(&img);
		return -1;
	}

	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&img);

	//DecodeAndClipRegion(pImg, rImg, code);


	end = clock();

	cout << code << endl;

	cout << "\n\n " << end - begin << endl;

	getchar();
	return 0;

	//Gray_Image_Processing action;
	//action.Scaling(pImg, pMidImg, 1.0 / 4.0, Gray_Image_Processing::SCALING_METHOD::SC_NONE);
	////action.Rotate(pImg, pMidImg, 2, pImg->GetWidthPixel()*0.5, pImg->GetHeight()*0.5, 1, 1, 0, 0, 0xFF, Gray_Image_Processing::ROTATE_METHOD::RO_BILINEAR, Gray_Image_Processing::ROTATE_CLIP_METHOD::RC_ORIG);

	//long width, height;//ͼƬ�ĳߴ�
	//width = pMidImg->GetWidthPixel();
	//height = pMidImg->GetHeight();

	////new
	//unsigned char* source = new unsigned char[height*width];

	////trans
	//for (long j = 0; j < height; ++j) {
	//	for (long i = 0; i < width; ++i) {
	//		BYTE* pBuff = pMidImg->GetPixelAddressRow(j);
	//		source[j*width + i] = pBuff[i];
	//	}
	//}


	//DmtxEncode     *enc = NULL;
	//DmtxImage      *img = NULL;
	//DmtxDecode     *dec = NULL;
	//DmtxRegion     *reg = NULL;
	//DmtxMessage    *msg = NULL;

	//img = dmtxImageCreate(source, width, height, DmtxPack8bppK);
	//assert(img != NULL);

	//dec = dmtxDecodeCreate(img, 1);
	//assert(dec != NULL);

	//reg = dmtxRegionFindNext(dec, NULL);
	//if (reg != NULL) {
	//	msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
	//	if (msg != NULL) {
	//		fputs("output: \"", stdout);
	//		fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
	//		fputs("\"\n", stdout);
	//		dmtxMessageDestroy(&msg);
	//	}
	//	//dmtxRegionDestroy(&reg);
	//}
	//else {
	//	dmtxDecodeDestroy(&dec);
	//	dmtxImageDestroy(&img);
	//	return -1;
	//}

	//dmtxDecodeDestroy(&dec);
	//dmtxImageDestroy(&img);

	////cout << "LeftLine: (" << "LocBeg_X:" << reg->leftLine.locBeg.X << " LocBeg_Y:" << reg->leftLine.locBeg.Y << " - "
	////	<< "LocNeg_X:" << reg->leftLine.locNeg.X << " LocNeg_Y:" << reg->leftLine.locNeg.Y << " - "
	////	<< "LocPos_X:" << reg->leftLine.locPos.X << " LocPos_Y:" << reg->leftLine.locPos.Y << " ) " << endl;

	////cout << "BottomLine: (" << "LocBeg_X:" << reg->bottomLine.locBeg.X << " LocBeg_Y:" << reg->bottomLine.locBeg.Y << " - "
	////	<< "LocNeg_X:" << reg->bottomLine.locNeg.X << " LocNeg_Y:" << reg->bottomLine.locNeg.Y << " - "
	////	<< "LocPos_X:" << reg->bottomLine.locPos.X << " LocPos_Y:" << reg->bottomLine.locPos.Y << " ) " << endl;

	//float left_pos_x[3], left_pos_y[3], bottom_pos_x[3], bottom_pos_y[3];
	//left_pos_x[0] = reg->leftLine.locPos.X;
	//left_pos_x[1] = reg->leftLine.locBeg.X;
	//left_pos_x[2] = reg->leftLine.locNeg.X;
	//left_pos_y[0] = reg->leftLine.locPos.Y;
	//left_pos_y[1] = reg->leftLine.locBeg.Y;
	//left_pos_y[2] = reg->leftLine.locNeg.Y;

	//bottom_pos_x[0] = reg->bottomLine.locPos.X;
	//bottom_pos_x[1] = reg->bottomLine.locBeg.X;
	//bottom_pos_x[2] = reg->bottomLine.locNeg.X;
	//bottom_pos_y[0] = reg->bottomLine.locPos.Y;
	//bottom_pos_y[1] = reg->bottomLine.locBeg.Y;
	//bottom_pos_y[2] = reg->bottomLine.locNeg.Y;

	//float leftLine_a = 0, leftLine_b = 0, bottomLine_a = 0, bottomLine_b = 0;
	////y = a * x + b;
	//leftLine_b = (((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))*(left_pos_y[0] + left_pos_y[1] + left_pos_y[2])
	//	- (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*((left_pos_x[0] * left_pos_y[0]) + (left_pos_x[1] * left_pos_y[1]) + (left_pos_x[2] * left_pos_y[2]))) /
	//	(3.0*(((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))) - (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_x[0] + left_pos_x[1] + left_pos_x[2]));

	//leftLine_a = (3.0*((left_pos_x[0] * left_pos_y[0]) + (left_pos_x[1] * left_pos_y[1]) + (left_pos_x[2] * left_pos_y[2])) - ((left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_y[0] + left_pos_y[1] + left_pos_y[2]))) /
	//	(3.0*(((left_pos_x[0] * left_pos_x[0]) + (left_pos_x[1] * left_pos_x[1]) + (left_pos_x[2] * left_pos_x[2]))) - (left_pos_x[0] + left_pos_x[1] + left_pos_x[2])*(left_pos_x[0] + left_pos_x[1] + left_pos_x[2]));

	//bottomLine_b = (((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))*(bottom_pos_y[0] + bottom_pos_y[1] + bottom_pos_y[2])
	//	- (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*((bottom_pos_x[0] * bottom_pos_y[0]) + (bottom_pos_x[1] * bottom_pos_y[1]) + (bottom_pos_x[2] * bottom_pos_y[2]))) /
	//	(3.0*(((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))) - (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2]));

	//bottomLine_a = (3.0*((bottom_pos_x[0] * bottom_pos_y[0]) + (bottom_pos_x[1] * bottom_pos_y[1]) + (bottom_pos_x[2] * bottom_pos_y[2])) - ((bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_y[0] + bottom_pos_y[1] + bottom_pos_y[2]))) /
	//	(3.0*(((bottom_pos_x[0] * bottom_pos_x[0]) + (bottom_pos_x[1] * bottom_pos_x[1]) + (bottom_pos_x[2] * bottom_pos_x[2]))) - (bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2])*(bottom_pos_x[0] + bottom_pos_x[1] + bottom_pos_x[2]));

	//float cx = (bottomLine_b - leftLine_b) / (leftLine_a - bottomLine_a);
	//float cy = leftLine_a * cx + leftLine_b;

	//DmtxVector2 topLeft, topRight, bottomLeft, bottomRight;
	//DmtxPixelLoc pxTopLeft, pxTopRight, pxBottomLeft, pxBottomRight;

	//topLeft.X = bottomLeft.X = topLeft.Y = topRight.Y = -0.1;
	//topRight.X = bottomRight.X = bottomLeft.Y = bottomRight.Y = 1.1;

	//dmtxMatrix3VMultiplyBy(&topLeft, reg->fit2raw);
	//dmtxMatrix3VMultiplyBy(&topRight, reg->fit2raw);
	//dmtxMatrix3VMultiplyBy(&bottomLeft, reg->fit2raw);
	//dmtxMatrix3VMultiplyBy(&bottomRight, reg->fit2raw);

	//pxTopLeft.X = (int)(0.5 + topLeft.X);
	//pxTopLeft.Y = (int)(0.5 + topLeft.Y);
	//pxBottomLeft.X = (int)(0.5 + bottomLeft.X);
	//pxBottomLeft.Y = (int)(0.5 + bottomLeft.Y);
	//pxTopRight.X = (int)(0.5 + topRight.X);
	//pxTopRight.Y = (int)(0.5 + topRight.Y);
	//pxBottomRight.X = (int)(0.5 + bottomRight.X);
	//pxBottomRight.Y = (int)(0.5 + bottomRight.Y);

	////cout << "TopLeft: " << "X: " << pxTopLeft.X << " - " <<
	////	"Y: " << pxTopLeft.Y <<
	////	"\nBottomLeft: " << "X: " << pxBottomLeft.X << " - " <<
	////	"Y: " << pxBottomLeft.Y <<
	////	"\nTopRight: " << "X: " << pxTopRight.X << " - " <<
	////	"Y: " << pxTopRight.Y <<
	////	"\nBottomRight: " << "X: " << pxBottomRight.X << " - " <<
	////	"Y: " << pxBottomRight.Y << endl;

	//vector<DmtxPixelLoc > region_point_set;
	//region_point_set.push_back(pxTopLeft);
	//region_point_set.push_back(pxBottomLeft);
	//region_point_set.push_back(pxTopRight);
	//region_point_set.push_back(pxBottomRight);

	//vector<int > x_set, y_set;
	//x_set.push_back(pxTopLeft.X);
	//x_set.push_back(pxBottomLeft.X);
	//x_set.push_back(pxTopRight.X);
	//x_set.push_back(pxBottomRight.X);

	//y_set.push_back(fabs(height - pxTopLeft.Y));
	//y_set.push_back(fabs(height - pxBottomLeft.Y));
	//y_set.push_back(fabs(height - pxTopRight.Y));
	//y_set.push_back(fabs(height - pxBottomRight.Y));

	//sort(x_set.begin(), x_set.end());
	//sort(y_set.begin(), y_set.end());

	//for (vector<int >::iterator p = x_set.begin(); p != x_set.end(); ++p) {
	//	*p = 4 * *p;
	//	if (*p < 0)
	//		*p = 0;
	//}
	//for (vector<int >::iterator p = y_set.begin(); p != y_set.end(); ++p) {
	//	*p = 4 * *p;
	//	if (*p < 0)
	//		*p = 0;
	//}

	//float flipcx = cx, flipcy = fabs(height - cy);
	//float dx = reg->bottomLine.locBeg.X - flipcx;
	//float dy = flipcy - fabs(height - reg->bottomLine.locBeg.Y);

	//vector<DmtxPixelLoc > region_flip_point_set;
	//for each(auto point in region_point_set) {
	//	DmtxPixelLoc pTmp;
	//	pTmp.X = point.X;
	//	pTmp.Y = fabs(height - point.Y);
	//	region_flip_point_set.push_back(pTmp);
	//}



	//float angle = 0;

	//if (dy >= 0)
	//	angle = -reg->bottomLine.angle;
	//else
	//	angle = 180 - reg->bottomLine.angle;

	//angle = PI * angle / 180;

	//CImg* himg = NULL;
	//CImg* rimg = NULL;

	//bool ret = action.ClipRectangle(pImg, himg, *(x_set.begin()), *(y_set.begin()), *prev(x_set.end()) - *(x_set.begin()) + 1, *prev(y_set.end()) - *(y_set.begin()) + 1);

	//himg->SaveToFile("..//results//region_before_rotate.bmp");

	//DmtxPixelLoc pxTopLeft_a, pxTopRight_a, pxBottomLeft_a, pxBottomRight_a;

	//pxTopLeft_a.X = (pxTopLeft.X < 0 ? 0 : pxTopLeft.X) * 4 - *(x_set.begin());
	//pxTopRight_a.X = (pxTopRight.X < 0 ? 0 : pxTopRight.X) * 4 - *(x_set.begin());
	//pxBottomLeft_a.X = (pxBottomLeft.X < 0 ? 0 : pxBottomLeft.X) * 4 - *(x_set.begin());
	//pxBottomRight_a.X = (pxBottomRight.X < 0 ? 0 : pxBottomRight.X) * 4 - *(x_set.begin());

	//pxTopLeft_a.Y = fabs(height - pxTopLeft.Y) * 4 - *(y_set.begin());
	//pxTopRight_a.Y = fabs(height - pxTopRight.Y) * 4 - *(y_set.begin());
	//pxBottomLeft_a.Y = fabs(height - pxBottomLeft.Y) * 4 - *(y_set.begin());
	//pxBottomRight_a.Y = fabs(height - pxBottomRight.Y) * 4 - *(y_set.begin());

	//vector<DmtxPixelLoc > point_set_after_clip;
	//point_set_after_clip.push_back(pxTopLeft_a);
	//point_set_after_clip.push_back(pxTopRight_a);
	//point_set_after_clip.push_back(pxBottomLeft_a);
	//point_set_after_clip.push_back(pxBottomRight_a);

	//long rwidth = himg->GetWidthPixel();
	//long rheight = himg->GetHeight();

	//ret = action.Rotate(himg, rimg, angle, rwidth*0.5, rheight*0.5, 1, 1, 0, 0, 0x00, Gray_Image_Processing::ROTATE_METHOD::RO_BILINEAR, Gray_Image_Processing::ROTATE_CLIP_METHOD::RC_ORIG);

	////x'= (x - rx0)*cos(RotaryAngle) + (y - ry0)*sin(RotaryAngle) + rx0 ;
	////y'=-(x - rx0)*sin(RotaryAngle) + (y - ry0)*cos(RotaryAngle) + ry0 ;

	//vector<int > x_a_set, y_a_set;
	//for (vector<DmtxPixelLoc >::iterator p = point_set_after_clip.begin(); p < point_set_after_clip.end(); ++p) {
	//	int x, y;
	//	x = (p->X - rwidth*0.5)*cos(angle) + (p->Y - rheight*0.5)*sin(angle) + rwidth*0.5;
	//	if (x < 0)
	//		x = 0;
	//	x_a_set.push_back(x);
	//	y = -(p->X - rwidth*0.5)*sin(angle) + (p->Y - rheight*0.5)*cos(angle) + rheight*0.5;
	//	if (y < 0)
	//		y = 0;
	//	y_a_set.push_back(y);
	//}

	//sort(x_a_set.begin(), x_a_set.end());
	//sort(y_a_set.begin(), y_a_set.end());

	//DmtxPixelLoc str_point;
	//str_point.X = x_a_set.at(1);
	//str_point.Y = y_a_set.at(1);

	////vector<DmtxPixelLoc >::iterator pMin = point_set_after_clip.begin();
	////int min = pMin->X + pMin->Y;

	////for (vector<DmtxPixelLoc >::iterator p = point_set_after_clip.begin(); p < point_set_after_clip.end(); ++p) {
	////	if ((p->X + p->Y) < min) {
	////		pMin = p;
	////	}
	////}


	//int width_a = *(x_a_set.end() - 2) - str_point.X; //*(x_a_set.end() - 2) - pMin->X;
	//int	height_a = *(y_a_set.end() - 2) - str_point.Y;//*(y_a_set.end() - 2) - pMin->Y;


	//if (width_a >= 0 && height_a >= 0) {
	//	CImg* rcimg = NULL;
	//	ret = action.ClipRectangle(rimg, rcimg, str_point.X, str_point.Y, width_a, height_a);
	//	if (ret)
	//		rt = rcimg->SaveToFile("..//results//region_rclip.bmp");

	//}
	//else {
	//	rimg->SaveToFile("..//results//region_rclip.bmp");
	//}

	//rt = rimg->SaveToFile("..//results//region.bmp");



	//delete source;

	//if (reg != NULL) {
	//	dmtxRegionDestroy(&reg);
	//}


}