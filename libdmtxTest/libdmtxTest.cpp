#include <iostream>

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

#include "..//libdmtx-0.7.4//dmtx.h"

#include <ctime>

using namespace std;


int main() {

	clock_t begin, end;

	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//barcode-test-66.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//DST5.bmp");
	if (!rt)
		return -1;


	long width, height;//ͼƬ�ĳߴ�
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

	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&img);

	end = clock();


	cout << "\n SUM TIME:\t" << end - begin << end;

	delete source;

	getchar();
	return 0;
}