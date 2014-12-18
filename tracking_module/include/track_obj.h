/***************************************************************************************/
/*! Project component: tracking
 *  \file   track_obj.h
 *  \brief  Purpose: This file contains the prototypes for functions related to tracking of object
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

#ifndef CV_PARTICLE_OBSERVE_TEMPLATE_H
#define CV_PARTICLE_OBSERVE_TEMPLATE_H

# include "defines.h"
# include "cvparticle.h"
# include "compression.h"

# define ABS(a) ((a) < 0 ? -(a) : (a))


/********************* Globals **********************************/
int num_observes = 1;
CvSize feature_size;

/******************** Function Prototypes **********************/
//extern 
void cvParticleObserveInitialize( CvSize featsize );

int cvParticleObserveLikelihood( int obj_number, CvParticle* p, IplImage* cur_frame, IplImage *pre_frame, CvBox32f box32f, double *min_dist, double *avg_min_dist, int spread_r, int spread_g, int spread_b, CvSize featsize);

/**
 * Initialize
 */
void cvParticleObserveInitialize( CvSize featsize )
{
    feature_size = featsize;
}


int cvParticleObserveLikelihood( int obj_number, CvParticle* p, IplImage* frame, IplImage *reference, CvBox32f box, double *min_dist, double *avg_min_dist, int spread_r, int spread_g, int spread_b, CvSize featsize )
{
	//box : pass this param, last particle will be previously-tracked position only 			
    int i, j,sample_index;	
	double max_dist = 0;

# ifdef OBJ_FEAT_SIZE
	feature_size = featsize;
	int length_h = feature_size.height; 
	int length_w = feature_size.width;
# endif

	int dim = (frame_cnt_dup[obj_number] > num_vect)? num_vect:frame_cnt_dup[obj_number];  /*** frame_cnt_dup instead of frame_cnt ***/
	
	IplImage* avg_obj = NULL, *appearance = NULL;
	avg_obj = cvCreateImage( cvSize(reference->width, reference->height) , reference->depth, reference->nChannels);
	appearance = cvCreateImage( cvSize(reference->width, reference->height) , reference->depth, reference->nChannels);

    create_average_content(avg_obj, obj_number );
	create_appearance_content(appearance, obj_number, dim);

	float *likeli1;
	float *likeli2;
	uchar *newV_R, *newV_G, *newV_B;
	int *I;
	double *SD;
	likeli1 = (float*)(calloc(num_particles,sizeof(float)));
	likeli2 = (float*)(calloc(num_particles,sizeof(float)));
	newV_R = (uchar*)(calloc(length_h*length_w,sizeof(uchar)));
	newV_G = (uchar*)(calloc(length_h*length_w,sizeof(uchar)));
	newV_B = (uchar*)(calloc(length_h*length_w,sizeof(uchar)));

    SD = (double *)(calloc((num_particles/2),sizeof(double)));
	I = (int*)(calloc((num_particles/2),(sizeof(int))));

    IplImage *patch = NULL;
    IplImage *resize = NULL;
    resize = cvCreateImage( feature_size, frame->depth, frame->nChannels );
    
	CvParticleState s; 
	CvBox32f box32f; 

# ifndef REF_MAT
	char **ref_mat = NULL;	//int; //test : subarna
	
	ref_mat = (char **)(calloc(length_h*length_w,sizeof(char**))); //int; //test : subarna 
	for ( i = 0; i < length_h*length_w; i++)
	{
		ref_mat[i] = (char*)(calloc(3,sizeof(char*))); //int; //test : subarna
		for (j = 0; j < 3; j++ )
			ref_mat[i][j] = 0;
	}
# else
	CvMat *ref_mat = cvCreateMat(3, (length_h*length_w), CV_64FC1);
# endif

    for( i = 0; i < p->num_particles; i++ ) 
    {
		if ( i == (p->num_particles - 1))
		{
			box32f = cvBox32f(box.cx, box.cy, box.width, box.height, box.angle);
		}
		else
		{
			s = cvParticleStateGet( p, i );
			box32f = cvBox32f( (float)s.x, (float)s.y, (float)s.width, (float)s.height, (float)s.angle );
		}
		
        CvRect32f rect32f = cvRect32fFromBox32f( box32f );
        CvRect rect = cvRectFromRect32f( rect32f );
        
        patch = cvCreateImage( cvSize(rect.width,rect.height), frame->depth, frame->nChannels );
        cvCropImageROI( frame, patch, rect32f, cvPoint2D32f(0,0) );

# ifdef WIN32 /*** not sure whether it applies to linux also ****/	

		if(0 == patch->origin)
			cvFlip(patch,patch);

# endif

        cvResize( patch, resize );
     
		int count = 0;

# ifndef REF_MAT
		for (int y=0; y<resize->height; y++)
		{
			for (int x=0; x<resize->width; x++) 
			{			
             	for (int c=0; c<resize->nChannels; c++) 
				{
					count = y*resize->width + x;
      				int idx=x*resize->nChannels+y*resize->widthStep+c;
      				//int val=reference->imageData[idx];  // subarna : commented
					int val=resize->imageData[idx];
      				ref_mat[count][c]=val;
	   			} 
			}
		}
# else
        CvMat *PreMat, hdr;
		PreMat = cvCreateMat(resize->height, resize->width, CV_64FC3);
		cvConvert(reference, PreMat);
		cvTranspose(cvReshape(PreMat, &hdr, 1, (resize->height* resize->width)), ref_mat); 
		cvReleaseMat(&PreMat);
# endif


		for (int y=0; y<resize->height; y++)
		{
			for (int x=0; x<resize->width; x++) 
			{
				count = y*resize->width + x;
				for (int c=0; c<resize->nChannels; c++) 
				{
# ifndef REF_MAT
					xmat[i][count][c] = (uchar)ref_mat[count][c];
# else
      				xmat[i][count][c] = (uchar)cvmGet(ref_mat, c, count);
# endif
				}
			}
        }
    
		/***** returns the distance from average  ********/		
		likeli1[i] = (float) cvNorm( resize, avg_obj, CV_L2 );
		/*** but average shape can undergo many changes, better to keep last 3/4 frames average ***/

		if ( likeli1[i] > max_dist)
		{
			max_dist = likeli1[i];
		}

		likeli2[i] = likeli1[i];

        cvReleaseImage( &patch );
    }
	/**** measurement and re-sample ******/
	cvParticleSetWeight(p, likeli2, max_dist);
	/******************************/

# ifndef REF_MAT
	for ( i = 0; i < length_h*length_w; i++)
	{
		free(ref_mat[i]);
	}
	free(ref_mat);
# else
	cvReleaseMat(&ref_mat);
# endif

	/////////// added : subarna
	quicksort(likeli1,likeli1+(num_particles-1));
	///////////////////////////////
	memset(I,0,(num_particles/2)*sizeof(int));  //subarna
    

    for(i=0;i <(num_particles/2);i++)  
	{ 
		for(j=0; j < (p->num_particles) ; j++) 
		{
			//subarna
			int a = (int)(likeli1[i]); 
			int b = (int)(likeli2[j]); 
			if ( a == b )
			{
				I[i]=j;
				break;    
			}
		}
	}	

	for(i=0; i<(num_particles/2); i++)
	{
# ifndef NEW_MEM_PTR_CPY
		for ( int j = 0; j < (length_w*length_h); j++ )
		{
			for (int c =0; c < 3; c++ )
				newV[i][j][c] = xmat[I[i]][j][c];
		}		
# else
		newV[i] = (uchar**)xmat[I[i]];
# endif
	}
    
	//memset(SD, 0 , (num_particles/2)*sizeof(double));

# ifdef DIST_TO_SS
	closest_to_subspace(obj_number, SD, appearance /*reference*/, spread_r, spread_g, spread_b); 
# endif
	
	*avg_min_dist = likeli1[0]/(feature_size.width*feature_size.height*3);

    int index = 0;
	
	
	// till the subspace is not well formed (assume at least 2 observations), we will use the mean-color as measurement model
# ifdef DIST_TO_SS
	if ( frame_cnt_dup[obj_number] < 4 /*2 */ )
	{
		*min_dist = likeli1[0]/(feature_size.width*feature_size.height*3);
	}
	else
	{
		*min_dist = SD[0]/(feature_size.width*feature_size.height*3); //SD[0];
	}
# else
	*min_dist = likeli1[0]/(feature_size.width*feature_size.height*3);
# endif

	if ( frame_cnt_dup[obj_number] > 1 )
	{
		for(i = 1; i < ( num_particles/2) - 1 ; i++) //subarna : i starts from 1 instead of 0
		{
			float temp_dist1, temp_dist2, temp_dist;
		
			// per pixel error
			temp_dist1 = likeli1[i]/(feature_size.width*feature_size.height*3);

			//temp_dist2 = SD[i];  //per pixel reconstruction error
			temp_dist2 = (float)(SD[i]/(feature_size.width*feature_size.height*3));

# ifdef DIST_TO_SS
			if ( frame_cnt_dup[obj_number] < 2 )
			{
				temp_dist = temp_dist1; 
				if(temp_dist < *min_dist ) 
				{
					*avg_min_dist = temp_dist;
					*min_dist = temp_dist;
					index = i;
				}
			}
			else
			{
				temp_dist = temp_dist2;  //temp_dist1
				if(temp_dist < *min_dist ) 
				{
					*min_dist = temp_dist;
					index = i;
				}
			}
# else
			temp_dist = temp_dist1; 
			if(temp_dist < *min_dist ) 
			{
				*avg_min_dist = temp_dist;
				*min_dist = temp_dist;
				index = i;
			}
# endif			

		}
	}
	
    
	for(i=0; i<(feature_size.width*feature_size.height); i++)
	{
        newV_R[i] = newV[index][i][2];
		newV_G[i] = newV[index][i][1];
		newV_B[i] = newV[index][i][0];	
	}
    /*********************************************/
   

# ifdef DEBUG_PRINT
	if ( I[index] != (num_particles - 1) )
	{
		printf ("\n..... bounding box changed at least one parameter(s) ....");
	}
# endif

	sample_index = I[index];

	
	if ( NULL != likeli1 ) {free(likeli1);likeli1 = NULL;}
	if ( NULL != likeli2 ){free(likeli2);likeli2 = NULL;}
	if ( NULL != newV_R) {free(newV_R);newV_R = NULL;}
	if ( NULL != newV_G) {free(newV_G);newV_G = NULL;}
	if ( NULL != newV_B) {free(newV_B);newV_B = NULL;}
	if ( NULL != I) {free(I); I = NULL;} 
	if ( NULL != SD){ free(SD); SD = NULL;}
	if ( NULL != avg_obj ){ cvReleaseImage( &avg_obj );	avg_obj = NULL;}
	if ( NULL != appearance ){ cvReleaseImage( &appearance ); appearance = NULL;}

	return sample_index; 
}

#endif
