/***************************************************************************************/
/*! Project component: subspace
 *  \file   subspace.h
 *  \brief  Purpose: This file contains the prototypes for functions related to subspace(appearance) formation and update
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
#ifndef SUB_SPACE_INCLUDED
#define SUB_SPACE_INCLUDED

# include "defines.h"
#include "defines.h"
#include <stdio.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"
# include "cxtypes.h"
# include <math.h>

      
/******************************* Global variables ***************************************/
int frame_cnt;
int *frame_cnt_dup;

int tot_obj_number;  /* no limit as per coding technique as long as memory supports */

int num_vect;

# ifndef OBJ_FEAT_SIZE
int length_w;
int length_h;
# else
int length_h[MAX_NUM_OBJECTS];
int length_w[MAX_NUM_OBJECTS];
# endif

int num_particles;

# ifndef REF_MAT
int **ref_mat = NULL;
# else
CvMat *ref_mat;
# endif

uchar ***newV = NULL;
uchar ***xmat = NULL;

# ifndef LOCAL_MEAN
uchar *local_mean_r = NULL;
uchar *local_mean_g = NULL;
uchar *local_mean_b = NULL;
# else
CvMat *local_mean_r;
CvMat *local_mean_g;
CvMat *local_mean_b;
# endif

int max_height, max_width;

float ****subspace = NULL;
uchar ***mean_buffer = NULL;
/******************************************************************************************/


/****** function prototypes ***************************/
double compare_norm_hist(IplImage *initial_ref_obj, IplImage *reference);


# ifdef DIST_TO_SS
void closest_to_subspace (int obj_num, double  *s, IplImage* reference, int spread_r, int spread_g, int spread_b);
# endif
void initialize_subspace_mean(int obj_num, IplImage* reference, int *spread_r, int *spread_g, int *spread_b) ;
void update_mean(int obj_num, IplImage* reference, int *spread_r, int *spread_g, int *spread_b) ;
void update_subspace(int obj_num ) ;

void initVars(void);
void releaseVars(void);
void create_average_content(IplImage *avg_obj, int obj_number);
void create_appearance_content(IplImage *appearance, int obj_number, int dim);


void initialize_subspace_mean(int obj_num, IplImage* reference, int *spread_r, int *spread_g, int *spread_b) 
{   
	int count = 0;
	int i, j, k;

	int min_r = 0, max_r = 0;
	int min_g = 0, max_g = 0;
	int min_b = 0, max_b = 0;


# ifndef REF_MAT
	for (int y=0; y<reference->height; y++)
	{
		for (int x=0; x<reference->width; x++)
		{
      		for (int c=0; c<reference->nChannels; c++) 
			{
      			int idx=x*reference->nChannels+y*reference->widthStep+c;
				uchar val=reference->imageData[idx];
      			ref_mat[count][c] = val;
				
				if ( c == 0 )
				{
					if ( val > max_b) max_b = val;
					if ( val < min_b) min_b = val;
				}
				else if ( c == 1 )
				{
					if ( val > max_g) max_g = val;
					if ( val < min_g) min_g = val;
				}
				else /*if ( c == 2 )*/
				{
					if ( val > max_r) max_r = val;
					if ( val < min_r) min_r = val;
				}
      		}
			local_mean_b[count]= (uchar) ref_mat[count][0];  
            local_mean_g[count]= (uchar) ref_mat[count][1];
            local_mean_r[count]= (uchar) ref_mat[count][2];
            count++;
  		}	
	}
# else
	
	CvMat *PreMat, hdr;
	PreMat = cvCreateMat(reference->height, reference->width, CV_64FC3);
	cvConvert(reference, PreMat);
	cvTranspose(cvReshape(PreMat, &hdr, 1, (reference->height* reference->width)), ref_mat); 
	cvReleaseMat(&PreMat);
# endif


# ifndef LOCAL_MEAN
	for (i = 0; i < (reference->height*reference->width); i++ )
	{
# ifndef REF_MAT
		local_mean_b[i]= (uchar) ref_mat[i][0];  
        local_mean_g[i]= (uchar) ref_mat[i][1]; 
        local_mean_r[i]= (uchar) ref_mat[i][2]; 
# else
		local_mean_b[i]= (uchar) cvmGet(ref_mat, 0, i);  
        local_mean_g[i]= (uchar) cvmGet(ref_mat, 1, i);  
        local_mean_r[i]= (uchar) cvmGet(ref_mat, 2, i);
# endif

		if ( local_mean_b[i] > max_b) max_b = local_mean_b[i];
		if ( local_mean_b[i] < min_b) min_b = local_mean_b[i];

		if ( local_mean_g[i] > max_g) max_g = local_mean_g[i];
		if ( local_mean_g[i] < min_g) min_g = local_mean_g[i];

		if ( local_mean_r[i] > max_r) max_r = local_mean_r[i];
		if ( local_mean_r[i] < min_r) min_r = local_mean_r[i];
	}
# else

	CvScalar minval, maxval;

	IplImage* r = cvCreateImage( cvGetSize(reference), IPL_DEPTH_8U, 1 );
	IplImage* g = cvCreateImage( cvGetSize(reference), IPL_DEPTH_8U, 1 );
	IplImage* b = cvCreateImage( cvGetSize(reference), IPL_DEPTH_8U, 1 );

	// Split image onto the color planes
	cvSplit( reference, r, g, b, NULL );

	PreMat = cvCreateMat(reference->height, reference->width, CV_64FC1);
	cvConvert(r, PreMat);
	cvTranspose(cvReshape(PreMat, &hdr, 1, (reference->height* reference->width)), local_mean_r); 
	cvConvert(g, PreMat);
	cvTranspose(cvReshape(PreMat, &hdr, 1, (reference->height* reference->width)), local_mean_g); 
	cvConvert(b, PreMat);
	cvTranspose(cvReshape(PreMat, &hdr, 1, (reference->height* reference->width)), local_mean_b); 
	
	cvReleaseMat(&PreMat);
	cvReleaseImage( &r );
	cvReleaseImage( &g );
	cvReleaseImage( &b );

	cvMinMaxLoc( local_mean_r, &minval.val[0], &maxval.val[0]);
	min_r = minval.val[0]; max_r = maxval.val[0]; 
	cvMinMaxLoc( local_mean_g, &minval.val[0], &maxval.val[0]);
	min_g = minval.val[0]; max_g = maxval.val[0]; 
	cvMinMaxLoc( local_mean_b, &minval.val[0], &maxval.val[0]);
	min_b = minval.val[0]; max_b = maxval.val[0]; 
# endif


	*spread_r = max_r - min_r;
	*spread_g = max_g - min_g;
	*spread_b = max_b - min_b;

	# ifdef OBJ_FEAT_SIZE
	int curr_length_w = length_w[obj_num];
	int curr_length_h = length_h[obj_num];
	# else
	int curr_length_w = length_w;
	int curr_length_h = length_h;
	# endif

    /**** explicit memcpy ****/
	for( i=0;i<curr_length_h*curr_length_w ;i++)
	{
# ifndef LOCAL_MEAN
		mean_buffer[obj_num][i][2]= local_mean_r[i];
		mean_buffer[obj_num][i][1]= local_mean_g[i];
		mean_buffer[obj_num][i][0]= local_mean_b[i];
# else
		mean_buffer[obj_num][i][2]= cvmGet(local_mean_r,0,i);
		mean_buffer[obj_num][i][1]= cvmGet(local_mean_g,0,i);
		mean_buffer[obj_num][i][0]= cvmGet(local_mean_b,0,i);
# endif
		
		for (j = 0; j < num_vect; j++)
		{
			for ( k = 0; k < 3; k++ )
			{
				subspace[obj_num][i][j][k] = 0;
			}
		}
	}
} 

void update_mean(int obj_num, IplImage* reference, int *spread_r, int *spread_g, int *spread_b) 
{
	int min_r = 0, max_r = 0;
	int min_g = 0, max_g = 0;
	int min_b = 0, max_b = 0;
	int i;

	/**** incremental mean can have higher weitage to recent observations than older ones ****/
	float amnesic_param = 2; //0 means normal mean ( range: 0 to 6 ), higher value means higher weight to last observation ***/
	/**************************/

# ifndef REF_MAT
		int count=0;
	for (int y=0; y<reference->height; y++)
	{
	  for (int x=0; x<reference->width; x++)
	  {
      	for (int c=0; c<reference->nChannels; c++) 
		{
      		int idx=x*reference->nChannels+y*reference->widthStep+c;
      		uchar val=reference->imageData[idx];
      		ref_mat[count][c]= val;

			if ( c == 0 )
			{
				if ( val > max_b) max_b = val;
				if ( val < min_b) min_b = val;
			}
			else if ( c == 1 )
			{
				if ( val > max_g) max_g = val;
				if ( val < min_g) min_g = val;
			}
			else /*if ( c == 2 )*/
			{
				if ( val > max_r) max_r = val;
				if ( val < min_r) min_r = val;
			}
      	}
		local_mean_r[count]= (uchar)ref_mat[count][2];            
        local_mean_g[count]= (uchar)ref_mat[count][1];
        local_mean_b[count]= (uchar)ref_mat[count][0];

        count++;  
  	  }	
	}
# else
	CvMat *PreMat, hdr;
	PreMat = cvCreateMat(reference->height, reference->width, CV_64FC3);
	cvConvert(reference, PreMat);
	cvTranspose(cvReshape(PreMat, &hdr, 1, (reference->height*reference->width)), ref_mat); 
	cvReleaseMat(&PreMat);
# endif


# ifndef LOCAL_MEAN
	for (i = 0; i < (reference->height*reference->width); i++ )
	{
# ifndef REF_MAT
		local_mean_b[i]= (uchar) ref_mat[i][0];  
        local_mean_g[i]= (uchar) ref_mat[i][1];  
        local_mean_r[i]= (uchar) ref_mat[i][2];  
# else
        local_mean_b[i]= (uchar) cvmGet(ref_mat, 0, i);  
        local_mean_g[i]= (uchar) cvmGet(ref_mat, 1, i);  
        local_mean_r[i]= (uchar) cvmGet(ref_mat, 2, i);
# endif

		if ( local_mean_b[i] > max_b) max_b = local_mean_b[i];
		if ( local_mean_b[i] < min_b) min_b = local_mean_b[i];

		if ( local_mean_g[i] > max_g) max_g = local_mean_g[i];
		if ( local_mean_g[i] < min_g) min_g = local_mean_g[i];

		if ( local_mean_r[i] > max_r) max_r = local_mean_r[i];
		if ( local_mean_r[i] < min_r) min_r = local_mean_r[i];
	}
# else

	CvScalar minval, maxval;

	cvGetRow( ref_mat, local_mean_r, 2 );
	cvGetRow( ref_mat, local_mean_g, 1 );
	cvGetRow( ref_mat, local_mean_b, 0 );

	
	cvMinMaxLoc( local_mean_r, &minval.val[0], &maxval.val[0]);
	min_r = minval.val[0]; max_r = maxval.val[0]; 
	cvMinMaxLoc( local_mean_g, &minval.val[0], &maxval.val[0]);
	min_g = minval.val[0]; max_g = maxval.val[0]; 
	cvMinMaxLoc( local_mean_b, &minval.val[0], &maxval.val[0]);
	min_b = minval.val[0]; max_b = maxval.val[0]; 

# endif

	*spread_r = max_r - min_r;
	*spread_g = max_g - min_g;
	*spread_b = max_b - min_b;

	# ifdef OBJ_FEAT_SIZE
	int curr_length_w = length_w[obj_num];
	int curr_length_h = length_h[obj_num];
	# else
	int curr_length_w = length_w;
	int curr_length_h = length_h;
	# endif
	

	for(int i=0;i<curr_length_h*curr_length_w ;i++)
	{
		/*** frame_cnt_dup instead of frame_cnt ***/
		float old_weight = (float)((frame_cnt_dup[obj_num] - amnesic_param)*1.0)/(frame_cnt_dup[obj_num]+1);
		float curr_weight = (float)(amnesic_param*1.0)/(frame_cnt_dup[obj_num]+1);

# ifndef LOCAL_MEAN

# if 1
		mean_buffer[obj_num][i][2]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][2]+local_mean_r[i])/(frame_cnt_dup[obj_num]+1));
		mean_buffer[obj_num][i][1]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][1]+local_mean_g[i])/(frame_cnt_dup[obj_num]+1));
		mean_buffer[obj_num][i][0]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][0]+local_mean_b[i])/(frame_cnt_dup[obj_num]+1));
# else
		/***** weighted mean ******/
		int val1, val2;
		val1 = ( mean_buffer[obj_num][i][2] < 0 ) ? (mean_buffer[obj_num][i][2] + 256) : mean_buffer[obj_num][i][2]; 
		val2 = ( local_mean_r[i]< 0 ) ? (local_mean_r[i]+ 256) : local_mean_r[i]; 

		mean_buffer[obj_num][i][2] = (uchar)(old_weight*val1 + curr_weight*val2);

		val1 = ( mean_buffer[obj_num][i][1] < 0 ) ? (mean_buffer[obj_num][i][1] + 256) : mean_buffer[obj_num][i][1]; 
		val2 = ( local_mean_g[i]< 0 ) ? (local_mean_g[i]+ 256) : local_mean_g[i]; 

		mean_buffer[obj_num][i][1]= (uchar)(old_weight*val1 + curr_weight*val2);

		val1 = ( mean_buffer[obj_num][i][0] < 0 ) ? (mean_buffer[obj_num][i][0] + 256) : mean_buffer[obj_num][i][0]; 
		val2 = ( local_mean_b[i]< 0 ) ? (local_mean_b[i]+ 256) : local_mean_b[i]; 

		mean_buffer[obj_num][i][0]= (uchar)(old_weight*val1 + curr_weight*val2);
# endif

# else
		mean_buffer[obj_num][i][2]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][2]+cvmGet(local_mean_r, 0, i))/(frame_cnt_dup[obj_num]+1));
		mean_buffer[obj_num][i][1]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][1]+cvmGet(local_mean_g, 0, i))/(frame_cnt_dup[obj_num]+1));
		mean_buffer[obj_num][i][0]= (uchar)((frame_cnt_dup[obj_num]*mean_buffer[obj_num][i][0]+cvmGet(local_mean_b, 0, i))/(frame_cnt_dup[obj_num]+1));
# endif
	
	}
}


# ifdef DIST_TO_SS
void closest_to_subspace (int obj_num, double  *s, IplImage* reference, int spread_r, int spread_g,int spread_b)
{
	int i, c, iRow, iCol,j;
	int temp_val;
	double diff;

	# ifdef OBJ_FEAT_SIZE
	int curr_length_w = length_w[obj_num];
	int curr_length_h = length_h[obj_num];
	# else
	int curr_length_w = length_w;
	int curr_length_h = length_h;
	# endif

	/**** frame_cnt_dup instead of frame_cnt *****/
	int dim = (frame_cnt_dup[obj_num] > num_vect)? num_vect:frame_cnt_dup[obj_num];

	int *temp3 = (int*)(calloc(curr_length_h*curr_length_w,sizeof(int)));
	double **scalar;
	scalar = (double**)(calloc(dim,sizeof(double*)));
	for (i = 0; i < dim; i++)
		scalar[i] = (double*)(calloc(3,sizeof(double)));
	int **recon = (int**)(calloc(curr_length_h*curr_length_w,sizeof(int)));
	for (i = 0; i < (curr_length_h*curr_length_w); i++)
		recon[i] = (int*)(calloc(3,sizeof(int)));


	for(i=0;i< num_particles/2 ;i++) 
	{
		s[i] = 0; 

		/**** reset recon for each particle ****/
		for (j = 0; j < (curr_length_h*curr_length_w); j++)
		{
			for (c = 0; c < 3; c++)
			{
				recon[j][c] = 0;
			}
		}
			
		for ( j = 0; j < dim; j++ )
		{
			int k;
			for(c=0; c<3; c++) 
			{
				diff = 0;
				scalar[j][c] = 0;
				/*** first calculate the scalar [subspace'(vector - mean )] *******/
				for(iRow = 0; iRow < curr_length_h; iRow++ ) 
				{
					for(iCol = 0; iCol < curr_length_w; iCol++ ) 
					{
						/*int*/ k = iRow*curr_length_w + iCol;					
						temp3[k]= newV[i][k][c]- mean_buffer[obj_num][k][c];
				
						/*** val1 element represents the scalar [subspace_j'*(vector-mean)] ****/
						scalar[j][c] += (subspace[obj_num][k][j][c]*temp3[k]);
					}
				}

				for(iRow = 0; iRow < curr_length_h; iRow++ ) 
				{
					for(iCol = 0; iCol < curr_length_w; iCol++ ) 
					{
						/*int*/ k = iRow*curr_length_w + iCol;
						recon[k][c] += (int) (subspace[obj_num][k][j][c]*scalar[j][c]);
					}
				}
			}
		}

		for ( c = 0; c < 3; c++ )
		{
			diff = 0;
			for(iRow = 0; iRow < curr_length_h; iRow++ ) 
			{
				for(iCol = 0; iCol < curr_length_w; iCol++ ) 
				{			
					int k = iRow*curr_length_w + iCol;	
					int index = 3*iCol + iRow*reference->widthStep + c;
					temp_val = (int)reference->imageData[index];
					temp_val = ( 0 > temp_val )? ( 256 + temp_val ): temp_val;


					/*** diff elements represents [orig - {subspace'(vector-mean) + mean }] ***/
					diff += abs(temp_val - (recon[k][c] + (int)mean_buffer[obj_num][k][c]));
				}
			}		
			s[i] += diff; /* w.r.t each color plane ***/
		}
		s[i] = s[i]/(3*curr_length_w*curr_length_h);
	}

	if (temp3) {free(temp3);temp3 = NULL;}
	
	if (scalar) 
	{	
		for ( i = 0; i < dim; i++ )
			free(scalar[i]);
		free(scalar);
		scalar = NULL;
	}
	if (recon) 
	{
		for ( i = 0; i < (curr_length_h*curr_length_w); i++ )
			free(recon[i]);
		free(recon);
		recon = NULL;
	}
}
# endif


/****** new 1D formulation *******/
void update_subspace(int obj_num) 
{
	/**** frame_cnt_dup instead of frame_cnt *****/
	int dim = (frame_cnt_dup[obj_num] >= num_vect)? num_vect:(frame_cnt_dup[obj_num] +1); //subarna
	
	int AP = 0;         // amnesiac parameter

	int i;
	float *k1;
	float **observ;

	# ifdef OBJ_FEAT_SIZE
	int curr_length_w = length_w[obj_num];
	int curr_length_h = length_h[obj_num];
	# else
	int curr_length_w = length_w;
	int curr_length_h = length_h;
	# endif

	k1 = (float *)(calloc(curr_length_h*curr_length_w,sizeof(float)));
	observ = (float **)(calloc(curr_length_h*curr_length_w,sizeof(float*)));
	for (i = 0; i < (curr_length_h*curr_length_w); i++ )
	{
		observ[i] = (float*)calloc(3,sizeof(float));
	}
			
	int loop = dim; 

    double norm = 0;
	double scalar = 0;
	
	for(i = 0; i <curr_length_h*curr_length_w; i++) 
	{
# ifndef LOCAL_MEAN
		observ[i][0]= (float)local_mean_b[i]- (float) mean_buffer[obj_num][i][0];
		observ[i][1]= (float)local_mean_g[i]- (float) mean_buffer[obj_num][i][1];
		observ[i][2]= (float)local_mean_r[i]- (float) mean_buffer[obj_num][i][2];
# else
		observ[i][0]= (float)cvmGet(local_mean_b, 0, i)- (float) mean_buffer[obj_num][i][0];
		observ[i][1]= (float)cvmGet(local_mean_g, 0, i)- (float) mean_buffer[obj_num][i][1];
		observ[i][2]= (float)cvmGet(local_mean_r, 0, i)- (float) mean_buffer[obj_num][i][2];
		
# endif
	}

	for(int m=0; m < loop /*dim*/ ;m++)
	{
		if( frame_cnt_dup[obj_num] == m) /* dim */   /* frame_cnt_dup instead of frame_cnt ****/
		{
			for(int i = 0; i <curr_length_h*curr_length_w; i++) 
			{
				for(int  k = 0; k < 3; k++)
				{
					subspace[obj_num][i][m][k]= observ[i][k]; /* observation is already mean-centered */
				}			
			}
		}
		else 
		{
			/***** B plane : start *****/			
			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][0]*subspace[obj_num][i][m][0]); /* norm */
			norm = sqrt(norm);

			if(0 != norm ) 
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][0]/norm);
			}
			else
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0


			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][0]*subspace[obj_num][i][m][0])/norm;
				scalar += (observ[i][0]*k1[i]);
			}
			/**********************/

			/**** frame_cnt_dup instead of frame_cnt ****/
			for(int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				subspace[obj_num][i][m][0]=((frame_cnt_dup[obj_num]- 1 - AP)/frame_cnt_dup[obj_num])*subspace[obj_num][i][m][0]
					+
					(observ[i][0]*(AP+1)/frame_cnt_dup[obj_num])*((float)scalar);
			}

			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][0]*subspace[obj_num][i][m][0]);
			norm = sqrt(norm);

			if(0 != norm )  
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][0]/norm);
			}
			else 
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0

			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][0]*subspace[obj_num][i][m][0])/norm;
				scalar += (observ[i][0]*k1[i]);
			}
			/**********************/
			for(int i = 0; i <curr_length_h*curr_length_w; i++) 
			{
				observ[i][0]= observ[i][0] - ((float)scalar*k1[i]);
			}
			/**** B plane : end ********/


			/***** G plane : start *****/			
			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][1]*subspace[obj_num][i][m][1]);
			norm = sqrt(norm);

			if(0 != norm )  
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][1]/norm);
			}
			else 
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0

			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][1]*subspace[obj_num][i][m][1])/norm;
				scalar += (observ[i][1]*k1[i]);
			}
			/**********************/
			
			for(int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				subspace[obj_num][i][m][1]=((frame_cnt_dup[obj_num]- 1 - AP)/frame_cnt_dup[obj_num])*subspace[obj_num][i][m][1]
					+
						(observ[i][1]*(AP+1)/frame_cnt_dup[obj_num])*((float)scalar);
			}

			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][1]*subspace[obj_num][i][m][1]);
			norm = sqrt(norm);

			if(0 != norm ) 
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][1]/norm);
			}
			else 
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0

			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][1]*subspace[obj_num][i][m][1])/norm;
				scalar += (observ[i][1]*k1[i]);
			}
			/**********************/
			for(int i = 0; i <curr_length_h*curr_length_w; i++) 
			{
				observ[i][1]= observ[i][1] - ((float)scalar*k1[i]);
			}
			/**** G plane : end ********/


			/***** R plane : start *****/			
			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][2]*subspace[obj_num][i][m][2]); /* norm */
			norm = sqrt(norm);

			if(0 != norm ) 
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][2]/norm);
			}
			else
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0


			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][2]*subspace[obj_num][i][m][2])/norm;
				scalar += (observ[i][2]*k1[i]);
			}
			/**********************/

			/**** frame_cnt_dup instead of frame_cnt ****/
			for(int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				subspace[obj_num][i][m][2]=((frame_cnt_dup[obj_num]- 1 - AP)/frame_cnt_dup[obj_num])*subspace[obj_num][i][m][2]
					+
					(observ[i][2]*(AP+1)/frame_cnt_dup[obj_num])*((float)scalar);
			}

			norm = 0;
			for ( int i = 0; i < (curr_length_h*curr_length_w); i++)
				norm += (subspace[obj_num][i][m][2]*subspace[obj_num][i][m][2]);
			norm = sqrt(norm);

			if(0 != norm )  
			{
				for(int i = 0; i <curr_length_h*curr_length_w; i++) 
					k1[i]= (float)(subspace[obj_num][i][m][2]/norm);
			}
			else 
				memset(k1, 1 , (curr_length_h*curr_length_w)*sizeof(float)); //0

			/**** calc scalar *****/
			scalar = 0;
			for (int i = 0; i <curr_length_h*curr_length_w; i++)
			{
				//scalar += (observ[i][2]*subspace[obj_num][i][m][2])/norm;
				scalar += (observ[i][2]*k1[i]);
			}
			/**********************/
			for(int i = 0; i <curr_length_h*curr_length_w; i++) 
			{
				observ[i][2]= observ[i][2] - ((float)scalar*k1[i]);
			}
			/**** R plane : end ********/
		}
        
        /////////////////////////////////////////////////
		/**** normalize columns of the eigen vectors ***/ 
		/////////////////////////////////////////////////

		/***** now normalize vectors to make the subspace orthonormal ******/
		double sum1 = 0;
		double sum2 = 0;
		double sum3 = 0;

		for(int i = 0; i <curr_length_h*curr_length_w; i++) 
		{
			sum1 += abs(subspace[obj_num][i][m][0]);
			sum2 += abs(subspace[obj_num][i][m][1]);
			sum3 += abs(subspace[obj_num][i][m][2]);
		}

		for(int i = 0; i <curr_length_h*curr_length_w; i++) 
		{
			if ( sum1 )
				subspace[obj_num][i][m][0] = float (subspace[obj_num][i][m][0]/sum1);
			else
				subspace[obj_num][i][m][0] = 0 ;

			if ( sum2 )
				subspace[obj_num][i][m][1] = float (subspace[obj_num][i][m][1]/sum2);
			else
				subspace[obj_num][i][m][1] = 0 ;

			if ( sum3 )
				subspace[obj_num][i][m][2] = float (subspace[obj_num][i][m][2]/sum3);
			else
				subspace[obj_num][i][m][2] = 0 ;
		}
	}

	/**** free memory *****/
	free(k1);

	for (i = 0; i < (curr_length_h*curr_length_w); i++ )
		free(observ[i]);
	free(observ);
} 


double compare_norm_hist(IplImage *initial_ref_obj, IplImage *reference)
{
	double similarity = 0;

    int hist_size = 256;			// size of histogram (number of bins)
	float range_0[]= {(float)0,(float)hist_size};
	float* ranges[] = { range_0 };
	CvHistogram *currentHistogram, *initialHistogram;
	IplImage *ref, *init;
	int color_planes;

	currentHistogram = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	initialHistogram = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

	for ( color_planes = 2; color_planes >= 0; color_planes-- )
	{
		// colors stored as G,R,B in openCV
		ref = cvCreateImage(cvSize(reference->width, reference->height), reference->depth, 1);
		init = cvCreateImage(cvSize(reference->width, reference->height), reference->depth, 1);

		//converts all values into the red value of the original image image is arranged by BGR for each channel
		for (int i = 0; i < ref->height; i++)
		{
			for (int j = 0; j < ref->width; j++) 
			{
				unsigned char r_o = reference->imageData[i * reference->widthStep + j * reference->nChannels + color_planes];
				unsigned char i_o = initial_ref_obj->imageData[i * reference->widthStep + j * reference->nChannels + color_planes];
				
				ref->imageData[i * reference->width + j] = r_o;
				init->imageData[i * reference->width + j] = i_o;
			}
		}
		
		cvCalcHist( &ref, currentHistogram, 0, NULL );
		cvNormalizeHist(currentHistogram, 1); 
	
		cvCalcHist( &init, initialHistogram, 0, NULL );
		cvNormalizeHist(initialHistogram, 1); 

		similarity += (1 - cvCompareHist(currentHistogram,initialHistogram,CV_COMP_BHATTACHARYYA));  // subarna
	}

	return (similarity/3);  //normalized distance
}




void initVars(int tot_obj_num)
{
	int i,j,k,l;
	int max_height, max_width;

	num_vect = 3; /* max value can be 8 */

# ifndef OBJ_FEAT_SIZE

	# ifdef GENERIC
	length_w = 60; //30;
	length_h = 80; //40;
	# endif

	if(Exchange_Data_p->auto_init_basis_dim)
	{
	  Exchange_Data_p->b_w = length_w;
	  Exchange_Data_p->b_h = length_h;
	}
	else
	{
		//length_w = Exchange_Data_p->b_w;
		//length_h = Exchange_Data_p->b_h;
	}
# else
	memset(length_w, 0, MAX_NUM_OBJECTS*sizeof(int));
	memset(length_h, 0, MAX_NUM_OBJECTS*sizeof(int));

	# ifdef GENERIC
	length_w[0] = 20; //90; //60 //30;
	length_h[0] = 40; //60; //80 //40;

	for (i = 1; i < MAX_NUM_OBJECTS; i++ )
	{
		length_w[i] = length_w[0];
		length_h[i] = length_h[0];
	}
	max_height = length_h[0];
	max_width = length_w[0];

	# endif

	for (i = 0; i < MAX_NUM_OBJECTS; i++)
	{
		if ( max_height < length_h[i]) { max_height = length_h[i];}
		if ( max_width < length_w[i]) { max_width = length_w[i];}
	}
# endif


	num_particles = Exchange_Data_p->num_particles; //300; 

/**** write number of BASIS_NAL period ****/
# ifdef PARAM_FILE_WRITE
	FILE *param_file;
	param_file = fopen("param_file.dat", "ab");
	fwrite(&num_vect,1, sizeof(unsigned short), param_file);
	fwrite(&length_w,1, sizeof(unsigned short), param_file);
	fwrite(&length_h,1, sizeof(unsigned short), param_file);
	fclose(param_file);
# endif


# ifndef OBJ_FEAT_SIZE
	max_height = length_h;
	max_width = length_w;
# endif


    /******** newV *************************/
	newV = (uchar***)(calloc(num_particles,sizeof(uchar**)));
	for ( i = 0; i < num_particles; i++)
	{
		newV[i] = (uchar**)(calloc(max_height*max_width,sizeof(uchar*)));
		for (j = 0; j < max_height*max_width; j++)
		{
			newV[i][j] = (uchar*)(calloc(3,sizeof(uchar)));
			for ( k = 0; k < 3; k++ )
			{
				newV[i][j][k] = 0;
			}
		}
	}

	/******* xmat *******************************************/
	xmat = (uchar***)(calloc(num_particles,sizeof(uchar**)));
    for ( i = 0; i < (num_particles); i++)
	{
		xmat[i] = (uchar**)(calloc(max_height*max_width,sizeof(uchar*)));
		for (j = 0; j < max_height*max_width ; j++)
		{
			xmat[i][j] = (uchar*)(calloc(3,sizeof(uchar)));
			for ( k = 0; k < 3; k++ )
			{
				xmat[i][j][k] = 0;
			}
		}
	}

# ifndef LOCAL_MEAN
	/**** local means/ or current observations  *****/
	local_mean_r = (uchar*)(calloc(max_height*max_width,sizeof(uchar)));
	local_mean_g = (uchar*)(calloc(max_height*max_width,sizeof(uchar)));
	local_mean_b = (uchar*)(calloc(max_height*max_width,sizeof(uchar)));
# else
	local_mean_r = cvCreateMat(1, (length_h[0]*length_w[0]), CV_64FC1);
	local_mean_g = cvCreateMat(1, (length_h[0]*length_w[0]), CV_64FC1);
	local_mean_b = cvCreateMat(1, (length_h[0]*length_w[0]), CV_64FC1);
# endif


# ifndef REF_MAT
	/************** ref_mat **********************************/
	ref_mat = (int **)(calloc(max_height*max_width,sizeof(int**)));
	for ( i = 0; i < max_height*max_width; i++)
	{
		ref_mat[i] = (int*)(calloc(3,sizeof(int*)));
		for (j = 0; j < 3; j++)
		{
			ref_mat[i][j]= 0;
		}
	}
# else
	ref_mat = cvCreateMat(3, (length_h[0]*length_w[0]), CV_64FC1);
# endif

	/******** subspace *************************/
	subspace = (float****)(calloc(tot_obj_num,sizeof(float***)));
	for ( l = 0; l < tot_obj_num; l++ )
	{
		# ifdef OBJ_FEAT_SIZE
		int curr_length_w = length_w[l];
		int curr_length_h = length_h[l];
		# else
		int curr_length_w = length_w;
		int curr_length_h = length_h;
		# endif

		subspace[l] = (float***)(calloc(curr_length_h*curr_length_w,sizeof(float**)));
		for ( i = 0; i < (curr_length_h*curr_length_w); i++)
		{
			subspace[l][i] = (float**)(calloc(num_vect,sizeof(float*)));
			for (j = 0; j < num_vect; j++)
			{
				subspace[l][i][j] = (float*)(calloc(3,sizeof(float)));
				for ( k = 0; k < 3; k++ )
				{
					subspace[l][i][j][k] = 0;
				}
			}
		}
	}
	/**** mean_buffer   *****/
	mean_buffer = (uchar***)(calloc(tot_obj_num,sizeof(uchar**)));
	for ( int l = 0; l < tot_obj_num; l++ )
	{
		# ifdef OBJ_FEAT_SIZE
		int curr_length_w = length_w[l];
		int curr_length_h = length_h[l];
		# else
		int curr_length_w = length_w;
		int curr_length_h = length_h;
		# endif

		mean_buffer[l] = (uchar**)(calloc(curr_length_h*curr_length_w,sizeof(uchar*)));
		for ( i = 0; i < (curr_length_h*curr_length_w); i++)
		{
			mean_buffer[l][i] = (uchar*)(calloc(3,sizeof(uchar)));
			
			for ( k = 0; k < 3; k++ )
			{
				mean_buffer[l][i][k] = 0;
			}
		}
	}
}

void releaseVars(int tot_obj_num)
{
	int i,j;

# ifndef REF_MAT
	/**** ref_mat *******/
	for ( i = 0; i < max_height*max_width; i++)
	{
		free(ref_mat[i]);
	}
	free(ref_mat);
# else	
	cvReleaseMat(&ref_mat);
# endif
   
	
	/******* xmat *******************************************/
	for ( i = 0; i < num_particles; i++)
	{
		for (j = 0; j < (max_height*max_width); j++)
		{
			if ( xmat[i][j]) { free(xmat[i][j]);xmat[i][j] = NULL;}
		}
		if ( xmat[i] ) { free(xmat[i]);xmat[i] = NULL;} 
	}
	if ( xmat) { free(xmat); xmat = NULL;}	


	/******** newV *************************/
	for ( i = 0; i < num_particles; i++)
	{
		for (j = 0; j < (max_height*max_width); j++)
		{
			if (newV[i][j]) {free(newV[i][j]); newV[i][j] = NULL;}
		}
		if ( newV[i] ) { free(newV[i]); newV[i] = NULL;}
	}

	if (newV) { free(newV); newV = NULL;}


# ifndef LOCAL_MEAN
	/**** local means  *****/
	free(local_mean_r) ;
	free(local_mean_g) ;
	free(local_mean_b) ;
# else
	cvReleaseMat(&local_mean_r);
	cvReleaseMat(&local_mean_g) ;
	cvReleaseMat(&local_mean_b) ;
# endif

	/******** subspace *************************/
	for ( int l = 0; l < tot_obj_num; l++ )
	{
		# ifdef OBJ_FEAT_SIZE
		int curr_length_w = length_w[l];
		int curr_length_h = length_h[l];
		# else
		int curr_length_w = length_w;
		int curr_length_h = length_h;
		# endif

		for ( i = 0; i < (curr_length_h*curr_length_w); i++)
		{
			for (j = 0; j < num_vect; j++)
				free(subspace[l][i][j]);
			free(subspace[l][i]);
		}
		free(subspace[l]);
	}
	free(subspace);

	/**** mean_buffer *****/
	for ( int l = 0; l < tot_obj_num; l++ )
	{
		# ifdef OBJ_FEAT_SIZE
		int curr_length_w = length_w[l];
		int curr_length_h = length_h[l];
		# else
		int curr_length_w = length_w;
		int curr_length_h = length_h;
		# endif

		for ( i = 0; i < (curr_length_h*curr_length_w); i++)
		{
			free(mean_buffer[l][i]);
		}
		free(mean_buffer[l]);
	}
	free(mean_buffer);
}

void create_average_content(IplImage *avg_obj, int obj_num)
{
	int count=0;

	for (int y=0; y<avg_obj->height; y++)
	{
	  for (int x=0; x<avg_obj->width; x++)
	  {
      	for (int c=0; c<avg_obj->nChannels; c++) 
		{
      		int idx=x*avg_obj->nChannels+y*avg_obj->widthStep+c;
      		avg_obj->imageData[idx] = mean_buffer[obj_num][count][c];
      	}
        count++;  
  	  }	
	}
}


void create_appearance_content(IplImage *appearance, int obj_num, int dim)
{
	int count=0;

	for (int y=0; y<appearance->height; y++)
	{
	  for (int x=0; x<appearance->width; x++)
	  {
      	for (int c=0; c<appearance->nChannels; c++) 
		{
      		int idx=x*appearance->nChannels+y*appearance->widthStep+c;
      		appearance->imageData[idx] = 0;

			for (int num_vect_index = 0; num_vect_index < dim ; num_vect_index++ )
			{
				appearance->imageData[idx] += (char)(subspace[obj_num][count][num_vect_index][c]);
			}
			appearance->imageData[idx] += mean_buffer[obj_num][count][c];
      	}
        count++;  
  	  }	
	}
}


 
# endif

