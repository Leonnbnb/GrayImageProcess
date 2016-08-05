/****************************************************************************************************
 * Copyright (c) 2013
 ****************************************************************************************************
 * @file: dataType.h
 *
 * @description: data type 
 *
 * @date: Jun 23, 2012
 * @author: ou shile
 * @last modified by:
 *        $Author: 
 ****************************************************************************************************
 */
#include "../shared/List.h"

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#ifdef _PATTERN_RECOGNIZE_LIB_
#define PATTERN_RECOGNIZE_DLL extern "C" __declspec(dllexport)  
#define PATTERN_RECOGNIZE_CLASS_DLL  __declspec(dllexport)
#else
#define PATTERN_RECOGNIZE_DLL extern "C" __declspec(dllimport) 
#define PATTERN_RECOGNIZE_CLASS_DLL  __declspec(dllimport)
#endif

#ifdef _GRAPHICS_WITH_MFC_LIB_
#define GRAPHICS_WITH_MFC_DLL extern "C" __declspec(dllexport)  
#else
#define GRAPHICS_WITH_MFC_DLL extern "C" __declspec(dllimport)
#endif


#define pi 3.1415926
typedef double* PDOUBLE;


#ifndef _INT16
#  define _INT16      short
#endif

#ifndef _INT32
#  define _INT32      int
#endif

#define  INT1      char              /* integer, signed 1 Byte         */
#define  UINT1     unsigned char     /* integer, unsigned 1 Byte       */

#define  INT2      _INT16            /* integer, signed 2 Byte         */
#define  UINT2     unsigned _INT16   /* integer, unsigned 2 Byte       */

#define  INT4      _INT32            /* integer, signed 4 Byte         */
#define  UINT4     unsigned _INT32   /* integer, unsigned 4 Byte       */

//#define  INT4_8      _INT64          /* integer, signed 4-8 Byte       */
//#define  UINT4_8     unsigned _INT64 /* integer, unsigned 4-8 Byte     */

typedef INT4 HIMGDIM;   /* Image dimension, e.g., width and height */
typedef INT2 HIMGCOOR;  /* Image coordinates, e.g, row and column */
typedef INT4 HLINCOOR;  /* Linearized image coordinates */
typedef INT4 HIMGCNT;   /* Number of pixels, e.g., area or buffer size */
typedef INT4 HITEMCNT;  /* Number of elements, e.g., contours or runlengths */
typedef float HSUBCOOR; /* Sub-pixel precise coordinates */
typedef float HSUBATTR; /* Sub-pixel precise attributes of contours */





		

/****************************************************************************/
/*                       Regionen                                           */
/****************************************************************************/


/* Index range for coordinates of run length regions                        */
/*#if !defined(HC_LARGE_IMAGES)
#define MIN_COORD INT2_MIN
#define MAX_COORD INT2_MAX
#else
#define MIN_COORD INT4_MIN
#define MAX_COORD INT4_MAX
#endif
*/

/* Data type for a single run of a region encoded with runlength encoding   */
typedef struct {
  HIMGCOOR l;               /* line number (row) of run                     */
  HIMGCOOR cb;              /* column index of beginning of run             */
  HIMGCOOR ce;              /* column index of ending of run                */
} Hrun;

/* vertical run */
typedef struct {
  HIMGCOOR c;               /* column index of run                          */
  HIMGCOOR lb;              /* row index of beginning of run                */
  HIMGCOOR le;              /* row index of ending of run                   */
} Hvrun;

#define SHAPE_RECTANGLE1     (UINT1)1
#define SHAPE_RECTANGLE2     (UINT1)2
#define SHAPE_CIRCLE         (UINT1)3
#define SHAPE_ELLIPSE        (UINT1)4

typedef  UINT1 HBOOL;                  /* TRUE (1) or FALSE (0) */
typedef  UINT1 HBYTE;                  /* unsigned Byte */


/* Bit-array to check if a feature of a region was calculated               */
typedef struct {
  unsigned int shape               : 1;
  unsigned int is_convex           : 1;
  unsigned int is_filled           : 1;
  unsigned int is_connected4       : 1;
  unsigned int is_connected8       : 1;
  unsigned int is_thin             : 1;
  unsigned int circularity         : 1;
  unsigned int compactness         : 1;
  unsigned int contlength          : 1;
  unsigned int convexity           : 1;
  unsigned int phi                 : 1;
  unsigned int elliptic_axis       : 1;   /* ra, rb                         */
  unsigned int elliptic_shape      : 1;   /* ra_, rb_                       */
  unsigned int excentricity        : 1;   /* anisometry,bulkiness,str_faktor*/
  unsigned int moments             : 1;   /* m11, m20, m02, ia, ib          */
  unsigned int center_area         : 1;   /* row, col, area                 */
  unsigned int smallest_rectangle1 : 1;   /* row1,col1,row2,col2            */
  unsigned int smallest_rectangle2 : 1;   /* row_rect, col_rect, phi_rect,  */
                                          /* length1, length2               */
  unsigned int smallest_circle     : 1;   /* row_circle, col_circle, radius */
  unsigned int min_max_chord       : 1;   /* longest/shortest run           */
  unsigned int min_max_chord_gap   : 1;   /* longest/shortest run (holes)   */
  unsigned int rectangularity      : 1;
} HFeatureFlags;


/* Shape feature caching inside the region structure                        */
typedef struct {
  union {
    HFeatureFlags single;
    long          all;      /* if possible use 64 bits!                     */
  }               def;
  UINT1    shape;           /* SHAPE_*                                      */
  HBOOL    is_convex;
  HBOOL    is_filled;
  HBOOL    is_connected4;
  HBOOL    is_connected8;
  HBOOL    is_thin;         /* one pixel thin                               */
  double   circularity;
  double   compactness;
  double   contlength;
  double   convexity;
  double   phi;
  double   ra, rb;          /* elliptic_axis                                */
  double   ra_, rb_;        /* elliptic_shape                               */
  double   anisometry, bulkiness, structure_factor;
  double   m11, m20, m02, ia, ib;
  double   row, col;
  HIMGCNT  area;
  HIMGCOOR row1, col1, row2, col2;
  double   row_rect, col_rect, phi_rect, length1, length2;
  double   row_circle, col_circle, radius;
  HIMGCOOR min_chord, max_chord;
  HIMGCOOR min_chord_gap, max_chord_gap;
  double rectangularity;
} HRegFeature;


typedef struct {
  HBOOL       is_compl;     /* region is complement                         */
  HITEMCNT    num;          /* number of runs                               */
  HITEMCNT    num_max;      /* maximum number of runs                       */
  HRegFeature feature;      /* already processed features                   */
  Hrun        *rl;          /* pointer on array of run lengths              */
} Hrlregion;


/* vertical runlength encoding */
typedef struct {
  HBOOL       is_compl;     /* region is complement                         */
  HITEMCNT    num;          /* number of runs                               */
  HITEMCNT    num_max;      /* maximum number of runs                       */
  HRegFeature feature;      /* already processed features                   */
  Hvrun       *rc;          /* array of runs                                */
} Hvrlregion;

/* Data structure for fast single pixel access for Hrlregion regions        */
typedef struct {
  HIMGCOOR min, max;        /* Min/Max row index                            */
  Hrun     *rl;             /* pointer to runs of the region                */
  HITEMCNT *first;          /* array of index values (row begin)            */
  HITEMCNT *last;           /* array of index values (row end)              */
  HBOOL    *no_row;         /* is there this row available?                 */
} Hrlaccess;

typedef struct float_pos
{
	float x;
	float y;
}FLOAT_POS,* P_FLOAT_POS;

typedef struct feature_node
{
	int nrow;
	int ncol;
	int direction;
	struct feature_node* pN[8];
}tag_feature_node, *p_feature_node;

typedef struct feature_component
{
	int rows;
	int cols;
	int nCounts;
	p_feature_node root;
	SYS::List<p_feature_node> recycle_node;
}tag_feature_component, *p_feature_component;

typedef struct candidate_pos
{
	BYTE active_flag;
	float x;
	float y;
	int angle;
	int scale;
	float score;
}CANDIDATE_POS,* P_CANDIDATE_POS;

enum FilterName {
		filter_start, canny, deriche1, deriche2, lanser1, lanser2,shen,sobel,filter_end};

enum measure_interpolation {nearest_neighbor,bilinear,facet,bicubic };
enum measure_Transition {t_all, t_positive, t_negative };
enum measure_select {s_all, s_first, s_last };
enum measure_type {measure_rectangle,measure_arc };
typedef struct tag_MeasureHandle
{
	int nLen_seq;
	float* pPos_long_seq_x;
	float* pPos_long_seq_y;
	float* pGray_long_seq;
	enum measure_type enType;
	float fStep;
	double SinPhi;
	double CosPhi;
	double centr_x;
	double centr_y;
	double dRadius;
	double dAngleStart;
}MeasureHandle,* P_MeasureHandle;

#endif