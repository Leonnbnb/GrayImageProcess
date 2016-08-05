#ifndef _EXTERN_PATTERN_RECOGNIZE
#define _EXTERN_PATTERN_RECOGNIZE

#include "../shared/Img.h"
#include "../shared/List.h"
#include "../shared/dataType.h"
//#pragma comment(lib,"patternRecognize.lib") 
//#pragma message("Automatically linking with patternRecognize.lib")
#define SHAPE_MODEL       1
#define XLD_MODEL         2

enum Polarity_state { use_polarity, ignore_global_polarity, ignore_local_polarity };

class PATTERN_RECOGNIZE_CLASS_DLL CPoint2D
{
public:
	CPoint2D();
	CPoint2D(int x1, int y1);
	CPoint2D(const CPoint2D& p): x(p.x), y(p.y){}
	CPoint2D & operator = (const CPoint2D &p);
	BOOL operator == (const CPoint2D &p) const   { return ((x == p.x) && (y == p.y)); }
	
	int Y(void) const { return y; }
  	int X(void) const { return x; }

public:
	int x;
	int y;	
};



class PATTERN_RECOGNIZE_CLASS_DLL CRectangle1
{
public:
  CRectangle1(void);
  CRectangle1(int left, int top, int right, int bottom);
  CRectangle1(const CPoint2D &top_left, const CPoint2D &bottom_right);
  CRectangle1 & operator = (const CRectangle1 &rect);

  CPoint2D const  & topLeft(void) const {return m_top_left;}
  CPoint2D const  &bottomRight(void) const{return m_bottom_right;}
  //CPoint2D          Center(void) const;
  int            Width(void) const;
  int            Height(void) const;
  BOOL             IsEmpty(void) const;


  private:
  	CPoint2D  m_top_left;
  	CPoint2D  m_bottom_right;
};

class PATTERN_RECOGNIZE_CLASS_DLL CRectangle2
{
public:
  CRectangle2(void);
  CRectangle2(const CPoint2D &Center, double Ra, double Rb, double Phi =0.0);
  CRectangle2 & operator = (const CRectangle2 &rect);

  CPoint2D Point1(void) const;
  CPoint2D Point2(void) const;
  CPoint2D Point3(void) const;
  CPoint2D Point4(void) const;
  CPoint2D Center(void) const;
  BOOL             IsEmpty(void) const;

  double  Ra(void) const { return m_ra; }
  double  Rb(void) const { return m_rb; }
  double  Phi(void) const { return m_phi; }
  
  private:
	  CPoint2D  m_center;
	  double   m_phi;     
	  double   m_ra, m_rb;
};

class PATTERN_RECOGNIZE_CLASS_DLL CEllipse
{
public:
	CEllipse(void);
	CEllipse(const CPoint2D &Center, double Ra, double Rb, double Phi = 0.0);
	CEllipse & operator = (const CEllipse &rect);

	CPoint2D Point1(void) const;
	CPoint2D Point2(void) const;
	CPoint2D Point3(void) const;
	CPoint2D Point4(void) const;
	CPoint2D Center(void) const;
	BOOL             IsEmpty(void) const;

	double  Ra(void) const { return m_ra; }
	double  Rb(void) const { return m_rb; }
	double  Phi(void) const { return m_phi; }

private:
	CPoint2D  m_center;
	double   m_phi;
	double   m_ra, m_rb;
};



class PATTERN_RECOGNIZE_CLASS_DLL CExtRegion
{
public:
	CExtRegion();
	CExtRegion(const CRectangle1& rectangle1);
	CExtRegion(const CRectangle2& rectangle2);
	CExtRegion(const CEllipse& ellipse);
	CExtRegion(const int& startX, const int& startY, BYTE* p2binRegion, const int& w, const int& h);
	CExtRegion(Hrlregion& rl);
	virtual ~CExtRegion();
	
	CExtRegion & operator = (const CExtRegion &region);

	//interface
	void OmitRgn(const CExtRegion &omitted_rgn);
	void OmitRgn(const CExtRegion &org_rgn, const CExtRegion &omitted_rgn);
	void AddRgn(const CExtRegion &added_rgn);
	void AddRgn(const CExtRegion &org_rgn, const CExtRegion &added_rgn);

	//
	void and(const CExtRegion &region1,const CExtRegion &region2);
	void add(const CExtRegion &region1);

	virtual BOOL addBinArray(const int& startX, const int& startY, BYTE* p2binRegion, const int& w, const int& h);
	virtual BOOL delBinArray(const int& startX, const int& startY, BYTE* p2binRegion, const int& w, const int& h);
	Hrlregion *GetRL(void) const {return m_rl;}

	virtual int saveToBuff(BYTE*& buff);
	virtual BOOL LoadFromBuff(BYTE* buff, int buffSize);

	BOOL gen_contourArray(int& startX, int& startY, BYTE*& p2binContour, int& width, int& height);
	const BOOL gen_binArray(int& startX, int& startY, BYTE*& p2binRegion, int& width, int& height) const;
private:
	void clearResource();
	void generate_RLE_by_binArray(const int& startX, const int& startY, BYTE* p2binRegion, const int& width, const int& height);
	BOOL erosion(BYTE* binImgeIn,BYTE* binImgeOut, int width, int height, BYTE pix_shrink);

private:
	Hrlregion  *m_rl;
};

// 区域数组类
class PATTERN_RECOGNIZE_CLASS_DLL CExtRegionArray
{
public:
	CExtRegionArray();
	virtual ~CExtRegionArray();
	inline const  int GetRegionNum() const;
	CExtRegion* GetRegion(int Index) const;
	void AddItem(CExtRegion* pRgn);
protected:
	int m_RegionNum;
	int m_ContainerCapability;
	CExtRegion** m_ppRegion;
};

//XLD
class PATTERN_RECOGNIZE_CLASS_DLL CExtxld
{
public:
	CExtxld();
	~CExtxld();

	void addPoint(float col, float row);
	void getPoints(float* &pCol, float* &pRow, int& nCount);

private:
	float* m_pX;
	float* m_pY;
	int m_count;
	int m_nAllocCount;
};

// 图像模板配置接口
class PATTERN_RECOGNIZE_CLASS_DLL CExtShapeMode
{
public:	
	static CExtShapeMode* CreateShapeModeFromFile(LPCTSTR lpcPathName);
	virtual ~CExtShapeMode();

	// 将模板保存到文件
	virtual BOOL SaveToFile(LPCTSTR lpcPathName) = 0;

	//获取创建模板使用的图像对比度门限值
	virtual void getContrastThreshold(int& contrastH, int& contrastL) = 0;
	
	//与旋转相关的函数
	virtual void getRotateMode(double& angle_start, double& angle_end) = 0;

	//与伸缩相关的函数
	virtual void getScaleMode(double& scale_down, double& scale_up) = 0;
	
	//金字塔层数相关函数
	virtual void  getPyramidNum(int& nPyramidNums) = 0;

	//此函数返回目标轮廓点，pos是一个height×width的矩阵,1表示轮廓点
	virtual BOOL getEdgePositon(LPBYTE*& pos, int& height, int& width, int& startPosX, int& startPosY) = 0;

	virtual BOOL getObjPositon(int& startPosX, int& startPosY) = 0;
	virtual BOOL getObjSize(int& width, int& height) = 0;

	virtual BYTE getModelType() = 0; // SHAPE_MODEL  XLD_MODEL 
	virtual void getXLDcount(int& count, int*& pPosX, int*& pPosY) = 0;
	
	virtual void setPolarity(Polarity_state emPolarity ) = 0;
	virtual Polarity_state getPolarity() = 0;

	/////////////////////////////////////////////////////////////
	//下面的函數用於調試
	/////////////////////////////////////////////////////////////
	virtual int getPyramidLevel_debug() = 0;

};


// 图像模板配置接口
class PATTERN_RECOGNIZE_CLASS_DLL CExtNCCMode
{
public:
	static CExtNCCMode* CreateNCCModeFromFile(LPCTSTR lpcPathName);
	virtual ~CExtNCCMode();

	// 将模板保存到文件
	virtual BOOL SaveToFile(LPCTSTR lpcPathName) = 0;

	//与旋转相关的函数
	virtual void getRotateMode(double& angle_start, double& angle_end) = 0;

	//金字塔层数相关函数
	virtual void  getPyramidNum(int& nPyramidNums) = 0;

	//此函数返回目标轮廓点，pos是一个height×width的矩阵,1表示轮廓点
	virtual BOOL getEdgePositon(LPBYTE*& pos, int& height, int& width, int& startPosX, int& startPosY) = 0;

	virtual BOOL getObjPositon(int& startPosX, int& startPosY) = 0;
	virtual BOOL getObjSize(int& width, int& height) = 0;

	virtual void setPolarity(Polarity_state emPolarity) = 0;
	virtual Polarity_state getPolarity() = 0;

	/////////////////////////////////////////////////////////////
	//下面的函數用於調試
	/////////////////////////////////////////////////////////////
	virtual int getPyramidLevel_debug() = 0;

};
//********************* 图像识别接口函数********************
//
/**************************************************
CImg* create_image(CImg* img = NULL)
功能：创建一个CImg对象，并且返回指针。
         如果输入的参数为NULL，创建的CImg对象是一个没有图像数据的对象
		 如果输入的参数为有效的CImg对象指针，创建的CImg对象复制输入参数对象的图像数据
	

参数：
	INPUT：
			CImg * pImg       要复制的图像对象指针

	OUTPUT：
			NULL
	
返回值：
	如果成功返回为非0的指针；如果失败返回NULL指针
***************************************************/
PATTERN_RECOGNIZE_DLL CImg* create_image(CImg* img = NULL);
PATTERN_RECOGNIZE_DLL CImg* create_gray_image(CImg* pColorImg);

PATTERN_RECOGNIZE_DLL BOOL decompose3(CImg* MultiChannelImg,CImg*& pImg1,CImg*& pImg2,CImg*& pImg3);

PATTERN_RECOGNIZE_DLL BOOL abs_diff_image(CImg* pImg1, CImg* pImg2, CImg*& pImageAbsDiff);
PATTERN_RECOGNIZE_DLL BOOL sub_image(CImg* pImg1, CImg* pImg2, CImg*& pSubImage, float Mult, float Add);

PATTERN_RECOGNIZE_DLL BOOL extract_image(CImg* pImg, const CPoint2D& tl, const CPoint2D& rb, CImg*& pExtractImage);

/**************************************************
BOOL create_scaled_shape_model(const CImg& img, CExtShapeMode*& pshapemode, UINT1 levels=0, 
                INT2 angleStart=0, INT2 angleExtent=0, double scaleMin=1.0, double scaleMax=1.0, 
				UINT2 contrast=0, UINT2 contrastMin=0,BYTE minComponentPixs = 1)
功能：基于形状的模板创建函数

参数：
	INPUT：
			CImg& img       样本图片对象
			UINT1 levels    金字塔层数设置。如果设置为0表示由算法设置一个最优金字塔层数值
			INT2  angleStart 模板偏移的起始角度。范围-180至180
			INT2  angleExtent 模板偏移的结束角度。范围angleStart至180
			double scaleMin   模板缩小的比例。范围 0.1至1
			double scaleMax   模板放大的比例。范围1至2
			UINT2 contrast    模板目标与背景的最大对比度偏差。范围10至1000
			UINT2 contrastMin 模板目标与背景的最小对比度偏差。范围10至contrast
			BYTE minComponentPixs 模板的最小组件像素个数。缺省为1

	OUTPUT：
			CExtShapeMode*& pshapemode 模板的C++内指针。注意：返回的指针需要调用者释放内存。
	
返回值：
	如果成功返回为TRUE；如果失败返回FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL create_scaled_shape_model(const CImg& img, CExtShapeMode*& pshapemode, UINT1 levels=0, INT2 angleStart=0, INT2 angleExtent=0, float scaleMin=1.0f, float scaleMax=1.0f, UINT2 contrast=0, UINT2 contrastMin=0,BYTE minComponentPixs = 1);


PATTERN_RECOGNIZE_DLL BOOL create_scaled_shape_model_xld(CExtxld& xld, CExtShapeMode*& pshapemode, UINT1 levels=0, INT2 angleStart=0, INT2 angleExtent=0, float scaleMin=1.0f, float scaleMax=1.0f);

/**************************************************
BOOL find_scaled_shape_model(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, 
           double& Row, double& Column, double& Angle, double& Scale, double& Score, 
		   double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL)
功能：查找基于形状的模板函数（单个目标的查找）
	
参数：
	INPUT：
			CImg& img       测试图片对象
			CExtShapeMode* pshapemode 基于形状的模板指针
			int NumLevels             在金字塔层数的哪一层开始查找。如果此值大于模板的最大值，算法库选择模板的最大值
			double MinScore  查找目标时最大相似度。范围0.1至1。 1表示与模板目标完全相似
			int NumMatches	 查找目标在图片中的最大个数
			BYTE nSubPix     亚像素精度。选项包括：无、0.1精度和0.01精度
			CExtRegion* pSearchRgn	设定查找的区域，NULL表示全幅图片搜索

	OUTPUT：
			double Row	    输出目标的行位置
			double Column	输出目标的列位置
			double Angle	输出目标与模板的角度偏差
			double Scale	输出目标与模板的伸缩偏差
			double Score  	输出目标与模板相似度分数

	
返回值：
	如果成功返回为TRUE；如果失败返回FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL find_scaled_shape_model(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, double& Row, double& Column, double& Angle, double& Scale, double& Score, double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL);

/**************************************************
BOOL find_scaled_shape_model_m(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, 
                   double Row[], double Column[], double Angle[], double Scale[], double Score[], int& nCount, 
				   double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL)
功能：查找基于形状的模板函数（多个目标的查找）
	
参数：
	INPUT：
			CImg& img       测试图片对象
			CExtShapeMode* pshapemode 基于形状的模板指针
			int NumLevels             在金字塔层数的哪一层开始查找。如果此值大于模板的最大值，算法库选择模板的最大值
			double MinScore  查找目标时最大相似度。范围0.1至1。 1表示与模板目标完全相似
			int NumMatches	 查找目标在图片中的最大个数
			BYTE nSubPix     亚像素精度。选项包括：无、0.1精度和0.01精度
			CExtRegion* pSearchRgn	设定查找的区域，NULL表示全幅图片搜索

	OUTPUT：
			double Row[]	    输出目标的行位置数组
			double Column[]		输出目标的列位置数组
			double Angle[]		输出目标与模板的角度偏差数组
			double Scale[]		输出目标与模板的伸缩偏差数组
			double Score[]  	输出目标与模板相似度分数数组
			int& nCount         输出目标的个数
	
返回值：
	如果成功返回为TRUE；如果失败返回FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL find_scaled_shape_model_m(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, double Row[], double Column[], double Angle[], double Scale[], double Score[], int& nCount, double MinScore=0.5, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL);

//图像标定
/**************************************************
BOOL circle_center_extrt(const CImg& pImg, int left,int top,int right,int botton, const enum FilterName&  filter, float& center_x,float& center_y, float& radius)

功能：提取RIO区域的圆心坐标和半径。
		  用于确定标定板的圆心

参数：
	INPUT：
	CImg * pImg             标定板图片
	int left,int top		标定图片中某选择区域的左上角像素坐标
	int right,int botton	标定图片中某选择区域的右下角像素坐标
	enum FilterName&        filter	提取圆心像素坐标的算法。有canny, deriche1, deriche2, lanser1, lanser2,shen。
							目前建议使用deriche1

	OUTPUT：
	float& center_x,float& center_y   提取RIO区域的圆心坐标
	float& radius                     提取RIO区域的圆的半径
	
返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL circle_center_extrt(const CImg& pImg, int left,int top,int right,int botton, const enum FilterName&  filter, float& center_x,float& center_y, float& radius);

/**************************************************
BOOL camera_par_est(const double arr_pix_x[],const double arr_pix_y[], const double arr_world_x[],const double arr_world_y[], 
                      const int& nCount, float& theta, float& Sx,float& Sy,float& Ox,float& Oy)

功能：根据输入标定板的像素坐标和世界坐标，提取相机的相关参数

参数：
	INPUT：
		double arr_pix_x[]
		double arr_pix_y[]
		double arr_world_x[]
		double arr_world_y[]	像素X坐标数组
		像素Y坐标数组
		实际X坐标数组
		实际Y坐标数组
		int& nCount	坐标数组长度

	OUTPUT：
		float& theta   标定参数，旋转角度
		float& Sx	   标定参数，X轴伸缩比例
		float& Sy 	   标定参数，Y轴伸缩比例
		float& Ox	   标定参数，X轴偏移
		float& Oy	   标定参数，Y轴偏移
	
返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL camera_par_est(const double arr_pix_x[],const double arr_pix_y[], const double arr_world_x[],const double arr_world_y[], const int& nCount, float& theta, float& Sx,float& Sy,float& Ox,float& Oy);

/**************************************************
BOOL affine_trans_point_2d(float in_x,float in_y, const float& theta,const float& Sx,const float& Sy,const float& Ox,
                  const float& Oy, float& out_x, float& out_y)

功能：坐标转换函数。将目标的像素坐标转换为世界坐标

参数：
	INPUT：
		float in_x
		float in_y	要转换的X像素坐标
		要转换的Y像素坐标
		const float& theta	标定参数，旋转角度
		const float& Sx	标定参数，X轴伸缩比例
		const float& Sy	标定参数，Y轴伸缩比例
		const float& Ox	标定参数，X轴偏移
		const float& Oy	标定参数，Y轴偏移

  OUTPUT:
		float& out_x  转换后的实际物理X坐标
		float& out_y  转换后的实际物理Y坐标
	
返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL affine_trans_point_2d(float in_x,float in_y, const float& theta,const float& Sx,const float& Sy,const float& Ox,const float& Oy, float& out_x, float& out_y);

//区域转换
PATTERN_RECOGNIZE_DLL BOOL hom_mat2d_identity(float HomMat2DIdentity[][3]);
PATTERN_RECOGNIZE_DLL BOOL hom_mat2d_scale(const float HomMat2D[][3],const float& Sx,const float& Sy,const float& Px,const float& Py,float HomMat2DScale[][3]);
PATTERN_RECOGNIZE_DLL BOOL hom_mat2d_rotate(const float HomMat2D[][3],const float& Phi,const float& Px,const float& Py,float HomMat2DRotate[][3]);
PATTERN_RECOGNIZE_DLL BOOL hom_mat2d_translate(const float HomMat2D[][3],const float& Tx,const float& Ty,float HomMat2DTranslate[][3]);

PATTERN_RECOGNIZE_DLL BOOL affine_trans_region(const CExtRegion& region_in,CExtRegion* &region_out, const float HomMat2D[][3]);
PATTERN_RECOGNIZE_DLL BOOL vector_angle_to_rigid(float Row1,float Column1,float Angle1,float Row2,float Column2,float Angle2, float HomMat2DIdentity[][3]);
//Region
PATTERN_RECOGNIZE_DLL  BOOL reduce_domain(CImg& img,const CExtRegion& Rectangle);
PATTERN_RECOGNIZE_DLL  BOOL threshold(CImg& img, CExtRegion& RgnInterest, CExtRegion* & pOutRgn, int minGray, int maxGray );
PATTERN_RECOGNIZE_DLL  BOOL area_center(const CExtRegion& RgnInterest, int& area, int& row, int& col);
PATTERN_RECOGNIZE_DLL  BOOL connection(const CExtRegion& RgnInput, CExtRegionArray& ArrayOutRgn, int& nCount);
PATTERN_RECOGNIZE_DLL  BOOL select_shape(const CExtRegion& RgnInput, CExtRegion*& pSelectRegion, int AreaMin, int AreaMax);
PATTERN_RECOGNIZE_DLL  BOOL select_shape1(const CExtRegionArray& ArrayRgn, CExtRegion*& pSelectRegion, int AreaMin, int AreaMax);
PATTERN_RECOGNIZE_DLL  BOOL closing_circle(const CExtRegion& RgnInput, CExtRegion*& pRegionClosing, BYTE Radius);
PATTERN_RECOGNIZE_DLL  BOOL fill_up(const CExtRegion& RgnInput, CExtRegion*& pRegionFillUp);
PATTERN_RECOGNIZE_DLL  BOOL erosion_pix(const CExtRegion& RgnInput, CExtRegion*& pRegionErosion, BYTE pix_shrink);
PATTERN_RECOGNIZE_DLL  BOOL dilation_pix(const CExtRegion& RgnInput, CExtRegion*& pRegionDilation, BYTE pix_shrink);
PATTERN_RECOGNIZE_DLL  BOOL closing_circle1(const CExtRegion& RgnInput, CExtRegion*& pRegionClosing, BYTE Radius);
PATTERN_RECOGNIZE_DLL  BOOL opening_circle(const CExtRegion& RgnInput, CExtRegion*& pRegionOpening, BYTE Radius);

//brob
PATTERN_RECOGNIZE_DLL BOOL smallest_circle(const CExtRegion& region_in, float& dRow, float& dColumn, float& dRadius);

PATTERN_RECOGNIZE_DLL BOOL rectangle2binregion(const int& CenPosX,const int& CenPosY, const double& Phi, const int& LongEdge, const int& ShortEdge, BYTE* region_BinImage, int regHight, int regWith, int startX, int startY );
PATTERN_RECOGNIZE_DLL BOOL ellipse2binregion(const int& CenterPosX,const int& CenterPosY, const double& Phi, const int& LongEdge, const int& ShortEdge, BYTE* region_BinImage, int regHight, int regWith, int startX, int startY );

//1D 测量相关的函数
PATTERN_RECOGNIZE_DLL BOOL measure(const CImg& pImg, float fSigma,int nThreshold ,double cenX,double cenY,double dPhi,int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation,int& nCnt, float*& pColEdge, float*& pRowEdge,	float*& pAmplitude,	float*& pDistance);

/**************************************************
BOOL measure1(const CImg& pImg, float fSigma,int nThreshold ,int x1,int y1,int x2,int y2,int nRIOwidth, 
enum measure_interpolation  eInterpolation,int& nCnt, float*& pColEdge, float*& pRowEdge,	float*& pAmplitude,	float*& pDistance)

功能：1D测量函数

参数：
	INPUT：
		const CImg& pImg 输入图片对象
		float fSigma     用于一维信号平滑的高斯平滑滤波器的标准差
		int nThreshold   用于从一维梯度信号中提取边缘的门限
		int x1,int y1    测量线段的起始点像素坐标
		int x2,int y2    测量线段的终点像素坐标
		int nRIOwidth    参与测量计算的宽度值
		enum measure_interpolation  eInterpolation	插值算法：'nearest_neighbor', 最近点
		                                                     'bilinear'        , 双线性插值                
															 'bicubic'         , 双立法插值    
															 'facet'             facet模型

   OUTPUT:
		int& nCnt	边缘点的个数
		float*& pColEdge	每个边缘点的列像素坐标
		float*& pRowEdge	每个边缘点的行像素坐标
		float*& pAmplitude	每个边缘点的梯度幅度（带符号）
		float*& pDistance	边缘点之间的距离

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL measure1(const CImg& pImg, float fSigma, int nThreshold, int x1, int y1, int x2, int y2, int nRIOwidth, enum measure_Transition Transition, enum measure_interpolation  eInterpolation, int& nCnt, float*& pColEdge, float*& pRowEdge, float*& pAmplitude, float*& pDistance);


/**************************************************
BOOL measure_pos(int& nCnt,float*& pColEdge, float*& pRowEdge,float*& pAmplitude,float*& pDistance,
const MeasureHandle& hMeasure, float fSigma, int nThreshold,enum measure_Transition Transition, enum measure_select select)
功能：1D测量点的获取

参数：
	INPUT：
		const MeasureHandle& hMeasure 测量参数句柄
		float fSigma                  用于一维信号平滑的高斯平滑滤波器的标准差
		int nThreshold                用于从一维梯度信号中提取边缘的门限
		enum measure_Transition Transition   提取边缘的过度方式  'position': 表示基于测量方向，由黑到白的边缘
																'negative': 表示基于测量方向，由白到黑的边缘  
																'all'     : 所有类型的边缘


		enum measure_select select           边缘点个数的选      'first'   : 表示基于测量方向，检测到的第一个边缘  
		                                                        'last'    : 表示基于测量方向，检测到的最后一个边缘 
																'all'     : 检测到的所有边缘



   OUTPUT:
		int& nCnt	边缘点的个数
		float*& pColEdge	每个边缘点的列像素坐标
		float*& pRowEdge	每个边缘点的行像素坐标
		float*& pAmplitude	每个边缘点的梯度幅度（带符号）
		float*& pDistance	边缘点之间的距离


返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL measure_pos(int& nCnt,float*& pColEdge, float*& pRowEdge,float*& pAmplitude,float*& pDistance,const MeasureHandle& hMeasure, float fSigma, int nThreshold,enum measure_Transition Transition, enum measure_select select);

/**************************************************
BOOL gen_measure_rectangle2(MeasureHandle& hMeasure, const CImg& pImg, double cenX,double cenY,double dPhi,
                                   int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation)
功能：获取测量矩形区域的点测量句柄参数

参数：
	INPUT：
		const CImg& pImg 输入图片对象
		double cenX,double cenY,double dPhi,int nLongEdge  测量线段描述（圆心，角度和长度）
		int nShortEdge                                     参与测量计算的宽度值
		enum measure_interpolation  eInterpolation	插值算法：'nearest_neighbor', 最近点
		                                                     'bilinear',         双线性插值
															 'bicubic',          双立法插值   
															 'facet'             facet模型
   OUTPUT:
		MeasureHandle& hMeasure	测量参数

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL gen_measure_rectangle2(MeasureHandle& hMeasure, const CImg& pImg, double cenX,double cenY,double dPhi,int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation);

/**************************************************
BOOL gen_measure_rectangle3(MeasureHandle& hMeasure, const CImg& pImg, int x1,int y1,int x2,int y2, int nShortEdge, 
enum measure_interpolation  eInterpolation)
功能：获取测量矩形区域的点测量句柄参数

参数：
	INPUT：
		const CImg& pImg             输入图片对象
		int x1,int y1,int x2,int y2  测量线段描述（起点和终点）
		int nShortEdge                                     参与测量计算的宽度值
		enum measure_interpolation  eInterpolation	插值算法：'nearest_neighbor', 最近点
		                                                     'bilinear',         双线性插值
															 'bicubic',          双立法插值   
															 'facet'             facet模型
   OUTPUT:
		MeasureHandle& hMeasure	测量参数

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL gen_measure_rectangle3(MeasureHandle& hMeasure, const CImg& pImg, int x1,int y1,int x2,int y2, int nShortEdge, enum measure_interpolation  eInterpolation);

//直线拟合
/**************************************************
BOOL linear_fit_ransac(float* fPointSeqX, float* fPointSeqY, int num_point, float& StrPosX,float& StrPosY,float& EndPosX,float& EndPosY)
功能：直线拟合函数

参数：
	INPUT：
		float* fPointSeqX   点数组的X像素坐标
		float* fPointSeqY   点数组的Y像素坐标
		int num_point       输入的点个数

   OUTPUT:
		float& StrPosX,float& StrPosY	线段的起始点像素坐标
		float& EndPosX,float& EndPosY	线段的结束点像素坐标


返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL linear_fit_ransac(float* fPointSeqX, float* fPointSeqY, int num_point, float& StrPosX, float& StrPosY, float& EndPosX, float& EndPosY);

PATTERN_RECOGNIZE_DLL BOOL fit_circle(P_FLOAT_POS& edge_point_sub_pix,const int& NumPoints, float& center_x, float& center_y, float& radius);

//点线关系函数
/**************************************************
BOOL angle_ll(int c11, int r11, int c12, int r12,int c21, int r21, int c22, int r22, double& angle)
功能：直线与直线的夹角函数

参数：
	INPUT：
		int c11, int r11, int c12, int r12		线条1的点坐标
		int c21, int r21, int c22, int r22      线条2的点坐标

   OUTPUT:
		double& angle	两条线的夹角

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL angle_ll(double c11, double r11, double c12, double r12,double c21, double r21, double c22, double r22, double& angle);

/**************************************************
BOOL cross_ll(int c11, int r11, int c12, int r12,int c21, int r21, int c22, int r22, double& c_cross, double& r_cross)
功能：直线与直线的交点函数

参数：
	INPUT：
		int c11, int r11, int c12, int r12		线条1的点坐标
		int c21, int r21, int c22, int r22      线条2的点坐标

   OUTPUT:
		double& c_cross, double& r_cross	两条线的交点坐标

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL cross_ll(double c11, double r11, double c12, double r12,double c21, double r21, double c22, double r22, double& c_cross, double& r_cross);
PATTERN_RECOGNIZE_DLL BOOL angle_lx(double c11, double r11, double c12, double r12, double& angle);
PATTERN_RECOGNIZE_DLL double distance_pp(float c1, float r1, float c2, float r2);
PATTERN_RECOGNIZE_DLL double distance_pl(double c11, double r11, double c12, double r12, double px, double py);
PATTERN_RECOGNIZE_DLL void projection_pl(double px, double py,double line_s_c, double line_s_r, double line_e_c, double line_e_r,double& projectX, double& projectY);
PATTERN_RECOGNIZE_DLL void mirror_pl(double px, double py,double line_s_c, double line_s_r, double line_e_c, double line_e_r,double& mirrorX, double& mirrorY);

//图像预处理
PATTERN_RECOGNIZE_DLL BOOL mean_image(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, BYTE MaskWidth, BYTE MaskHeight);
PATTERN_RECOGNIZE_DLL BOOL scale_image(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, double multi, double add);
PATTERN_RECOGNIZE_DLL BOOL emphasize(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, BYTE MaskWidth, BYTE MaskHeight, double factor);
//其他
/**************************************************
BOOL intensity(const CImg& img, const CExtRegion& rgn, double& mean, double& deviation)
功能：选择区域的灰度平均值计算

参数：
	INPUT：
		const CImg& pImg             输入图片对象
		const CExtRegion& rgn        需要计算的区域

   OUTPUT:
		double& mean                 选择区域部分图像的灰度平均值

返回值：TRUE  成功;   FALSE 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL intensity(const CImg& img, const CExtRegion& rgn, double& mean, double& deviation);


//xld support
PATTERN_RECOGNIZE_DLL BOOL gen_rectangle2_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);
PATTERN_RECOGNIZE_DLL BOOL gen_ellipse_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);
//PATTERN_RECOGNIZE_DLL BOOL gen_rectangle_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);

//ncc
PATTERN_RECOGNIZE_DLL BOOL create_ncc_model(const CImg& img, CExtNCCMode*& pNCCmode, UINT1 levels, INT2 angleStart, INT2 angleExtent);
PATTERN_RECOGNIZE_DLL BOOL find_NCC_model(const CImg& Image, CExtNCCMode* pnccmode, int NumLevels, double Row[], double Column[], double Angle[], double Score[], int& nCount, double MinScore, BYTE nSubPix, CExtRegion* pSearchRgn);


//畸变矫正
PATTERN_RECOGNIZE_DLL bool CrdRadialDistCrt2(double row_in, double cel_in, double K, double Alpha, double& row_out, double& cel_out);

//
struct distortion_param
{
	double Phi_r_est;   //梯形畸变调整参数
	double Phi_c_est;   //
	double Distance_est;//
	double K_est;       //径向畸变调整参数
	double Alpha_est;   //
	double Phi_p_est;   //平行四边形调整参数
	double Theta_est;   //
	double Sr_est;      //x，y缩放参数
	double Sc_est;      //
};
/**************************************************
功能：自动参数估计。根据输入的各点之间的距离，自动给出合适的调整参数。
     注意： 各点之间的距离不能够混淆

参数：      A        F         B
            -------------------
            |        |        | 
            |        |        |
            |        |        |
        E   ---------O---------G
            |        |        |
            |        |        |
            |        |        |
            --------------------
            D        H         C
	INPUT：
		double AO, double BO,double CO,double DO,double EO,double FO,double GO,double HO,   A～H点到中心点O点的距离
		double AE,double AF,double BF,double BG,double CG, double CH, double DH,double DE,  各点之间的距离
		double Or, double Oc                                                                中心点偏差
   OUTPUT:                                      输出畸变调整参数
		distortion_param& param
返回值：true  成功;   false 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL bool auto_par_est(distortion_param& param, \
double AO, double BO,double CO,double DO,double EO,double FO,double GO,double HO,double AE,double AF,double BF,double BG,double CG, double CH, double DH,double DE, double Or, double Oc);


/**************************************************
功能：坐标点畸变矫正函数

参数：     
	INPUT：
	    double pX, double pY       需要矫正的点坐标
		distortion_param& param     畸变调整参数
		double CenterRow,  double CenterCol 中心原点的坐标值
		double Or, double Oc                中心原点偏移值
   OUTPUT:                            
		
返回值：true  成功;   false 失败
	
***************************************************/
PATTERN_RECOGNIZE_DLL bool coordinate_correct(double& cel_out, double& row_out, double cel_in, double row_in, const distortion_param& param,double CenterRow,  double CenterCol, double Or, double Oc );



#endif
