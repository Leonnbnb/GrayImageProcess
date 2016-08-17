#ifndef BARCODE_GRADING
#define BARCODE_GRADING

#include <vector>
#include <cmath>
#include <memory.h>

#include <ctime>
#include <string>

#include <windows.h>
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

#define CALC_TIME //计算消耗时间,结果在控制台下输出,如果非控制台环境,请注释该宏

using namespace std;

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
private:
	//评级等级符号及其对应分数
	typedef enum GradeSymbol {
		A = 4,
		B = 3,
		C = 2,
		D = 1,
		F = 0
	}GradeSymbol;
	//各项等级评分数据以及等级
	typedef struct Grade {
		//最高反射率
		unsigned char Rmax;
		//最低反射率
		unsigned char Rmin;
		//符号反差
		double SC;
		//最小边缘反差
		unsigned char ECmin;
		//最大单元反射率不均匀度
		unsigned char ERNmax;
		//缺陷度
		double Defects;
		//调制比
		double MOD;
		//全局阈值
		double GT;
		//可译码度
		double Decodability;
		//可译码度等级
		GradeSymbol Decodability_Grade;
		//符号反差等级
		GradeSymbol SC_Grade;
		//调制比等级
		GradeSymbol MOD_Grade;
		//缺陷度等级
		GradeSymbol Defects_Grade;
		//最低反射率等级
		GradeSymbol Rmin_Grade;
		//最小边缘反差等级
		GradeSymbol ECmin_Grade;
		//能否解码成功
		GradeSymbol DECODE_Grade;
		Grade() {
			Rmax = 0;
			Rmin = 0;
			SC = 0;
			ECmin = 0;
			ERNmax = 0;
			Defects = 0;
			MOD = 0;
			Decodability = 0;
			SC_Grade = GradeSymbol::F;
			MOD_Grade = GradeSymbol::F;
			Defects_Grade = GradeSymbol::F;
			Rmin_Grade = GradeSymbol::F;
			ECmin_Grade = GradeSymbol::F;
			Decodability_Grade = GradeSymbol::F;
			DECODE_Grade = GradeSymbol::F;
		}

	}Grade;
	//条码各字符的信息
	typedef class BarcodeCharacterInfo final {
	public:
		float p;
		double RT[7];
		float b1;
		float b2;
		float b3;
		float e1;
		float e2;
		float e3;
		float e4;

		BarcodeCharacterInfo() {
			p = 0;
			memset(RT, 0, sizeof(double) * 7);
			b1 = 0;
			b2 = 0;
			b3 = 0;
			e1 = 0;
			e2 = 0;
			e3 = 0;
			e4 = 0;
		}
		void setValue(float loc1, float loc2, float loc3, float loc4, float loc5, float loc6, float loc7);
		double calcV1();
	private:
		bool _setRT();
	}BChInfo;
private:
	bool InitFlag;
	Grade OneDBarcodeGrade;
public:
	//构造
	OneDBarcodeGrading();

	~OneDBarcodeGrading();

	//函数功能:对指定的一维码进行评级
	//参数:
	//pImg:需要评级的一维码图像
	//type:一维码类型

	//返回值:是否成功评级
	bool Grading(CImg* pImg, OneDBarCodeType type);

	//函数功能:设置解码时获得的译码正确性级别
	//参数:
	//decode_grade:	解码时获得的译码正确性级别
	//						  （4为成功解码 0为不能解码）
	//返回值:是否成功执行（当设置其他数值时将使DECODE级别设置为F,并返回false）
	bool SetDECODEGrade(int decode_grade);

	//函数功能:获得可译码度级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetDecodabilityGrade();

	//函数功能:获得符号反差级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetSCGrade();

	//函数功能:获得调制比级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetMODGrade();

	//函数功能:获得缺陷度级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetDefectsGrade();

	//函数功能:获得最低反射率级别
	//返回值:-1(图像未初始化).0(F).4(A)
	int GetRminGrade();

	//函数功能:获得最小边缘反差级别
	//返回值:-1(图像未初始化).0(F).4(A)
	int GetECminGrade();

	//函数功能:获得译码正确性级别
	//返回值:-1(图像未初始化).0(F).4(A)
	int GetDECODEGrade();

private:
	bool EAN128(CImg* pImg, Grade &grade);

	//将图片沿X方向映射，累加灰度值
	bool Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height);

	//将映射后的信号差分
	bool Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX);

	//将小峰抹平
	bool Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M);

	//找到仍然存在的波峰
	bool Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc);

	//找到距离最远的两个峰顶
	bool Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right);

	//得到灰度最大值最小值以及求得GT 
	bool Func_GetGT(unsigned char** img, unsigned char &maxgray, unsigned char &mingray, double &SC, double &GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height);

	//获得指定行号的像素值
	bool _Func_GetSelectedRowPixels(unsigned char** img, vector<unsigned char> &rowSet, const unsigned long rowNum, const long width);

	//将分割线经过的一维信号以GT为界限划分
	bool _Func_Split(vector<unsigned char > &rowSet, vector<unsigned long > &locSet, double GT, vector<vector<unsigned char > > &sets);

	//根据分段后的数据获得最小EC值和最大ERN值
	bool _Func_ECminERNmax(vector<vector<vector<unsigned char > > > dataSet, double GT, Grade &grade);

	//根据topmost和downmost两条界限确定的条码范围，进行分割，建立多条扫描线进行横向扫描
	bool OneDBarcodeGrading::Func_Scan(const CImg* pImg, unsigned char** img, vector<vector<unsigned long > > &locDataSet, vector<vector<float > > &locDataSet2, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, OneDBarcodeGrading::Grade &grade);

	//根据topmost和downmost两条界限确定的条码范围，进行分割，建立多条扫描线进行横向扫描(DEBUG)
	bool Func_Scan(const CImg* pImg, unsigned char** img, vector<vector<unsigned long > > &locDataSet, vector<vector<float > > &locDataSet2, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, Grade &grade, vector<pair<string, clock_t > > &T);

	//根据各项数据评级
	bool Func_Grading(Grade &grade);

	//EAN128
	bool _Func_Decode_EAN128(vector<vector<float > > locDataSet, Grade &grade);

	//计算可译码度
	bool Func_Decodability(vector<vector<float > > locDataSet, OneDBarCodeType type, Grade &grade);

};


class TwoDBarcodeGrading final {
private:
	//评级等级符号及其对应分数
	typedef enum GradeSymbol {
		A = 4,
		B = 3,
		C = 2,
		D = 1,
		F = 0
	}GradeSymbol;
	//L形区域的类型(L区域 QZL区域 CTSA区域)
	typedef enum LShapeAreaStyle {
		L = 0,
		QZL = 1,
		CT1 = 2,
		CT2 = 3,
		SA = 4
	}LSAS;
	//各项等级评分数据以及等级
	typedef struct Grade {
		GradeSymbol SC;
		double SC_Score;
		GradeSymbol MOD;
		GradeSymbol AN;
		double AN_Score;
		GradeSymbol GN;
		double GN_Score;
		GradeSymbol FPD;
		GradeSymbol UEC;
		double UEC_Score;
		GradeSymbol DECODE;

		Grade() {
			SC = GradeSymbol::F;		SC_Score = 0;
			MOD = GradeSymbol::F;
			AN = GradeSymbol::F;		AN_Score = 0;
			GN = GradeSymbol::F;		GN_Score = 0;
			FPD = GradeSymbol::F;
			UEC = GradeSymbol::F;
			DECODE = GradeSymbol::F;
		}

	}Grade;
	//L形区域即几个静态区域的单元像素均值以及MOD数据
	typedef struct LShapeAreaInfo {
		unsigned long leftdistance;
		unsigned long downdistance;
		unsigned long topdistance;
		unsigned long rightdistance;
		unsigned long L1width;
		unsigned long CT1height;
		unsigned long L2height;
		unsigned long CT2width;
		vector<double > QZL1AVG;
		vector<double > L1AVG;
		vector<double > QZL2AVG;
		vector<double > L2AVG;
		vector<double > CT1AVG;
		vector<double > SA1AVG;
		vector<double > CT2AVG;
		vector<double > SA2AVG;
		vector<double > QZL1MOD;
		vector<double > L1MOD;
		vector<double > QZL2MOD;
		vector<double > L2MOD;
		vector<double > CT1MOD;
		vector<double > SA1MOD;
		vector<double > CT2MOD;
		vector<double > SA2MOD;
		LShapeAreaInfo() {
			leftdistance = 0;
			downdistance = 0;
			topdistance = 0;
			rightdistance = 0;
			L1width = 0;
			CT1height = 0;
			L2height = 0;
			CT2width = 0;
			QZL1AVG.clear();
			L1AVG.clear();
			QZL2AVG.clear();
			L2AVG.clear();
			CT1AVG.clear();
			SA1AVG.clear();
			CT2AVG.clear();
			SA2AVG.clear();
			QZL1MOD.clear();
			L1MOD.clear();
			QZL2MOD.clear();
			L2MOD.clear();
			CT1MOD.clear();
			SA1MOD.clear();
			CT2MOD.clear();
			SA2MOD.clear();
		}
	} LSAI;
	//16022 Table M.4(P111)
	typedef struct LShapeAreaInfoMODTable {
		unsigned int sum;
		unsigned int numofgradeE;
		unsigned int numofgradeD;
		unsigned int numofgradeC;
		unsigned int numofgradeB;
		unsigned int numofgradeA;
		vector<pair<double, GradeSymbol > > modinfo;

		LShapeAreaInfoMODTable() {
			sum = 0;
			numofgradeE = 0;
			numofgradeD = 0;
			numofgradeC = 0;
			numofgradeB = 0;
			numofgradeA = 0;
		}

	}LSAI_MT;
	//16022 Table M.5 L QZL SubTable(P111)
	typedef struct LShapeAreaInfoLQZLTable {
		double damaged_modules;
		unsigned int notional_damage_grade;
		LShapeAreaInfoLQZLTable() {
			damaged_modules = 0;
			notional_damage_grade = 0;
		}
	}LSAI_LQZLT;
	//16022 Table M.5 CT SA SubTable(P111)
	typedef struct LShapeAreaInfoCTSATable {
		//NO USE BUT FOR ALIGN
		double CT_regularity_modules;
		unsigned int CT_regularity_grade;
		double CT_damaged_modules;
		unsigned int CT_damage_grade;
		double SA_damaged_modules;
		unsigned int SA_damage_grade;

		LShapeAreaInfoCTSATable() {
			CT_regularity_modules = 0;
			CT_regularity_grade = 0;
			CT_damaged_modules = 0;
			CT_damage_grade = 0;
			SA_damaged_modules = 0;
			SA_damage_grade = 0;
		}
	}LSAI_CTSAT;
	//16022 Table M.5 SubTable Union(P111)
	typedef /*union*/struct LShapeAreaInfoSegmentGradingTable_SubTable {
		LSAI_LQZLT LQZLT;
		LSAI_CTSAT CTSAT;
		LShapeAreaInfoSegmentGradingTable_SubTable() {}
	}LSAI_SGIT_ST;
	//16022 Table M.5 MainTable(P111)
	typedef struct LShapeAreaInfoSegmentGradingTable {
		unsigned int MOD_grade_level;
		unsigned int no_of_modules;
		unsigned int cum_no_of_modules;
		unsigned int remainder_damaged_modules;
		LSAI_SGIT_ST ST;
		unsigned int lower_of_grades;

		LShapeAreaInfoSegmentGradingTable() {
			MOD_grade_level = 0;
			no_of_modules = 0;
			cum_no_of_modules = 0;
			remainder_damaged_modules = 0;
			lower_of_grades = 0;
		}

	}LSAI_SGIT;
	//CTSA Segment Grade
	typedef struct CTSASegmentGrade {
		unsigned int transition_ratio_test_grade;
		unsigned int notional_damage_test_grades;
		CTSASegmentGrade() {
			transition_ratio_test_grade = 0;
			notional_damage_test_grades = 0;
		}
	}FPD_CTSASG;
	//All FPD Grades
	typedef struct FPDGrades {
		unsigned int L1_grade;
		unsigned int L2_grade;
		unsigned int QZL1_grade;
		unsigned int QZL2_grade;
		vector<FPD_CTSASG > CTSA_segs_grades;
		FPDGrades() {
			L1_grade = 0;
			L2_grade = 0;
			QZL1_grade = 0;
			QZL2_grade = 0;

			FPD_CTSASG CTSA1;
			FPD_CTSASG CTSA2;
			CTSA_segs_grades.push_back(CTSA1);
			CTSA_segs_grades.push_back(CTSA2);
		}

	}FPD_G;
	//Data Area Modulations MOD info and Grade
	typedef struct DataAreaMODInfoTable {
		unsigned int sum;
		unsigned int numofgradeE;
		unsigned int numofgradeD;
		unsigned int numofgradeC;
		unsigned int numofgradeB;
		unsigned int numofgradeA;
		vector<pair<double, GradeSymbol > > modinfo;

		DataAreaMODInfoTable() {
			sum = 0;
			numofgradeE = 0;
			numofgradeD = 0;
			numofgradeC = 0;
			numofgradeB = 0;
			numofgradeA = 0;
		}

	}MOD_DAMIT;
	//15415 Table 7(A) (P19)
	typedef struct ModulationGradingTable {
		//a
		unsigned int MOD_codeword_grade_level;
		unsigned int No_of_codewords_at_level_a;
		//b
		unsigned int cumulative_no_of_codewords_at_level_a_or_higher;
		//sum-b
		//c
		unsigned int remaining_codewords;
		int notional_unused_error_correction_capacity;
		double notional_UEC;
		//d
		unsigned int notional_UEC_grade;
		//e
		unsigned int lower_of_a_or_d;
	}MOD_GIT;

private:
	Grade TwoDBarcodeGrade;
	bool InitFlag;

public:
	TwoDBarcodeGrading();

	~TwoDBarcodeGrading();

	//函数功能:对指定的二维码进行评级
	//参数:
	//pImg:需要评级的二维码图像
	//error_correction_capacity:纠错容量
	//type:二维码类型
	//返回值:是否成功评级
	bool Grading(CImg* pImg, const unsigned int error_correction_capacity, TwoDBarCodeType type);

	//函数功能:设置未使用的纠错的等级
	//参数:
	//t:已知的错误码字数
	//error_correction_capacity:纠错能力
	//返回值:是否成功设置
	bool SetUECScore(int t, int error_correction_capacity);

	//函数功能:设置解码时获得的译码正确性级别
	//参数:
	//decode_grade:	解码时获得的译码正确性级别
	//						  （4为成功解码 0为不能解码）
	//返回值:是否成功执行（当设置其他数值时将使DECODE级别设置为F,并返回false）
	bool SetDECODEGrade(int decode_grade);

	//函数功能:获得调制比级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetSCGrade();

	//函数功能:获得轴向不均匀度级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetANGrade();

	//函数功能:获得网格不均匀度级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetGNGrade();

	//函数功能:获得固定模式损坏级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetFPDGrade();

	//函数功能:获得调制比级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetMODGrade();

	//函数功能:获得未使用的纠错级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetUECGrade();

	//函数功能:获得译码正确性级别
	//返回值:-1(图像未初始化).0(F).1(D).2(C).3(B).4(A)
	int GetDECODEGrade();

private:
	//DataMatrix 评级
	bool DataMatrixGrading(CImg* pImg, const unsigned int error_correction_capacity, Grade &grade);

	//15415 Table 7(A) (P19) Create & Edit
	GradeSymbol _Func_MOD_T7A(MOD_DAMIT DataAreaMODGradeTable, vector<MOD_GIT > &MODGIT, const unsigned int error_correction_capacity);

	//计算CT转换率
	unsigned int _Func_FPD_TransitionRatioTest(vector<double > SA_T, vector<double > CT_T);

	//滑动大小为5的窗口判断module error //16022 P109 e
	bool _Func_FPD_CTRegularityTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT);

	//在CT中module error 占总module的比例 //16022 P109 f
	bool _Func_FPD_CTDamageTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT);

	//在SA中module error 占总module的比例 //16022 P109 g
	bool _Func_FPD_SAFixedPatternTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT);

	//16022 Table M.4 Create
	bool _Func_FPD_TM4(vector<double > info, LSAI_MT &MT, LSAS style);

	//16022 Table M.5 Init
	bool _Func_FPD_TM5_Init(vector<LSAI_SGIT > &SGIT);

	//16022 Table M.5 Edit
	bool _Func_FPD_TM5_Edit(LSAI_MT info, vector<LSAI_SGIT > &SGIT, LSAS style);

	//16022 Table M.5 Set Last Column
	bool _Func_FPD_TM5_SetLowest_For_LastColumn(vector<LSAI_SGIT > &SGIT, LSAS style);

	//16022 Table M.5 Set Last Row ( Highest of Last Column )
	unsigned int _Func_Fpd_TM5_GetHighest_Of_LastColumn(vector<LSAI_SGIT > SGIT);

	//计算SC值
	bool Func_SC(unsigned char** pBuffer, const long width, const long height, double &SC, double &GT, GradeSymbol &Grade);

	//Sobel算子运算
	bool Func_Sobel(unsigned char** img, long** &gx, long** &gy, const long width, const long height);

	//根据Sobel运算得到的X、Y方向的梯度值计算每个像素的梯度
	bool Func_Gradient(long** gx, long** gy, double** &G, const long width, const long height);

	//将运算得到的边缘梯度图的X、Y方向分别映射
	bool Func_Mapping(double** G, long long* &mx, long long* &my, const long width, const long height);

	//将映射后的一维信号进行滤波
	bool Func_Gauss(long long* mx, long long* my, long long* &Amx, long long* &Amy, const long width, const long height, const int* model, const int M);

	//排除滤波后一维信号中的中间干扰点
	bool Func_CompareHill(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height);

	//得到滤波后的一维信号的波峰点位
	bool Func_GetMaxima(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height);

	//求得每个信息点的像素值均值
	bool Func_ModuleAvg(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, vector<vector<unsigned char > > &avr, const long width, const long height);

	//求全图数据区MOD值等级
	bool Func_MOD_DataAreaModulation(vector<vector<unsigned char > > R, MOD_DAMIT &DataAreaMODGradeTable, const double GT, const double SC);

	//求AN值
	bool Func_AN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade);

	//求网格偏差GN
	bool Func_GN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade);

	//L形区域以及反L形区域的各单元像素均值与MOD值计算与FPD各项分值计算
	bool Func_FPD(unsigned char** img, FPD_G &FPD_Grades, vector<long > Xmaxima, vector<long > Ymaxima, const double SC, const double GT, const long width, const long height);

	//16022 P111 M.1.4 Calculation and grading of average grade(计算L1 L2 QZL1 QZL2 （以及两个Segment区域的最小值）五项的均值再取最小)
	bool Func_CalculateFPDAverageGrade(FPD_G FPD_grades, Grade &grade);
};

#endif