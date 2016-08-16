#pragma once
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

typedef enum TwoDBarCodeType {
	DataMatrix = 0
}TwoDBarCodeType;

typedef enum OneDBarCodeType {
	EAN_13 = 0,
	EAN_8 = 1,
	UPC_A = 2,
	UPC_E = 3,
	ITF_14 = 4,
	EAN_128 = 5
}OneDBarCodeType;

class OneDBarcodeGrading final {
public:
	OneDBarcodeGrading();

	~OneDBarcodeGrading();

	bool Func(CImg* pImg, OneDBarCodeType type);

	bool SetDECODEGrade(int decode_grade);

	int GetDecodabilityGrade();

	int GetSCGrade();

	int GetMODGrade();

	int GetDefectsGrade();

	int GetRminGrade();

	int GetECminGrade();

	int GetDECODEGrade();

};

class TwoDBarcodeGrading final {
public:

	TwoDBarcodeGrading();

	~TwoDBarcodeGrading();

	bool Func(CImg* pImg, const unsigned int error_correction_capacity, TwoDBarCodeType type);

	bool SetUECScore(int t, int error_correction_capacity);

	bool SetDECODEGrade(int decode_grade);

	int GetSCGrade();

	int GetANGrade();

	int GetGNGrade();

	int GetFPDGrade();

	int GetMODGrade();

	int GetUECGrade();

	int GetDECODEGrade();

};
