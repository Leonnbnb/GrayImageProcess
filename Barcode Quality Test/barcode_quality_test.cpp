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

using namespace std;

//�����ȼ����ż����Ӧ����
typedef enum GradeSymbol {
	A = 4,
	B = 3,
	C = 2,
	D = 1,
	E = 0
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
	GradeSymbol SC_Grade;
	GradeSymbol MOD_Grade;
	GradeSymbol Defects_Grade;
	GradeSymbol Rmin_Grade;
	GradeSymbol ECmin_Grade;
	Grade() {
		Rmax = 0;
		Rmin = 0;
		SC = 0;
		ECmin = 0;
		ERNmax = 0;
		Defects = 0;
		MOD = 0;
		SC_Grade = GradeSymbol::E;
		MOD_Grade = GradeSymbol::E;
		Defects_Grade = GradeSymbol::E;
		Rmin_Grade = GradeSymbol::E;
		ECmin_Grade = GradeSymbol::E;;
	}

}Grade;

#ifdef _DEBUG

//��ȡ��ǰ��ϵͳʱ���Դ����ļ����������������ʹ�ã�
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
	if (nowTime != pastTime && (!nowTime.empty())) {//��Ϊ��ȡʱ��ֻ��ȷ���룬���ǳ��������һ��֮�ڴ������ٸ��ļ�������Ҫ���ļ�����������
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

//���ݵõ��Ĳ����λ�������񣨽�����������м�����
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
						preview[y][i] = 0;
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

//��ͼƬ��X����ӳ�䣬�ۼӻҶ�ֵ
bool Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height) {
	if (img == NULL)
		return false;
	if (width == 0 || height == 0)
		return true;

	for (long j = 0; j < height; ++j) {
		long long sum = 0;
		for (long i = 0; i < width; ++i) {
			sum += img[j][i];
		}
		MX.push_back(sum);
	}
	return true;
}

//��ӳ�����źŲ��
bool Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX) {
	if (MX.size() <= 1)
		return false;
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

//��С��Ĩƽ
bool Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M) {
	if (AMX.size() <= 1 || MaxOfAMX == 0 || M == 0)
		return false;

	//��������ֵ��������-1��䣬���������Ҳ���Ĳ���
	for (vector<long long>::iterator p = AMX.begin(); p < AMX.end(); ++p) {
		if (*p < (MaxOfAMX*M)) {
			*p = -1;
		}
	}
	if (*AMX.begin() < 0)
		*AMX.begin() = 1;
	return true;
}

//�ҵ���Ȼ���ڵĲ���
bool Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc) {
	if (AMX.size() <= 1)
		return false;

	//��ֹ����û���ϱ߽�
	if (*AMX.begin() > *(AMX.begin() + 1))
		loc.push_back(0);

	for (vector<long long>::iterator p = AMX.begin() + 1; p < AMX.end() - 1; ++p) {
		if (*p > 0 && *p > *(p - 1) && *p > *(p + 1)) {
			loc.push_back((unsigned int)(p - AMX.begin()));
		}
	}
	//��ֹ����û���±߽�
	loc.push_back((unsigned int)((AMX.end() - 1) - AMX.begin()));
	return true;
}

//�ҵ�������Զ�������嶥
bool Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right) {
	if (loc.size() <= 1)
		return false;
	unsigned int distance = 0;
	for (vector<unsigned int >::iterator p = loc.begin() + 1; p < loc.end(); ++p) {
		//����֮��ľ���
		if (*p - *(p - 1) > distance) {
			left = *(p - 1) + 5;
			right = *p - 5;
			distance = *p - *(p - 1);
		}
	}
	return true;
}

//�õ��Ҷ����ֵ��Сֵ�Լ����GT 
bool Func_GetGT(unsigned char** img, unsigned char &maxgray, unsigned char &mingray, double &SC, double &GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height) {
	if (img == NULL || width == 0 || height == 0 || topmost<0 || downmost>height)
		return false;

	//ָ������ĻҶ�ֱ��ͼ
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));

	//ָ���������ظ���
	long long pixels = 0;
	//ʹ��ָ�������ʼ��ֱ��ͼ
	for (long i = topmost; i < downmost; ++i) {
		for (long j = 0; j < width; ++j) {
			++tab[(unsigned int)img[i][j]];
			++pixels;
		}
	}

	//ǰ�ٷ�֮ʮ������
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
	//ǰ�ٷ�֮ʮ����С�Ҷ�ֵ�����ؾ�ֵ
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
	//ǰ�ٷ�֮ʮ�����Ҷ�ֵ�����ؾ�ֵ
	double avggreater = (double)sum / count;

	SC = avggreater - avglower;
	GT = (avglower + avggreater) / 2;
	maxgray = avggreater;
	mingray = avglower;

	return true;
}

//���ָ���кŵ�����ֵ
bool _Func_GetSelectedRowPixels(unsigned char** img, vector<unsigned char> &rowSet, const unsigned char rowNum, const long width) {
	if (img == NULL)
		return false;
	if (width == 0)
		return false;
	for (unsigned long x = 0; x < width; ++x) {
		rowSet.push_back(img[rowNum][x]);
	}
	return true;
}

//���ָ��߾�����һά�ź���GTΪ���޻���
bool _Func_Split(vector<unsigned char > &rowSet, vector<unsigned long > &locSet, double GT, vector<vector<unsigned char > > &sets) {
	if (rowSet.size() == 0)
		return false;
	//vector<unsigned long> locSet;
	unsigned long loc = 0;
	//��ͷ:)
	if (*rowSet.begin() >= GT) {
		vector<unsigned char >::iterator pfront = rowSet.begin();
		while (pfront < rowSet.end() && *pfront >= GT)
			++pfront;
		loc = pfront - rowSet.begin();
		rowSet.erase(rowSet.begin(), pfront);
	}
	locSet.push_back(loc);

	//���Ϊ��ɫͼƬ����һ�������Ὣ����Ԫ�����
	if (rowSet.size() == 0)
		return false;
	//ȥβ:)
	if (*prev(rowSet.end()) >= GT) {
		vector<unsigned char >::iterator prear = prev(rowSet.end());
		while (prear > rowSet.begin() && *prear > GT)
			--prear;
		rowSet.erase(prear, rowSet.end());
	}

	//For safe
	if (rowSet.size() == 0)
		return false;

	//�и�ɶ�
	vector<unsigned char >::iterator p = rowSet.begin();
	vector<unsigned char > up, down;

	while (p < rowSet.end()) {
		if (p >= rowSet.end())
			break;

		//�����򣨺�ɫ��
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

		//�հ����򣨰�ɫ��
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

//���ݷֶκ�����ݻ����СECֵ�����ERNֵ
bool _Func_ECminERNmax(vector<vector<vector<unsigned char > > > dataSet, Grade &grade) {
	if (dataSet.size() == 0)
		return false;

	unsigned char ECmin = 255, ERNmax = 0;
	//pair��һ��Ԫ������Сֵ���ڶ���Ԫ�������ֵ
	for each(auto rowdata in dataSet) {
		//ÿһ��ɨ�����ϵ�ÿһ���е���С���Ҷ�ֵ�ļ���
		vector<pair<unsigned char, unsigned char > > minmaxSet;
		for each(auto seg in rowdata) {
			unsigned char max = 0, min = 255;
			for (vector<unsigned char >::iterator ppix = seg.begin() + 1; ppix < seg.end() - 1; ++ppix) {
				if (*ppix >= *(ppix + 1) && *ppix >= *(ppix - 1) && *ppix > max)
					max = *ppix;
				if (*ppix <= *(ppix + 1) && *ppix <= *(ppix - 1) && *ppix < min)
					min = *ppix;
			}
			//���ϵ���С���Ҷ�ֵ
			pair<unsigned char, unsigned char > minmaxofSeg;
			minmaxofSeg.first = min;
			minmaxofSeg.second = max;
			minmaxSet.push_back(minmaxofSeg);
		}

		//��¼��ÿ��ɨ�����ϵ�EC��ERN
		vector<unsigned char > ECset;
		vector<unsigned char > ERNset;
		if (minmaxSet.size() <= 1)
			break;
		ERNset.push_back(abs(minmaxSet.begin()->second - minmaxSet.begin()->first));
		for (auto p = minmaxSet.begin() + 1; p < minmaxSet.end(); ++p) {
			unsigned char EC;
			unsigned char ERN;
			//�ҵ����ڵ�SEGMENT�д�ֱ������Զ��ֵ
			EC = abs((p - 1)->first - p->second) > abs((p - 1)->second - p->first) ?
				abs((p - 1)->first - p->second) : abs((p - 1)->second - p->first);
			ECset.push_back(EC);

			//��ÿһ��SEGMENT�е������Сֵ�Ĳ�ֵ��¼����
			ERNset.push_back(abs(p->second - p->first));
		}

		//�ҵ�ECset��ERNset�е�EC��Сֵ��ERN���ֵ
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

//����topmost��downmost��������ȷ�������뷶Χ�����зָ��������ɨ���߽��к���ɨ��
bool Func_Scan(unsigned char** img, vector<vector<unsigned long > > &locDataSet, double GT, const unsigned int topmost, const unsigned int downmost, const long width, const long height, Grade &grade) {
	if (img == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	if (topmost<0 || downmost>height)
		return false;
	//����ɨ���ߵ��������ݼ���
	vector<vector<vector<unsigned char > > > pixDataSet;

	//vector<vector<unsigned long > > locDataSet;
	//ɨ����֮��ľ���
	double distance = (downmost - topmost)*1.0 / 10.0;
	for (long y = topmost; y <= downmost; y += distance) {
		//ָ��ɨ���ߵ����ػҶ�ֵ
		vector<unsigned char> rowPixels;
		_Func_GetSelectedRowPixels(img, rowPixels, topmost, width);
		//����ǰɨ���ߵ����ػҶ�ֵ�ֶ�
		vector<vector<unsigned char > > peakAndvalley;
		vector<unsigned long > locSet;
		_Func_Split(rowPixels, locSet, GT, peakAndvalley);
		pixDataSet.push_back(peakAndvalley);
		locDataSet.push_back(locSet);
	}

	_Func_ECminERNmax(pixDataSet, grade);
	grade.MOD = grade.ECmin*1.0 / grade.SC*1.0;

	return true;
}

//���ݸ�����������
bool Func_Grading(Grade &grade) {
	//Rmin Grade
	if (grade.Rmin <= 0.5*grade.Rmax)
		grade.Rmin_Grade = GradeSymbol::A;
	else
		grade.Rmin_Grade = GradeSymbol::E;

	//SC Grade
	double SC_rate = grade.SC / grade.Rmax;
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
	double ECmin_rate = grade.ECmin / grade.SC*1.0;//
	if (ECmin_rate >= 0.15)
		grade.ECmin_Grade = GradeSymbol::A;
	else
		grade.ECmin_Grade = GradeSymbol::E;

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

//ȷ��ÿ�������հ����Ŀ��
bool Func_GetModuleWidth(vector<vector<unsigned long > > locDataSet, vector<unsigned long > &loc) {
	if (locDataSet.size() == 0)
		return false;
	//for each(auto p in locDataSet) {
	//	for each(auto pp in p) {
	//		cout << pp << " ";
	//	}
	//	cout << endl;
	//}
	for (unsigned long i = 0; i < locDataSet.begin()->size() * 2; ++i) {
		long long sum = 0;
		long long count = 0;
		bool exist = false;
		for (vector<vector<unsigned long > >::iterator p = locDataSet.begin(); p < locDataSet.end(); ++p) {
			if ((p->begin() + i) < p->end()) {
				sum += *(p->begin() + i);
				++count;
				exist = true;
			}
		}
		if (exist == false)
			break;
		if (count != 0)
			loc.push_back(sum / count);
	}
	return true;
}

//-----�������-----
bool Func(CImg* pImg, Grade &grade) {
	if (pImg == NULL)
		return false;

	long width, height;//ͼƬ�ĳߴ�
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
	vector<long long > MX;//��ͼ��Ҷ�ֵ����ӳ���ۼ�
	Func_Mapping(source, MX, width, height);
	vector<long long > AMX;//���ۼӵĻҶ�ֵ���в������
	long long MaxOfAMX = 0;//AMX�е����ֵ
	Func_MappedDifference(MX, AMX, MaxOfAMX);
	Func_SmoothDifference(AMX, MaxOfAMX, 0.35);
	vector<unsigned int> loc;//�����λ��
	Func_FindSurvivingPeak(AMX, loc);
	unsigned int topmost = 0, downmost = height - 1;//������������½�
	Func_FindFarmost(loc, topmost, downmost);

	//#ifdef _DEBUG
	//	__Func_DrawLine(source, topmost, downmost, width, height, 0xEE);
	//#endif

	unsigned char MaxGrayValue = 0, MinGrayValue = 255;//�������������С�Ҷ�ֵ
	double GT = 0, SC = 0;//ȫ����ֵ
	Func_GetGT(source, MaxGrayValue, MinGrayValue, SC, GT, topmost, downmost, width, height);
	//���õ��Ľ���洢
	grade.Rmax = MaxGrayValue;
	grade.Rmin = MinGrayValue;
	grade.SC = SC;
	grade.GT = GT;
	if (SC != 0) {
		vector<vector<unsigned long > > locDataSet;
		Func_Scan(source, locDataSet, GT, topmost, downmost, width, height, grade);
		Func_Grading(grade);
		vector<unsigned long > loc;
		Func_GetModuleWidth(locDataSet, loc);
#ifdef _DEBUG
		__Func_DrawLine(source, topmost, downmost, loc, width, height, 0xFF);
#endif
	}

	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	return true;
}

//-----�������-----
int main() {
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//barcode-test-07.bmp");
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