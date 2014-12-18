/***************************************************************************************/
/*! Project component: OBJECT_TRACKER
 *  \file   appearance_tracker.c
 *  \brief  Purpose: This file contains the object tracker and coder functions
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009-11-12
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 12 November   2009 ST: Creation
 *  - 23 August     2010 ST: adding ROI NAL formation
 ***************************************************************************************
 ***************************************************************************************/
# include "global_defines.h"
# include "shared_mem.h"

/************************************ Headers files******************************/
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#include "track_obj.h"
#include <time.h>

#ifdef WIN32
#include "WinInterface.h"
#endif


# define DIST_THRESHOLD 0.30 /*0.25 */  /* threshold for distance from subspace */
# define SIMILARITY_THRESHOLD  0.65
# define MIN_SIMILARITY_THRESHOLD  0.45 /* at least 55% of matching in terms of normalized color range, otherwise occlusion declared*/


/****************************** Global *****************************/
# ifndef OBJ_FEAT_SIZE
extern int length_h;
extern int length_w;
# else
extern int length_h[MAX_NUM_OBJECTS];
extern int length_w[MAX_NUM_OBJECTS];
# endif

extern int num_particles;
extern int num_states;
extern int num_vect; 
CvSize featsize;


//////////// test : subarna
// stream specific for optimized result
# ifdef GENERIC
float std_x = 4; //8; //2.0;  //6
float std_y = 4; //2.0;  //6
float std_w = 2.0; //3; //2.0 //10 
float std_h = 2.0; //3; //2.0  //10
float std_r = 2; //2.0; //5.0; //4; //2.0;  
# endif

# ifdef PF_2nd_ORDER_AR
float std_xp = 4; //8; //2.0;  //6
float std_yp = 4; //2.0;  //6
float std_wp = 2.0;  //10 
float std_hp = 2.0;  //10
float std_rp = 5; //5.0; //4; //2.0;
# endif


double min_dist;
double avg_min_dist;
const char* vid_file ;
int read_avi = 0;

/******************************* Structures ************************/

typedef struct IcvMouseParam {
    CvPoint loc1;
    CvPoint loc2;
    char* win_name;
    IplImage* frame;
} IcvMouseParam;

IcvMouseParam p_temp;

# define ABS(a) ((a) < 0 ? -(a) : (a))

int read_cam = 0;
/**************************** Function Prototypes ******************/
void icvGetRegion( IplImage*, CvRect* /*, int*/ );
void icvMouseCallback( int, int, int, int, void*);

extern void makeRGBfromfile(unsigned char *Dst, int width, int height, char *name, int fr_num);

CvScalar Boundary_Color(int obj_number);
/******************************************/


/***************************************************************************************/
/*!
 *  \fn void main(int argc, char** argv)
 *  \brief This is the main function for object tracker and coder
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param IN    : int argc : number of arguments
 * \param IN    : char** argv: name of the video ( later to be configuration file ).
 * \return NONE
 *
 ***************************************************************************************/
#if (defined WIN32) 
DWORD WINAPI /*int*/ tracking_thread(LPVOID p)
# else
int main(int argc, char **argv)
# endif
{
# ifdef WIN32  
  CNTRL_STRUCT *control_data_ptr = (CNTRL_STRUCT *)p;
  int argc = control_data_ptr->argc_tracking;
  char **argv = control_data_ptr->argv_tracking;
# endif 


  /********************************************************/
#ifdef CALC_TIME
  static long long track_start, track_end;
  static long long tot_track_time = 0;
  FILE *clk1;
#endif

	/********************************************************/
	/**** input yuv related variables ****/
	int read_yuv = 0, yuv_width = 0, yuv_height = 0; 
	unsigned char *source_data = NULL;
	/************************************/

    IplImage *frame = NULL;

	bool bInitialized = false; /*** memory allocated ***/
	bool bObjectInitialized = false;

	unsigned short tot_obj_num = 1;
	int obj_number = 1, last_obj_num = 1;

	IplImage **tracked_obj = NULL;
	IplImage *initial_obj = NULL, *reference = NULL;

	/***** for subspace threshold ******/
	int spread_r = 100, spread_g = 100, spread_b = 100;
	/***********************************/

    CvCapture* video = NULL;
    int init=1; 
	/*int*/ read_avi = 0;
	int tot_frame_num = 0;

	CvParticleState std;
	CvRect32f region32f = {0,0,0,0,0};
	CvScalar color;
	CvParticleState update_state;
	CvParticleState *box_update_state = NULL;
	CvParticle *init_particle = NULL;	
	CvParticle **particle = NULL;
	CvBox32f *box = NULL; 
	CvRect32f *obj_position = NULL;
	

	/****** importance sampling ******/
	int use_importance_sampling = 1;

# ifdef IMPORTANCE_SAMPLING
	use_importance_sampling = 1;
# else
	use_importance_sampling = 0;
# endif

	# ifndef PF_2nd_ORDER_AR
	CvParticleState temp_update_state = {0,0,0,0,0};
	# else
	CvParticleState temp_update_state = {0,0,0,0,0,0,0,0,0,0};
	# endif


# ifndef OBJ_FEAT_SIZE
	char *temp_image_data = NULL;
# else
	char **temp_image_data = NULL;
# endif


	bool *object_present_flag = NULL;
	IplImage **initial_ref_obj = NULL;

	Exchange_Data_p->x_last = NULL;
	Exchange_Data_p->y_last = NULL;;
	Exchange_Data_p->width_last = NULL;
	Exchange_Data_p->height_last = NULL;
	Exchange_Data_p->angle_last = NULL;

	double best_similarity, similarity;
	
	/*********************************/

	system("mkdir track_result");
  
	if( /*(argc < 2 ) &&*/ (strlen(argv[1])) < 5 )
    {
        printf("capturing video from camera instead of AVI or YUV \n");
		printf("if you want to read file, then please exit and enter AVI/YUV file name in the config file \n\n\n");
		read_avi = 0;
		read_cam = 1;
    }	
	else if ( 1 == atoi(argv[2]) ) 
		read_avi = 1;
	else
		read_avi = 0;

	
	read_yuv = ( 1 - atoi(argv[2]));
	if ( (read_yuv == 1 ) && (argc > 4 ))
	{
		yuv_width = atoi(argv[3]);
		yuv_height = atoi(argv[4]);
	}

	//capture video
	if ( 1 == read_avi )
	{
		cvSetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES, 0);
		video = cvCaptureFromFile(argv[1]);

		tot_frame_num = (int)cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_COUNT);
		Exchange_Data_p->tot_frame_num = tot_frame_num;
	}
	else if (1 == read_cam) 
		video = cvCaptureFromCAM(CV_CAP_ANY);

	if( ( 1 == read_cam ) || (read_avi == 1) )
	{
		if( (frame = cvQueryFrame( video )) == NULL )
		{
			if ( read_avi == 1)
			{
				vid_file=argv[1];
				fprintf( stderr, "Video %s is not loadable.\n", vid_file );
				printf( "Video %s is not loadable.\n", vid_file );
			}
			else
				fprintf( stderr, "cannot read from camera captured.\n" );
			exit(1);
		}
	}
	else if (read_yuv == 1 )
	{	
		FILE *fp = fopen(argv[1],"rb");
		fseek(fp, 0, SEEK_END);
		tot_frame_num = ftell(fp)/((int)(yuv_width* yuv_height*1.5));
		Exchange_Data_p->tot_frame_num = tot_frame_num;
		fclose(fp);
		frame = cvCreateImage( cvSize(yuv_width, yuv_height) , 8 /*frame->depth*/, 3 /*frame->nChannels*/); 
		frame->dataOrder = 0;
		frame->alphaChannel = 0;
		frame->origin = 1;
		frame->align = 4;
		frame->ID = 0;
		frame->widthStep = frame->width*3;
		frame->imageSize = frame->height*frame->widthStep;

		source_data = (unsigned char*)(calloc(frame->width*frame->height*3,sizeof(unsigned char)));  
	}
	/**************************************************************/

# ifdef MAKE_VIDEO
	CvVideoWriter *tracked_writer = NULL;
# endif
	
	int isColor = 1;
	int fps     = 12;  // or 30
	int frameW  = frame->width; // 744 for firewire cameras
	int frameH  = frame->height; // 480 for firewire cameras


# ifdef PARAM_FILE_WRITE
	FILE *param_file;
	param_file = fopen("param_file.dat", "wb");
	fclose(param_file);
# endif

	/*******************************************
	CV_FOURCC('P','I','M','1')    = MPEG-1 codec
	CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
	CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
	CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
	CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
	CV_FOURCC('U', '2', '6', '3') = H263 codec
	CV_FOURCC('I', '2', '6', '3') = H263I codec
	CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
	CV_FOURCC('D', 'I', 'B', ' ') = RGB avi files
	CV_FOURCC('M', 'S', 'V', 'C') = MSVC files
	CV_FOURCC('I', 'Y', 'U', 'V') = uncompressed 4:2:0 YUV
	-1, under Windows prompts for codec
	********************************************/


# ifdef MAKE_VIDEO
	tracked_writer = cvCreateVideoWriter(
			"tracking_video.avi",
			//CV_FOURCC('M', 'S', 'V', 'C')
			CV_FOURCC('D', 'I', 'B', ' ')
			/*CV_FOURCC('I', 'Y', 'U', 'V')*/,
			fps,
            cvSize(frameW,frameH),
			isColor
            );
# endif

# ifdef H264_COMPRESS
	// reconstruction related 
	InitLookupTable();
# endif

	frame_cnt = 0;

	Exchange_Data_p->End_Of_Bitstream_Flag = 0;

   /**** frame loop ****/
   for ( ; ; )
   {
	    init = 0;
	   /***********************************/
		char c = (char) cvWaitKey( 10 );
		unsigned short initialization_period = 500;
		if(c == '\x1b')
			break;
		else if( c == 'c') 
			init=1;

		if ( 0 == (frame_cnt % initialization_period ))  //15
		{
			init = 1;
		}


		Exchange_Data_p->init_frame = init;

		/**** write initialization period ****/
# ifdef PARAM_FILE_WRITE
		param_file = fopen("param_file.dat", "ab");
		fwrite(&initialization_period,1, sizeof(unsigned short), param_file);
		fclose(param_file);
# endif

	   /**** if the input is not an AVI, 4:2:0 YUV then convert YUV to RGB before the algorithm starts ******/
		if (read_yuv == 1 )
		{
			if (frame_cnt >= tot_frame_num)
				break;

			makeRGBfromfile(source_data, frame->width, frame->height, argv[1], frame_cnt );
			/**** store Iplimage format, so that openCV can understand it for further processing ****/
			for ( int iRow = 0; iRow < frame->height; iRow++ ) 
			{
				memcpy( (frame->imageData + iRow*frame->width*3),(source_data + (iRow*frame->width*3) ), frame->width*3*sizeof(char));  
			}
		}
		else /*( read_avi == 1 )*/
		{
			if ( frame_cnt != 0 )
			{
				cvSetCaptureProperty(video, CV_CAP_PROP_POS_FRAMES, frame_cnt);
				if((frame = cvQueryFrame( video ) ) == NULL)	
					break;
			}
		}

	   	if( 1 == init ) 
		{
			static int counter = -1;
			counter++;

			char c = (char) cvWaitKey( 10 );
			if(c == '\x1b')
				break;
			if ( !frame )
				break;

			# ifdef MULTIPLE_OBJECTS
			/*
			printf("\n\nEnter total number of objects :  ");
			scanf("%d", &tot_obj_num);
			*/
			tot_obj_num = 5;
			# else
			tot_obj_num = 1;
			# endif

			tot_obj_num = Exchange_Data_p->num_objects;
			Exchange_Data_p->tot_obj_num = tot_obj_num;


			/**** write number of objects period ****/
# ifdef PARAM_FILE_WRITE
			param_file = fopen("param_file.dat", "ab");
			fwrite(&tot_obj_num,1, sizeof(unsigned short), param_file);
			fclose(param_file);
# endif


			if (bInitialized == true )
			{
				if (NULL != temp_image_data ) 
				{
				# ifndef OBJ_FEAT_SIZE
					free(temp_image_data);					
				# else
					for (int i = 0; i < last_obj_num; i++)
					{
						free(temp_image_data[i]);
						temp_image_data[i] = NULL;
					}
				# endif
					temp_image_data = NULL;
				}

				releaseVars(last_obj_num);
				for ( int i = 0; i < last_obj_num; i++ )
				{
					if (NULL != initial_ref_obj[i]) 
					{
						cvReleaseImage( &initial_ref_obj[i] );
						initial_ref_obj[i] = NULL;
					}
					bObjectInitialized/*[obj_number]*/ = false;
					object_present_flag[i] = false;
				}

				if ( box_update_state) { free(box_update_state); box_update_state = NULL;}
				if ( obj_position ) { free(obj_position); obj_position = NULL;}
				if ( object_present_flag) { free(object_present_flag); object_present_flag = NULL;}
				if ( frame_cnt_dup) { free(frame_cnt_dup); frame_cnt_dup;}
			}

			box_update_state = (CvParticleState *)(calloc(tot_obj_num,sizeof(CvParticleState)));
			
			obj_position = (CvRect32f *)(calloc(tot_obj_num,sizeof(CvRect32f)));

			object_present_flag = (bool*)(calloc(tot_obj_num,sizeof(bool)));
			frame_cnt_dup = (int*)(calloc(tot_obj_num,sizeof(int)));
			for ( int i = 0; i < tot_obj_num; i++)
			{
				object_present_flag[i] = true;
				frame_cnt_dup[i] = 0;
			}

			last_obj_num = tot_obj_num;
			object_present_flag = (bool*)(calloc(tot_obj_num,sizeof(bool)));
			initial_ref_obj = (IplImage **)(calloc(tot_obj_num,sizeof(IplImage*)));
			box = (CvBox32f*)(calloc(tot_obj_num,sizeof(CvBox32f)));
			particle = (CvParticle**)(calloc(tot_obj_num,sizeof(CvParticle*)));

			// initialize variables
			initVars(tot_obj_num);

			bInitialized = true;

# ifndef OBJ_FEAT_SIZE
			featsize = cvSize(length_w,length_h);

			// initialization
			cvParticleObserveInitialize( featsize );

			/**** importance sampling *****/
			for ( i = 0; i < tot_obj_num ; i ++ )
				initial_ref_obj[i] = cvCreateImage( featsize, frame->depth, frame->nChannels );								
			if ( 0 != use_importance_sampling )
				temp_image_data = (char *)calloc(featsize.height*(featsize.width+3)*3,sizeof(char));
# else

			/**** importance sampling *****/
			temp_image_data = (char**)calloc(tot_obj_num,sizeof(char*));
			for ( int i = 0; i < tot_obj_num ; i ++ )
			{
				temp_image_data[i] = NULL;
				featsize = cvSize(length_w[i],length_h[i]);
				initial_ref_obj[i] = cvCreateImage( featsize, frame->depth, frame->nChannels );	

				if ( 0 != use_importance_sampling )
				{
					temp_image_data[i] = (char *)calloc(featsize.height*(featsize.width+3)*3,sizeof(char));
				}
			}

# endif
			/*************************************/
			bObjectInitialized/*[obj_number]*/ = false;

			/**** free if existing ******/
			if (Exchange_Data_p->x_last) free(Exchange_Data_p->x_last); Exchange_Data_p->x_last = NULL;
			if (Exchange_Data_p->y_last) free(Exchange_Data_p->y_last); Exchange_Data_p->y_last = NULL;
			if (Exchange_Data_p->width_last) free(Exchange_Data_p->width_last); Exchange_Data_p->width_last = NULL;
			if (Exchange_Data_p->height_last) free(Exchange_Data_p->height_last); Exchange_Data_p->height_last = NULL;
			if (Exchange_Data_p->angle_last) free(Exchange_Data_p->angle_last); Exchange_Data_p->angle_last = NULL;
			/**********************************8/


			/**** allocate memory for last saved warping params for all ROIs ****/
			Exchange_Data_p->x_last = (unsigned short*)calloc(tot_obj_num,sizeof(unsigned short*));
			Exchange_Data_p->y_last = (unsigned short*)calloc(tot_obj_num,sizeof(unsigned short*));
			Exchange_Data_p->width_last = (unsigned short*)calloc(tot_obj_num,sizeof(unsigned short*));
			Exchange_Data_p->height_last = (unsigned short*)calloc(tot_obj_num,sizeof(unsigned short*));
			Exchange_Data_p->angle_last = (unsigned short*)calloc(tot_obj_num,sizeof(unsigned short*));
			/*********************************************************************/
			
			for ( obj_number = 0; obj_number < tot_obj_num; obj_number++ )
			{
				CvRect region;
				printf("\n      click points :  \n");

				/****** allows user to select initial region ******/
				region.height = 0;
				region.width = 0;
			
# ifdef GENERIC
                icvGetRegion( frame, &region /*, obj_number*/ ); 
# endif


				bObjectInitialized/*[obj_number]*/ = true;
				object_present_flag[obj_number] = true;

				if ( ( region.height <= 8 ) || ( region.width <= 8 ) )
				{
					object_present_flag[obj_number] = false;
				}
				else
				{
					bObjectInitialized/*[obj_number]*/ = true;
					object_present_flag[obj_number] = true;
				}
				/**************************************************/


				/**** write number of object_present flag ****/
# ifdef PARAM_FILE_WRITE
				param_file = fopen("param_file.dat", "ab");
				fwrite(&object_present_flag[obj_number],1, sizeof(bool), param_file);

				/* subarna : object warping parameters for initialization frame was missing earlier */
				if ( true == object_present_flag[obj_number])
				{
					fwrite(&region,1, sizeof(CvRect), param_file);
				}
				fclose(param_file);
# endif
			
				// configure particle filter
				particle[obj_number] = cvCreateParticle( num_states, 1, num_particles );


# ifdef OBJ_FEAT_SIZE
				featsize = cvSize(length_w[obj_number],length_h[obj_number]);
# endif

				if ( true == object_present_flag[obj_number] )
				{
					init_particle = cvCreateParticle( num_states, 1, 1 );

			# ifndef PF_2nd_ORDER_AR
					std = cvParticleState (std_x,std_y,std_w,std_h,std_r);
			# else
                    std = cvParticleState (std_x,std_y,std_w,std_h,std_r,std_xp,std_yp,std_wp,std_hp,std_rp);
			# endif
					cvParticleStateConfig( particle[obj_number], cvGetSize(frame), std );
				    
					CvParticleState s;

					region32f = cvRect32fFromRect( region, 0 );

									    
					box[obj_number] = cvBox32fFromRect32f( region32f ); // centerize

# ifndef PF_2nd_ORDER_AR
					s = cvParticleState( box[obj_number].cx, box[obj_number].cy, box[obj_number].width, box[obj_number].height, 0.0 );
# else
					s = cvParticleState( box[obj_number].cx, box[obj_number].cy, box[obj_number].width, box[obj_number].height, 0.0, 
						Exchange_Data_p->x_last[obj_number],
						Exchange_Data_p->y_last[obj_number],
						Exchange_Data_p->width_last[obj_number],
						Exchange_Data_p->height_last[obj_number],
						Exchange_Data_p->angle_last[obj_number] ); 
# endif
					cvParticleStateSet( init_particle, 0, s );

					# ifdef PARAM_FILE_WRITE
				    param_file = fopen("param_file.dat", "ab");
				    fwrite(&region32f,1, sizeof(CvRect32f), param_file);
					fclose(param_file);
					# endif

				    /*** there is a need to write these into PARAM NAL associated with initialization frame ***/
					Exchange_Data_p->object_present_flag = (int)object_present_flag[obj_number];

					/* object just initialized, so warping params to be written but proj_coeff and residual data would not ***/
					Exchange_Data_p->object_init_flag = 1;  

					Exchange_Data_p->object_ID = obj_number;


					//Warping Parameters
# ifndef DIFF_WARP
					Exchange_Data_p->x = (unsigned short)region32f.x;
					Exchange_Data_p->y = (unsigned short)region32f.y;
					Exchange_Data_p->width = (unsigned short)region32f.width;
					Exchange_Data_p->height = (unsigned short)region32f.height;
					Exchange_Data_p->angle = (unsigned short)region32f.angle;
# else
					Exchange_Data_p->x = (short)region32f.x;
					Exchange_Data_p->y = (short)region32f.y;
					Exchange_Data_p->width = (short)region32f.width;
					Exchange_Data_p->height = (short)region32f.height;
					Exchange_Data_p->angle = (short)region32f.angle;
# endif					
					Exchange_Data_p->x_last[obj_number] = Exchange_Data_p->x;
					Exchange_Data_p->y_last[obj_number] = Exchange_Data_p->y;
					Exchange_Data_p->width_last[obj_number] = Exchange_Data_p->width;
					Exchange_Data_p->height_last[obj_number] = Exchange_Data_p->height;
					Exchange_Data_p->angle_last[obj_number] = Exchange_Data_p->angle;

					cvParticleInit( particle[obj_number], init_particle );
					cvReleaseParticle( &init_particle );

  					reference = cvCreateImage( featsize, frame->depth, frame->nChannels );
					initial_obj = cvCreateImage( cvSize(region.width,region.height), frame->depth, frame->nChannels );


					cvCropImageROI( frame, initial_obj, region32f, cvPoint2D32f(0,0) );


# ifdef DEBUG_PRINT
					printf("\n obj_width = %d, obj_height =%d", initial_obj->width, initial_obj->height);
# endif


#ifdef WIN32 /*** not sure whether it applies to linux also ****/
	
					if(0 == initial_obj->origin)
						cvFlip(initial_obj,initial_obj);
#endif

					cvResize( initial_obj, reference );

					initialize_subspace_mean(obj_number, reference, &spread_r, &spread_g, &spread_b);

					/**** importance sampling *****/
					cvResize( initial_obj, initial_ref_obj[obj_number]);
					/******************************/
					
					if (NULL != initial_obj) {cvReleaseImage(&initial_obj); initial_obj = NULL;}
					if (NULL != reference) {cvReleaseImage(&reference); reference = NULL;}

				    /***** added initialization frame also in output video *****/
# ifndef MULTIPLE_OBJECTS
					color = CV_RGB(0,0,255);
# else
					color = Boundary_Color(obj_number);
# endif
					cvDrawRectangle( frame, region32f, cvPoint2D32f(0,0), color );
					/***********************************************************/


# ifdef OBJ_FEAT_SIZE
					Exchange_Data_p->b_w = featsize.width;
					Exchange_Data_p->b_h = featsize.height;
# endif			
				}
			}

			init=0;
			Exchange_Data_p->init_frame = init;
	
			/*************************************/
# ifdef MAKE_VIDEO
			cvWriteFrame(tracked_writer, frame);
# endif
		}
		else
		{
			/* object initialized earlier, so warping params and proj_coeffs both are to be written ***/
			Exchange_Data_p->object_init_flag = 0;  

#ifdef CALC_TIME
			track_start = clock(); /*clock(NULL);*/
# endif

			tracked_obj = (IplImage **)(calloc(tot_obj_num,sizeof(IplImage*)));

			for ( obj_number = 0; obj_number < tot_obj_num; obj_number++ )
			{

	# ifdef OBJ_FEAT_SIZE
				featsize = cvSize(length_w[obj_number],length_h[obj_number]);
	# endif 
				reference = cvCreateImage( featsize, frame->depth, frame->nChannels );

				/********  importance sampling : while loop *******/
				best_similarity = 0;
				int iter = 0;

				if ( (bObjectInitialized == true ) && ( true == object_present_flag[obj_number]) )
				{
					for(;;)
					{				
						iter++;
						cvParticleTransition( particle[obj_number] );

					# ifdef ADDITIONAL_BOUND
						cvParticleStateAdditionalBound(particle, box[obj_number] );
					# endif

						min_dist = 0;
						avg_min_dist = 0;

						int update = cvParticleObserveLikelihood( obj_number, particle[obj_number], frame, reference, box[obj_number], &min_dist, &avg_min_dist,spread_r, spread_g, spread_b, featsize);
					
					# ifdef OCCLUSION
						/**** test *****/
						if ( ( false == object_present_flag[obj_number] ) && ((num_particles - 1) == update ) )
							update = 0;
					# endif /* OCCLUSION */

						if ( update == (num_particles - 1) )
						{
							update_state.x = box[obj_number].cx;
							update_state.y = box[obj_number].cy;
							update_state.width = box[obj_number].width;
							update_state.height = box[obj_number].height;
							update_state.angle = box[obj_number].angle;

					# ifdef PF_2nd_ORDER_AR
							update_state.xp = update_state.x;
							update_state.yp = update_state.y;
							update_state.widthp = update_state.width;
							update_state.heightp = update_state.height;
							update_state.anglep = update_state.angle;
					# endif
						}
						else
						{
							update_state = cvParticleStateGet( particle[obj_number],update);
						}
						box[obj_number] = cvBox32f( (float)update_state.x, (float)update_state.y, (float)update_state.width, (float)update_state.height, (float)update_state.angle );
						CvRect32f rect32f = cvRect32fFromBox32f( box[obj_number] );
													
						CvRect update_region = cvRectFromRect32f(rect32f );

						tracked_obj[obj_number] = cvCreateImage( cvSize(update_region.width,update_region.height), frame->depth, frame->nChannels );
						region32f = cvRect32fFromRect( update_region, box[obj_number].angle );  //added angle					
						cvCropImageROI( frame, tracked_obj[obj_number], region32f, cvPoint2D32f(0,0));

					

					#ifdef WIN32 /*** not sure whether it applies to linux also ****/
						if(0 == tracked_obj[obj_number]->origin)
							cvFlip(tracked_obj[obj_number],tracked_obj[obj_number]);
					#endif

						cvResize( tracked_obj[obj_number], reference ); //subarna

						if ( 1 == use_importance_sampling )
						{
							similarity = compare_norm_hist(initial_ref_obj[obj_number], reference);

							
							if ( similarity > best_similarity )
							{
								best_similarity = similarity;
								temp_update_state = update_state;


						# ifndef OBJ_FEAT_SIZE
								memcpy( temp_image_data, reference->imageData, reference->height*reference->widthStep*sizeof(char));  
						# else
								memcpy( temp_image_data[obj_number], reference->imageData, reference->height*reference->widthStep*sizeof(char));  
						# endif
							}


							if ( similarity > SIMILARITY_THRESHOLD) 
							{
								if ( (min_dist < DIST_THRESHOLD /* 0.1*/ ) && (avg_min_dist <= 0.60 ) )
								{
									if ( ( update_state.width > 16 ) && (update_state.height > 16 ) )
									{
										if ( false == object_present_flag[obj_number] )
											printf("\n\n\n\n\n =====> object came out of occlusion \n\n");

										object_present_flag[obj_number] = true;
									}
									break;
								}
							}
						}


						/*********************************************/
						if ( (1 == use_importance_sampling ) && (iter >= IS_ITERATIONS ) ) /* %max_iterations*/ 
						{
					# ifdef OCCLUSION
							if( 
								( min_dist > DIST_THRESHOLD ) 
								//|| ( avg_min_dist > 0.65 /* 2 */)
								|| ( best_similarity < MIN_SIMILARITY_THRESHOLD )
								)
							{							
								/**** occlusion detected ****/
								object_present_flag[obj_number] = false; 
					# ifdef DEBUG_PRINT
								printf("\n\n\n\n\n =====> occlusion detected \n");
								printf("   update_state.width = %d, update_state.height = %d \n", (int)update_state.width, (int)update_state.height);
								printf("   min_dist = %f, DIST_THRESHOLD = %f, avg_min_dist = %f, max_dist = 2.0  ", (float)min_dist, (float)DIST_THRESHOLD, (float)avg_min_dist);
								printf("   best_similarity = %f, min_similarity_threshold = %f \n\n\n\n ", (float)best_similarity, (float)MIN_SIMILARITY_THRESHOLD);
					# endif
							}

					# if 0
							/*****************************/
							else if ( ( update_state.width >= 6 ) && (update_state.height >= 6) )  //test
							{
								if( ( min_dist <= DIST_THRESHOLD /*0.1 */ ) 
								//|| ( avg_min_dist > 2.0 )
								|| (best_similarity > MIN_SIMILARITY_THRESHOLD )
								)
								{
									if ( false == object_present_flag[obj_number] )
									{
										printf("\n\n\n\n\n =====> object came out of occlusion \n\n");
									}
									object_present_flag[obj_number] = true;
								}
							}
						# endif
					# endif /* OCCLUSION */
							/*****************************/
							temp_update_state = update_state;

							break;
						}
						/*********************************************/
						if ( 0 == use_importance_sampling )
						{
							temp_update_state = update_state;

					# ifdef OCCLUSION
							if( (( update_state.width < 4 ) || (update_state.height < 4 ) ) 
								//|| ( min_dist > DIST_THRESHOLD ) 
								|| ( avg_min_dist > 0.65 /*2.0*/ )
								)
							{							
								/**** occlusion detected ****/
								object_present_flag[obj_number] = false;  
					# ifdef DEBUG_PRINT
								printf("\n\n\n\n\n =====> occlusion detected \n");
								printf("   update_state.width = %d, update_state.height = %d \n", (int)update_state.width, (int)update_state.height);
								printf("   min_dist = %f, DIST_THRESHOLD = %f, avg_min_dist = %f, max_dist = 2.0  ", (float)min_dist, (float)DIST_THRESHOLD, (float)avg_min_dist);
					# endif
							}
					# endif /*OCCLUSION */

							break;
						}
						/**** calculate posterior */
						/* already normalized; so further normalization not required */
						cvParticleResample(particle[obj_number],0); 
					}
		
				# ifdef OCCLUSION
					/**** importance sampling ******/
					if (true == object_present_flag[obj_number])
				# endif /* OCCLUSION */
					{
						update_state = temp_update_state;
						if ( 0 != use_importance_sampling )
						{
						# ifndef OBJ_FEAT_SIZE
							memcpy( reference->imageData, temp_image_data, reference->height*reference->widthStep*sizeof(char));
						# else
							memcpy( reference->imageData, temp_image_data[obj_number], reference->height*reference->widthStep*sizeof(char));
						# endif
						}
					}
				# ifdef OCCLUSION
					else 
					{
						/**** occlusion detected ****/
						/*** in case of occlusion, take random sample for PF distribution ***/
						//int update = cvRandInt( &rnggstate ) % num_particles;
						int update = num_particles-1; //0; //take last position

						update_state = cvParticleStateGet( particle[obj_number], update);
					}
				# endif /* OCCLUSION */

					box_update_state[obj_number] = update_state;
					/**** draw lines after all objects tracking, otherwise original frame will be wrongly added box line, problematic in case of overlapped objects ****/

				
					/***** update particle filter even in case of occlusion ******/
					/**** but do updation of subspace after reconstruction, otherwise no sync ****/
					
					CvParticle *init_particle1 = NULL;
					init_particle1 = cvCreateParticle( num_states, 1, 1 );
					cvParticleStateSet( init_particle1, 0, update_state );

					cvParticleInit( particle[obj_number], init_particle1 );
					cvReleaseParticle( &init_particle1 );
				}


				/*******   compression and reconstruction result ********/
				if ( (true == object_present_flag[obj_number] ) && (true == bObjectInitialized/*[obj_number]*/ ) )
				{
					int dim = (frame_cnt_dup[obj_number] > num_vect)? num_vect:(frame_cnt_dup[obj_number]); //subarna
				
				    /**** write subspace coeffs if object is present ****/
				# ifdef PARAM_FILE_WRITE
					param_file = fopen("param_file.dat", "ab");
					fwrite(a_r,1, dim*sizeof(float), param_file); /*double*/
					fwrite(a_g,1, dim*sizeof(float), param_file); /*double*/
					fwrite(a_b,1, dim*sizeof(float), param_file); /*double*/
					fclose(param_file);
				# endif

					int offset;
					offset = tracked_obj[obj_number]->imageSize; 
					Exchange_Data_p->object_present_flag = (int)object_present_flag[obj_number];
					Exchange_Data_p->object_ID = obj_number;
					Exchange_Data_p->inst_num_basis_vectors = (unsigned char)dim;


					//Warping Parameters
# ifdef DIFF_WARP
					Exchange_Data_p->x = (unsigned short)region32f.x - Exchange_Data_p->x_last[obj_number] ;
					Exchange_Data_p->y = (unsigned short)region32f.y - Exchange_Data_p->y_last[obj_number];
					Exchange_Data_p->width = (unsigned short)region32f.width - Exchange_Data_p->width_last[obj_number];
					Exchange_Data_p->height = (unsigned short)region32f.height - Exchange_Data_p->height_last[obj_number];
					Exchange_Data_p->angle = (unsigned short)region32f.angle - Exchange_Data_p->angle_last[obj_number];
					

					Exchange_Data_p->x_last[obj_number] = (unsigned short)region32f.x;
					Exchange_Data_p->y_last[obj_number] = (unsigned short)region32f.y;
					Exchange_Data_p->width_last[obj_number] = (unsigned short)region32f.width;
					Exchange_Data_p->height_last[obj_number] = (unsigned short)region32f.height;
					Exchange_Data_p->angle_last[obj_number] = (unsigned short)region32f.angle;
# else
					Exchange_Data_p->x = (unsigned short)region32f.x;
					Exchange_Data_p->y = (unsigned short)region32f.y;
					Exchange_Data_p->width = (unsigned short)region32f.width;
					Exchange_Data_p->height = (unsigned short)region32f.height;
					Exchange_Data_p->angle = (unsigned short)region32f.angle;
# endif
		
					/****************************************************************/				
					obj_position[obj_number] = region32f;

# if 0
					cvNamedWindow( "tracked_obj", 1 );
					cvShowImage( "tracked_obj", tracked_obj[obj_number] );
					cvWaitKey(-1);
# endif
					/**** do subspace-updation after reconstruction ****/
					update_mean(obj_number, reference, &spread_r, &spread_g, &spread_b);
					update_subspace(obj_number);

				    /**** write warping params if object is present ****/
# ifdef PARAM_FILE_WRITE
					param_file = fopen("param_file.dat", "ab");
					fwrite(&region32f,1, sizeof(CvRect32f), param_file);
					fclose(param_file);
# endif
				}
				Exchange_Data_p->object_present_flag = object_present_flag[obj_number];

				if (NULL != reference) {cvReleaseImage(&reference); reference = NULL;}

			}

#ifdef CALC_TIME
			track_end = clock(); /*clock(NULL);*/
			tot_track_time += (track_end - track_start);
# endif
			
			if (tracked_obj ) { free(tracked_obj); tracked_obj = NULL; }

#ifdef CALC_TIME
			track_start = clock(); /*clock(NULL);*/
# endif

			/***** draw boundary on tracked frame here after all obj tracking and reconstruction *******************/
			for ( int obj_num = 0; obj_num < tot_obj_num; obj_num++ )
			{
				if ( (true == object_present_flag[obj_num] ) && (true == bObjectInitialized/*[obj_num]*/ ) )
				{
					color = Boundary_Color(obj_num);								
					cvParticleStateDraw( box_update_state[obj_num], frame, color );
				}
			}

#ifdef CALC_TIME
			track_end = clock(); /*clock(NULL);*/
			tot_track_time += (track_end - track_start);
# endif
			// show tracked_object in frame : after tracking all the objects
			
			//cvNamedWindow( "Appearence Tracker", 1 );
    		cvShowImage( "Appearence Tracker", frame );

			/*******   write tracking video ********/
# ifdef MAKE_VIDEO
			cvWriteFrame(tracked_writer, frame);
# endif
			/***************************************/
		}
# ifdef DEBUG_PRINT
		printf("\nprocessed frame number = %d", frame_cnt ); //subarna
# endif

		frame_cnt++;

		for ( int obj_number = 0; obj_number < tot_obj_num; obj_number++ )
		{
			if ( true == object_present_flag[obj_number] )
				frame_cnt_dup[obj_number]++;
		}
	}
	printf("\ntotal frame number tracked is = %d\n", frame_cnt);


# ifdef CALC_TIME
	clk1 = fopen( "obj_track_time.csv", "w" );
	fprintf(clk1, "\n frame = %d\n", frame_cnt);
	fprintf( clk1, "%llu",	tot_track_time );
    fclose(clk1);
# endif


	if (read_yuv == 1 )
	{
		if (NULL != frame)
		{
			cvReleaseImage(&frame);
			frame = NULL;
		}
		if( NULL != source_data)
		{
		 free(source_data);
		 source_data = NULL;
		}
	}
	/**** release allocated variables  ****/
	releaseVars(tot_obj_num);

    cvDestroyWindow( "Appearence Tracker" );
    
	//if (NULL != reference) {cvReleaseImage(&reference);reference = NULL;}
	//if (NULL != initial_obj) {cvReleaseImage(&initial_obj); initial_obj = NULL;}

	for (int i = 0; i < tot_obj_num; i++)
	{
		if (NULL != initial_ref_obj[i]){ cvReleaseImage(&initial_ref_obj[i]); initial_ref_obj[i] = NULL;}
		if (NULL != particle[i]) {cvReleaseParticle(&particle[i]); particle[i] = NULL;}	
	}
	free(initial_ref_obj); initial_ref_obj = NULL;

	free(particle); particle = NULL;

	if (NULL != video) {cvReleaseCapture( &video ); video = NULL;}

# ifdef MAKE_VIDEO
	/**** video writer release ***/
	if (NULL != tracked_writer) {cvReleaseVideoWriter(&tracked_writer); tracked_writer = NULL;}
# endif

# ifndef OBJ_FEAT_SIZE
	if (NULL != temp_image_data ) {free(temp_image_data); temp_image_data = NULL;}
# else
	if (NULL != temp_image_data ) 
	{
		if ( 0 != use_importance_sampling )
		{
			for ( int i = 0; i < tot_obj_num; i++)
			{
				if (temp_image_data[i]) { free(temp_image_data[i]); temp_image_data[i] = NULL;} 
			}
		}
		temp_image_data = NULL;
	}
# endif
	/*****************************/


	Exchange_Data_p->End_Of_Bitstream_Flag = 1;


# ifdef WIN32
		{
			char command[300];
			sprintf( command, "MOVE tracking_video.avi track_result/.");
			system( command );


	# ifdef PARAM_FILE_WRITE
			sprintf( command, "MOVE param_file.dat track_result/.");
			system( command );
    # endif
		}
# endif  /* WIN32 */


#ifdef WIN32
	ReleaseSemaphore(control_data_ptr->tracking_done,1,NULL);
# ifdef PARALLEL_BCK
	WaitForSingleObject(control_data_ptr->encoding_done, INFINITE);
# endif /* ifdef PARALLEL_BCK */
	ReleaseSemaphore(control_data_ptr->end_track_code,1,NULL);
#endif  /* WIN32 */

    return 0;
}

/***************************************************************************************/
/*!
 *  \fn void icvGetRegion( IplImage* frame, CvRect* region , int obj_num )
 *  \brief Allows the user to interactively select the initial object region
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param INOUT    : IplImage* frame : entire frame 
 * \param INOUT    : CvRect* region  : region selected.
 * \return NONE
 *
 ***************************************************************************************/
void icvGetRegion( IplImage* frame, CvRect* region /*, int obj_num*/ )
{
    IcvMouseParam p;

	region->height = 1;
	region->width = 1;

    /* use mouse callback to allow user to define object regions */
    p.win_name = "Appearence Tracker";
    p.frame = frame;

    cvNamedWindow( p.win_name, 1 );
    cvShowImage( p.win_name, frame );
    cvSetMouseCallback( p.win_name, &icvMouseCallback, &p );
    cvWaitKey( 0 );
    //cvDestroyWindow( win_name );

    /* extract regions defined by user; store as a rectangle */
    region->x      = MIN( p.loc1.x, p.loc2.x );

#ifndef WIN32 /*** not sure whether it applies to linux also ****/
    region->y      = MIN( p.loc1.y, p.loc2.y );
#else
	region->y      = MIN( p_temp.loc1.y, p_temp.loc2.y ); //subarna
#endif

	region->width  = MAX( p.loc1.x, p.loc2.x ) - region->x + 1;

#ifndef WIN32 /*** not sure whether it applies to linux also ****/
	region->height = MAX( p.loc1.y, p.loc2.y ) - region->y + 1;
#else
	region->height = ABS( (p_temp.loc1.y - p_temp.loc2.y ))+ 1; 
#endif

	//printf ( "x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", p.loc1.x, p.loc1.y, p.loc2.x, p.loc2.y ); 
}

/***************************************************************************************/
/*!
 *  \fn void icvMouseCallback( int event, int x, int y, int flags, void* param )
 *  \brief Mouse callback function that allows user to specify the initial object region. 
 *
 * \author <a href="mailto: subarna.tripathi@st.com"> Subarna Tripathi (subarna.tripathi@st.com) </a>
 *
 * \date   2009-11-12
 * \param INOUT    : void* param 
 * \param IN    : int event  
 * \param IN    : int x  : x coordinate
 * \param IN    : int y  : y coordinate
 * \param IN    : int flags.  
 * \return NONE
 *
 ***************************************************************************************/
 void icvMouseCallback( int event, int x, int y, int flags, void* param )
{
    IcvMouseParam* p = (IcvMouseParam*)param;
	
	IplImage* clone = NULL;
    static int pressed = false;
	CvScalar color;

# ifndef MULTIPLE_OBJECTS
	color = CV_RGB(255,0,0);
# else
	color = Boundary_Color(0); 
# endif

 	/* on left button press, remember first corner of rectangle around object */
    if( event == CV_EVENT_LBUTTONDOWN )
    {
        p->loc1.x = x;
        p->loc1.y = y;

		clone = (IplImage*)cvClone( p->frame );

		p_temp.loc1.x = p->loc1.x;

		if ( (0 == read_avi ) && (0 == read_cam) ) //read_cam
			p_temp.loc1.y = clone->height - y; 
		else
			p_temp.loc1.y = y; 

        pressed = true;

    }

    /* on left button up, finalize the rectangle and draw it */
    else if( event == CV_EVENT_LBUTTONUP )
    {
        p->loc2.x = x;
        p->loc2.y = y;

		clone = (IplImage*)cvClone( p->frame );

		p_temp.loc2.x = p->loc2.x;

		if ( (0 == read_avi ) && (0 == read_cam) ) //read_cam
			p_temp.loc2.y = clone->height - y; 
		else
			p_temp.loc2.y = y; 

# ifndef WIN32 /*** not sure whether it applies to linux also ****/
        cvRectangle( clone, p->loc1, p->loc2, color, 1, 8, 0 );
# else
        cvRectangle( clone, p_temp.loc1, p_temp.loc2, color, 1, 8, 0 );
# endif


        cvShowImage( p->win_name, clone );
        if ( NULL != clone) cvReleaseImage( &clone );
        pressed = false;
    }

    /* on mouse move with left button down, draw rectangle */
    else if( event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON )
    {
        clone = (IplImage*)cvClone( p->frame );
# ifndef WIN32 /*** not sure whether it applies to linux also ****/
        cvRectangle( clone, p->loc1, cvPoint(x, y), color, 1, 8, 0 );
# else
		if ( (0 == read_avi ) && (0 == read_cam) ) //read_cam
			cvRectangle( clone, p_temp.loc1, cvPoint(x, clone->height - y), color, 1, 8, 0 );
		else
			cvRectangle( clone, p_temp.loc1, cvPoint(x, y), color, 1, 8, 0 );
# endif
        cvShowImage( p->win_name, clone );
        if ( NULL != clone ) cvReleaseImage( &clone );
    }
}

//# ifdef MULTIPLE_OBJECTS
CvScalar Boundary_Color(int obj_number)
{
	CvScalar color;

# if 0
	int i, j, k, l;
	l = obj_number%9;

	i = (l & (0x00)) ? 1 : 0;
	j = (l & (0x01)) ? 1 : 0;
	k = (l & (0x11)) ? 1 : 0;
	
	color = CV_RGB(	i*255, j*255, k*255);

# else
	switch(obj_number)
	{
	case 0 : 
		color = CV_RGB(255, 0, 0);
		break;
	case 1 : 
		color = CV_RGB(0, 255, 0);
		break;
	case 2 : 
		color = CV_RGB(0, 0, 255);
		break;
	case 3 : 
		color = CV_RGB(255, 255, 0);
		break;
	case 4 : 
		color = CV_RGB(0, 255, 255);
		break;
	case 5 : 
		color = CV_RGB(255, 0, 255 );
		break;
	case 6 : 
		color = CV_RGB(255, 255, 255);
		break;
	default: 
		color = CV_RGB(0, 0, 0);
		break;
	}
# endif

	return color;
}
//# endif


