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

// ����������
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

// ͼ��ģ�����ýӿ�
class PATTERN_RECOGNIZE_CLASS_DLL CExtShapeMode
{
public:	
	static CExtShapeMode* CreateShapeModeFromFile(LPCTSTR lpcPathName);
	virtual ~CExtShapeMode();

	// ��ģ�屣�浽�ļ�
	virtual BOOL SaveToFile(LPCTSTR lpcPathName) = 0;

	//��ȡ����ģ��ʹ�õ�ͼ��Աȶ�����ֵ
	virtual void getContrastThreshold(int& contrastH, int& contrastL) = 0;
	
	//����ת��صĺ���
	virtual void getRotateMode(double& angle_start, double& angle_end) = 0;

	//��������صĺ���
	virtual void getScaleMode(double& scale_down, double& scale_up) = 0;
	
	//������������غ���
	virtual void  getPyramidNum(int& nPyramidNums) = 0;

	//�˺�������Ŀ�������㣬pos��һ��height��width�ľ���,1��ʾ������
	virtual BOOL getEdgePositon(LPBYTE*& pos, int& height, int& width, int& startPosX, int& startPosY) = 0;

	virtual BOOL getObjPositon(int& startPosX, int& startPosY) = 0;
	virtual BOOL getObjSize(int& width, int& height) = 0;

	virtual BYTE getModelType() = 0; // SHAPE_MODEL  XLD_MODEL 
	virtual void getXLDcount(int& count, int*& pPosX, int*& pPosY) = 0;
	
	virtual void setPolarity(Polarity_state emPolarity ) = 0;
	virtual Polarity_state getPolarity() = 0;

	/////////////////////////////////////////////////////////////
	//����ĺ�������{ԇ
	/////////////////////////////////////////////////////////////
	virtual int getPyramidLevel_debug() = 0;

};


// ͼ��ģ�����ýӿ�
class PATTERN_RECOGNIZE_CLASS_DLL CExtNCCMode
{
public:
	static CExtNCCMode* CreateNCCModeFromFile(LPCTSTR lpcPathName);
	virtual ~CExtNCCMode();

	// ��ģ�屣�浽�ļ�
	virtual BOOL SaveToFile(LPCTSTR lpcPathName) = 0;

	//����ת��صĺ���
	virtual void getRotateMode(double& angle_start, double& angle_end) = 0;

	//������������غ���
	virtual void  getPyramidNum(int& nPyramidNums) = 0;

	//�˺�������Ŀ�������㣬pos��һ��height��width�ľ���,1��ʾ������
	virtual BOOL getEdgePositon(LPBYTE*& pos, int& height, int& width, int& startPosX, int& startPosY) = 0;

	virtual BOOL getObjPositon(int& startPosX, int& startPosY) = 0;
	virtual BOOL getObjSize(int& width, int& height) = 0;

	virtual void setPolarity(Polarity_state emPolarity) = 0;
	virtual Polarity_state getPolarity() = 0;

	/////////////////////////////////////////////////////////////
	//����ĺ�������{ԇ
	/////////////////////////////////////////////////////////////
	virtual int getPyramidLevel_debug() = 0;

};
//********************* ͼ��ʶ��ӿں���********************
//
/**************************************************
CImg* create_image(CImg* img = NULL)
���ܣ�����һ��CImg���󣬲��ҷ���ָ�롣
         �������Ĳ���ΪNULL��������CImg������һ��û��ͼ�����ݵĶ���
		 �������Ĳ���Ϊ��Ч��CImg����ָ�룬������CImg������������������ͼ������
	

������
	INPUT��
			CImg * pImg       Ҫ���Ƶ�ͼ�����ָ��

	OUTPUT��
			NULL
	
����ֵ��
	����ɹ�����Ϊ��0��ָ�룻���ʧ�ܷ���NULLָ��
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
���ܣ�������״��ģ�崴������

������
	INPUT��
			CImg& img       ����ͼƬ����
			UINT1 levels    �������������á��������Ϊ0��ʾ���㷨����һ�����Ž���������ֵ
			INT2  angleStart ģ��ƫ�Ƶ���ʼ�Ƕȡ���Χ-180��180
			INT2  angleExtent ģ��ƫ�ƵĽ����Ƕȡ���ΧangleStart��180
			double scaleMin   ģ����С�ı�������Χ 0.1��1
			double scaleMax   ģ��Ŵ�ı�������Χ1��2
			UINT2 contrast    ģ��Ŀ���뱳�������Աȶ�ƫ���Χ10��1000
			UINT2 contrastMin ģ��Ŀ���뱳������С�Աȶ�ƫ���Χ10��contrast
			BYTE minComponentPixs ģ�����С������ظ�����ȱʡΪ1

	OUTPUT��
			CExtShapeMode*& pshapemode ģ���C++��ָ�롣ע�⣺���ص�ָ����Ҫ�������ͷ��ڴ档
	
����ֵ��
	����ɹ�����ΪTRUE�����ʧ�ܷ���FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL create_scaled_shape_model(const CImg& img, CExtShapeMode*& pshapemode, UINT1 levels=0, INT2 angleStart=0, INT2 angleExtent=0, float scaleMin=1.0f, float scaleMax=1.0f, UINT2 contrast=0, UINT2 contrastMin=0,BYTE minComponentPixs = 1);


PATTERN_RECOGNIZE_DLL BOOL create_scaled_shape_model_xld(CExtxld& xld, CExtShapeMode*& pshapemode, UINT1 levels=0, INT2 angleStart=0, INT2 angleExtent=0, float scaleMin=1.0f, float scaleMax=1.0f);

/**************************************************
BOOL find_scaled_shape_model(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, 
           double& Row, double& Column, double& Angle, double& Scale, double& Score, 
		   double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL)
���ܣ����һ�����״��ģ�庯��������Ŀ��Ĳ��ң�
	
������
	INPUT��
			CImg& img       ����ͼƬ����
			CExtShapeMode* pshapemode ������״��ģ��ָ��
			int NumLevels             �ڽ�������������һ�㿪ʼ���ҡ������ֵ����ģ������ֵ���㷨��ѡ��ģ������ֵ
			double MinScore  ����Ŀ��ʱ������ƶȡ���Χ0.1��1�� 1��ʾ��ģ��Ŀ����ȫ����
			int NumMatches	 ����Ŀ����ͼƬ�е�������
			BYTE nSubPix     �����ؾ��ȡ�ѡ��������ޡ�0.1���Ⱥ�0.01����
			CExtRegion* pSearchRgn	�趨���ҵ�����NULL��ʾȫ��ͼƬ����

	OUTPUT��
			double Row	    ���Ŀ�����λ��
			double Column	���Ŀ�����λ��
			double Angle	���Ŀ����ģ��ĽǶ�ƫ��
			double Scale	���Ŀ����ģ�������ƫ��
			double Score  	���Ŀ����ģ�����ƶȷ���

	
����ֵ��
	����ɹ�����ΪTRUE�����ʧ�ܷ���FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL find_scaled_shape_model(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, double& Row, double& Column, double& Angle, double& Scale, double& Score, double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL);

/**************************************************
BOOL find_scaled_shape_model_m(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, 
                   double Row[], double Column[], double Angle[], double Scale[], double Score[], int& nCount, 
				   double MinScore=0.5, int NumMatches=1, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL)
���ܣ����һ�����״��ģ�庯�������Ŀ��Ĳ��ң�
	
������
	INPUT��
			CImg& img       ����ͼƬ����
			CExtShapeMode* pshapemode ������״��ģ��ָ��
			int NumLevels             �ڽ�������������һ�㿪ʼ���ҡ������ֵ����ģ������ֵ���㷨��ѡ��ģ������ֵ
			double MinScore  ����Ŀ��ʱ������ƶȡ���Χ0.1��1�� 1��ʾ��ģ��Ŀ����ȫ����
			int NumMatches	 ����Ŀ����ͼƬ�е�������
			BYTE nSubPix     �����ؾ��ȡ�ѡ��������ޡ�0.1���Ⱥ�0.01����
			CExtRegion* pSearchRgn	�趨���ҵ�����NULL��ʾȫ��ͼƬ����

	OUTPUT��
			double Row[]	    ���Ŀ�����λ������
			double Column[]		���Ŀ�����λ������
			double Angle[]		���Ŀ����ģ��ĽǶ�ƫ������
			double Scale[]		���Ŀ����ģ�������ƫ������
			double Score[]  	���Ŀ����ģ�����ƶȷ�������
			int& nCount         ���Ŀ��ĸ���
	
����ֵ��
	����ɹ�����ΪTRUE�����ʧ�ܷ���FALSE
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL find_scaled_shape_model_m(const CImg& Image, CExtShapeMode* pshapemode, int NumLevels, double Row[], double Column[], double Angle[], double Scale[], double Score[], int& nCount, double MinScore=0.5, BYTE nSubPix = 0, CExtRegion* pSearchRgn = NULL);

//ͼ��궨
/**************************************************
BOOL circle_center_extrt(const CImg& pImg, int left,int top,int right,int botton, const enum FilterName&  filter, float& center_x,float& center_y, float& radius)

���ܣ���ȡRIO�����Բ������Ͱ뾶��
		  ����ȷ���궨���Բ��

������
	INPUT��
	CImg * pImg             �궨��ͼƬ
	int left,int top		�궨ͼƬ��ĳѡ����������Ͻ���������
	int right,int botton	�궨ͼƬ��ĳѡ����������½���������
	enum FilterName&        filter	��ȡԲ������������㷨����canny, deriche1, deriche2, lanser1, lanser2,shen��
							Ŀǰ����ʹ��deriche1

	OUTPUT��
	float& center_x,float& center_y   ��ȡRIO�����Բ������
	float& radius                     ��ȡRIO�����Բ�İ뾶
	
����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL circle_center_extrt(const CImg& pImg, int left,int top,int right,int botton, const enum FilterName&  filter, float& center_x,float& center_y, float& radius);

/**************************************************
BOOL camera_par_est(const double arr_pix_x[],const double arr_pix_y[], const double arr_world_x[],const double arr_world_y[], 
                      const int& nCount, float& theta, float& Sx,float& Sy,float& Ox,float& Oy)

���ܣ���������궨�������������������꣬��ȡ�������ز���

������
	INPUT��
		double arr_pix_x[]
		double arr_pix_y[]
		double arr_world_x[]
		double arr_world_y[]	����X��������
		����Y��������
		ʵ��X��������
		ʵ��Y��������
		int& nCount	�������鳤��

	OUTPUT��
		float& theta   �궨��������ת�Ƕ�
		float& Sx	   �궨������X����������
		float& Sy 	   �궨������Y����������
		float& Ox	   �궨������X��ƫ��
		float& Oy	   �궨������Y��ƫ��
	
����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL camera_par_est(const double arr_pix_x[],const double arr_pix_y[], const double arr_world_x[],const double arr_world_y[], const int& nCount, float& theta, float& Sx,float& Sy,float& Ox,float& Oy);

/**************************************************
BOOL affine_trans_point_2d(float in_x,float in_y, const float& theta,const float& Sx,const float& Sy,const float& Ox,
                  const float& Oy, float& out_x, float& out_y)

���ܣ�����ת����������Ŀ�����������ת��Ϊ��������

������
	INPUT��
		float in_x
		float in_y	Ҫת����X��������
		Ҫת����Y��������
		const float& theta	�궨��������ת�Ƕ�
		const float& Sx	�궨������X����������
		const float& Sy	�궨������Y����������
		const float& Ox	�궨������X��ƫ��
		const float& Oy	�궨������Y��ƫ��

  OUTPUT:
		float& out_x  ת�����ʵ������X����
		float& out_y  ת�����ʵ������Y����
	
����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL affine_trans_point_2d(float in_x,float in_y, const float& theta,const float& Sx,const float& Sy,const float& Ox,const float& Oy, float& out_x, float& out_y);

//����ת��
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

//1D ������صĺ���
PATTERN_RECOGNIZE_DLL BOOL measure(const CImg& pImg, float fSigma,int nThreshold ,double cenX,double cenY,double dPhi,int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation,int& nCnt, float*& pColEdge, float*& pRowEdge,	float*& pAmplitude,	float*& pDistance);

/**************************************************
BOOL measure1(const CImg& pImg, float fSigma,int nThreshold ,int x1,int y1,int x2,int y2,int nRIOwidth, 
enum measure_interpolation  eInterpolation,int& nCnt, float*& pColEdge, float*& pRowEdge,	float*& pAmplitude,	float*& pDistance)

���ܣ�1D��������

������
	INPUT��
		const CImg& pImg ����ͼƬ����
		float fSigma     ����һά�ź�ƽ���ĸ�˹ƽ���˲����ı�׼��
		int nThreshold   ���ڴ�һά�ݶ��ź�����ȡ��Ե������
		int x1,int y1    �����߶ε���ʼ����������
		int x2,int y2    �����߶ε��յ���������
		int nRIOwidth    �����������Ŀ��ֵ
		enum measure_interpolation  eInterpolation	��ֵ�㷨��'nearest_neighbor', �����
		                                                     'bilinear'        , ˫���Բ�ֵ                
															 'bicubic'         , ˫������ֵ    
															 'facet'             facetģ��

   OUTPUT:
		int& nCnt	��Ե��ĸ���
		float*& pColEdge	ÿ����Ե�������������
		float*& pRowEdge	ÿ����Ե�������������
		float*& pAmplitude	ÿ����Ե����ݶȷ��ȣ������ţ�
		float*& pDistance	��Ե��֮��ľ���

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL measure1(const CImg& pImg, float fSigma, int nThreshold, int x1, int y1, int x2, int y2, int nRIOwidth, enum measure_Transition Transition, enum measure_interpolation  eInterpolation, int& nCnt, float*& pColEdge, float*& pRowEdge, float*& pAmplitude, float*& pDistance);


/**************************************************
BOOL measure_pos(int& nCnt,float*& pColEdge, float*& pRowEdge,float*& pAmplitude,float*& pDistance,
const MeasureHandle& hMeasure, float fSigma, int nThreshold,enum measure_Transition Transition, enum measure_select select)
���ܣ�1D������Ļ�ȡ

������
	INPUT��
		const MeasureHandle& hMeasure �����������
		float fSigma                  ����һά�ź�ƽ���ĸ�˹ƽ���˲����ı�׼��
		int nThreshold                ���ڴ�һά�ݶ��ź�����ȡ��Ե������
		enum measure_Transition Transition   ��ȡ��Ե�Ĺ��ȷ�ʽ  'position': ��ʾ���ڲ��������ɺڵ��׵ı�Ե
																'negative': ��ʾ���ڲ��������ɰ׵��ڵı�Ե  
																'all'     : �������͵ı�Ե


		enum measure_select select           ��Ե�������ѡ      'first'   : ��ʾ���ڲ������򣬼�⵽�ĵ�һ����Ե  
		                                                        'last'    : ��ʾ���ڲ������򣬼�⵽�����һ����Ե 
																'all'     : ��⵽�����б�Ե



   OUTPUT:
		int& nCnt	��Ե��ĸ���
		float*& pColEdge	ÿ����Ե�������������
		float*& pRowEdge	ÿ����Ե�������������
		float*& pAmplitude	ÿ����Ե����ݶȷ��ȣ������ţ�
		float*& pDistance	��Ե��֮��ľ���


����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL measure_pos(int& nCnt,float*& pColEdge, float*& pRowEdge,float*& pAmplitude,float*& pDistance,const MeasureHandle& hMeasure, float fSigma, int nThreshold,enum measure_Transition Transition, enum measure_select select);

/**************************************************
BOOL gen_measure_rectangle2(MeasureHandle& hMeasure, const CImg& pImg, double cenX,double cenY,double dPhi,
                                   int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation)
���ܣ���ȡ������������ĵ�����������

������
	INPUT��
		const CImg& pImg ����ͼƬ����
		double cenX,double cenY,double dPhi,int nLongEdge  �����߶�������Բ�ģ��ǶȺͳ��ȣ�
		int nShortEdge                                     �����������Ŀ��ֵ
		enum measure_interpolation  eInterpolation	��ֵ�㷨��'nearest_neighbor', �����
		                                                     'bilinear',         ˫���Բ�ֵ
															 'bicubic',          ˫������ֵ   
															 'facet'             facetģ��
   OUTPUT:
		MeasureHandle& hMeasure	��������

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL gen_measure_rectangle2(MeasureHandle& hMeasure, const CImg& pImg, double cenX,double cenY,double dPhi,int nLongEdge, int nShortEdge, enum measure_interpolation  eInterpolation);

/**************************************************
BOOL gen_measure_rectangle3(MeasureHandle& hMeasure, const CImg& pImg, int x1,int y1,int x2,int y2, int nShortEdge, 
enum measure_interpolation  eInterpolation)
���ܣ���ȡ������������ĵ�����������

������
	INPUT��
		const CImg& pImg             ����ͼƬ����
		int x1,int y1,int x2,int y2  �����߶������������յ㣩
		int nShortEdge                                     �����������Ŀ��ֵ
		enum measure_interpolation  eInterpolation	��ֵ�㷨��'nearest_neighbor', �����
		                                                     'bilinear',         ˫���Բ�ֵ
															 'bicubic',          ˫������ֵ   
															 'facet'             facetģ��
   OUTPUT:
		MeasureHandle& hMeasure	��������

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL gen_measure_rectangle3(MeasureHandle& hMeasure, const CImg& pImg, int x1,int y1,int x2,int y2, int nShortEdge, enum measure_interpolation  eInterpolation);

//ֱ�����
/**************************************************
BOOL linear_fit_ransac(float* fPointSeqX, float* fPointSeqY, int num_point, float& StrPosX,float& StrPosY,float& EndPosX,float& EndPosY)
���ܣ�ֱ����Ϻ���

������
	INPUT��
		float* fPointSeqX   �������X��������
		float* fPointSeqY   �������Y��������
		int num_point       ����ĵ����

   OUTPUT:
		float& StrPosX,float& StrPosY	�߶ε���ʼ����������
		float& EndPosX,float& EndPosY	�߶εĽ�������������


����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL linear_fit_ransac(float* fPointSeqX, float* fPointSeqY, int num_point, float& StrPosX, float& StrPosY, float& EndPosX, float& EndPosY);

PATTERN_RECOGNIZE_DLL BOOL fit_circle(P_FLOAT_POS& edge_point_sub_pix,const int& NumPoints, float& center_x, float& center_y, float& radius);

//���߹�ϵ����
/**************************************************
BOOL angle_ll(int c11, int r11, int c12, int r12,int c21, int r21, int c22, int r22, double& angle)
���ܣ�ֱ����ֱ�ߵļнǺ���

������
	INPUT��
		int c11, int r11, int c12, int r12		����1�ĵ�����
		int c21, int r21, int c22, int r22      ����2�ĵ�����

   OUTPUT:
		double& angle	�����ߵļн�

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL angle_ll(double c11, double r11, double c12, double r12,double c21, double r21, double c22, double r22, double& angle);

/**************************************************
BOOL cross_ll(int c11, int r11, int c12, int r12,int c21, int r21, int c22, int r22, double& c_cross, double& r_cross)
���ܣ�ֱ����ֱ�ߵĽ��㺯��

������
	INPUT��
		int c11, int r11, int c12, int r12		����1�ĵ�����
		int c21, int r21, int c22, int r22      ����2�ĵ�����

   OUTPUT:
		double& c_cross, double& r_cross	�����ߵĽ�������

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL cross_ll(double c11, double r11, double c12, double r12,double c21, double r21, double c22, double r22, double& c_cross, double& r_cross);
PATTERN_RECOGNIZE_DLL BOOL angle_lx(double c11, double r11, double c12, double r12, double& angle);
PATTERN_RECOGNIZE_DLL double distance_pp(float c1, float r1, float c2, float r2);
PATTERN_RECOGNIZE_DLL double distance_pl(double c11, double r11, double c12, double r12, double px, double py);
PATTERN_RECOGNIZE_DLL void projection_pl(double px, double py,double line_s_c, double line_s_r, double line_e_c, double line_e_r,double& projectX, double& projectY);
PATTERN_RECOGNIZE_DLL void mirror_pl(double px, double py,double line_s_c, double line_s_r, double line_e_c, double line_e_r,double& mirrorX, double& mirrorY);

//ͼ��Ԥ����
PATTERN_RECOGNIZE_DLL BOOL mean_image(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, BYTE MaskWidth, BYTE MaskHeight);
PATTERN_RECOGNIZE_DLL BOOL scale_image(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, double multi, double add);
PATTERN_RECOGNIZE_DLL BOOL emphasize(CImg& img, int topleft_x,int topleft_y, int rightbottom_x,int rightbottom_y, BYTE MaskWidth, BYTE MaskHeight, double factor);
//����
/**************************************************
BOOL intensity(const CImg& img, const CExtRegion& rgn, double& mean, double& deviation)
���ܣ�ѡ������ĻҶ�ƽ��ֵ����

������
	INPUT��
		const CImg& pImg             ����ͼƬ����
		const CExtRegion& rgn        ��Ҫ���������

   OUTPUT:
		double& mean                 ѡ�����򲿷�ͼ��ĻҶ�ƽ��ֵ

����ֵ��TRUE  �ɹ�;   FALSE ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL BOOL intensity(const CImg& img, const CExtRegion& rgn, double& mean, double& deviation);


//xld support
PATTERN_RECOGNIZE_DLL BOOL gen_rectangle2_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);
PATTERN_RECOGNIZE_DLL BOOL gen_ellipse_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);
//PATTERN_RECOGNIZE_DLL BOOL gen_rectangle_contour_xld(CExtxld& contourXLD, int CentrRow, int CentrCol, double Phi, int Length1, int Length2);

//ncc
PATTERN_RECOGNIZE_DLL BOOL create_ncc_model(const CImg& img, CExtNCCMode*& pNCCmode, UINT1 levels, INT2 angleStart, INT2 angleExtent);
PATTERN_RECOGNIZE_DLL BOOL find_NCC_model(const CImg& Image, CExtNCCMode* pnccmode, int NumLevels, double Row[], double Column[], double Angle[], double Score[], int& nCount, double MinScore, BYTE nSubPix, CExtRegion* pSearchRgn);


//�������
PATTERN_RECOGNIZE_DLL bool CrdRadialDistCrt2(double row_in, double cel_in, double K, double Alpha, double& row_out, double& cel_out);

//
struct distortion_param
{
	double Phi_r_est;   //���λ����������
	double Phi_c_est;   //
	double Distance_est;//
	double K_est;       //��������������
	double Alpha_est;   //
	double Phi_p_est;   //ƽ���ı��ε�������
	double Theta_est;   //
	double Sr_est;      //x��y���Ų���
	double Sc_est;      //
};
/**************************************************
���ܣ��Զ��������ơ���������ĸ���֮��ľ��룬�Զ��������ʵĵ���������
     ע�⣺ ����֮��ľ��벻�ܹ�����

������      A        F         B
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
	INPUT��
		double AO, double BO,double CO,double DO,double EO,double FO,double GO,double HO,   A��H�㵽���ĵ�O��ľ���
		double AE,double AF,double BF,double BG,double CG, double CH, double DH,double DE,  ����֮��ľ���
		double Or, double Oc                                                                ���ĵ�ƫ��
   OUTPUT:                                      ��������������
		distortion_param& param
����ֵ��true  �ɹ�;   false ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL bool auto_par_est(distortion_param& param, \
double AO, double BO,double CO,double DO,double EO,double FO,double GO,double HO,double AE,double AF,double BF,double BG,double CG, double CH, double DH,double DE, double Or, double Oc);


/**************************************************
���ܣ����������������

������     
	INPUT��
	    double pX, double pY       ��Ҫ�����ĵ�����
		distortion_param& param     �����������
		double CenterRow,  double CenterCol ����ԭ�������ֵ
		double Or, double Oc                ����ԭ��ƫ��ֵ
   OUTPUT:                            
		
����ֵ��true  �ɹ�;   false ʧ��
	
***************************************************/
PATTERN_RECOGNIZE_DLL bool coordinate_correct(double& cel_out, double& row_out, double cel_in, double row_in, const distortion_param& param,double CenterRow,  double CenterCol, double Or, double Oc );



#endif
