#include <iostream>

#include "class_barcode_grade_NEW.h"

int main() {
	//OneDBarcodeGrading OneDgrading;
	//CImg* pImg2 = create_image();
	//BOOL rt2 = pImg2->AttachFromFile("..//imgs//barcodes//TEST2//Minimal Edge Contrast_0__0_12_4.bmp");
	//if (!rt2)
	//	return -1;

	//OneDgrading.Grading(pImg2, OneDBarCodeType::EAN_128);
	//OneDgrading.SetDECODEGrade(4);


	//cout << "SC: " << OneDgrading.GetSCGrade() << endl;
	//cout << "MOD: " << OneDgrading.GetMODGrade() << endl;
	//cout << "Rmin: " << OneDgrading.GetRminGrade() << endl;
	//cout << "ECmin: " << OneDgrading.GetECminGrade() << endl;
	//cout << "Decfects: " << OneDgrading.GetDefectsGrade() << endl;
	//cout << "Decodability: " << OneDgrading.GetDecodabilityGrade() << endl;
	//cout << "DECODE: " << OneDgrading.GetDECODEGrade() << endl;



	TwoDBarcodeGrading TwoDgrading;
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//2DBarCodeTest//08.bmp");
	if (!rt)
		return -1;
	//TwoDgrading.Grading(pImg, 30, TwoDBarCodeType::DataMatrix);

	CImg* img = NULL;//create_image();
	unsigned char** InfoTable = NULL;
	long Twidth = 0, Theight = 0;

	TwoDgrading.SetContrastMValue(3.5);
	TwoDgrading.GetInfoTable(pImg, img, InfoTable, Twidth, Theight);

	for (unsigned long i = 0; i < Theight; ++i) {
		delete[] InfoTable[i];
	}
	delete[] InfoTable;
	InfoTable = NULL;

	rt = img->SaveToFile("..//results//dqt//Grid2.bmp");
	if (rt == false)
	return false;

	//TwoDgrading.SetDECODEGrade(4);
	//TwoDgrading.SetUECScore(0, 30);

	//cout << "SC: " << TwoDgrading.GetSCGrade() << endl;
	//cout << "MOD: " << TwoDgrading.GetMODGrade() << endl;
	//cout << "AN: " << TwoDgrading.GetANGrade() << endl;
	//cout << "GN: " << TwoDgrading.GetGNGrade() << endl;
	//cout << "FPD: " << TwoDgrading.GetFPDGrade() << endl;
	//cout << "UEC: " << TwoDgrading.GetUECGrade() << endl;
	//cout << "DECODE: " << TwoDgrading.GetDECODEGrade() << endl;

	getchar();
	return 0;
}