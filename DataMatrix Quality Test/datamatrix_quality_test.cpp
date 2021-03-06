#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

//filename
#include <ctime>
#include <sstream>
#include <iomanip>

//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

//#define SOBEL_IMG
//#define COUT_MAPPING

using namespace std;

//-----枚举-----

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

//-----结构-----

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

	Grade() {
		SC = GradeSymbol::F;		SC_Score = 0;
		MOD = GradeSymbol::F;
		AN = GradeSymbol::F;		AN_Score = 0;
		GN = GradeSymbol::F;		GN_Score = 0;
		FPD = GradeSymbol::F;
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
typedef union LShapeAreaInfoSegmentGradingTable_SubTable {
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


//-----实现函数-----
#ifdef _DEBUG

//获取当前的系统时间以创建文件名（仅供调试输出使用）
string NowTimeToFileName(const string preStr, const string suffixalNameStr) {
	static string pastTime;
	time_t t = time(NULL);
	tm tm = *localtime(&t);
	string nowTime;
	stringstream os;
	os.clear();
	os << put_time(&tm, "%y%m%d_%H%M%S");
	nowTime = os.str();
	string fileName;
	static short k = 0;
	if (nowTime != pastTime && (!nowTime.empty())) {//因为获取时间只精确到秒，但是程序可以在一秒之内创建数百个文件，所以要对文件名进行区分
		fileName = preStr + " - " + nowTime + "_0000" + suffixalNameStr;
		k = 0;
	}
	else {
		char extra[5];
		sprintf_s(extra, sizeof(extra), "%04d", ++k);//
		fileName = preStr + " - " + nowTime + "_" + extra + suffixalNameStr;
	}
	pastTime = nowTime;
	return fileName;
}

//根据得到的波峰点位绘制网格（仅供调试输出中间结果）
bool __Func_DrawLine(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, const long width, const long height, const unsigned char color) {
	if (img == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(preview[i], img[i], width * sizeof(unsigned char));
	}

	for each(long y in Xmaxima) {
		for (long i = 0; i < width; ++i) {
			preview[y][i] = (unsigned char)color;
		}
	}

	for each(long x in Ymaxima) {
		for (long j = 0; j < height; ++j) {
			preview[j][x] = (unsigned char)color;
		}
	}
	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	string filepath = NowTimeToFileName("..//results//dqt//DrawLine", ".bmp");
	pPreviewImg->SaveToFile(filepath.c_str());

	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

	return true;
}

//输出M5表格
bool __Func_Display_M5(vector<LSAI_SGIT > SGIT, LSAS style) {
	if (style == LSAS::L || style == LSAS::QZL) {
		cout << "MOD.G.L" << "\tNo.M" << "\tC.No.M" << "\tR.D.M" << "\tD.M" << "\tN.D.G" << "\tL.G" << endl;
		for (unsigned int G = 0; G <= 4; ++G) {
			cout << SGIT[G].MOD_grade_level << "\t" << SGIT[G].no_of_modules << "\t" << SGIT[G].cum_no_of_modules
				<< "\t" << SGIT[G].remainder_damaged_modules << "\t" << SGIT[G].ST.LQZLT.damaged_modules
				<< "\t" << SGIT[G].ST.LQZLT.notional_damage_grade << "\t" << SGIT[G].lower_of_grades << endl;
		}
	}
	else if (style == LSAS::CT1 || style == LSAS::CT2 || style == LSAS::SA) {
		cout << "MOD.G.L" << "\tNo.M" << "\tC.No.M" << "\tR.D.M" << "\tCT.R.M" << "\tCT.R.G" << "\tCT.D.M" << "\tCT.D.G" << "\tSA.D.M" << "\tSA.D.G" << "\tL.G" << endl;
		for (unsigned int G = 0; G <= 4; ++G) {
			cout << SGIT[G].MOD_grade_level << "\t" << SGIT[G].no_of_modules << "\t" << SGIT[G].cum_no_of_modules << "\t" << SGIT[G].remainder_damaged_modules
				<< "\t" << /*SGIT[G].ST.CTSAT.CT_regularity_modules*/"-" << "\t" << SGIT[G].ST.CTSAT.CT_regularity_grade
				<< "\t" << SGIT[G].ST.CTSAT.CT_damaged_modules << "\t" << SGIT[G].ST.CTSAT.CT_damage_grade
				<< "\t" << SGIT[G].ST.CTSAT.SA_damaged_modules << "\t" << SGIT[G].ST.CTSAT.SA_damage_grade
				<< "\t" << SGIT[G].lower_of_grades << endl;
		}
	}
	return true;
}

//输出T7A表格
bool __Func_Display_T7A(vector<MOD_GIT > MODGIT) {
	cout << "MOD.C.G.L" << "\tNo.C.L" << "\tC.No.C" << "\tR.C" << "\tN.U.ECC" << "\tN.UEC" << "\tN.UEC.G" << "\tL.G" << endl;
	for (unsigned int G = 0; G <= 4; ++G) {
		cout << MODGIT[G].MOD_codeword_grade_level << "\t" << MODGIT[G].No_of_codewords_at_level_a << "\t" << MODGIT[G].cumulative_no_of_codewords_at_level_a_or_higher
			<< "\t" << MODGIT[G].remaining_codewords << "\t" << MODGIT[G].notional_unused_error_correction_capacity
			<< "\t" << MODGIT[G].notional_UEC << "\t" << MODGIT[G].notional_UEC_grade
			<< "\t" << MODGIT[G].lower_of_a_or_d << endl;
	}
	return true;
}

#endif

//15415 Table 7(A) (P19) Create & Edit
GradeSymbol _Func_MOD_T7A(MOD_DAMIT DataAreaMODGradeTable, vector<MOD_GIT > &MODGIT, const unsigned int error_correction_capacity) {
	if (DataAreaMODGradeTable.modinfo.size() == 0)
		return GradeSymbol::F;

	//第一列 Init
	for (unsigned int G = 0; G <= 4; ++G) {
		MOD_GIT row;
		row.MOD_codeword_grade_level = (4 - G);
		MODGIT.push_back(row);
	}

	//第二列
	MODGIT[0].No_of_codewords_at_level_a = DataAreaMODGradeTable.numofgradeA;
	MODGIT[1].No_of_codewords_at_level_a = DataAreaMODGradeTable.numofgradeB;
	MODGIT[2].No_of_codewords_at_level_a = DataAreaMODGradeTable.numofgradeC;
	MODGIT[3].No_of_codewords_at_level_a = DataAreaMODGradeTable.numofgradeD;
	MODGIT[4].No_of_codewords_at_level_a = DataAreaMODGradeTable.numofgradeE;

	//第三、四列
	MODGIT[0].cumulative_no_of_codewords_at_level_a_or_higher = DataAreaMODGradeTable.numofgradeA;
	MODGIT[0].remaining_codewords = DataAreaMODGradeTable.sum - MODGIT[0].cumulative_no_of_codewords_at_level_a_or_higher;
	MODGIT[1].cumulative_no_of_codewords_at_level_a_or_higher = DataAreaMODGradeTable.numofgradeA + DataAreaMODGradeTable.numofgradeB;
	MODGIT[1].remaining_codewords = DataAreaMODGradeTable.sum - MODGIT[1].cumulative_no_of_codewords_at_level_a_or_higher;
	MODGIT[2].cumulative_no_of_codewords_at_level_a_or_higher = DataAreaMODGradeTable.numofgradeA + DataAreaMODGradeTable.numofgradeB + DataAreaMODGradeTable.numofgradeC;
	MODGIT[2].remaining_codewords = DataAreaMODGradeTable.sum - MODGIT[2].cumulative_no_of_codewords_at_level_a_or_higher;
	MODGIT[3].cumulative_no_of_codewords_at_level_a_or_higher = DataAreaMODGradeTable.numofgradeA + DataAreaMODGradeTable.numofgradeB + DataAreaMODGradeTable.numofgradeC + DataAreaMODGradeTable.numofgradeD;
	MODGIT[3].remaining_codewords = DataAreaMODGradeTable.sum - MODGIT[3].cumulative_no_of_codewords_at_level_a_or_higher;
	MODGIT[4].cumulative_no_of_codewords_at_level_a_or_higher = DataAreaMODGradeTable.sum;
	MODGIT[4].remaining_codewords = 0;

	//第五列
	MODGIT[0].notional_unused_error_correction_capacity = error_correction_capacity - MODGIT[0].remaining_codewords;
	MODGIT[1].notional_unused_error_correction_capacity = error_correction_capacity - MODGIT[1].remaining_codewords;
	MODGIT[2].notional_unused_error_correction_capacity = error_correction_capacity - MODGIT[2].remaining_codewords;
	MODGIT[3].notional_unused_error_correction_capacity = error_correction_capacity - MODGIT[3].remaining_codewords;
	MODGIT[4].notional_unused_error_correction_capacity = error_correction_capacity - MODGIT[4].remaining_codewords;

	//第六列
	MODGIT[0].notional_UEC = MODGIT[0].notional_unused_error_correction_capacity / error_correction_capacity*1.0;
	MODGIT[1].notional_UEC = MODGIT[1].notional_unused_error_correction_capacity / error_correction_capacity*1.0;
	MODGIT[2].notional_UEC = MODGIT[2].notional_unused_error_correction_capacity / error_correction_capacity*1.0;
	MODGIT[3].notional_UEC = MODGIT[3].notional_unused_error_correction_capacity / error_correction_capacity*1.0;
	MODGIT[4].notional_UEC = MODGIT[4].notional_unused_error_correction_capacity / error_correction_capacity*1.0;

	//第七列
	for (unsigned int G = 0; G <= 4; ++G) {
		if (MODGIT[G].notional_unused_error_correction_capacity >= 0) {
			if (MODGIT[G].notional_UEC < 0.25) {
				MODGIT[G].notional_UEC_grade = GradeSymbol::F;
			}
			else if (MODGIT[G].notional_UEC < 0.37) {
				MODGIT[G].notional_UEC_grade = GradeSymbol::D;
			}
			else if (MODGIT[G].notional_UEC < 0.5) {
				MODGIT[G].notional_UEC_grade = GradeSymbol::C;
			}
			else if (MODGIT[G].notional_UEC < 0.62) {
				MODGIT[G].notional_UEC_grade = GradeSymbol::B;
			}
			else {
				MODGIT[G].notional_UEC_grade = GradeSymbol::A;
			}
		}
		else {
			MODGIT[G].notional_UEC_grade = GradeSymbol::F;
		}
	}

	//第八列
	unsigned int max = 0;
	for (unsigned int G = 0; G <= 4; ++G) {
		MODGIT[G].lower_of_a_or_d = MODGIT[G].MOD_codeword_grade_level < MODGIT[G].notional_UEC_grade ? MODGIT[G].MOD_codeword_grade_level : MODGIT[G].notional_UEC_grade;
		if (MODGIT[G].lower_of_a_or_d > max)
			max = MODGIT[G].lower_of_a_or_d;
	}


	switch (max)
	{
	case 4:
		return GradeSymbol::A;
	case 3:
		return GradeSymbol::B;
	case 2:
		return GradeSymbol::C;
	case 1:
		return GradeSymbol::D;
	case 0:
		return GradeSymbol::F;
	}

	return GradeSymbol::F;
}

//计算CT转换率
unsigned int _Func_FPD_TransitionRatioTest(vector<double > SA_T, vector<double > CT_T) {
	vector<int > SA;
	for each(auto var in SA_T) {
		if (var >= 0)
			SA.push_back(1);
		else
			SA.push_back(0);
	}

	vector<int > CT;
	for each(auto var in CT_T) {
		if (var >= 0)
			CT.push_back(1);
		else
			CT.push_back(0);
	}

	unsigned int Ts = 0;
	for (vector<int >::iterator p = SA.begin() + 1; p != SA.end(); ++p) {
		unsigned int increment = abs(*p - *(p - 1));
		Ts += increment;
	}

	unsigned int Tc = 0;
	for (vector<int >::iterator p = CT.begin() + 1; p != CT.end(); ++p) {
		unsigned int increment = abs(*p - *(p - 1));
		Tc += increment;
	}

	unsigned int Ts_ = Ts == 0 ? 0 : (Ts - 1);

	double TR = 1;
	if (Tc != 0)
		TR = Ts_ * 1.0 / Tc * 1.0;
	else
		return 0;

	unsigned int grade = 0;
	if (TR < 0.06) {
		grade = 4;
	}
	else if (TR < 0.08) {
		grade = 3;
	}
	else if (TR < 0.10) {
		grade = 2;
	}
	else if (TR < 0.12) {
		grade = 1;
	}
	else {
		grade = 0;
	}

	return grade;
}

//滑动大小为5的窗口判断module error //16022 P109 e
bool _Func_FPD_CTRegularityTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT) {
	if (MT.modinfo.size() == 0 || SGIT.size() == 0)
		return false;
	//当module的数量小于5的时候，窗口不需要进行滑动
	if (MT.modinfo.size() <= 5) {
		for (unsigned int G = 0; G < 4; ++G) {
			if (SGIT[G].cum_no_of_modules <= MT.modinfo.size() - 1) {
				SGIT[G].ST.CTSAT.CT_regularity_grade = GradeSymbol::F;
			}
			else {
				SGIT[G].ST.CTSAT.CT_regularity_grade = GradeSymbol::A;
			}
		}
	}
	else {
		bool endflag[4];
		for (unsigned int i = 0; i < 4; ++i)
			endflag[i] = false;

		//当窗口在L或者QZL区域上滑动的时候，只要发现同一个窗口中存在两个或者以上的module error，那么在窗口继续滑动的时候便不需要再进行判断
		for (vector<pair<double, GradeSymbol > >::iterator p = MT.modinfo.begin() + 4; p < MT.modinfo.end(); ++p) {
			for (unsigned int G = 0; G < 4; ++G) {
				if (endflag[0] == true && endflag[1] == true && endflag[2] == true && endflag[3] == true)
					break;
				if (endflag[G] == true)
					continue;
				int count = 0;
				if ((p - 4)->second < (4 - G)) ++count;
				if ((p - 3)->second < (4 - G)) ++count;
				if ((p - 2)->second < (4 - G)) ++count;
				if ((p - 1)->second < (4 - G)) ++count;
				if (p->second < (4 - G)) ++count;
				if (count >= 2) {
					SGIT[G].ST.CTSAT.CT_regularity_grade = GradeSymbol::F;
					endflag[G] = true;
				}
				else {
					SGIT[G].ST.CTSAT.CT_regularity_grade = GradeSymbol::A;
				}
			}
		}
	}
	//M5表格最后一行即等级为0的一行的Cum. no. of modules列的值一定为modules的数量，所以不需要进行判断
	SGIT[4].ST.CTSAT.CT_regularity_grade = GradeSymbol::A;

}

//在CT中module error 占总module的比例 //16022 P109 f
bool _Func_FPD_CTDamageTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT) {
	if (MT.modinfo.size() == 0 || SGIT.size() == 0)
		return false;
	SGIT[0].ST.CTSAT.CT_damaged_modules = (double)(MT.numofgradeB + MT.numofgradeC + MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[1].ST.CTSAT.CT_damaged_modules = (double)(MT.numofgradeC + MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[2].ST.CTSAT.CT_damaged_modules = (double)(MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[3].ST.CTSAT.CT_damaged_modules = (double)MT.numofgradeE / MT.sum * 1.0;
	SGIT[4].ST.CTSAT.CT_damaged_modules = 0;

	for (unsigned int G = 0; G <= 4; ++G) {
		if (SGIT[G].ST.CTSAT.CT_damaged_modules < 0.1) {
			SGIT[G].ST.CTSAT.CT_damage_grade = GradeSymbol::A;
		}
		else if (SGIT[G].ST.CTSAT.CT_damaged_modules < 0.15) {
			SGIT[G].ST.CTSAT.CT_damage_grade = GradeSymbol::B;
		}
		else if (SGIT[G].ST.CTSAT.CT_damaged_modules < 0.20) {
			SGIT[G].ST.CTSAT.CT_damage_grade = GradeSymbol::C;
		}
		else if (SGIT[G].ST.CTSAT.CT_damaged_modules < 0.25) {
			SGIT[G].ST.CTSAT.CT_damage_grade = GradeSymbol::D;
		}
		else {
			SGIT[G].ST.CTSAT.CT_damage_grade = GradeSymbol::F;
		}
	}
	return true;
}

//在SA中module error 占总module的比例 //16022 P109 g
bool _Func_FPD_SAFixedPatternTest(LSAI_MT MT, vector<LSAI_SGIT > &SGIT) {
	if (MT.modinfo.size() == 0 || SGIT.size() == 0)
		return false;
	SGIT[0].ST.CTSAT.SA_damaged_modules = (double)(MT.numofgradeB + MT.numofgradeC + MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[1].ST.CTSAT.SA_damaged_modules = (double)(MT.numofgradeC + MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[2].ST.CTSAT.SA_damaged_modules = (double)(MT.numofgradeD + MT.numofgradeE) / MT.sum * 1.0;
	SGIT[3].ST.CTSAT.SA_damaged_modules = (double)MT.numofgradeE / MT.sum * 1.0;
	SGIT[4].ST.CTSAT.SA_damaged_modules = 0;

	for (unsigned int G = 0; G <= 4; ++G) {
		if (SGIT[G].ST.CTSAT.SA_damaged_modules < 0.1) {
			SGIT[G].ST.CTSAT.SA_damage_grade = GradeSymbol::A;
		}
		else if (SGIT[G].ST.CTSAT.SA_damaged_modules < 0.15) {
			SGIT[G].ST.CTSAT.SA_damage_grade = GradeSymbol::B;
		}
		else if (SGIT[G].ST.CTSAT.SA_damaged_modules < 0.20) {
			SGIT[G].ST.CTSAT.SA_damage_grade = GradeSymbol::C;
		}
		else if (SGIT[G].ST.CTSAT.SA_damaged_modules < 0.25) {
			SGIT[G].ST.CTSAT.SA_damage_grade = GradeSymbol::D;
		}
		else {
			SGIT[G].ST.CTSAT.SA_damage_grade = GradeSymbol::F;
		}
	}
	return true;
}

//16022 Table M.4 Create
bool _Func_FPD_TM4(vector<double > info, LSAI_MT &MT, LSAS style) {
	if (style == LSAS::L) {
		for each(double mod in info) {
			++MT.sum;
			mod = -mod;
			if (mod >= 0.5) {
				++MT.numofgradeA;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::A;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.4) {
				++MT.numofgradeB;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::B;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.3) {
				++MT.numofgradeC;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::C;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.2) {
				++MT.numofgradeD;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::D;
				MT.modinfo.push_back(ipa);
			}
			else {
				++MT.numofgradeE;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::F;
				MT.modinfo.push_back(ipa);
			}
		}
		return true;
	}
	else if (style == LSAS::QZL || style == LSAS::SA) {
		for each(double mod in info) {
			++MT.sum;
			if (mod >= 0.5) {
				++MT.numofgradeA;
				pair<double, GradeSymbol> ipa;
				ipa.first = mod;
				ipa.second = GradeSymbol::A;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.4) {
				++MT.numofgradeB;
				pair<double, GradeSymbol> ipa;
				ipa.first = mod;
				ipa.second = GradeSymbol::B;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.3) {
				++MT.numofgradeC;
				pair<double, GradeSymbol> ipa;
				ipa.first = mod;
				ipa.second = GradeSymbol::C;
				MT.modinfo.push_back(ipa);
			}
			else if (mod >= 0.2) {
				++MT.numofgradeD;
				pair<double, GradeSymbol> ipa;
				ipa.first = mod;
				ipa.second = GradeSymbol::D;
				MT.modinfo.push_back(ipa);
			}
			else {
				++MT.numofgradeE;
				pair<double, GradeSymbol> ipa;
				ipa.first = mod;
				ipa.second = GradeSymbol::F;
				MT.modinfo.push_back(ipa);
			}
		}
		return true;
	}
	else if (style == LSAS::CT1) {
		bool bow[2] = { false,true };
		int flag = 0;
		for each(double mod in info) {
			++MT.sum;
			if (bow[flag % 2]) {
				mod = mod;
			}
			else {
				mod = -mod;
			}
			if (mod >= 0.5) {
				++MT.numofgradeA;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::A;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.4) {
				++MT.numofgradeB;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::B;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.3) {
				++MT.numofgradeC;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::C;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.2) {
				++MT.numofgradeD;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::D;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else {
				if (mod > 0)
					++flag;
				++MT.numofgradeE;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::F;
				MT.modinfo.push_back(ipa);
			}

		}
		return true;
	}
	else if (style == LSAS::CT2) {
		bool bow[2] = { true,false };
		int flag = 0;
		for each(double mod in info) {
			++MT.sum;
			if (bow[flag % 2]) {
				mod = mod;
			}
			else {
				mod = -mod;
			}
			//mod = mod * flag;
			if (mod >= 0.5) {
				++MT.numofgradeA;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::A;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.4) {
				++MT.numofgradeB;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::B;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.3) {
				++MT.numofgradeC;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::C;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else if (mod >= 0.2) {
				++MT.numofgradeD;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::D;
				MT.modinfo.push_back(ipa);
				++flag;
			}
			else {
				if (mod > 0)
					++flag;
				++MT.numofgradeE;
				pair<double, GradeSymbol> ipa;
				ipa.first = -mod;
				ipa.second = GradeSymbol::F;
				MT.modinfo.push_back(ipa);
			}

		}
		return true;
	}
	return false;
}

//16022 Table M.5 Init
bool _Func_FPD_TM5_Init(vector<LSAI_SGIT > &SGIT) {
	for (int g = 4; g >= 0; --g) {
		LSAI_SGIT row;
		row.MOD_grade_level = g;
		row.no_of_modules = 0;
		row.cum_no_of_modules = 0;
		row.remainder_damaged_modules = 0;
		SGIT.push_back(row);
	}
	return true;
}

//16022 Table M.5 Edit
bool _Func_FPD_TM5_Edit(LSAI_MT info, vector<LSAI_SGIT > &SGIT, LSAS style) {
	//当区域为L或者QZL时，子表格有1项2列
	if (style == LSAS::L || style == LSAS::QZL) {
		//4 填写表格第2列到第5列
		SGIT[0].no_of_modules = info.numofgradeA;
		SGIT[0].cum_no_of_modules = info.numofgradeA;
		SGIT[0].remainder_damaged_modules = info.sum - SGIT[0].cum_no_of_modules;
		SGIT[0].ST.LQZLT.damaged_modules = SGIT[0].remainder_damaged_modules * 1.0 / info.sum*1.0;

		//3
		SGIT[1].no_of_modules = info.numofgradeB;
		SGIT[1].cum_no_of_modules = info.numofgradeA + info.numofgradeB;
		SGIT[1].remainder_damaged_modules = info.sum - SGIT[1].cum_no_of_modules;
		SGIT[1].ST.LQZLT.damaged_modules = SGIT[1].remainder_damaged_modules * 1.0 / info.sum*1.0;

		//2
		SGIT[2].no_of_modules = info.numofgradeC;
		SGIT[2].cum_no_of_modules = info.numofgradeA + info.numofgradeB + info.numofgradeC;
		SGIT[2].remainder_damaged_modules = info.sum - SGIT[2].cum_no_of_modules;
		SGIT[2].ST.LQZLT.damaged_modules = SGIT[2].remainder_damaged_modules * 1.0 / info.sum*1.0;

		//1
		SGIT[3].no_of_modules = info.numofgradeD;
		SGIT[3].cum_no_of_modules = info.numofgradeA + info.numofgradeB + info.numofgradeC + info.numofgradeD;
		SGIT[3].remainder_damaged_modules = info.sum - SGIT[3].cum_no_of_modules;
		SGIT[3].ST.LQZLT.damaged_modules = SGIT[3].remainder_damaged_modules * 1.0 / info.sum*1.0;

		//0
		SGIT[4].no_of_modules = info.numofgradeE;
		SGIT[4].cum_no_of_modules = info.sum;
		SGIT[4].remainder_damaged_modules = 0;
		SGIT[4].ST.LQZLT.damaged_modules = SGIT[4].remainder_damaged_modules * 1.0 / info.sum*1.0;

		//按照M.1表获得等级，并填到表格第6列
		for (unsigned int G = 0; G <= 4; ++G) {
			if (SGIT[G].ST.LQZLT.damaged_modules == 0) {
				SGIT[G].ST.LQZLT.notional_damage_grade = GradeSymbol::A;
			}
			else if (SGIT[G].ST.LQZLT.damaged_modules <= 0.09) {
				SGIT[G].ST.LQZLT.notional_damage_grade = GradeSymbol::B;
			}
			else if (SGIT[G].ST.LQZLT.damaged_modules <= 0.13) {
				SGIT[G].ST.LQZLT.notional_damage_grade = GradeSymbol::C;
			}
			else if (SGIT[G].ST.LQZLT.damaged_modules <= 0.17) {
				SGIT[G].ST.LQZLT.notional_damage_grade = GradeSymbol::D;
			}
			else if (SGIT[G].ST.LQZLT.damaged_modules > 0.17) {
				SGIT[G].ST.LQZLT.notional_damage_grade = GradeSymbol::F;
			}

			//将等级较小值填到表格第7列
			//SGIT[G].lower_of_grades = SGIT[G].ST.LQZLT.notional_damage_grade < SGIT[G].MOD_grade_level ? SGIT[G].ST.LQZLT.notional_damage_grade : SGIT[G].MOD_grade_level;
		}

		return true;
	}
	//当区域为CT或者SA时，子表格有3项6列
	else if (style == LSAS::CT1 || style == LSAS::CT2 || style == LSAS::SA) {
		//4 填写第2列到第4列的数据
		SGIT[0].no_of_modules = info.numofgradeA;
		SGIT[0].cum_no_of_modules = info.numofgradeA;
		SGIT[0].remainder_damaged_modules = info.sum - SGIT[0].cum_no_of_modules;

		//3
		SGIT[1].no_of_modules = info.numofgradeB;
		SGIT[1].cum_no_of_modules = info.numofgradeA + info.numofgradeB;
		SGIT[1].remainder_damaged_modules = info.sum - SGIT[1].cum_no_of_modules;

		//2
		SGIT[2].no_of_modules = info.numofgradeC;
		SGIT[2].cum_no_of_modules = info.numofgradeA + info.numofgradeB + info.numofgradeC;
		SGIT[2].remainder_damaged_modules = info.sum - SGIT[2].cum_no_of_modules;

		//1
		SGIT[3].no_of_modules = info.numofgradeD;
		SGIT[3].cum_no_of_modules = info.numofgradeA + info.numofgradeB + info.numofgradeC + info.numofgradeD;
		SGIT[3].remainder_damaged_modules = info.sum - SGIT[3].cum_no_of_modules;

		//0
		SGIT[4].no_of_modules = info.numofgradeE;
		SGIT[4].cum_no_of_modules = info.sum;
		SGIT[4].remainder_damaged_modules = 0;

		return true;
	}
	return false;
}

//16022 Table M.5 Set Last Column
bool _Func_FPD_TM5_SetLowest_For_LastColumn(vector<LSAI_SGIT > &SGIT, LSAS style) {
	if (style == LSAS::L || style == LSAS::QZL) {
		for (unsigned int G = 0; G <= 4; ++G) {
			SGIT[G].lower_of_grades = SGIT[G].ST.LQZLT.notional_damage_grade < SGIT[G].MOD_grade_level ? SGIT[G].ST.LQZLT.notional_damage_grade : SGIT[G].MOD_grade_level;
		}
		return true;
	}
	else if (style == LSAS::CT1 || style == LSAS::CT2 || style == LSAS::SA) {
		for (unsigned int G = 0; G <= 4; ++G) {
			unsigned int MIN_Grade = 4;
			if (SGIT[G].MOD_grade_level <= MIN_Grade)
				MIN_Grade = SGIT[G].MOD_grade_level;
			if (SGIT[G].ST.CTSAT.CT_regularity_grade <= MIN_Grade)
				MIN_Grade = SGIT[G].ST.CTSAT.CT_regularity_grade;
			if (SGIT[G].ST.CTSAT.CT_damage_grade <= MIN_Grade)
				MIN_Grade = SGIT[G].ST.CTSAT.CT_damage_grade;
			if (SGIT[G].ST.CTSAT.SA_damage_grade <= MIN_Grade)
				MIN_Grade = SGIT[G].ST.CTSAT.SA_damage_grade;

			SGIT[G].lower_of_grades = MIN_Grade;
		}

		return true;
	}
	return false;
}

//16022 Table M.5 Set Last Row ( Highest of Last Column )
unsigned int _Func_Fpd_TM5_GetHighest_Of_LastColumn(vector<LSAI_SGIT > SGIT) {
	unsigned int max = 0;
	for (unsigned int G = 0; G <= 4; ++G) {
		if (SGIT[G].lower_of_grades >= max)
			max = SGIT[G].lower_of_grades;
	}
	return max;
}

//计算SC值
bool Func_SC(unsigned char** pBuffer, const long width, const long height, double &SC, double &GT, GradeSymbol &Grade) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));

	for (long i = 0; i < height; ++i) {
		for (long j = 0; j < width; ++j) {
			++tab[(unsigned int)pBuffer[i][j]];
		}
	}

	unsigned long long limit = 0.1*width*height;
	unsigned long long count = 0;
	unsigned long long sum = 0;

	for (int i = 0; i < 256; ++i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avglower = (double)sum / count;

	count = 0;
	sum = 0;
	for (int i = 255; i >= 0; --i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avggreater = (double)sum / count;

	SC = avggreater - avglower;

	GT = (avglower + avggreater) / 2;

	double score;
	//score = 1.0 * (avggreater - avglower) / avggreater;
	score = 1.0 * (avggreater - avglower) / 255.0;

	if (score >= 0.7) {
		Grade = GradeSymbol::A;
	}
	else if (score >= 0.55) {
		Grade = GradeSymbol::B;
	}
	else if (score >= 0.4) {
		Grade = GradeSymbol::C;
	}
	else if (score >= 0.2) {
		Grade = GradeSymbol::D;
	}
	else {
		Grade = GradeSymbol::F;
	}

	return true;
}

//Sobel算子运算
bool Func_Sobel(unsigned char** img, long** &gx, long** &gy, const long width, const long height) {
	if (img == NULL || gx == NULL || gy == NULL)
		return false;
	unsigned char* r1 = NULL;
	unsigned char* r2 = NULL;
	unsigned char* r3 = NULL;
	for (long i = 0; i < height; ++i) {
		if (i == 0) {
			r1 = img[i];
		}
		else {
			r1 = img[i - 1];
		}
		r2 = img[i];
		if (i == height - 1) {
			r3 = img[i];
		}
		else {
			r3 = img[i + 1];
		}
		if (r1 == NULL || r2 == NULL || r3 == NULL)
			return false;
		gx[i][0] = r1[0] * (-1) + r2[0] * (-2) + r3[0] * (-1) + r1[2] + r2[2] * 2 + r3[2];
		gy[i][0] = r1[0] * (-1) + r1[1] * (-2) + r1[2] * (-1) + r3[0] + r3[1] * 2 + r3[2];
		for (long j = 1; j < width - 1; ++j) {
			gx[i][j] = r1[j - 1] * (-1) + r2[j - 1] * (-2) + r3[j - 1] * (-1) + r1[j + 1] + r2[j + 1] * 2 + r3[j + 1];
			gy[i][j] = r1[j - 1] * (-1) + r1[j] * (-2) + r1[j + 1] * (-1) + r3[j - 1] + r3[j] * 2 + r3[j + 1];
		}
		gx[i][width - 1] = r1[width - 2] * (-1) + r2[width - 2] * (-2) + r3[width - 2] * (-1) + r1[width - 1] + r2[width - 1] * 2 + r3[width - 1];
		gy[i][width - 1] = r1[width - 2] * (-1) + r1[width - 1] * (-2) + r1[width - 1] * (-1) + r3[width - 2] + r3[width - 1] * 2 + r3[width - 1];
	}
	return true;
}

//根据Sobel运算得到的X、Y方向的梯度值计算每个像素的梯度
bool Func_Gradient(long** gx, long** gy, double** &G, const long width, const long height) {
	if (gx == NULL || gy == NULL || G == NULL)
		return false;

#ifdef SOBEL_IMG
	////
	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

#endif

	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			long long x = gx[j][i];
			long long y = gy[j][i];
			G[j][i] = sqrt((x * x) + (y * y)) > 255 ? 255 : sqrt((x * x) + (y * y));
#ifdef SOBEL_IMG
			preview[j][i] = (unsigned char)G[j][i];
#endif
		}
	}

#ifdef SOBEL_IMG
	////
	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	string filepath = NowTimeToFileName("..//results//dqt//Preview", ".bmp");
	pPreviewImg->SaveToFile(filepath.c_str());

	////
	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

#endif

	return true;
}

//将运算得到的边缘梯度图的X、Y方向分别映射
bool Func_Mapping(double** G, long long* &mx, long long* &my, const long width, const long height) {
	if (G == NULL || mx == NULL || my == NULL)
		return false;
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			mx[j] += G[j][i];
			my[i] += G[j][i];
		}
	}
	return true;
}

//将映射后的一维信号进行滤波
bool Func_Gauss(long long* mx, long long* my, long long* &Amx, long long* &Amy, const long width, const long height, const int* model, const int M) {
	if (mx == NULL || Amx == NULL)
		return false;
	Amx[0] = (mx[0] * model[3] + mx[1] * model[4] + mx[2] * model[5] + mx[3] * model[6]) / M;
	Amx[1] = (mx[0] * model[2] + mx[1] * model[3] + mx[2] * model[4] + mx[3] * model[5] + mx[4] * model[6]) / M;
	Amx[2] = (mx[0] * model[1] + mx[1] * model[2] + mx[2] * model[3] + mx[3] * model[4] + mx[4] * model[5] + mx[5] * model[6]) / M;
	for (long y = 3; y < height; ++y) {
		Amx[y] = (mx[y - 3] * model[0] + mx[y - 2] * model[1] + mx[y - 1] * model[2] + mx[y] * model[3] + mx[y + 1] * model[4] + mx[y + 2] * model[5] + mx[y + 3] * model[6]) / M;
	}
	Amx[height - 3] = (mx[height - 6] * model[0] + mx[height - 5] * model[1] + mx[height - 4] * model[2] + mx[height - 3] * model[3] + mx[height - 2] * model[4] + mx[height - 1] * model[5]) / M;
	Amx[height - 2] = (mx[height - 5] * model[0] + mx[height - 4] * model[1] + mx[height - 3] * model[2] + mx[height - 2] * model[3] + mx[height - 1] * model[4]) / M;
	Amx[height - 1] = (mx[height - 4] * model[0] + mx[height - 3] * model[1] + mx[height - 2] * model[2] + mx[height - 1] * model[3]) / M;

	if (my == NULL || Amy == NULL)
		return false;

	Amy[0] = (my[0] * model[3] + my[1] * model[4] + my[2] * model[5] + my[3] * model[6]) / M;
	Amy[1] = (my[0] * model[2] + my[1] * model[3] + my[2] * model[4] + my[3] * model[5] + my[4] * model[6]) / M;
	Amy[2] = (my[0] * model[1] + my[1] * model[2] + my[2] * model[3] + my[3] * model[4] + my[4] * model[5] + my[5] * model[6]) / M;
	for (long x = 3; x < width; ++x) {
		Amy[x] = (my[x - 3] * model[0] + my[x - 2] * model[1] + my[x - 1] * model[2] + my[x] * model[3] + my[x + 1] * model[4] + my[x + 2] * model[5] + my[x + 3] * model[6]) / M;
	}
	Amy[width - 3] = (my[width - 6] * model[0] + my[width - 5] * model[1] + my[width - 4] * model[2] + my[width - 3] * model[3] + my[width - 2] * model[4] + my[width - 1] * model[5]) / M;
	Amy[width - 2] = (my[width - 5] * model[0] + my[width - 4] * model[1] + my[width - 3] * model[2] + my[width - 2] * model[3] + my[width - 1] * model[4]) / M;
	Amy[width - 1] = (my[width - 4] * model[0] + my[width - 3] * model[1] + my[width - 2] * model[2] + my[width - 1] * model[3]) / M;
	return true;
}

//排除滤波后一维信号中的中间干扰点
bool Func_CompareHill(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
	if (Amx == NULL || Amy == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;

	long ypre = 0, xpre = 0;
	vector<pair<long, long> > Xdifference;
	for each(long y in Xmaxima) {
		long max = 0;
		long d1 = abs(Amx[y - 1] - Amx[y - 2]);
		if (d1 >= max) {
			max = d1;
		}
		long d2 = abs(Amx[y] - Amx[y - 1]);
		if (d2 >= max) {
			max = d2;
		}
		long d3 = abs(Amx[y + 1] - Amx[y]);
		if (d3 >= max) {
			max = d3;
		}
		long d4 = abs(Amx[y + 2] - Amx[y + 1]);
		if (d4 >= max) {
			max = d4;
		}
		pair<long, long> temp;
		temp.first = y;
		temp.second = max;
		Xdifference.push_back(temp);

		//cout << "Y LOCATION:" << y << "\tDISTANCE:" << y - ypre << "\tMAXVALLUE:" << max << endl;
		ypre = y;
	}

	long XMAX = 0;
	for each(auto pa in Xdifference) {
		if (pa.second > XMAX)
			XMAX = pa.second;
	}

	long Xthreshold = XMAX * 15 / 100;
	for each(auto pa in Xdifference) {
		if (pa.second < Xthreshold) {
			Xmaxima.erase(find(Xmaxima.begin(), Xmaxima.end(), pa.first));
		}
	}



	//cout << "------------------------" << endl;

	vector<pair<long, long> > Ydifference;
	for each(long x in Ymaxima) {
		long max = 0;
		long d1 = abs(Amy[x - 1] - Amy[x - 2]);
		if (d1 >= max) {
			max = d1;
		}
		long d2 = abs(Amy[x] - Amy[x - 1]);
		if (d2 >= max) {
			max = d2;
		}
		long d3 = abs(Amy[x + 1] - Amy[x]);
		if (d3 >= max) {
			max = d3;
		}
		long d4 = abs(Amy[x + 2] - Amy[x + 1]);
		if (d4 >= max) {
			max = d4;
		}
		pair<long, long> temp;
		temp.first = x;
		temp.second = max;
		Ydifference.push_back(temp);

		//cout << "X LOCATION:" << x << "\tDISTANCE:" << x - xpre << "\tMAXVALLUE:" << max << endl;
		xpre = x;
	}

	long YMAX = 0;
	for each(auto pa in Ydifference) {
		if (pa.second > YMAX)
			YMAX = pa.second;
	}

	long Ythreshold = YMAX * 15 / 100;
	for each(auto pa in Ydifference) {
		if (pa.second < Ythreshold) {
			Ymaxima.erase(find(Ymaxima.begin(), Ymaxima.end(), pa.first));
		}
	}

	return true;
}

//得到滤波后的一维信号的波峰点位
bool Func_GetMaxima(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
	if (Amx == NULL || Amy == NULL)
		return false;
	for (long y = 2; y < height - 2; ++y) {
		if (Amx[y] >= Amx[y - 1] && Amx[y - 1] >= Amx[y - 2] && Amx[y] > Amx[y + 1] && Amx[y + 1] >= Amx[y + 2])
			//if (Amx[y] >= Amx[y - 1] && Amx[y] > Amx[y + 1])
			Xmaxima.push_back(y);
	}
	//Xmaxima.push_back(height - 1);

	for (long x = 2; x < width - 2; ++x) {
		if (Amy[x] >= Amy[x - 1] && Amy[x - 1] >= Amy[x - 2] && Amy[x] > Amy[x + 1] && Amy[x + 1] >= Amy[x + 2])
			//if (Amy[x] >= Amy[x - 1] && Amy[x] > Amy[x + 1])
			Ymaxima.push_back(x);
	}
	//Ymaxima.push_back(width - 1);
	return true;
}

//求得每个信息点的像素值均值
bool Func_ModuleAvg(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, vector<vector<unsigned char > > &avr, const long width, const long height) {
	if (img == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	long Xpre = 0, Ypre = 0;
	long long sum = 0;
	long count = 0;
	vector<unsigned char > temp;
	for each(long j in Xmaxima) {
		for each(long i in Ymaxima) {
			for (long y = Ypre; y < j; ++y) {
				for (long x = Xpre; x < i; ++x) {
					++count;
					sum += img[y][x];
				}
			}
			Xpre = i + 1;
			if (count != 0) {
				temp.push_back((unsigned char)(sum / count));
				sum = 0;
				count = 0;
			}
			else {
				sum = 0;
				count = 0;
				continue;
			}
		}
		Ypre = j + 1;
		avr.push_back(temp);
		temp.clear();
	}
	return true;
}

//求全图数据区MOD值等级
bool Func_MOD_DataAreaModulation(vector<vector<unsigned char > > R, MOD_DAMIT &DataAreaMODGradeTable, const double GT, const double SC) {
	if (R.size() < 4 || SC == 0)
		return false;
	for (vector<vector<unsigned char > >::iterator p = R.begin() + 2; p != R.end() - 2; ++p) {
		if (p->size() < 4)
			return false;
		for (vector<unsigned char >::iterator pp = p->begin() + 2; pp != p->end() - 2; ++pp) {
			++DataAreaMODGradeTable.sum;
			double temp = 2.00 * (double)abs(*pp - GT) / SC;
			if (temp >= 0.5) {
				pair<double, GradeSymbol> modulation_info;
				modulation_info.first = temp;
				modulation_info.second = GradeSymbol::A;
				DataAreaMODGradeTable.modinfo.push_back(modulation_info);
				++DataAreaMODGradeTable.numofgradeA;
			}
			else if (temp >= 0.4) {
				pair<double, GradeSymbol> modulation_info;
				modulation_info.first = temp;
				modulation_info.second = GradeSymbol::B;
				DataAreaMODGradeTable.modinfo.push_back(modulation_info);
				++DataAreaMODGradeTable.numofgradeB;
			}
			else if (temp >= 0.3) {
				pair<double, GradeSymbol> modulation_info;
				modulation_info.first = temp;
				modulation_info.second = GradeSymbol::C;
				DataAreaMODGradeTable.modinfo.push_back(modulation_info);
				++DataAreaMODGradeTable.numofgradeC;
			}
			else if (temp >= 0.2) {
				pair<double, GradeSymbol> modulation_info;
				modulation_info.first = temp;
				modulation_info.second = GradeSymbol::D;
				DataAreaMODGradeTable.modinfo.push_back(modulation_info);
				++DataAreaMODGradeTable.numofgradeD;
			}
			else {
				pair<double, GradeSymbol> modulation_info;
				modulation_info.first = temp;
				modulation_info.second = GradeSymbol::F;
				DataAreaMODGradeTable.modinfo.push_back(modulation_info);
				++DataAreaMODGradeTable.numofgradeE;
			}
		}

	}

	//mod = min;
	//if (min >= 0.5) {
	//	grade = A;
	//}
	//else if (min >= 0.4) {
	//	grade = B;
	//}
	//else if (min >= 0.3) {
	//	grade = C;
	//}
	//else if (min >= 0.2) {
	//	grade = D;
	//}
	//else {
	//	grade = E;
	//}

	return true;
}

//求AN值
bool Func_AN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade) {
	if (Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	double xavg = (*prev(Ymaxima.end()) - *Ymaxima.begin())*1.0 / (Ymaxima.size() - 1);
	double yavg = (*prev(Xmaxima.end()) - *Xmaxima.begin())*1.0 / (Xmaxima.size() - 1);

	score = abs(xavg - yavg)*2.0 / (xavg + yavg);

	if (score > 0.12) {
		grade = GradeSymbol::F;
	}
	if (score <= 0.12) {
		grade = GradeSymbol::D;
	}
	if (score <= 0.1) {
		grade = GradeSymbol::C;
	}
	if (score <= 0.08) {
		grade = GradeSymbol::B;
	}
	if (score <= 0.06) {
		grade = GradeSymbol::A;
	}

	return true;
}

//求网格偏差GN
bool Func_GN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade) {
	double xavg = (*prev(Ymaxima.end()) - *Ymaxima.begin())*1.0 / (Ymaxima.size() - 1);
	double yavg = (*prev(Xmaxima.end()) - *Xmaxima.begin())*1.0 / (Xmaxima.size() - 1);
	//int xsize = xavg + 0.5, ysize = yavg + 0.5;

	vector<double > Xdistance, Ydistance;
	for (int i = 0; i < (Ymaxima.size() - 1); ++i) {
		//Xideal.push_back(i*xsize);
		Xdistance.push_back(abs((Ymaxima[i] - Ymaxima[0]) - 1.0*i*xavg));
	}
	for (int i = 0; i < (Xmaxima.size() - 1); ++i) {
		//Yideal.push_back(i*ysize);
		Ydistance.push_back(abs((Xmaxima[i] - Xmaxima[0]) - 1.0*i*yavg));
	}

	double xmaxdistance = 0;
	for each(auto p in Xdistance) {
		if (p > xmaxdistance)
			xmaxdistance = p;
	}

	double ymaxdistance = 0;
	for each(auto p in Ydistance) {
		if (p > ymaxdistance)
			ymaxdistance = p;
	}

	score = sqrt((xmaxdistance*xmaxdistance) + (ymaxdistance*ymaxdistance)) / xavg/*sqrt((xavg*xavg) + (yavg*yavg))*/;

	if (score > 0.75) {
		grade = GradeSymbol::F;
	}
	if (score <= 0.75) {
		grade = GradeSymbol::D;
	}
	if (score <= 0.63) {
		grade = GradeSymbol::C;
	}
	if (score <= 0.50) {
		grade = GradeSymbol::B;
	}
	if (score <= 0.38) {
		grade = GradeSymbol::A;
	}

	return true;
}

//L形区域以及反L形区域的各单元像素均值与MOD值计算与FPD各项分值计算
bool Func_FPD(unsigned char** img, FPD_G &FPD_Grades, vector<long > Xmaxima, vector<long > Ymaxima, const double SC, const double GT, const long width, const long height) {
	if (img == NULL || Xmaxima.size() < 2 || Ymaxima.size() < 2)
		return false;
	LSAI info;

	//获得QZL1、QZL2、SA1、SA2 的实际宽度
	if (*Ymaxima.begin() - 0 > Ymaxima[1] - Ymaxima[0])
		info.leftdistance = Ymaxima[1] - Ymaxima[0];
	else
		info.leftdistance = *Ymaxima.begin();

	if (width - *prev(Ymaxima.end()) > *prev(Ymaxima.end()) - *(Ymaxima.end() - 2))
		info.rightdistance = *prev(Ymaxima.end()) - *(Ymaxima.end() - 2);
	else
		info.rightdistance = width - *prev(Ymaxima.end());

	if (*Xmaxima.begin() - 0 > Xmaxima[1] - Xmaxima[0])
		info.topdistance = Xmaxima[1] - Xmaxima[0];
	else
		info.topdistance = *Xmaxima.begin();

	if (height - *prev(Xmaxima.end()) > *prev(Xmaxima.end()) - *(Xmaxima.end() - 2))
		info.downdistance = *prev(Xmaxima.end()) - *(Xmaxima.end() - 2);
	else
		info.downdistance = height - *prev(Xmaxima.end());

	info.L1width = Ymaxima[1] - Ymaxima[0];
	info.L2height = *prev(Xmaxima.end()) - *(Xmaxima.end() - 2);
	info.CT1height = Xmaxima[1] - Xmaxima[0];
	info.CT2width = *prev(Xmaxima.end()) - *(Xmaxima.end() - 2);


	long xpre, ypre, sum, count;
	double mod;

	//QZL1
	ypre = Xmaxima[0] - info.topdistance + 1;
	for each(long ythr in Xmaxima) {
		sum = 0;
		count = 0;
		for (long y = ypre; y < ythr; ++y) {
			for (long x = Ymaxima[0] - info.leftdistance + 1; x < Ymaxima[0]; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		ypre = ythr + 1;
		if (count != 0) {
			mod = sum / count;
			info.QZL1AVG.push_back(mod);
		}
	}
	sum = 0;
	count = 0;
	for (long y = *prev(Xmaxima.end()) + 1; y < *prev(Xmaxima.end()) + info.downdistance; ++y) {
		for (long x = Ymaxima[0] - info.leftdistance + 1; x < Ymaxima[0]; ++x) {
			sum += img[y][x];
			++count;
		}
	}
	if (count != 0) {
		mod = sum / count;
		info.QZL1AVG.push_back(mod);
	}

	//L1
	ypre = Xmaxima[0] + 1;
	for each(long ythr in Xmaxima) {
		if (ythr == Xmaxima[0])
			continue;
		sum = 0;
		count = 0;
		for (long y = ypre; y < ythr; ++y) {
			for (long x = Ymaxima[0] + 1; x < Ymaxima[1]; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		ypre = ythr + 1;
		if (count != 0) {
			mod = sum / count;
			info.L1AVG.push_back(mod);
		}
	}
	//sum = 0;
	//count = 0;
	//for (long y = *prev(Xmaxima.end()) + 1; y < *prev(Xmaxima.end()) + info.downdistance; ++y) {
	//	for (long x = Ymaxima[0] + 1; x < Ymaxima[1]; ++x) {
	//		sum += img[y][x];
	//		++count;
	//	}
	//}
	//if (count != 0) {
	//	mod = sum / count;
	//	info.L1AVG.push_back(mod);
	//}

	//QZL2
	xpre = Ymaxima[0] - info.leftdistance + 1;
	for each(long xthr in Ymaxima) {
		sum = 0;
		count = 0;
		for (long y = *prev(Xmaxima.end()) + 1; y < *prev(Xmaxima.end()) + info.downdistance; ++y) {
			for (long x = xpre; x < xthr; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		xpre = xthr + 1;
		if (count != 0) {
			mod = sum / count;
			info.QZL2AVG.push_back(mod);
		}
	}
	sum = 0;
	count = 0;
	for (long y = *prev(Xmaxima.end()) + 1; y < *prev(Xmaxima.end()) + info.downdistance; ++y) {
		for (long x = *prev(Ymaxima.end()) + 1; x < *prev(Ymaxima.end()) + info.rightdistance; ++x) {
			sum += img[y][x];
			++count;
		}
	}
	if (count != 0) {
		mod = sum / count;
		info.QZL2AVG.push_back(mod);
	}

	//L2
	xpre = Ymaxima[0]/* - info.leftdistance*/ + 1;
	for each(long xthr in Ymaxima) {
		if (xthr == Ymaxima[0])//
			continue;//
		sum = 0;
		count = 0;
		for (long y = *(Xmaxima.end() - 2) + 1; y < *(Xmaxima.end() - 1); ++y) {
			for (long x = xpre; x < xthr; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		xpre = xthr + 1;
		if (count != 0) {
			mod = sum / count;
			info.L2AVG.push_back(mod);
		}
	}

	//SA1
	xpre = Ymaxima[0] + 1;
	for each(long xthr in Ymaxima) {
		if (xthr == Ymaxima[0])
			continue;
		sum = 0;
		count = 0;
		for (long y = Xmaxima[0] - info.topdistance + 1; y < Xmaxima[0]; ++y) {
			for (long x = xpre; x < xthr; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		xpre = xthr + 1;
		if (count != 0) {
			mod = sum / count;
			info.SA1AVG.push_back(mod);
		}
	}

	//CT1
	xpre = Ymaxima[0] + 1;
	for each(long xthr in Ymaxima) {
		if (xthr == Ymaxima[0])
			continue;
		sum = 0;
		count = 0;
		for (long y = Xmaxima[0] + 1; y < Xmaxima[1]; ++y) {
			for (long x = xpre; x < xthr; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		xpre = xthr + 1;
		if (count != 0) {
			mod = sum / count;
			info.CT1AVG.push_back(mod);
		}
	}

	//SA2
	ypre = Xmaxima[0] - info.topdistance + 1;
	for each(long ythr in Xmaxima) {
		sum = 0;
		count = 0;
		for (long y = ypre; y < ythr; ++y) {
			for (long x = *(Ymaxima.end() - 1) + 1; x < *(Ymaxima.end() - 1) + info.rightdistance; ++x) {
				sum += img[y][x];
				++count;
			}
		}
		ypre = ythr + 1;
		if (count != 0) {
			mod = sum / count;
			info.SA2AVG.push_back(mod);
		}
	}

	//CT2
	ypre = Xmaxima[0] + 1;
	for each(long ythr in Xmaxima) {
		sum = 0;
		count = 0;
		for (long y = ypre; y < ythr; ++y) {
			for (long x = *(Ymaxima.end() - 2) + 1; x < *(Ymaxima.end() - 1); ++x) {
				sum += img[y][x];
				++count;
			}
		}
		ypre = ythr + 1;
		if (count != 0) {
			mod = sum / count;
			info.CT2AVG.push_back(mod);
		}
	}

	//计算各module的MOD值并存储
	for each(double var in info.L1AVG) {
		double mod = 2 * (var - GT) / SC;
		info.L1MOD.push_back(mod);
	}
	for each(double var in info.QZL1AVG) {
		double mod = 2 * (var - GT) / SC;
		info.QZL1MOD.push_back(mod);
	}
	for each(double var in info.L2AVG) {
		double mod = 2 * (var - GT) / SC;
		info.L2MOD.push_back(mod);
	}
	for each(double var in info.QZL2AVG) {
		double mod = 2 * (var - GT) / SC;
		info.QZL2MOD.push_back(mod);
	}
	for each(double var in info.SA1AVG) {
		double mod = 2 * (var - GT) / SC;
		info.SA1MOD.push_back(mod);
	}
	for each(double var in info.CT1AVG) {
		double mod = 2 * (var - GT) / SC;
		info.CT1MOD.push_back(mod);
	}
	for each(double var in info.SA2AVG) {
		double mod = 2 * (var - GT) / SC;
		info.SA2MOD.push_back(mod);
	}
	for each(double var in info.CT2AVG) {
		double mod = 2 * (var - GT) / SC;
		info.CT2MOD.push_back(mod);
	}

	{
		cout << "L1AVG:\t";
		for each(double var in info.L1AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "QZL1AVG:\t";
		for each(double var in info.QZL1AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "L2AVG:\t";
		for each(double var in info.L2AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "QZL2AVG:\t";
		for each(double var in info.QZL2AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "SA1AVG:\t";
		for each(double var in info.SA1AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "CT1AVG:\t";
		for each(double var in info.CT1AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "SA2AVG:\t";
		for each(double var in info.SA2AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << "CT2AVG:\t";
		for each(double var in info.CT2AVG) {
			cout << var << " ";
		}
		cout << endl;
		cout << endl;

		cout.precision(2);
		cout << "**L1MOD:\t";
		for each(double var in info.L1MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**QZL1MOD:\t";
		for each(double var in info.QZL1MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**L2MOD:\t";
		for each(double var in info.L2MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**QZL2MOD:\t";
		for each(double var in info.QZL2MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**SA1MOD:\t";
		for each(double var in info.SA1MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**CT1MOD:\t";
		for each(double var in info.CT1MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**SA2MOD:\t";
		for each(double var in info.SA2MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout << "**CT2MOD:\t";
		for each(double var in info.CT2MOD) {
			cout << var << " ";
		}
		cout << endl;
		cout.precision(6);
	}


	//L1
	LSAI_MT L1_modinfotab;//M4
	_Func_FPD_TM4(info.L1MOD, L1_modinfotab, LSAS::L);

	vector<LSAI_SGIT > L1_seggradingtab;//M5
	_Func_FPD_TM5_Init(L1_seggradingtab);
	_Func_FPD_TM5_Edit(L1_modinfotab, L1_seggradingtab, LSAS::L);

	_Func_FPD_TM5_SetLowest_For_LastColumn(L1_seggradingtab, LSAS::L);
	FPD_Grades.L1_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(L1_seggradingtab);

	cout << "L1M5T:" << endl;
	__Func_Display_M5(L1_seggradingtab, LSAS::L);

	//L2
	LSAI_MT L2_modinfotab;//M4
	_Func_FPD_TM4(info.L2MOD, L2_modinfotab, LSAS::L);

	vector<LSAI_SGIT > L2_seggradingtab;//M5
	_Func_FPD_TM5_Init(L2_seggradingtab);
	_Func_FPD_TM5_Edit(L2_modinfotab, L2_seggradingtab, LSAS::L);

	_Func_FPD_TM5_SetLowest_For_LastColumn(L2_seggradingtab, LSAS::L);
	FPD_Grades.L2_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(L2_seggradingtab);

	cout << "L2M5T:" << endl;
	__Func_Display_M5(L2_seggradingtab, LSAS::L);

	//QZL1
	LSAI_MT QZL1_modinfotab;//M4
	_Func_FPD_TM4(info.QZL1MOD, QZL1_modinfotab, LSAS::QZL);

	vector<LSAI_SGIT > QZL1_seggradingtab;//M5
	_Func_FPD_TM5_Init(QZL1_seggradingtab);
	_Func_FPD_TM5_Edit(QZL1_modinfotab, QZL1_seggradingtab, LSAS::QZL);

	_Func_FPD_TM5_SetLowest_For_LastColumn(QZL1_seggradingtab, LSAS::QZL);
	FPD_Grades.QZL1_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(QZL1_seggradingtab);

	cout << "QZL1M5T:" << endl;
	__Func_Display_M5(QZL1_seggradingtab, LSAS::QZL);

	//QZL2
	LSAI_MT QZL2_modinfotab;//M4
	_Func_FPD_TM4(info.QZL2MOD, QZL2_modinfotab, LSAS::QZL);

	vector<LSAI_SGIT > QZL2_seggradingtab;//M5
	_Func_FPD_TM5_Init(QZL2_seggradingtab);
	_Func_FPD_TM5_Edit(QZL2_modinfotab, QZL2_seggradingtab, LSAS::QZL);

	_Func_FPD_TM5_SetLowest_For_LastColumn(QZL2_seggradingtab, LSAS::QZL);
	FPD_Grades.QZL2_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(QZL2_seggradingtab);

	cout << "QZL2M5T:" << endl;
	__Func_Display_M5(QZL2_seggradingtab, LSAS::QZL);

	//CTSA1
	LSAI_MT CTSA1_modinfotab;//M4
	_Func_FPD_TM4(info.CT1MOD, CTSA1_modinfotab, LSAS::CT1);
	_Func_FPD_TM4(info.SA1MOD, CTSA1_modinfotab, LSAS::SA);

	vector<LSAI_SGIT > CTSA1_seggradingtab;//M5
	_Func_FPD_TM5_Init(CTSA1_seggradingtab);
	_Func_FPD_TM5_Edit(CTSA1_modinfotab, CTSA1_seggradingtab, LSAS::CT1);

	LSAI_MT CT1_modinfotab;
	_Func_FPD_TM4(info.CT1MOD, CT1_modinfotab, LSAS::CT1);
	LSAI_MT SA1_modinfotab;
	_Func_FPD_TM4(info.SA1MOD, SA1_modinfotab, LSAS::SA);

	_Func_FPD_CTRegularityTest(CT1_modinfotab, CTSA1_seggradingtab);
	_Func_FPD_CTDamageTest(CT1_modinfotab, CTSA1_seggradingtab);
	_Func_FPD_SAFixedPatternTest(SA1_modinfotab, CTSA1_seggradingtab);

	_Func_FPD_TM5_SetLowest_For_LastColumn(CTSA1_seggradingtab, LSAS::CT1);
	FPD_Grades.CTSA_segs_grades.begin()->notional_damage_test_grades = _Func_Fpd_TM5_GetHighest_Of_LastColumn(CTSA1_seggradingtab);
	FPD_Grades.CTSA_segs_grades.begin()->transition_ratio_test_grade = _Func_FPD_TransitionRatioTest(info.SA1MOD, info.CT1MOD);

	cout << "CT1M5T:" << endl;
	__Func_Display_M5(CTSA1_seggradingtab, LSAS::CT1);

	cout << endl;

	//CTSA2
	LSAI_MT CTSA2_modinfotab;//M4
	_Func_FPD_TM4(info.CT2MOD, CTSA2_modinfotab, LSAS::CT2);
	_Func_FPD_TM4(info.SA2MOD, CTSA2_modinfotab, LSAS::SA);



	vector<LSAI_SGIT > CTSA2_seggradingtab;//M5
	_Func_FPD_TM5_Init(CTSA2_seggradingtab);
	_Func_FPD_TM5_Edit(CTSA2_modinfotab, CTSA2_seggradingtab, LSAS::CT2);

	LSAI_MT CT2_modinfotab;
	_Func_FPD_TM4(info.CT2MOD, CT2_modinfotab, LSAS::CT2);
	LSAI_MT SA2_modinfotab;
	_Func_FPD_TM4(info.SA2MOD, SA2_modinfotab, LSAS::SA);

	_Func_FPD_CTRegularityTest(CT2_modinfotab, CTSA2_seggradingtab);
	_Func_FPD_CTDamageTest(CT2_modinfotab, CTSA2_seggradingtab);
	_Func_FPD_SAFixedPatternTest(SA2_modinfotab, CTSA2_seggradingtab);

	_Func_FPD_TM5_SetLowest_For_LastColumn(CTSA2_seggradingtab, LSAS::CT2);
	(FPD_Grades.CTSA_segs_grades.begin() + 1)->notional_damage_test_grades = _Func_Fpd_TM5_GetHighest_Of_LastColumn(CTSA2_seggradingtab);
	(FPD_Grades.CTSA_segs_grades.begin() + 1)->transition_ratio_test_grade = _Func_FPD_TransitionRatioTest(info.SA2MOD, info.CT2MOD);

	cout << "CT2M5T:" << endl;
	__Func_Display_M5(CTSA2_seggradingtab, LSAS::CT2);

}

//16022 P111 M.1.4 Calculation and grading of average grade(计算L1 L2 QZL1 QZL2 （以及两个Segment区域的最小值）五项的均值再取最小)
bool Func_CalculatePFDAverageGrade(FPD_G FPD_grades, Grade &grade) {
	//unsigned int FPD = FPD_grades. < FPD_grades[1] ? FPD_grades[0] : FPD_grades[1];
	unsigned int CTSA_FPD_1 =
		FPD_grades.CTSA_segs_grades[0].notional_damage_test_grades < FPD_grades.CTSA_segs_grades[0].transition_ratio_test_grade ?
		FPD_grades.CTSA_segs_grades[0].notional_damage_test_grades : FPD_grades.CTSA_segs_grades[0].transition_ratio_test_grade;
	unsigned int CTSA_FPD_2 =
		FPD_grades.CTSA_segs_grades[1].notional_damage_test_grades < FPD_grades.CTSA_segs_grades[1].transition_ratio_test_grade ?
		FPD_grades.CTSA_segs_grades[1].notional_damage_test_grades : FPD_grades.CTSA_segs_grades[1].transition_ratio_test_grade;
	unsigned int FPD = CTSA_FPD_1 < CTSA_FPD_2 ? CTSA_FPD_1 : CTSA_FPD_2;
	double AG = (FPD_grades.L1_grade + FPD_grades.L2_grade + FPD_grades.QZL1_grade + FPD_grades.QZL2_grade + FPD)*1.0 / 5.0;
	double min = AG;
	if (FPD_grades.L1_grade < min)
		min = FPD_grades.L1_grade;
	if (FPD_grades.L2_grade < min)
		min = FPD_grades.L2_grade;
	if (FPD_grades.QZL1_grade < min)
		min = FPD_grades.QZL1_grade;
	if (FPD_grades.QZL2_grade < min)
		min = FPD_grades.QZL2_grade;
	if (FPD < min)
		min = FPD;
	if ((unsigned int)min == 4)
		grade.FPD = GradeSymbol::A;
	else if ((unsigned int)min == 3)
		grade.FPD = GradeSymbol::B;
	else if ((unsigned int)min == 2)
		grade.FPD = GradeSymbol::C;
	else if ((unsigned int)min == 1)
		grade.FPD = GradeSymbol::D;
	else
		grade.FPD = GradeSymbol::F;

	return true;
}

//-----功能入口-----
bool Func(CImg* pImg, const unsigned int error_correction_capacity, Grade &grade) {
	if (pImg == NULL)
		return false;
	if (error_correction_capacity == 0)
		return false;

	long width;
	long height;

	width = pImg->GetWidthPixel();
	height = pImg->GetHeight();

	//new

	unsigned char** source = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		source[i] = new unsigned char[width];
	}

	long** gx = new long*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gx[i] = new long[width];
	}

	long** gy = new long*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gy[i] = new long[width];
	}

	double** gradient = new double*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gradient[i] = new double[width];
	}

	long long *mappedx = new long long[height];
	long long *mappedy = new long long[width];
	memset(mappedx, 0, sizeof(long long)*height);
	memset(mappedy, 0, sizeof(long long)*width);

	long long *aftermx = new long long[height];
	long long *aftermy = new long long[width];
	memset(aftermx, 0, sizeof(long long)*height);
	memset(aftermy, 0, sizeof(long long)*width);

	long long *remx = new long long[height];
	long long *remy = new long long[width];
	memset(remx, 0, sizeof(long long)*height);
	memset(remy, 0, sizeof(long long)*width);

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//action
	double GT;

	bool rt = true;
	rt = Func_SC(source, width, height, grade.SC_Score, GT, grade.SC);
	if (rt == false)
		return false;

	rt = Func_Sobel(source, gx, gy, width, height);
	if (rt == false)
		return false;

	rt = Func_Gradient(gx, gy, gradient, width, height);
	if (rt == false)
		return false;

	rt = Func_Mapping(gradient, mappedx, mappedy, width, height);
	if (rt == false)
		return false;

#ifdef COUT_MAPPING
	cout << endl;
	for (unsigned long x = 0; x < width; ++x) {
		cout << mappedy[x] << " ";
	}

	cout << "\n---------------------------------" << endl;


	cout << endl;
	for (unsigned long y = 0; y < height; ++y) {
		cout << mappedx[y] << " ";
	}
	cout << endl;

	cout << "\n++++++++++++++++++++++++++++++++" << endl;

#endif

	const int model[7] = { 5,10,45,50,45,10,5 };
	//const int model[7] = { 0,0,0,100,0,0,0 };
	const int M = 100;
	rt = Func_Gauss(mappedx, mappedy, aftermx, aftermy, width, height, model, M);

#ifdef COUT_MAPPING
	cout << endl;
	for (unsigned long x = 0; x < width; ++x) {
		cout << aftermy[x] << " ";
	}

	cout << "\n---------------------------------" << endl;


	cout << endl;
	for (unsigned long y = 0; y < height; ++y) {
		cout << aftermx[y] << " ";
	}
	cout << endl;

	cout << "\n++++++++++++++++++++++++++++++++" << endl;

#endif

	if (rt == false)
		return false;
	vector<long > Xmaxima, Ymaxima;
	rt = Func_GetMaxima(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;
	rt = Func_CompareHill(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;

#ifdef _DEBUG
	rt = __Func_DrawLine(source, Xmaxima, Ymaxima, width, height, 0xDD);
	if (rt == false)
		return false;
#endif

	vector<vector<unsigned char > > avr;
	rt = Func_ModuleAvg(source, Xmaxima, Ymaxima, avr, width, height);
	if (rt == false)
		return false;

	MOD_DAMIT DataAreaMODInfo;
	vector<MOD_GIT> MODGradingTable;
	rt = Func_MOD_DataAreaModulation(avr, DataAreaMODInfo, GT, grade.SC_Score);
	if (rt != false) {
		grade.MOD = _Func_MOD_T7A(DataAreaMODInfo, MODGradingTable, error_correction_capacity);
#ifdef _DEBUG
		//__Func_Display_T7A(MODGradingTable);
#endif
	}
	else {
		grade.MOD = GradeSymbol::F;
	}

	rt = Func_AN(Xmaxima, Ymaxima, grade.AN_Score, grade.AN);
	if (rt == false)
		return false;

	rt = Func_GN(Xmaxima, Ymaxima, grade.GN_Score, grade.GN);
	if (rt == false)
		return false;

	FPD_G FPD_Grades;
	rt = Func_FPD(source, FPD_Grades, Xmaxima, Ymaxima, grade.SC_Score, GT, width, height);
	if (rt == false)
		return false;


	Func_CalculatePFDAverageGrade(FPD_Grades, grade);


	//delete
	delete mappedx;
	mappedx = NULL;
	delete mappedy;
	mappedy = NULL;
	delete aftermx;
	aftermx = NULL;
	delete aftermy;
	aftermy = NULL;


	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gx[i];
	}
	delete[] gx;
	gx = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gy[i];
	}
	delete[] gy;
	gy = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gradient[i];
	}
	delete[] gradient;
	gradient = NULL;


	return true;
}

//-----程序入口-----
int main() {
	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//2-1-0.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//2-1-0.bmp");
	//BOOL rt = pImg->AttachFromFile("..//imgs//DST - 160814_192213_0000.bmp");
	if (!rt)
		return -1;
	Grade Gr;

	bool rt1 = true;
	rt1 = Func(pImg, 30, Gr);

	cout << "\nSC_Score = " << Gr.SC_Score << "\tSC_Grade = " << Gr.SC << endl;
	cout << "MOD_Grade = " << Gr.MOD << endl;
	cout << "AN_Score = " << Gr.AN_Score << "\tAN_Grade = " << Gr.AN << endl;
	cout << "GN_Score = " << Gr.GN_Score << "\tGN_Grade = " << Gr.GN << endl;
	cout << "FPD = " << Gr.FPD << endl;

	getchar();
	return 0;
}
