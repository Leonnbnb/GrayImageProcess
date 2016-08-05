#ifndef _EXTERN_GRAPHICS_INTERFACE_H_
#define _EXTERN_GRAPHICS_INTERFACE_H_

#include "../shared/Img.h"
#include "../shared/List.h"
#include "../shared/dataType.h"
#include "..\\shared\\ExtPatternRecognize.h"

#pragma  comment(lib, "HawkCommon.lib")
#pragma  comment(lib, "hawkvis.lib")
//#pragma comment(lib,"patternRecognize.lib") 
//#pragma message("Automatically linking with patternRecognize.lib")

enum DrawShape
{
	selection, 
	line,
	straightline,
	rect,
	roundRect,
	ellipse,
	poly,
	text,
	bitmapTool,
	measure_line
};

class CHawkvisView;
// CGraphicsObj - base class for all 'CGraphics objects'
class AFX_EXT_CLASS CGraphicsObj : public CObject
{
#ifdef _DEBUG
	void AssertValid();
#endif
public:
	enum TrackerState { normal, selected, active };
	enum Shape {
		None,rectangle, ellipse,poly,bitmap,text,roundRectangle,chord,arc,pie,line,straightline,measure_line};
protected:
	DECLARE_SERIAL(CGraphicsObj);
	CGraphicsObj();
// Constructors
public:
	CGraphicsObj(const CRect& position);
	virtual ~CGraphicsObj();
protected:
	//旋转中心点
	CPoint		m_CenterPoint;	//图形的中心点
	CPoint		m_CenOffset;	//与图形的中心点的偏移量，要与中心点一同使用
	CPoint		m_OldRotateCenter;
	double		m_rotateAngle;//旋转角度
	double		m_oldRotateAngle;
	CString		m_objName,m_oldObjName;	//图形的名称
	int         m_obj_id;
	CPoint      m_orgPosition;
	double		m_Angle4Draw;
	////////////////////////////////////
	BOOL		m_bPen;
	LOGPEN		m_logpen;
	BOOL		m_bBrush;
	LOGBRUSH	m_logbrush;
	Shape		m_nShape;
	BOOL		m_bHide;//隐藏
//	BOOL		m_bLock;
	BOOL		m_bMonoPrint;
	BOOL		m_bEditEnable;
	////////////////////////////////////

public:

	//下面函数对属性的操作
	void SetShape(Shape Shape){m_nShape = Shape;};
	Shape	GetShape(){return m_nShape;};
	void	SetAngle(double dAngle){m_rotateAngle = dAngle;};
	double	GetAngle(){return m_rotateAngle;};
	double	GetOldAngle(){return m_oldRotateAngle;};
	CPoint	GetOldRotateCenter(){return m_OldRotateCenter;};
	CPoint	GetRotateCenter(){return m_CenterPoint+m_CenOffset;};
	CString GetObjName(){return m_objName;};
	void	SetObjName(const CString& strName){m_objName = strName;};
	CPoint	GetPageFromeWorldPoint(const CPoint& point);
	void	GetObjectRect(CRect &rect);//仅用在画面刷新
	void	Invalidate();//刷新画面

	double	GetRotateAngle(){return m_Angle4Draw;};
	
	void    enableEdit(){m_bEditEnable = TRUE;}
	void    disableEdit(){m_bEditEnable = FALSE;}
	BOOL    getEditEnable(){return m_bEditEnable;}

	void    setOrgPos(const CPoint& point);
	void    getOrgPos(CPoint& point);
	void    getObjectPosition(CRect& position);
	

	int     getObjID(){return m_obj_id;}
	void    setObjID(int id){m_obj_id = id;}
	BOOL	IsHide(){ return m_bHide;}
	LOGBRUSH	 GetLogBrush();
	LOGPEN		 GetLogPen(); 
	/////////////////////////////////////////
	void		SetLineColor(COLORREF color);
	void		SetFillColor(COLORREF color);
	void		SetPenStyle(int style) {m_logpen.lopnStyle = style;}
	void		SetPenWidth(int width);
	void		SetBrushStyle(int style){m_logbrush.lbStyle = style;m_bBrush = (style != BS_HOLLOW);}
	void		SetBrushHatch(int style){m_logbrush.lbHatch = style;}
	COLORREF	GetLineColor()			{return m_logpen.lopnColor ;}
	COLORREF	GetFillColor()			{return m_logbrush.lbColor;}
	long		GetPenWidth()			{return m_logpen.lopnWidth.y/10;} 
	short		GetPenStyle()			{return m_logpen.lopnStyle ;}
	short		GetBrushStyle()			{return m_logbrush.lbStyle;}
	long		GetBrushHatch()			{return m_logbrush.lbHatch;}
	/////////////////////////////////////////////////
	BOOL IsTextObj() {return m_nShape == text;}
	BOOL IsBitMap() {return m_nShape == bitmap;}
	BOOL IsPoly() {return m_nShape == poly;}
	BOOL IsPartCircle() {return m_nShape ==pie||m_nShape ==chord||m_nShape ==arc;}
	BOOL IsLine() {return (m_nShape == line||m_nShape == straightline||m_nShape == measure_line);}
	//下面函数供派生类使用
	virtual void	Draw(CDC* pDC);//OK
	virtual void	DrawTracker(CDC* pDC, TrackerState state);//OK
	virtual void	MoveTo(const CRect& rect, CHawkvisView* pView = NULL);//OK
	virtual int		HitTest(CPoint point, CHawkvisView* pView, BOOL bSelected);//OK
	virtual int		GetHandleCount();//OK
	virtual void	MoveHandleTo(int nHandle, CPoint point, CHawkvisView* pView = NULL);//OK
	virtual CPoint	GetHandle(int nHandle);//OK
	virtual HCURSOR	GetHandleCursor(int nHandle);//OK
	virtual	void	MoveCenterPointTo(CPoint delta, CHawkvisView* pView);//OK
	virtual BOOL	Intersects(const CRect& rect);//OK
	virtual void	Remove();//OK
	virtual void	Serialize(CArchive& ar);//OK
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam){return TRUE;};
	virtual BOOL	OnEditProperties(CView* pView = NULL);//OK
	virtual void RotateHandler(const CPoint& p, const CPoint& lastp);
//	virtual void	ChangePosition(CRect &position,CCVdevelopView *pView);
	virtual void setMeasurePos(int nCnt,float* pColEdge,float* pRowEdge,double dPhi);
protected:
	CRect	GetHandleRect(int nHandleID, CHawkvisView* pView);
	XFORM	CalcWorldTransform(CDC * pDC);//计算当前图形的XFORM
	CPoint	CalcWorldTransformPoint(CPoint oldpoint);
	CRect	CalcWorldTransformInvalidRect(CRect  rect);
	void	CalcXFormDxDy(CPoint center,double angle,float &dx,float &dy);

public:		
	BOOL		m_bLock;
	CRect		m_position;
	CRect		m_position0;
	UINT     m_bkMode,m_oldBkMode;
public:
public:
	COLORREF	m_FloodClr;//填充色
//Action_END
// Implementation
public:
	static CSize GetTextDimension(CHawkvisView *pView, LOGFONT lf, const CString& cs);
////TEXT
	virtual bool GetText(CString &cs) {return FALSE;}
	virtual void SetFont(LOGFONT lf, CHawkvisView* pView) {};
	virtual void SetText(char *str, CHawkvisView *pView = NULL) {};
	LOGFONT GetFont();
	void DispText(CDC* pDC,const CString& csText,CRect rect,UINT fmt);
//	virtual void ChangePosition(CRect &position,CCVdevelopView *pView);
//	CRect m_position0;//tmp
	friend class CStearoStypeSet;

};

// special 'list' class for this application (requires afxtempl.h)
typedef CTypedPtrList<CObList, CGraphicsObj*> CGraphicsObjList;


////////////////////////////////////////
class AFX_EXT_CLASS CHawkvisView : public CView
{
public: // 仅从序列化创建
	CHawkvisView();
	DECLARE_DYNCREATE(CHawkvisView)

public:
	CGraphicsObjList m_selection;
	CGraphicsObjList m_objects;
	CGraphicsObjList m_binImage_objects;
	CGraphicsObjList m_pic_objects;
	CGraphicsObjList m_express_objects;
	//中间键按下
	BOOL m_MButton_Captured;
	//放大比例
	double	m_docScale;

	CPoint org_point;
	CPoint lastMButtonPoint;

protected:
	CSize	m_objectSize;
	bool m_bSavetoImage;
	CString m_strFileName;
	// 操作
public:
	CGraphicsObjList* getSelectionObj(){ return &m_selection; }
	CGraphicsObjList* getAllObj(){ return &m_objects; }
	CGraphicsObjList* getpicObj(){ return &m_pic_objects; }
	void clearAllObject();
	void clearDrawObject();
	void clearRgnObject();
	void clearExpressObject();

	void removeDisplayObject(CGraphicsObj* pGraphicsObj);
	void removeRgnObject(CGraphicsObj* pGraphicsObjRgn);

	void OnMouseMoveWithMButtonPresses(UINT nFlags, const CPoint& point);
	void updateOrgPosition(const CPoint& centrPos, double coefficient, double scale);
	void resetOrgPosition();

	void DocToClient(CRect& rect);
	void DocToClient(CPoint& point);
	void ClientToDoc(CPoint& point);
	void ClientToDoc(CRect& rect);
	void Select(CGraphicsObj* pObj, BOOL bAdd = FALSE);
	void SelectWithinRect(CRect rect, BOOL bAdd = FALSE);
	void Deselect(CGraphicsObj* pObj);
	CGraphicsObj* ObjectAt(const CPoint& point);
	void InvalObj(CGraphicsObj* pObj);

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	
	// 实现
public:
	virtual ~CHawkvisView();


	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnSavetoImage(LPCSTR strFileName);
};

class AFX_EXT_CLASS CDrawTool
{
// Constructors
public:
	CDrawTool(DrawShape nDrawShape);

// Overridables
	virtual void OnLButtonDown(CHawkvisView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CHawkvisView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CHawkvisView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CHawkvisView* pView, UINT nFlags, const CPoint& point);
	virtual void OnCancel();
	virtual void OnChar(CHawkvisView* pView, UINT nChar, UINT nRepCnt, UINT nFlags){};
	virtual void OnKeyDown(CHawkvisView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) {};
	virtual void OnKeyUp(CHawkvisView* pView, UINT nChar, UINT nRepCnt, UINT nFlags){};

//operator
public:
	
	static void OnMButtonDown(CHawkvisView* pView, UINT nFlags, const CPoint& point);

	static void clearAllObject();

	static CDrawTool* FindTool(DrawShape drawShape);

	static CExtRegion* create_rgn_with_draw(CHawkvisView* pView);
	static void add_rgn_with_draw(CHawkvisView* pView, CExtRegion*& pRgnTobeAdded);
	static BOOL del_rgn_with_draw(CHawkvisView* pView, CExtRegion*& pRgnTobeAdded);

	static CGraphicsObjList* getSelectionObj();
	static CGraphicsObjList* getAllObj();
	static CGraphicsObjList* getpicObj();
	
private:
	static BOOL getBinArrayByDraw(CHawkvisView* pView,CSize& size);
// Attributes
public:
	DrawShape m_drawShape;

	static CPtrList c_tools;
	static CPoint c_down;
	static UINT c_nDownFlags;
	static CPoint c_last;
	static DrawShape c_drawShape;
	
	//
};

//GRAPHICS_WITH_MFC_DLL  CGraphicsObj* generate_BMP(const CRect& rc, CImg* pDrawbmpImg);
//GRAPHICS_WITH_MFC_DLL  CGraphicsObj* generate_Rect(const CRect& rc, const double& rotateAngle);
GRAPHICS_WITH_MFC_DLL BOOL read_image(CImg* & pImg,LPCTSTR lpszPathName);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_image(const CRect& rc, CImg* pDrawbmpImg, CHawkvisView* pView);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_image1(const CRect& rc, CImg* pDrawbmpImg, CHawkvisView* pView);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_region(CExtRegion* pRegion, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_region1(CExtRegion* pRegion, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_FeaturePos(CExtShapeMode* pShapeMode, CHawkvisView* pView, COLORREF color, int startX = 0, int startY = 0, double angle = 0.0, double scale = 1.0);

GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_ellipse(int centrX, int centrY, int radiusX, int radiusY, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_rectangle(int x1, int y1, int x2, int y2, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_measureLine(int x1, int y1, int x2, int y2, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* display_Line(int x1, int y1, int x2, int y2, CHawkvisView* pView, COLORREF color);
GRAPHICS_WITH_MFC_DLL  CGraphicsObj* displayText(const CString& text, const CRect& rc, CHawkvisView* pView, COLORREF color);
#endif
