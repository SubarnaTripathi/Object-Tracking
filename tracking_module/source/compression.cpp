/***************************************************************************************/
/*! Project component: compression
 *  \file   compression.c
 *  \brief  Purpose: This file contains the functions implementations for the compression of object
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-11-12
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 12 November   2009 ST: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/

/*** includes *****/
# include "global_defines.h"
# include "shared_mem.h"
# include "compression.h"
# include "WinInterface.h"
# include "time.h"
# include "stdlib.h"

# if 1
// to be deleted
////////////////////
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
////////////////////
# endif


/*** variables *****/
static int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];   
static int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];   
static int RGB2YUV_VG[256], RGB2YUV_VB[256]; 

unsigned char *yuv;

/**** needed for init_dither_tab() *****/
long int crv_tab[256];   
long int cbu_tab[256];   
long int cgu_tab[256];   
   
long int cgv_tab[256];   
long int tab_76309[256];   
 
unsigned char *clp = NULL;   
unsigned char *clp1 = NULL; 

/***************************************************************************************/
/*!
 *  \fn void void init_dither_tab () 
 *  \brief LUT for color model conversion.
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param IN    : NONE
 * \return NONE
 *
 ***************************************************************************************/
void init_dither_tab ()   
{   
  long int crv, cbu, cgu, cgv;   
  int i;   
   
  crv = 104597;   
  cbu = 132201;                 
  cgu = 25675;   
  cgv = 53279;   
   
  for (i = 0; i < 256; i++)   
  {   
    crv_tab[i] = (i - 128) * crv;   
    cbu_tab[i] = (i - 128) * cbu;   
    cgu_tab[i] = (i - 128) * cgu;   
    cgv_tab[i] = (i - 128) * cgv;   
    tab_76309[i] = 76309 * (i - 16);   
  }   
  clp = (unsigned char *)calloc(sizeof(unsigned char),1024); 
  if ( NULL == clp)
    printf("calloc failed\n");   
  clp1 = clp;   
   
  clp += 384;   
   
  for (i = -384; i < 640; i++)   
    clp[i] = (unsigned char)((i < 0) ? 0 : ((i > 255) ? 255 : i));   
}   

/***************************************************************************************/
/*!
 *  \fn int void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2,unsigned char *dst_ori,int width,int height)   
 *  \brief It converts YUV image to RGB (packed mode)   
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-12-11
 * \param IN    : unsigned char *src0   : pointer to source luma  
 * \param IN    : unsigned char *src1   : pointer to source cb  
 * \param IN    : unsigned char *src2   : pointer to source cr  
 * \param IN    : unsigned char *dst_ori: pointer to destination  
 * \param IN    : int width    : image width
 * \param IN    : int height   : image height 
 * \return NONE
 *
 ***************************************************************************************/ 
void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2,   
                      unsigned char *dst_ori,int width,int height)     
{     
  int y11, y21;   
  int y12, y22;   
  int y13, y23;   
  int y14, y24;   
  int u, v;   
  int i, j;   
  int c11, c21, c31, c41;   
  int c12, c22, c32, c42;   
  unsigned int DW;   
  unsigned int *id1, *id2;   
  unsigned char *py1, *py2, *pu, *pv;   
  unsigned char *d1, *d2;   
   
  d1 = dst_ori;   
  d1 += width * height * 3 - width * 3;   
  d2 = d1 - width * 3;   
   
  py1 = src0;   
  pu = src1;   
  pv = src2;   
  py2 = py1 + width;   
   
  id1 = (unsigned int *) d1;   
  id2 = (unsigned int *) d2;   
   
  for (j = 0; j < height; j += 2)   
  {   
    /* line j + 0 */   
    for (i = 0; i < width; i += 4)   
    {   
      u = *pu++;   
      v = *pv++;   
      c11 = crv_tab[v];   
      c21 = cgu_tab[u];   
      c31 = cgv_tab[v];   
      c41 = cbu_tab[u];   
      u = *pu++;   
      v = *pv++;   
      c12 = crv_tab[v];   
      c22 = cgu_tab[u];   
      c32 = cgv_tab[v];   
      c42 = cbu_tab[u];   
   
      y11 = tab_76309[*py1++];  /* (255/219)*65536 */   
      y12 = tab_76309[*py1++];   
      y13 = tab_76309[*py1++];  /* (255/219)*65536 */   
      y14 = tab_76309[*py1++];   
   
      y21 = tab_76309[*py2++];   
      y22 = tab_76309[*py2++];   
      y23 = tab_76309[*py2++];   
      y24 = tab_76309[*py2++];   
   
      /* RGBR */   
      DW = ((clp[(y11 + c41) >> 16])) |   
        ((clp[(y11 - c21 - c31) >> 16]) << 8) |   
        ((clp[(y11 + c11) >> 16]) << 16) |   
        ((clp[(y12 + c41) >> 16]) << 24);   
      *id1++ = DW;   
   
      /* GBRG */   
      DW = ((clp[(y12 - c21 - c31) >> 16])) |   
        ((clp[(y12 + c11) >> 16]) << 8) |   
        ((clp[(y13 + c42) >> 16]) << 16) |   
        ((clp[(y13 - c22 - c32) >> 16]) << 24);   
      *id1++ = DW;   
   
      /* BRGB */   
      DW = ((clp[(y13 + c12) >> 16])) |   
        ((clp[(y14 + c42) >> 16]) << 8) |   
        ((clp[(y14 - c22 - c32) >> 16]) << 16) |   
        ((clp[(y14 + c12) >> 16]) << 24);   
      *id1++ = DW;   
   
      /* RGBR */   
      DW = ((clp[(y21 + c41) >> 16])) |   
        ((clp[(y21 - c21 - c31) >> 16]) << 8) |   
        ((clp[(y21 + c11) >> 16]) << 16) |   
        ((clp[(y22 + c41) >> 16]) << 24);   
      *id2++ = DW;   
   
      /* GBRG */   
      DW = ((clp[(y22 - c21 - c31) >> 16])) |   
        ((clp[(y22 + c11) >> 16]) << 8) |   
        ((clp[(y23 + c42) >> 16]) << 16) |   
        ((clp[(y23 - c22 - c32) >> 16]) << 24);   
      *id2++ = DW;   
   
      /* BRGB */   
      DW = ((clp[(y23 + c12) >> 16])) |   
        ((clp[(y24 + c42) >> 16]) << 8) |   
        ((clp[(y24 - c22 - c32) >> 16]) << 16) |   
        ((clp[(y24 + c12) >> 16]) << 24);   
      *id2++ = DW;   
    }   
    id1 -= (9 * width) >> 2;   
    id2 -= (9 * width) >> 2;   
    py1 += width;   
    py2 += width;   
  }   
} 

# if 0
void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2,   
                      unsigned char *dst_ori,int width,int height)   
{

	int i,j,k, count = 0;
	double R, G, B;
	int y_index, ch_index;
	char *u = NULL;
	char *v = NULL;

	u = (char*)(calloc(width*height,sizeof(char)));
	v = (char*)(calloc(width*height,sizeof(char)));

	k = 0;
	for ( i = 0; i < height; i++)
	{
		for ( j = 0; j < width; j++ )
		{
			y_index = i*width + j;
			ch_index = i*(width/2) + (j/2);

			B = 1.164*(src0[y_index])   +  2.018*(src1[ch_index] - 128);
			G = 1.164*(src0[y_index])   -  0.813*(src2[ch_index] - 128) - 
										0.391*(src1[ch_index]-128);
			R = 1.164*(src0[y_index])   +  1.596*(src2[ch_index] - 128);

			if ( B < 0 ) B = 0;
			if ( B > 255 ) B = 255;
			if ( G < 0 ) G = 0;
			if ( G > 255 ) G = 255;
			if ( R < 0 ) R = 0;
			if ( R > 255 ) R = 255;

			dst_ori[count] = (char)B;
			dst_ori[count+1] = (char)G;
			dst_ori[count+2] = (char)R;

			count += 3;
		}
	}


	if ( NULL != u ){ free(u); u = NULL;}
	if ( NULL != u ){ free(u); u = NULL;}
}
# endif

/***************************************************************************************/
/*!
 *  \fn int makeRGBfromfile(unsigned char *Dst, int width, int height, char *name, int fr_num ) 
 *  \brief It takes a yuv file(4:2:0) and convert it 24-bit RGB format.   
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-12-11
 * \param IN    : unsigned char *Dst   : pointer to destination  
 * \param IN    : int width		: image width  
 * \param IN    : int height    : image height  
 * \param IN    : char *name    : yuv file name  
 * \param IN    : int fr_num    : frame number  
 * \return NONE
 *
 ***************************************************************************************/
/*** supported YUV is of 4:2:0 only, now ****/
/*** supported RGB is 24 bit, now *******/
void makeRGBfromfile(unsigned char *Dst, int width, int height, char *name, int fr_num )   
{   
  unsigned char *Y, *U, *V;   
  unsigned char *Fsrc;   
  FILE *file = NULL; 
  char file_name[255];
  int frame_size;
  int ret;
  static int init_first = 0;

  init_first++; 

  /*** encoder configuration file is such that reconstructed test_rec.yuv will be formed, 
  otherwise  one can use explicit decoder's output test_dec.yuv ***/
  if ( NULL != name )
	strcpy(file_name, name);
  else
    strcpy(file_name, "test_rec.yuv"); /* test_dec.yuv */
  
  file = fopen(file_name,"rb");
  if (NULL == file)
  {
	  printf("File open Error!"); 
	  exit(0);
  }
  
  frame_size = ((width*height*3)/2);
  long offset = fr_num*(width*height*sizeof(char)*3/2);
  ret = fseek(file, offset, SEEK_SET); 
  if ( 0 != ret )
  {
	  printf("Fseek error ");
  }

  Fsrc=(unsigned char *) calloc(frame_size,sizeof(char));   
   
  fread(Fsrc,frame_size *sizeof(char),sizeof(char), file);   
  fclose(file);

  Y=Fsrc;   
  U=Y+width*height;   
  V=Y+width*height*5/4;   
   
  if ( 1 == init_first )
  {
	init_dither_tab ();  
  }
  ConvertYUVtoRGB (Y,U,V,Dst,width,height);    
  free( Fsrc);
} 



/***************************************************************************************/
/*!
 *  \fn int RGB2YUV420 (int x_dim, int y_dim, unsigned char *bmp, unsigned char *yuv, int flip)  
 *  \brief It takes a 24-bit RGB bitmap and convert it into YUV (4:2:0) format.   
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param IN    : x_dim   : the x dimension of the bitmap  
 * \param IN    : y_dim   : the y dimension of the bitmap  
 * \param IN    : bmp     : pointer to the buffer of the bitmap  
 * \param IN    : yuv     : pointer to the YUV structure  
 * \param IN    : flip    : 0: RGB order in bmp; 1: BGR order in bmp  
 * \return NONE
 *
 ***************************************************************************************/
int RGB2YUV420 (int x_dim, int y_dim,    
             unsigned char *bmp,    
             unsigned char *yuv, int flip)   
{   
    int i, j;   
    unsigned char *r, *g, *b;   
    unsigned char *y, *u, *v;   
    unsigned char *uu, *vv;   
    unsigned char *pu1, *pu2,*pu3,*pu4;   
    unsigned char *pv1, *pv2,*pv3,*pv4;   
   

	if(flip == 0)
	{   
        r=bmp;   
        g=bmp+1;   
        b=bmp+2;   
    }   
    else
	{   
        b=bmp;   
        g=bmp+1;   
        r=bmp+2;   
    }   
   
    y=yuv;   
    uu=new unsigned char[x_dim*y_dim];   
    vv=new unsigned char[x_dim*y_dim];   
    u=uu;   
    v=vv;   
    for (i=0;i<y_dim;i++)
	{ 
		for (j = 0;j< x_dim; j++ )
		{
			*y++= (unsigned char)(( RGB2YUV_YR[*r]+RGB2YUV_YG[*g]+RGB2YUV_YB[*b]+1048576)>>16);   
            *u++= (unsigned char)((-RGB2YUV_UR[*r]-RGB2YUV_UG[*g]+RGB2YUV_UBVR[*b]+8388608)>>16);   
            *v++= (unsigned char)(( RGB2YUV_UBVR[*r]-RGB2YUV_VG[*g]-RGB2YUV_VB[*b]+8388608)>>16);  

			r+=3;   
            g+=3;   
            b+=3;   
        }   
    }   
   
    //dimension reduction for U and V components   
    u=yuv+x_dim*y_dim;   
    v=u+x_dim*y_dim/4;   
   
    pu1=uu;   
    pu2=pu1+1;   
    pu3=pu1+x_dim;   
    pu4=pu3+1;   
   
    pv1=vv;   
    pv2=pv1+1;   
    pv3=pv1+x_dim;   
    pv4=pv3+1;   
    for(i=0;i<y_dim;i+=2)
	{ 
		for(j= 0;j<x_dim;j+=2)
		{
			*u++= (unsigned char)(int(*pu1+*pu2+*pu3+*pu4)>>2);   
            *v++= (unsigned char)(int(*pv1+*pv2+*pv3+*pv4)>>2);   
            pu1+=2;   
            pu2+=2;   
            pu3+=2;   
            pu4+=2;   
            pv1+=2;   
            pv2+=2;   
            pv3+=2;   
            pv4+=2;   
        }   
        pu1+=x_dim;   
        pu2+=x_dim;   
        pu3+=x_dim;   
        pu4+=x_dim;   
        pv1+=x_dim;   
        pv2+=x_dim;   
        pv3+=x_dim;   
        pv4+=x_dim;   
    }   
   
    delete [] uu;   
    delete [] vv;   
    return 0;   
}   
/***************************************************************************************/
/*!
 *  \fn void InitLookupTable()
 *  \brief look up table for color model conversion   
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param INOUT NONE  
 * \return NONE
 *
 ***************************************************************************************/   
void InitLookupTable()   
{   
    int i;   
   
    for (i = 0; i < 256; i++) 
		RGB2YUV_YR[i] = (int)((float)65.481 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_YG[i] = (int)((float)128.553 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_YB[i] = (int)((float)24.966 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_UR[i] = (int)((float)37.797 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_UG[i] = (int)((float)74.203 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_VG[i] = (int)((float)93.786 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_VB[i] = (int)((float)18.214 * (i<<8));   
    for (i = 0; i < 256; i++) 
		RGB2YUV_UBVR[i] = (int)((float)112 * (i<<8));   
} 




