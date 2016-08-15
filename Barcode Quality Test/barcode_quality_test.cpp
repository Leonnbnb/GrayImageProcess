#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

#ifdef _DEBUG
//filename
#include <ctime>
#include <sstream>
#include <iomanip>
#endif

//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

//评级等级符号及其对应分数
typedef enum GradeSymbol {
	A = 4,
	B = 3,
	C = 2,
	D = 1,
	E = 0
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
	Grade() {
		Rmax = 0;
		Rmin = 0;
		SC = 0;
		ECmin = 0;
		ERNmax = 0;
		Defects = 0;
		MOD = 0;
		Decodability = 0;
		SC_Grade = GradeSymbol::E;
		MOD_Grade = GradeSymbol::E;
		Defects_Grade = GradeSymbol::E;
		Rmin_Grade = GradeSymbol::E;
		ECmin_Grade = GradeSymbol::E;
		Decodability_Grade = GradeSymbol::E;
	}

}Grade;

//条码类型
typedef enum BarcodeType {
	EAN_13 = 0,
	EAN_8 = 1,
	UPC_A = 2,
	UPC_E = 3,
	ITF_14 = 4,
	EAN_128 = 5
}BarcodeType;

//条码各字符的信息
typedef class BarcodeCharacterInfo final {
public:
	long p;
	double RT[7];
	long b1;
	long b2;
	long b3;
	long e1;
	long e2;
	long e3;
	long e4;

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

	void setValue(unsigned long loc1, unsigned long loc2, unsigned long loc3, unsigned long loc4, unsigned long loc5, unsigned long loc6, unsigned long loc7) {
		p = loc7 - loc1;
		b1 = loc2 - loc1;
		b2 = loc4 - loc3;
		b3 = loc6 - loc5;
		e1 = loc3 - loc1;
		e2 = loc4 - loc2;
		e3 = loc5 - loc3;
		e4 = loc6 - loc4;
		p = abs(p);
		b1 = abs(b1);
		b2 = abs(b2);
		b3 = abs(b3);
		e1 = abs(e1);
		e2 = abs(e2);
		e3 = abs(e3);
		e4 = abs(e4);
		_setRT();

	}

	double calcV1() {
		double E1V1T[7];
		double E2V1T[7];
		double E3V1T[7];
		double E4V1T[7];
		double p22 = p*1.0 / 22.0;
		double min1 = FP_INFINITE, min2 = FP_INFINITE, min3 = FP_INFINITE, min4 = FP_INFINITE;
		for (unsigned int i = 0; i < 7; ++i) {
			E1V1T[i] = abs(e1 - RT[i])*1.0 / p22;
			if (E1V1T[i] <= min1) {
				min1 = E1V1T[i];
			}
			E2V1T[i] = abs(e2 - RT[i])*1.0 / p22;
			if (E2V1T[i] <= min2) {
				min2 = E2V1T[i];
			}
			E3V1T[i] = abs(e3 - RT[i])*1.0 / p22;
			if (E3V1T[i] <= min3) {
				min3 = E3V1T[i];
			}
			E4V1T[i] = abs(e4 - RT[i])*1.0 / p22;
			if (E4V1T[i] <= min4) {
				min4 = E4V1T[i];
			}
		}

		double ret;
		if (min1 < min2)
			ret = min1;
		else
			ret = min2;
		if (ret > min3)
			ret = min3;
		if (ret > min4)
			ret = min4;

		//cout << "V1:" << ret << endl;
		return ret;
	}

	//double calcV2() {
	//	double v2 = (1.75 - abs(((b1 + b2 + b3)*(11.0 / p)) - 3)) / 1.75;
	//	//cout << "V2:" << v2 << endl;
	//	return v2;
	//}

private:
	bool _setRT() {
		if (p == 0) return false;
		RT[0] = (1.5*p) / 11.0;
		RT[1] = (2.5*p) / 11.0;
		RT[2] = (3.5*p) / 11.0;
		RT[3] = (4.5*p) / 11.0;
		RT[4] = (5.5*p) / 11.0;
		RT[5] = (6.5*p) / 11.0;
		RT[6] = (7.5*p) / 11.0;
	}
}BChInfo;

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
bool __Func_DrawLine(unsigned char** img, unsigned int topmost, unsigned int downmost, const long width, const long height, const unsigned char color) {
	if (img == NULL || downmost <= topmost || downmost - topmost < 2)
		return false;

	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(preview[i], img[i], width * sizeof(unsigned char));
	}

	for (long i = 0; i < width; ++i) {
		preview[topmost][i] = (unsigned char)color;
		preview[downmost][i] = (unsigned char)color;
	}

	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	string filepath = NowTimeToFileName("..//results//bqt//DrawLineToArea", ".bmp");
	pPreviewImg->SaveToFile(filepath.c_str());

	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

	return true;
}

//根据码带距离重新绘制条码（仅供调试输出中间结果）
bool __Func_DrawLine(unsigned char** img, unsigned int topmost, unsigned int downmost, vector<unsigned long > loc, const long width, const long height, const unsigned char color) {
	if (img == NULL || downmost <= topmost || downmost - topmost < 2)
		return false;

	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(preview[i], img[i], width * sizeof(unsigned char));
	}

	for (long i = 0; i < width; ++i) {
		preview[topmost][i] = (unsigned char)color;
		preview[downmost][i] = (unsigned char)color;
	}

	loc.push_back(width - 1);
	int flag = 1;
	unsigned long xprev = 0;
	for each(unsigned long x in loc) {
		if (x < width) {
			for (unsigned long y = topmost; y <= downmost; ++y) {
				preview[y][x] = (unsigned char)color;

				_ASSERTE(_CrtCheckMemory());
			}
		}
		if (flag > 0) {
			if (x < width) {
				for (unsigned long i = xprev; i <= x; ++i) {
					for (unsigned long y = topmost; y <= downmost; ++y) {
						preview[y][i] = (unsigned char)color;
						_ASSERTE(_CrtCheckMemory());
					}
				}
			}
		}
		else {
			if (x < width) {
				for (unsigned long i = xprev; i <= x; ++i) {
					for (unsigned long y = topmost; y <= downmost; ++y) {
						preview[y][i] = 0x22;
						_ASSERTE(_CrtCheckMemory());
					}
				}
			}
		}
		xprev = x;
		flag *= -1;
	}

	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	string filepath = NowTimeToFileName("..//results//bqt//DrawLineOfGrid", ".bmp");
	pPreviewImg->SaveToFile(filepath.c_str());

	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

	return true;
}
#endif

//将图片沿X方向映射，累加灰度值
bool Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height) {
	if (img == NULL) return false;
	if (width == 0 || height == 0)	return true;
	for (long j = 0; j < height; ++j) {
		long long sum = 0;
		for (long i = 0; i < width; ++i) {
			sum += img[j][i];
		}
		MX.push_back(sum);
	}
	return true;
}

//将映射后的信号差分
bool Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX) {
	if (MX.size() <= 1) return false;
	long long prev = *MX.begin();
	long long max = 0;
	for (vector<long long >::iterator p = MX.begin(); p != MX.end(); ++p) {
		if (p == MX.begin())
			continue;
		AMX.push_back(abs(*p - prev));
		if (abs(*p - prev) > max)
			max = abs(*p - prev);
		prev = *p;
	}
	MaxOfAMX = max;

	return true;
}

//将小峰抹平
bool Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M) {
	if (AMX.size() <= 1 || MaxOfAMX == 0 || M == 0)	return false;
	//低于门限值的数据以-1填充，方便后面的找波峰的操作
	for (vector<long long>::iterator p = AMX.begin(); p < AMX.end(); ++p) {
		if (*p < (MaxOfAMX*M)) {
			*p = -1;
		}
	}
	if (*AMX.begin() < 0)
		*AMX.begin() = 1;
	return true;
}

//找到仍然存在的波峰
bool Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc) {
	if (AMX.size() <= 1) return false;

	//防止条码没有上边界
	if (*AMX.begin() > *(AMX.begin() + 1))
		loc.push_back(0);

	for (vector<long long>::iterator p = AMX.begin() + 1; p < AMX.end() - 1; ++p) {
		if (*p > 0 && *p >= *(p - 1) && *p >= *(p + 1)) {
			loc.push_back((unsigned int)(p - AMX.begin()));
		}
	}
	//防止条码没有下边界
	loc.push_back((unsigned int)((AMX.end() - 1) - AMX.begin()));
	return true;
}

//找到距离最远的两个峰顶
bool Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right) {
	if (loc.size() <= 1) return false;
	unsigned int distance = 0;
	for (vector<unsigned int >::iterator p = loc.begin() + 1; p < loc.end(); ++p) {
		//两峰之间的距离
		if (*p - *(p - 1) > distance) {
			left = *(p - 1) + 5;
			right = *p - 5;
			distance = *p - *(p - 1);
		}
	}
	return true;
}

//得到灰度最大值最小值以及求得GT 
bool Func_GetGT(unsigned char** img, unsigned char &maxgray, unsigned char &mingray, double &SC, double &GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height) {
	if (img == NULL || width == 0 || height == 0 || topmost<0 || downmost>height) return false;

	//指定区域的灰度直方图
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));

	//指定区域像素个数
	long long pixels = 0;
	//使用指定区域初始化直方图
	for (long i = topmost; i < downmost; ++i) {
		for (long j = 0; j < width; ++j) {
			++tab[(unsigned int)img[i][j]];
			++pixels;
		}
	}

	//前百分之十的像素
	unsigned long long limit = 0.1*pixels;

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
	//前百分之十的最小灰度值的像素均值
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
	//前百分之十的最大灰度值的像素均值
	double avggreater = (double)sum / count;

	SC = avggreater - avglower;
	GT = (avglower + avggreater) / 2;
	maxgray = avggreater;
	mingray = avglower;

	return true;
}

//获得指定行号的像素值
bool _Func_GetSelectedRowPixels(unsigned char** img, vector<unsigned char> &rowSet, const unsigned char rowNum, const long width) {
	if (img == NULL) return false;
	if (width == 0)	return false;
	for (unsigned long x = 0; x < width; ++x) {
		rowSet.push_back(img[rowNum][x]);
	}
	return true;
}

//将分割线经过的一维信号以GT为界限划分
bool _Func_Split(vector<unsigned char > &rowSet, vector<unsigned long > &locSet, double GT, vector<vector<unsigned char > > &sets) {
	if (rowSet.size() == 0)	return false;
	unsigned long loc = 0;
	//掐头:)
	if (*rowSet.begin() >= GT) {
		vector<unsigned char >::iterator pfront = rowSet.begin();
		while (pfront < rowSet.end() && *pfront >= GT)
			++pfront;
		loc = pfront - rowSet.begin();
		rowSet.erase(rowSet.begin(), pfront);
	}
	locSet.push_back(loc);

	//如果为纯色图片，上一步操作会将所有元素清除
	if (rowSet.size() == 0)
		return false;
	//去尾:)
	if (*prev(rowSet.end()) >= GT) {
		vector<unsigned char >::iterator prear = prev(rowSet.end());
		while (prear > rowSet.begin() && *prear > GT)
			--prear;
		rowSet.erase(prear, rowSet.end());
	}
	//For safe
	if (rowSet.size() == 0)
		return false;
	//切割成段
	vector<unsigned char >::iterator p = rowSet.begin();
	vector<unsigned char > up, down;

	while (p < rowSet.end()) {
		if (p >= rowSet.end())
			break;
		//条区域（黑色）
		while (p < rowSet.end() && *p <= GT) {
			down.push_back(*p);
			++loc;
			if (p < rowSet.end())
				++p;
			else
				break;
		}
		if (down.size() != 0) {
			sets.push_back(down);
			locSet.push_back(loc);
			down.clear();
		}
		//空白区域（白色）
		while (p < rowSet.end() && *p > GT) {
			up.push_back(*p);
			++loc;
			if (p < rowSet.end())
				++p;
			else
				break;
		}
		if (up.size() != 0) {
			sets.push_back(up);
			locSet.push_back(loc);
			up.clear();
		}
	}
}

//根据分段后的数据获得最小EC值和最大ERN值
bool _Func_ECminERNmax(vector<vector<vector<unsigned char > > > dataSet, double GT, Grade &grade) {
	if (dataSet.size() == 0) return false;
	unsigned char ECmin = 255, ERNmax = 0;
	//pair第一个元素是最小值，第二个元素是最大值
	for each(auto rowdata in dataSet) {
		//每一条扫描线上的每一段中的最小最大灰度值的集合
		vector<pair<unsigned char, unsigned char > > minmaxSet;
		vector<vector<unsigned char > > peakAndvalley;
		for each(auto seg in rowdata) {
			unsigned char max = 0, min = 255;
			bool flagMIN = false, flagMAX = false;
			vector<unsigned char > segPeakAndvalley;
			for (vector<unsigned char >::iterator ppix = seg.begin() + 1; ppix < seg.end() - 1; ++ppix) {
				if (*ppix >= *(ppix + 1) && *ppix >= *(ppix - 1) && *ppix > max) {
					max = *ppix;
					flagMAX = true;
					segPeakAndvalley.push_back(*ppix);
				}
				if (*ppix <= *(ppix + 1) && *ppix <= *(ppix - 1) && *ppix < min) {
					min = *ppix;
					flagMIN = true;
					segPeakAndvalley.push_back(*ppix);
				}
			}
			peakAndvalley.push_back(segPeakAndvalley);

			if (flagMAX&&flagMIN) {
				//段上的最小最大灰度值
				pair<unsigned char, unsigned char > minmaxofSeg;
				minmaxofSeg.first = min;
				minmaxofSeg.second = max;
				minmaxSet.push_back(minmaxofSeg);
			}
			else {
				if (flagMAX) {
					pair<unsigned char, unsigned char > minmaxofSeg;
					minmaxofSeg.first = max;
					minmaxofSeg.second = max;
					minmaxSet.push_back(minmaxofSeg);
				}
				else if (flagMIN) {
					pair<unsigned char, unsigned char > minmaxofSeg;
					minmaxofSeg.first = min;
					minmaxofSeg.second = min;
					minmaxSet.push_back(minmaxofSeg);
				}
			}
		}
		//记录下每条扫描线上的EC
		vector<unsigned char > ECset;

		if (peakAndvalley.size() != 0) {
			for (vector<vector<unsigned char > >::iterator p = peakAndvalley.begin() + 1; p < peakAndvalley.end(); ++p) {
				if (p->size() == 0) {
					ECset.push_back(abs(*((p - 1)->end() - 1) - GT));
					++p;
					ECset.push_back(abs(GT - *(p->begin())));
				}
				else {
					ECset.push_back(abs(*((p - 1)->end() - 1) - *(p->begin())));
				}
			}
		}

		//记录下每条扫描线上的ERN
		vector<unsigned char > ERNset;
		if (minmaxSet.size() <= 1)
			break;
		ERNset.push_back(abs(minmaxSet.begin()->second - minmaxSet.begin()->first));
		for (auto p = minmaxSet.begin() + 1; p < minmaxSet.end(); ++p) {
			unsigned char EC;
			unsigned char ERN;
			////找到相邻的SEGMENT中垂直距离最远的值
			//EC = abs((p - 1)->first - p->second) > abs((p - 1)->second - p->first) ?
			//	abs((p - 1)->first - p->second) : abs((p - 1)->second - p->first);
			//ECset.push_back(EC);

			//将每一个SEGMENT中的最大最小值的差值记录下来
			ERNset.push_back(abs(p->second - p->first));
		}
		//找到ECset和ERNset中的EC最小值和ERN最大值
		for each(auto var in ECset) {
			if (var <= ECmin) {
				ECmin = var;
			}
		}
		for each(auto var in ERNset) {
			if (var >= ERNmax) {
				ERNmax = var;
			}
		}
	}
	grade.ERNmax = ERNmax;
	grade.ECmin = ECmin;
	return true;
}

//根据topmost和downmost两条界限确定的条码范围，进行分割，建立多条扫描线进行横向扫描
bool Func_Scan(unsigned char** img, vector<vector<unsigned long > > &locDataSet, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, Grade &grade) {
	if (img == NULL) return false;
	if (width == 0 || height == 0) return false;
	if (topmost<0 || downmost>height) return false;
	//所有扫描线的像素数据集合
	vector<vector<vector<unsigned char > > > pixDataSet;
	//扫描线之间的距离
	double distance = (downmost - topmost)*1.0 / 10.0;
	for (long y = topmost; y <= downmost; y += distance) {
		//指定扫描线的像素灰度值
		vector<unsigned char> rowPixels;
		_Func_GetSelectedRowPixels(img, rowPixels, y, width);
		//将当前扫描线的像素灰度值分段
		vector<vector<unsigned char > > peakAndvalley;
		vector<unsigned long > locSet;
		_Func_Split(rowPixels, locSet, GT, peakAndvalley);
		pixDataSet.push_back(peakAndvalley);
		locDataSet.push_back(locSet);
	}
	_Func_ECminERNmax(pixDataSet, GT, grade);
	grade.MOD = grade.ECmin*1.0 / grade.SC*1.0;
	return true;
}

//根据各项数据评级
bool Func_Grading(Grade &grade) {
	//Rmin Grade
	if (grade.Rmin <= 0.5*grade.Rmax)
		grade.Rmin_Grade = GradeSymbol::A;
	else
		grade.Rmin_Grade = GradeSymbol::E;

	//SC Grade
	double SC_rate = grade.SC / 255.0/*grade.Rmax*/;
	if (SC_rate >= 0.7) {
		grade.SC_Grade = GradeSymbol::A;
	}
	else if (SC_rate >= 0.55) {
		grade.SC_Grade = GradeSymbol::B;
	}
	else if (SC_rate >= 0.4) {
		grade.SC_Grade = GradeSymbol::C;
	}
	else if (SC_rate >= 0.2) {
		grade.SC_Grade = GradeSymbol::D;
	}
	else {
		grade.SC_Grade = GradeSymbol::E;
	}

	//ECmin Grade
	double ECmin_rate = grade.ECmin*1.0 / 255.0/*grade.SC*1.0*/;//
	if (ECmin_rate >= 0.15)
		grade.ECmin_Grade = GradeSymbol::A;
	else
		grade.ECmin_Grade = GradeSymbol::E;

	//MOD Grade
	grade.MOD = grade.ECmin*1.0 / grade.SC*1.0;
	if (grade.MOD >= 0.7) {
		grade.MOD_Grade = GradeSymbol::A;
	}
	else if (grade.MOD >= 0.6) {
		grade.MOD_Grade = GradeSymbol::B;
	}
	else if (grade.MOD >= 0.5) {
		grade.MOD_Grade = GradeSymbol::C;
	}
	else if (grade.MOD >= 0.4) {
		grade.MOD_Grade = GradeSymbol::D;
	}
	else {
		grade.MOD_Grade = GradeSymbol::E;
	}

	//ERNmax Grade
	grade.Defects = grade.ERNmax*1.0 / grade.SC*1.0;
	if (grade.Defects <= 0.15) {
		grade.Defects_Grade = GradeSymbol::A;
	}
	else if (grade.Defects <= 0.2) {
		grade.Defects_Grade = GradeSymbol::B;
	}
	else if (grade.Defects <= 0.25) {
		grade.Defects_Grade = GradeSymbol::C;
	}
	else if (grade.Defects <= 0.3) {
		grade.Defects_Grade = GradeSymbol::D;
	}
	else {
		grade.Defects_Grade = GradeSymbol::E;
	}

	return true;
}

//EAN128
bool _Func_Decode_EAN128(vector<vector<unsigned long > > locDataSet, Grade &grade) {
	if (locDataSet.size() == 0) return false;
	unsigned int lineNum = locDataSet.size();
	vector<double > rowScoreSet(lineNum);
	vector<vector<BChInfo > > characterInfoSet(lineNum);
	for (unsigned int ln = 0; ln < lineNum; ++ln) {
		if (locDataSet.at(ln).size() % 6 != 2) {
			rowScoreSet.at(ln) = 0;
			continue;
		}
		else {
			vector<double > row;
			for (vector<unsigned long >::iterator p = locDataSet.at(ln).begin(); p < locDataSet.at(ln).end(); p += 6) {

				if (p < locDataSet.at(ln).end() - 6) {
					BChInfo ChI;
					ChI.setValue(*p, *(p + 1), *(p + 2), *(p + 3), *(p + 4), *(p + 5), *(p + 6));
					characterInfoSet.at(ln).push_back(ChI);
					row.push_back(ChI.calcV1());
				}
				else {
					//rowScoreSet.at(ln) = 0;
					break;
				}

			}

			if (locDataSet.at(ln).end() - 7 > locDataSet.at(ln).begin()) {
				BChInfo ChI;
				ChI.setValue(*(locDataSet.at(ln).end() - 1),
					*(locDataSet.at(ln).end() - 2),
					*(locDataSet.at(ln).end() - 3),
					*(locDataSet.at(ln).end() - 4),
					*(locDataSet.at(ln).end() - 5),
					*(locDataSet.at(ln).end() - 6),
					*(locDataSet.at(ln).end() - 7));
				characterInfoSet.at(ln).push_back(ChI);
				double v1 = ChI.calcV1();
				//double v2 = ChI.calcV2();
				if (rowScoreSet.at(ln) >= v1)
					rowScoreSet.at(ln) = v1;
			}
			else {
				rowScoreSet.at(ln) = 0;
			}


			double min = FP_INFINITE;
			for each(auto var in row) {
				if (var <= min)
					min = var;
			}

			rowScoreSet.at(ln) = min;
		}
	}

	double min = FP_INFINITE;
	for each(auto var in rowScoreSet) {
		if (var <= min)
			min = var;
	}
	grade.Decodability = min;
	if (grade.Decodability < 0.25) {
		grade.Decodability_Grade = GradeSymbol::E;
	}
	else if (grade.Decodability < 0.37) {
		grade.Decodability_Grade = GradeSymbol::D;
	}
	else if (grade.Decodability < 0.50) {
		grade.Decodability_Grade = GradeSymbol::C;
	}
	else if (grade.Decodability < 0.62) {
		grade.Decodability_Grade = GradeSymbol::B;
	}
	else {
		grade.Decodability_Grade = GradeSymbol::A;
	}

	return true;
}

//计算可译码度
bool Func_Decodability(vector<vector<unsigned long > > locDataSet, BarcodeType type, Grade &grade) {
	if (locDataSet.size() == 0) return false;
	switch (type) {
	case BarcodeType::EAN_8: {
		break;
	}
	case BarcodeType::EAN_13: {
		break;
	}
	case BarcodeType::EAN_128: {
		_Func_Decode_EAN128(locDataSet, grade);
		break;
	}
	case BarcodeType::UPC_A: {
		break;
	}
	case BarcodeType::UPC_E: {
		break;
	}
	case BarcodeType::ITF_14: {
		break;
	}
	default:
		return false;
	}
}

//-----功能入口-----
bool Func(CImg* pImg, Grade &grade) {
	if (pImg == NULL)
		return false;

	long width, height;//图片的尺寸
	width = pImg->GetWidthPixel();
	height = pImg->GetHeight();

	//new
	unsigned char** source = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		source[i] = new unsigned char[width];
	}

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//action
	vector<long long > MX;//将图像灰度值横向映射累加
	Func_Mapping(source, MX, width, height);

	//for each (auto var in MX) {
	//	cout << var << " ";
	//}
	//cout << "\n+++++++++++++++++++" << endl;

	vector<long long > AMX;//将累加的灰度值进行差分运算
	long long MaxOfAMX = 0;//AMX中的最大值
	Func_MappedDifference(MX, AMX, MaxOfAMX);

	//for each (auto var in AMX) {
	//	cout << var << " ";
	//}
	//cout << "\n+++++++++++++++++++" << endl;

	Func_SmoothDifference(AMX, MaxOfAMX, 0.35);

	//for each (auto var in AMX) {
	//	cout << var << " ";
	//}
	//cout << "\n+++++++++++++++++++" << endl;

	vector<unsigned int> loc;//波峰的位置
	Func_FindSurvivingPeak(AMX, loc);

	//for each (auto var in loc) {
	//	cout << var << " ";
	//}
	//cout << "\n+++++++++++++++++++" << endl;

	unsigned int topmost = 0, downmost = height - 1;//条码区域的上下界
	Func_FindFarmost(loc, topmost, downmost);
#ifdef _DEBUG
	__Func_DrawLine(source, topmost, downmost, width, height, 0xEE);
#endif
	unsigned char MaxGrayValue = 0, MinGrayValue = 255;//条码区域最大最小灰度值
	double GT = 0, SC = 0;//全局阈值
	Func_GetGT(source, MaxGrayValue, MinGrayValue, SC, GT, topmost, downmost, width, height);
	//将得到的结果存储
	grade.Rmax = MaxGrayValue;
	grade.Rmin = MinGrayValue;
	grade.SC = SC;
	grade.GT = GT;
	if (SC != 0) {
		vector<vector<unsigned long > > locDataSet;
		Func_Scan(source, locDataSet, GT, topmost, downmost, width, height, grade);
		Func_Grading(grade);
		Func_Decodability(locDataSet, BarcodeType::EAN_128, grade);
		//vector<unsigned long > loc;
		////Func_GetModuleWidth(locDataSet, loc);
//#ifdef _DEBUG
//		//__Func_DrawLine(source, topmost, downmost, loc, width, height, 0xEE);
//#endif
	}

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	return true;
}

//-----程序入口-----
int main() {
	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//barcode-test-66.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//TEST2//Minimal Edge Contrast_4__0_16_2.bmp");
	if (!rt)
		return -1;

	Grade grade;
	Func(pImg, grade);
	cout << "Rmin_G:\t\t" << grade.Rmin_Grade << " Rmin_S:\t" << (unsigned int)grade.Rmin
		<< "\nSC_G:\t\t" << grade.SC_Grade << " SC_S:\t\t" << grade.SC
		<< "\nECmin_G:\t" << grade.ECmin_Grade << " ECmin_S:\t" << (unsigned int)grade.ECmin
		<< "\nMOD_G:\t\t" << grade.MOD_Grade << " MOD_S:\t" << grade.MOD
		<< "\nDefects_G:\t" << grade.Defects_Grade << " Defects_S:\t" << grade.Defects << endl;

	//getchar();
	return 0;
}