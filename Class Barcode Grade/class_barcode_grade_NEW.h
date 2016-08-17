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

#define CALC_TIME //��������ʱ��,����ڿ���̨�����,����ǿ���̨����,��ע�͸ú�

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
	//�����ȼ����ż����Ӧ����
	typedef enum GradeSymbol {
		A = 4,
		B = 3,
		C = 2,
		D = 1,
		F = 0
	}GradeSymbol;
	//����ȼ����������Լ��ȼ�
	typedef struct Grade {
		//��߷�����
		unsigned char Rmax;
		//��ͷ�����
		unsigned char Rmin;
		//���ŷ���
		double SC;
		//��С��Ե����
		unsigned char ECmin;
		//���Ԫ�����ʲ����ȶ�
		unsigned char ERNmax;
		//ȱ�ݶ�
		double Defects;
		//���Ʊ�
		double MOD;
		//ȫ����ֵ
		double GT;
		//�������
		double Decodability;
		//������ȵȼ�
		GradeSymbol Decodability_Grade;
		//���ŷ���ȼ�
		GradeSymbol SC_Grade;
		//���Ʊȵȼ�
		GradeSymbol MOD_Grade;
		//ȱ�ݶȵȼ�
		GradeSymbol Defects_Grade;
		//��ͷ����ʵȼ�
		GradeSymbol Rmin_Grade;
		//��С��Ե����ȼ�
		GradeSymbol ECmin_Grade;
		//�ܷ����ɹ�
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
	//������ַ�����Ϣ
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
	//����
	OneDBarcodeGrading();

	~OneDBarcodeGrading();

	//��������:��ָ����һά���������
	//����:
	//pImg:��Ҫ������һά��ͼ��
	//type:һά������

	//����ֵ:�Ƿ�ɹ�����
	bool Grading(CImg* pImg, OneDBarCodeType type);

	//��������:���ý���ʱ��õ�������ȷ�Լ���
	//����:
	//decode_grade:	����ʱ��õ�������ȷ�Լ���
	//						  ��4Ϊ�ɹ����� 0Ϊ���ܽ��룩
	//����ֵ:�Ƿ�ɹ�ִ�У�������������ֵʱ��ʹDECODE��������ΪF,������false��
	bool SetDECODEGrade(int decode_grade);

	//��������:��ÿ�����ȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetDecodabilityGrade();

	//��������:��÷��ŷ����
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetSCGrade();

	//��������:��õ��Ʊȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetMODGrade();

	//��������:���ȱ�ݶȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetDefectsGrade();

	//��������:�����ͷ����ʼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).4(A)
	int GetRminGrade();

	//��������:�����С��Ե�����
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).4(A)
	int GetECminGrade();

	//��������:���������ȷ�Լ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).4(A)
	int GetDECODEGrade();

private:
	bool EAN128(CImg* pImg, Grade &grade);

	//��ͼƬ��X����ӳ�䣬�ۼӻҶ�ֵ
	bool Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height);

	//��ӳ�����źŲ��
	bool Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX);

	//��С��Ĩƽ
	bool Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M);

	//�ҵ���Ȼ���ڵĲ���
	bool Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc);

	//�ҵ�������Զ�������嶥
	bool Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right);

	//�õ��Ҷ����ֵ��Сֵ�Լ����GT 
	bool Func_GetGT(unsigned char** img, unsigned char &maxgray, unsigned char &mingray, double &SC, double &GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height);

	//���ָ���кŵ�����ֵ
	bool _Func_GetSelectedRowPixels(unsigned char** img, vector<unsigned char> &rowSet, const unsigned long rowNum, const long width);

	//���ָ��߾�����һά�ź���GTΪ���޻���
	bool _Func_Split(vector<unsigned char > &rowSet, vector<unsigned long > &locSet, double GT, vector<vector<unsigned char > > &sets);

	//���ݷֶκ�����ݻ����СECֵ�����ERNֵ
	bool _Func_ECminERNmax(vector<vector<vector<unsigned char > > > dataSet, double GT, Grade &grade);

	//����topmost��downmost��������ȷ�������뷶Χ�����зָ��������ɨ���߽��к���ɨ��
	bool OneDBarcodeGrading::Func_Scan(const CImg* pImg, unsigned char** img, vector<vector<unsigned long > > &locDataSet, vector<vector<float > > &locDataSet2, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, OneDBarcodeGrading::Grade &grade);

	//����topmost��downmost��������ȷ�������뷶Χ�����зָ��������ɨ���߽��к���ɨ��(DEBUG)
	bool Func_Scan(const CImg* pImg, unsigned char** img, vector<vector<unsigned long > > &locDataSet, vector<vector<float > > &locDataSet2, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, Grade &grade, vector<pair<string, clock_t > > &T);

	//���ݸ�����������
	bool Func_Grading(Grade &grade);

	//EAN128
	bool _Func_Decode_EAN128(vector<vector<float > > locDataSet, Grade &grade);

	//����������
	bool Func_Decodability(vector<vector<float > > locDataSet, OneDBarCodeType type, Grade &grade);

};


class TwoDBarcodeGrading final {
private:
	//�����ȼ����ż����Ӧ����
	typedef enum GradeSymbol {
		A = 4,
		B = 3,
		C = 2,
		D = 1,
		F = 0
	}GradeSymbol;
	//L�����������(L���� QZL���� CTSA����)
	typedef enum LShapeAreaStyle {
		L = 0,
		QZL = 1,
		CT1 = 2,
		CT2 = 3,
		SA = 4
	}LSAS;
	//����ȼ����������Լ��ȼ�
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
	//L�����򼴼�����̬����ĵ�Ԫ���ؾ�ֵ�Լ�MOD����
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

	//��������:��ָ���Ķ�ά���������
	//����:
	//pImg:��Ҫ�����Ķ�ά��ͼ��
	//error_correction_capacity:��������
	//type:��ά������
	//����ֵ:�Ƿ�ɹ�����
	bool Grading(CImg* pImg, const unsigned int error_correction_capacity, TwoDBarCodeType type);

	//��������:����δʹ�õľ���ĵȼ�
	//����:
	//t:��֪�Ĵ���������
	//error_correction_capacity:��������
	//����ֵ:�Ƿ�ɹ�����
	bool SetUECScore(int t, int error_correction_capacity);

	//��������:���ý���ʱ��õ�������ȷ�Լ���
	//����:
	//decode_grade:	����ʱ��õ�������ȷ�Լ���
	//						  ��4Ϊ�ɹ����� 0Ϊ���ܽ��룩
	//����ֵ:�Ƿ�ɹ�ִ�У�������������ֵʱ��ʹDECODE��������ΪF,������false��
	bool SetDECODEGrade(int decode_grade);

	//��������:��õ��Ʊȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetSCGrade();

	//��������:������򲻾��ȶȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetANGrade();

	//��������:������񲻾��ȶȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetGNGrade();

	//��������:��ù̶�ģʽ�𻵼���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetFPDGrade();

	//��������:��õ��Ʊȼ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetMODGrade();

	//��������:���δʹ�õľ�����
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetUECGrade();

	//��������:���������ȷ�Լ���
	//����ֵ:-1(ͼ��δ��ʼ��).0(F).1(D).2(C).3(B).4(A)
	int GetDECODEGrade();

private:
	//DataMatrix ����
	bool DataMatrixGrading(CImg* pImg, const unsigned int error_correction_capacity, Grade &grade);

	//15415 Table 7(A) (P19) Create & Edit
	GradeSymbol _Func_MOD_T7A(MOD_DAMIT DataAreaMODGradeTable, vector<MOD_GIT > &MODGIT, const unsigned int error_correction_capacity);

	//����CTת����
	unsigned int _Func_FPD_TransitionRatioTest(vector<double > SA_T, vector<double > CT_T);

	//������СΪ5�Ĵ����ж�module error //16022 P109 e
	bool _Func_FPD_CTRegularityTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT);

	//��CT��module error ռ��module�ı��� //16022 P109 f
	bool _Func_FPD_CTDamageTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT);

	//��SA��module error ռ��module�ı��� //16022 P109 g
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

	//����SCֵ
	bool Func_SC(unsigned char** pBuffer, const long width, const long height, double &SC, double &GT, GradeSymbol &Grade);

	//Sobel��������
	bool Func_Sobel(unsigned char** img, long** &gx, long** &gy, const long width, const long height);

	//����Sobel����õ���X��Y������ݶ�ֵ����ÿ�����ص��ݶ�
	bool Func_Gradient(long** gx, long** gy, double** &G, const long width, const long height);

	//������õ��ı�Ե�ݶ�ͼ��X��Y����ֱ�ӳ��
	bool Func_Mapping(double** G, long long* &mx, long long* &my, const long width, const long height);

	//��ӳ����һά�źŽ����˲�
	bool Func_Gauss(long long* mx, long long* my, long long* &Amx, long long* &Amy, const long width, const long height, const int* model, const int M);

	//�ų��˲���һά�ź��е��м���ŵ�
	bool Func_CompareHill(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height);

	//�õ��˲����һά�źŵĲ����λ
	bool Func_GetMaxima(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height);

	//���ÿ����Ϣ�������ֵ��ֵ
	bool Func_ModuleAvg(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, vector<vector<unsigned char > > &avr, const long width, const long height);

	//��ȫͼ������MODֵ�ȼ�
	bool Func_MOD_DataAreaModulation(vector<vector<unsigned char > > R, MOD_DAMIT &DataAreaMODGradeTable, const double GT, const double SC);

	//��ANֵ
	bool Func_AN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade);

	//������ƫ��GN
	bool Func_GN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade);

	//L�������Լ���L������ĸ���Ԫ���ؾ�ֵ��MODֵ������FPD�����ֵ����
	bool Func_FPD(unsigned char** img, FPD_G &FPD_Grades, vector<long > Xmaxima, vector<long > Ymaxima, const double SC, const double GT, const long width, const long height);

	//16022 P111 M.1.4 Calculation and grading of average grade(����L1 L2 QZL1 QZL2 ���Լ�����Segment�������Сֵ������ľ�ֵ��ȡ��С)
	bool Func_CalculateFPDAverageGrade(FPD_G FPD_grades, Grade &grade);
};

#endif