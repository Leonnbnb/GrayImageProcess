#include "class_barcode_grade_NEW.h"

/*****************************************

	Class OneDBarcodeGrading Interface

******************************************/

bool OneDBarcodeGrading::Grading(CImg* pImg, OneDBarCodeType type) {
	if (!pImg)
		return false;

	switch (type) {
	case OneDBarCodeType::EAN_128: {
		bool rt = EAN128(pImg, OneDBarcodeGrade);
		InitFlag = true;
		return rt;
	}
	default:
	{
		return false;
	}
	}
}

bool OneDBarcodeGrading::SetDECODEGrade(int decode_grade) {
	switch (decode_grade) {
	case 0:
		OneDBarcodeGrade.DECODE_Grade = GradeSymbol::F;
		break;
	case 4:
		OneDBarcodeGrade.DECODE_Grade = GradeSymbol::A;
		break;
	default:
		OneDBarcodeGrade.DECODE_Grade = GradeSymbol::F;
		return false;
	}
	return true;
}

int OneDBarcodeGrading::GetDecodabilityGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.Decodability_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetSCGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.SC_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetMODGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.MOD_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetDefectsGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.Defects_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetRminGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.Rmin_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetECminGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.ECmin_Grade;
	}
	else
		return -1;
}

int OneDBarcodeGrading::GetDECODEGrade() {
	if (InitFlag) {
		return OneDBarcodeGrade.DECODE_Grade;
	}
	else
		return -1;
}

/*****************************************

	Class OneDBarcodeGrading Private
				Funtions

******************************************/

bool OneDBarcodeGrading::EAN128(CImg* pImg, OneDBarcodeGrading::Grade &grade) {
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

	vector<long long > AMX;//将累加的灰度值进行差分运算
	long long MaxOfAMX = 0;//AMX中的最大值
	Func_MappedDifference(MX, AMX, MaxOfAMX);

	Func_SmoothDifference(AMX, MaxOfAMX, 0.35);

	vector<unsigned int> loc;//波峰的位置
	Func_FindSurvivingPeak(AMX, loc);

	unsigned int topmost = 0, downmost = height - 1;//条码区域的上下界
	Func_FindFarmost(loc, topmost, downmost);
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
		vector<vector<float > > locDataSet2;
		Func_Scan(pImg, source, locDataSet, locDataSet2, GT, topmost, downmost, width, height, grade);
		Func_Grading(grade);
		Func_Decodability(locDataSet2, OneDBarCodeType::EAN_128, grade);
	}

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	return true;
}

void OneDBarcodeGrading::BarcodeCharacterInfo::setValue(float loc1, float loc2, float loc3, float loc4, float loc5, float loc6, float loc7) {
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

double OneDBarcodeGrading::BarcodeCharacterInfo::calcV1() {
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
	return ret;
}

bool OneDBarcodeGrading::BarcodeCharacterInfo::_setRT() {
	if (p == 0) return false;
	RT[0] = (1.5*p) / 11.0;
	RT[1] = (2.5*p) / 11.0;
	RT[2] = (3.5*p) / 11.0;
	RT[3] = (4.5*p) / 11.0;
	RT[4] = (5.5*p) / 11.0;
	RT[5] = (6.5*p) / 11.0;
	RT[6] = (7.5*p) / 11.0;
}

OneDBarcodeGrading::OneDBarcodeGrading() {
	InitFlag = false;
}

OneDBarcodeGrading::~OneDBarcodeGrading() {

}

//将图片沿X方向映射，累加灰度值
bool OneDBarcodeGrading::Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height) {
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
bool OneDBarcodeGrading::Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX) {
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
bool OneDBarcodeGrading::Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M) {
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
bool OneDBarcodeGrading::Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc) {
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
bool OneDBarcodeGrading::Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right) {
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
bool OneDBarcodeGrading::Func_GetGT(unsigned char** img, unsigned char &maxgray, unsigned char &mingray, double &SC, double &GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height) {
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
bool OneDBarcodeGrading::_Func_GetSelectedRowPixels(unsigned char** img, vector<unsigned char> &rowSet, const unsigned char rowNum, const long width) {
	if (img == NULL) return false;
	if (width == 0)	return false;
	for (unsigned long x = 0; x < width; ++x) {
		rowSet.push_back(img[rowNum][x]);
	}
	return true;
}

//将分割线经过的一维信号以GT为界限划分
bool OneDBarcodeGrading::_Func_Split(vector<unsigned char > &rowSet, vector<unsigned long > &locSet, double GT, vector<vector<unsigned char > > &sets) {
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
bool OneDBarcodeGrading::_Func_ECminERNmax(vector<vector<vector<unsigned char > > > dataSet, double GT, OneDBarcodeGrading::Grade &grade) {
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
bool OneDBarcodeGrading::Func_Scan(const CImg* pImg, unsigned char** img, vector<vector<unsigned long > > &locDataSet, vector<vector<float > > &locDataSet2, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, OneDBarcodeGrading::Grade &grade) {
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
		//
		vector<float > locSet2;
		int count = 0;
		float* pxset = NULL;
		float* pyset = NULL;
		float* pamplitude = NULL;
		float* pdistance = NULL;
		bool rt = false;
		rt = measure1(*pImg, 1.0, 10, 0, y, width - 1, y + 1, 1, measure_Transition::t_all, measure_interpolation::nearest_neighbor, count, pxset, pyset, pamplitude, pdistance);
		if (rt) {
			for (int i = 0; i < count; ++i) {
				locSet2.push_back(pxset[i]);
			}
			locDataSet2.push_back(locSet2);
			if (pxset)
				delete pxset;
			if (pyset)
				delete pyset;
			if (pamplitude)
				delete pamplitude;
			if (pdistance)
				delete pdistance;
		}
	}
	_Func_ECminERNmax(pixDataSet, GT, grade);
	grade.MOD = grade.ECmin*1.0 / grade.SC*1.0;
	return true;
}

//根据各项数据评级
bool OneDBarcodeGrading::Func_Grading(OneDBarcodeGrading::Grade &grade) {
	//Rmin Grade
	if (grade.Rmin <= 0.5*grade.Rmax)
		grade.Rmin_Grade = GradeSymbol::A;
	else
		grade.Rmin_Grade = GradeSymbol::F;

	//SC Grade
	double SC_rate = grade.SC / 255.0;
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
		grade.SC_Grade = GradeSymbol::F;
	}

	//ECmin Grade
	double ECmin_rate = grade.ECmin*1.0 / 255.0/*grade.SC*1.0*/;//
	if (ECmin_rate >= 0.15)
		grade.ECmin_Grade = GradeSymbol::A;
	else
		grade.ECmin_Grade = GradeSymbol::F;

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
		grade.MOD_Grade = GradeSymbol::F;
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
		grade.Defects_Grade = GradeSymbol::F;
	}

	return true;
}

//EAN128
bool OneDBarcodeGrading::_Func_Decode_EAN128(vector<vector<float > > locDataSet, OneDBarcodeGrading::Grade &grade) {
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
			for (vector<float >::iterator p = locDataSet.at(ln).begin(); p < locDataSet.at(ln).end(); p += 6) {

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
		grade.Decodability_Grade = GradeSymbol::F;
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
bool OneDBarcodeGrading::Func_Decodability(vector<vector<float > > locDataSet, OneDBarCodeType type, OneDBarcodeGrading::Grade &grade) {
	if (locDataSet.size() == 0) return false;
	switch (type) {
	case OneDBarCodeType::EAN_8: {
		break;
	}
	case OneDBarCodeType::EAN_13: {
		break;
	}
	case OneDBarCodeType::EAN_128: {
		_Func_Decode_EAN128(locDataSet, grade);
		break;
	}
	case OneDBarCodeType::UPC_A: {
		break;
	}
	case OneDBarCodeType::UPC_E: {
		break;
	}
	case OneDBarCodeType::ITF_14: {
		break;
	}
	default:
		return false;
	}
}

//---------------------------------------------------------------------------//

/*****************************************

	Class TwoDBarcodeGrading Interface

******************************************/

TwoDBarcodeGrading::TwoDBarcodeGrading() {
	InitFlag = false;
}

TwoDBarcodeGrading::~TwoDBarcodeGrading() {

}

bool TwoDBarcodeGrading::Func(CImg* pImg, const unsigned int error_correction_capacity, TwoDBarCodeType type) {
	if (!pImg) return false;
	if (error_correction_capacity == 0) return false;

	switch (type) {
	case TwoDBarCodeType::DataMatrix: {
		bool rt = DataMatrixGrading(pImg, error_correction_capacity, TwoDBarcodeGrade);
		InitFlag = true;
		return rt;
	}
	default:
	{
		return false;
	}
	}
}

bool TwoDBarcodeGrading::SetUECScore(int t, int error_correction_capacity) {
	double score = 1.0 - (t*2.0 / error_correction_capacity*1.0);
	if (score < 0) return false;
	TwoDBarcodeGrade.UEC_Score = score;
	if (score >= 0.62) {
		TwoDBarcodeGrade.UEC = GradeSymbol::A;
	}
	else if (score >= 0.5) {
		TwoDBarcodeGrade.UEC = GradeSymbol::B;
	}
	else if (score >= 0.37) {
		TwoDBarcodeGrade.UEC = GradeSymbol::C;
	}
	else if (score >= 0.25) {
		TwoDBarcodeGrade.UEC = GradeSymbol::D;
	}
	else {
		TwoDBarcodeGrade.UEC = GradeSymbol::F;
	}

	return true;
}

bool TwoDBarcodeGrading::SetDECODEGrade(int decode_grade) {
	switch (decode_grade) {
	case 0:
		TwoDBarcodeGrade.DECODE = GradeSymbol::F;
		break;
	case 1:
		TwoDBarcodeGrade.DECODE = GradeSymbol::D;
		break;
	case 2:
		TwoDBarcodeGrade.DECODE = GradeSymbol::C;
		break;
	case 3:
		TwoDBarcodeGrade.DECODE = GradeSymbol::B;
		break;
	case 4:
		TwoDBarcodeGrade.DECODE = GradeSymbol::A;
		break;
	default:
		return false;
	}
	return true;
}

int TwoDBarcodeGrading::GetSCGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.SC;
	else
		return -1;
}

int TwoDBarcodeGrading::GetANGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.AN;
	else
		return -1;
}

int TwoDBarcodeGrading::GetGNGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.GN;
	else
		return -1;
}

int TwoDBarcodeGrading::GetFPDGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.FPD;
	else
		return -1;
}

int TwoDBarcodeGrading::GetMODGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.MOD;
	else
		return -1;
}

int TwoDBarcodeGrading::GetUECGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.UEC;
	else
		return -1;
}

int TwoDBarcodeGrading::GetDECODEGrade() {
	if (InitFlag)
		return TwoDBarcodeGrade.DECODE;
	else
		return -1;
}

/*****************************************

	Class TwoDBarcodeGrading Private
				Funtions

******************************************/

//DataMatrix 评级
bool TwoDBarcodeGrading::DataMatrixGrading(CImg* pImg, const unsigned int error_correction_capacity, TwoDBarcodeGrading::Grade &grade) {
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

	const int model[7] = { 5,10,45,50,45,10,5 };
	const int M = 100;
	rt = Func_Gauss(mappedx, mappedy, aftermx, aftermy, width, height, model, M);

	if (rt == false)
		return false;
	vector<long > Xmaxima, Ymaxima;
	rt = Func_GetMaxima(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;
	rt = Func_CompareHill(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;

	vector<vector<unsigned char > > avr;
	rt = Func_ModuleAvg(source, Xmaxima, Ymaxima, avr, width, height);
	if (rt == false)
		return false;

	MOD_DAMIT DataAreaMODInfo;
	vector<MOD_GIT> MODGradingTable;
	rt = Func_MOD_DataAreaModulation(avr, DataAreaMODInfo, GT, grade.SC_Score);
	if (rt != false) {
		grade.MOD = _Func_MOD_T7A(DataAreaMODInfo, MODGradingTable, error_correction_capacity);
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

//15415 Table 7(A) (P19) Create & Edit
TwoDBarcodeGrading::GradeSymbol TwoDBarcodeGrading::_Func_MOD_T7A(TwoDBarcodeGrading::MOD_DAMIT DataAreaMODGradeTable, vector<TwoDBarcodeGrading::MOD_GIT > &MODGIT, const unsigned int error_correction_capacity) {
	if (DataAreaMODGradeTable.modinfo.size() == 0)
		return TwoDBarcodeGrading::GradeSymbol::F;

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
unsigned int TwoDBarcodeGrading::_Func_FPD_TransitionRatioTest(vector<double > SA_T, vector<double > CT_T) {
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
bool TwoDBarcodeGrading::_Func_FPD_CTRegularityTest(TwoDBarcodeGrading::LSAI_MT MT, vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT) {
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
bool TwoDBarcodeGrading::_Func_FPD_CTDamageTest(TwoDBarcodeGrading::LSAI_MT MT, vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT) {
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
bool TwoDBarcodeGrading::_Func_FPD_SAFixedPatternTest(TwoDBarcodeGrading::LSAI_MT MT, vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT) {
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
bool TwoDBarcodeGrading::_Func_FPD_TM4(vector<double > info, TwoDBarcodeGrading::LSAI_MT &MT, TwoDBarcodeGrading::LSAS style) {
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
bool TwoDBarcodeGrading::_Func_FPD_TM5_Init(vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT) {
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
bool TwoDBarcodeGrading::_Func_FPD_TM5_Edit(TwoDBarcodeGrading::LSAI_MT info, vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT, TwoDBarcodeGrading::LSAS style) {
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
bool TwoDBarcodeGrading::_Func_FPD_TM5_SetLowest_For_LastColumn(vector<TwoDBarcodeGrading::LSAI_SGIT > &SGIT, TwoDBarcodeGrading::LSAS style) {
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
unsigned int TwoDBarcodeGrading::_Func_Fpd_TM5_GetHighest_Of_LastColumn(vector<TwoDBarcodeGrading::LSAI_SGIT > SGIT) {
	unsigned int max = 0;
	for (unsigned int G = 0; G <= 4; ++G) {
		if (SGIT[G].lower_of_grades >= max)
			max = SGIT[G].lower_of_grades;
	}
	return max;
}

//计算SC值
bool TwoDBarcodeGrading::Func_SC(unsigned char** pBuffer, const long width, const long height, double &SC, double &GT, TwoDBarcodeGrading::GradeSymbol &Grade) {
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
bool TwoDBarcodeGrading::Func_Sobel(unsigned char** img, long** &gx, long** &gy, const long width, const long height) {
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
bool TwoDBarcodeGrading::Func_Gradient(long** gx, long** gy, double** &G, const long width, const long height) {
	if (gx == NULL || gy == NULL || G == NULL)
		return false;

	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			long long x = gx[j][i];
			long long y = gy[j][i];
			G[j][i] = sqrt((x * x) + (y * y)) > 255 ? 255 : sqrt((x * x) + (y * y));
		}
	}

	return true;
}

//将运算得到的边缘梯度图的X、Y方向分别映射
bool TwoDBarcodeGrading::Func_Mapping(double** G, long long* &mx, long long* &my, const long width, const long height) {
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
bool TwoDBarcodeGrading::Func_Gauss(long long* mx, long long* my, long long* &Amx, long long* &Amy, const long width, const long height, const int* model, const int M) {
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
bool TwoDBarcodeGrading::Func_CompareHill(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
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
		pair<long, long> temp;//
		temp.first = y;
		temp.second = max;
		Xdifference.push_back(temp);
		ypre = y;
	}

	long XMAX = 0;
	for (vector<pair<long, long> >::iterator pa = Xdifference.begin(); pa < Xdifference.end(); ++pa) {
		if (pa->second > XMAX)
			XMAX = pa->second;
	}

	long Xthreshold = XMAX * 15 / 100;
	for (vector<pair<long, long> >::iterator pa = Xdifference.begin(); pa < Xdifference.end(); ++pa) {
		if (pa->second < Xthreshold) {
			Xmaxima.erase(find(Xmaxima.begin(), Xmaxima.end(), pa->first));
		}
	}
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
		pair<long, long> temp;//
		temp.first = x;
		temp.second = max;
		Ydifference.push_back(temp);
		xpre = x;
	}

	long YMAX = 0;
	for (vector<pair<long, long> >::iterator pa = Ydifference.begin(); pa < Ydifference.end(); ++pa) {
		if (pa->second > YMAX)
			YMAX = pa->second;
	}

	long Ythreshold = YMAX * 15 / 100;
	for (vector<pair<long, long> >::iterator pa = Ydifference.begin(); pa < Ydifference.end(); ++pa) {
		if (pa->second < Ythreshold) {
			Ymaxima.erase(find(Ymaxima.begin(), Ymaxima.end(), pa->first));
		}
	}
	return true;
}

//得到滤波后的一维信号的波峰点位
bool TwoDBarcodeGrading::Func_GetMaxima(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
	if (Amx == NULL || Amy == NULL)
		return false;
	for (long y = 2; y < height - 2; ++y) {
		if (Amx[y] >= Amx[y - 1] && Amx[y - 1] >= Amx[y - 2] && Amx[y] > Amx[y + 1] && Amx[y + 1] >= Amx[y + 2])
			Xmaxima.push_back(y);
	}

	for (long x = 2; x < width - 2; ++x) {
		if (Amy[x] >= Amy[x - 1] && Amy[x - 1] >= Amy[x - 2] && Amy[x] > Amy[x + 1] && Amy[x + 1] >= Amy[x + 2])
			Ymaxima.push_back(x);
	}
	return true;
}

//求得每个信息点的像素值均值
bool TwoDBarcodeGrading::Func_ModuleAvg(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, vector<vector<unsigned char > > &avr, const long width, const long height) {
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
bool TwoDBarcodeGrading::Func_MOD_DataAreaModulation(vector<vector<unsigned char > > R, TwoDBarcodeGrading::MOD_DAMIT &DataAreaMODGradeTable, const double GT, const double SC) {
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
	return true;
}

//求AN值
bool TwoDBarcodeGrading::Func_AN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, TwoDBarcodeGrading::GradeSymbol &grade) {
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
bool TwoDBarcodeGrading::Func_GN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, TwoDBarcodeGrading::GradeSymbol &grade) {
	double xavg = (*prev(Ymaxima.end()) - *Ymaxima.begin())*1.0 / (Ymaxima.size() - 1);
	double yavg = (*prev(Xmaxima.end()) - *Xmaxima.begin())*1.0 / (Xmaxima.size() - 1);

	vector<double > Xdistance, Ydistance;
	for (int i = 0; i < (Ymaxima.size() - 1); ++i) {
		Xdistance.push_back(abs((Ymaxima[i] - Ymaxima[0]) - 1.0*i*xavg));
	}
	for (int i = 0; i < (Xmaxima.size() - 1); ++i) {
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

	score = sqrt((xmaxdistance*xmaxdistance) + (ymaxdistance*ymaxdistance)) / xavg;

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
bool TwoDBarcodeGrading::Func_FPD(unsigned char** img, TwoDBarcodeGrading::FPD_G &FPD_Grades, vector<long > Xmaxima, vector<long > Ymaxima, const double SC, const double GT, const long width, const long height) {
	if (img == NULL || Xmaxima.size() < 2 || Ymaxima.size() < 2)
		return false;
	TwoDBarcodeGrading::LSAI info;

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
	xpre = Ymaxima[0] + 1;
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

	//L1
	LSAI_MT L1_modinfotab;//M4
	_Func_FPD_TM4(info.L1MOD, L1_modinfotab, LSAS::L);

	vector<LSAI_SGIT > L1_seggradingtab;//M5
	_Func_FPD_TM5_Init(L1_seggradingtab);
	_Func_FPD_TM5_Edit(L1_modinfotab, L1_seggradingtab, LSAS::L);

	_Func_FPD_TM5_SetLowest_For_LastColumn(L1_seggradingtab, LSAS::L);
	FPD_Grades.L1_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(L1_seggradingtab);

	//L2
	LSAI_MT L2_modinfotab;//M4
	_Func_FPD_TM4(info.L2MOD, L2_modinfotab, LSAS::L);

	vector<LSAI_SGIT > L2_seggradingtab;//M5
	_Func_FPD_TM5_Init(L2_seggradingtab);
	_Func_FPD_TM5_Edit(L2_modinfotab, L2_seggradingtab, LSAS::L);

	_Func_FPD_TM5_SetLowest_For_LastColumn(L2_seggradingtab, LSAS::L);
	FPD_Grades.L2_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(L2_seggradingtab);

	//QZL1
	LSAI_MT QZL1_modinfotab;//M4
	_Func_FPD_TM4(info.QZL1MOD, QZL1_modinfotab, LSAS::QZL);

	vector<LSAI_SGIT > QZL1_seggradingtab;//M5
	_Func_FPD_TM5_Init(QZL1_seggradingtab);
	_Func_FPD_TM5_Edit(QZL1_modinfotab, QZL1_seggradingtab, LSAS::QZL);

	_Func_FPD_TM5_SetLowest_For_LastColumn(QZL1_seggradingtab, LSAS::QZL);
	FPD_Grades.QZL1_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(QZL1_seggradingtab);

	//QZL2
	LSAI_MT QZL2_modinfotab;//M4
	_Func_FPD_TM4(info.QZL2MOD, QZL2_modinfotab, LSAS::QZL);

	vector<LSAI_SGIT > QZL2_seggradingtab;//M5
	_Func_FPD_TM5_Init(QZL2_seggradingtab);
	_Func_FPD_TM5_Edit(QZL2_modinfotab, QZL2_seggradingtab, LSAS::QZL);

	_Func_FPD_TM5_SetLowest_For_LastColumn(QZL2_seggradingtab, LSAS::QZL);
	FPD_Grades.QZL2_grade = _Func_Fpd_TM5_GetHighest_Of_LastColumn(QZL2_seggradingtab);

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
}

//16022 P111 M.1.4 Calculation and grading of average grade(计算L1 L2 QZL1 QZL2 （以及两个Segment区域的最小值）五项的均值再取最小)
bool TwoDBarcodeGrading::Func_CalculatePFDAverageGrade(TwoDBarcodeGrading::FPD_G FPD_grades, TwoDBarcodeGrading::Grade &grade) {
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