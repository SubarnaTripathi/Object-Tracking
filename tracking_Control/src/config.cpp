/***************************************************************************************/
/*! \b Project component: OBJECT_TRACKER
 *  \file   config.c
 *  \brief  Purpose:  Input parameters management 
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *  \author
 *    Subarna Tripathi
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 * \b Organization: ST-IITD
 *
 * \b History:
 *  - 26 November  2009 STF: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
/* {{{ Include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
# include <strings.h>
#else
# include <shlwapi.h>
#endif
#include <ctype.h>

#include "TRACKER_types.h"
#include "gtypes.h"
#include "config_type.h"
//#include "TOOL_message.h"

/* }}} */

/* {{{ Global protypes */
char *ParamSkip(char *buf);
char *ParamScanToken(char *buf, char *tok);
void ParseOneSection(one_config_param *params , char *name,FILE *fid_input);
void DumpDefaultConfigFile(char *out_file,name_pointer_config_section *matrix_configuration_section);
void ParseConfigFile(char *in_file,name_pointer_config_section *matrix_configuration_section);
void Footer_Printing(FILE *f_output);
void Header_Printing(FILE *f_output);

#ifndef WIN32
extern int strcasecmp(const char *s1, const char *s2);
extern void TRACKER_info(FILE *fd);
#endif
/* }}} */

/* {{{ ParamSkip */
/*******************************************************************************
*
*   Name:		ParamSkip(char *buf)
*
*   Author:		Subarna Tripathi.
*   Purpose:		It is used to skip the spaces and '#' char when we 
*                       read one element from the configuration file.
*			
*   In: 		buf: pointer to the next char that must be read
*
*   Out:		pointer the new token that must be read
*						
*********************************************************************************/
char *ParamSkip(char *buf)
{
  /* skip spaces and comments */
  while(*buf && isspace((int)*buf))
    buf++;
  if (*buf == '#') 
    *buf = '\0';
  return buf;
}
/* }}} */

/* {{{ ParamScanToken */
/*******************************************************************************
*
*   Name:		ParamScanToken(char *buf, char *tok)
*
*   Author:		Subarna Tripathi
*   Purpose:		It is used to copy one token from the source buffer to the token buffer
*			
*   In: 		buf: pointer to the next char that must be read
*                       tok: pointer to the buffer  where store the next token
*
*   Out:		pointer the new char that must be read.
*						
*********************************************************************************/
char *ParamScanToken(char *buf, char *tok)
{
    /* skip spaces and comments */
    buf = ParamSkip(buf);

    if (*buf == '\0') 
      return NULL;
    
    /* copy the token */
    while(*buf && !isspace((int)*buf))
      *tok++ = *buf++;
    *tok = 0;
    
    return buf;
}
/* }}} */

/* {{{ ParseOneSection */

/*******************************************************************************
*
*   Name:		ParseOneSection(one_config_param *params , char *name,FILE *fid_input)
*
*   Author:		Subarna Tripathi
*   Purpose:		It is used to parse one section of the configuration file
*			
*   In: 		params    : pointer to the structure of one section
*                       name      : section name
*                       *fid_input: configuration file ID
*
*   Out:		None.
*						
*********************************************************************************/
void ParseOneSection(one_config_param *params , char *name,FILE *fid_input)
{
  /* {{{ local variables */
  char sectname[256],paramname[256],value[256];
  char buf[1024];
  int number_line;
  one_config_param *local_ptr_to_section;
  char *pointer_for_string;
  /* }}} */

  /* start from the beginning */
  fseek(fid_input,0,SEEK_SET);
  number_line=0;
  sprintf(sectname,"[%s]",name);
  for(;;)
    {
      char *s;
      void *vptr;
      
      /* read of one line */
      s = fgets(buf,sizeof(buf),fid_input);
      number_line++;
      if(s == NULL) 
	break;
      
      /* skip the zeros */
      s = ParamSkip(s);
     
      /* compare the section name  */
      if (!strncmp(s,sectname,strlen(sectname)))
	{
	  /* we found the section */
	  for(;;) 
	    { 
	      /* read one line */
	      s = fgets(buf,sizeof(buf),fid_input);
	      number_line++;
	      
	      /* end section */
	      if(s == NULL) 
		goto end_section;

	      /* if we don't found one string */
	      if (!(s = ParamScanToken(s,paramname)))  
		continue;

		  pointer_for_string = ParamSkip(s);
	      

	      /* if we found a new section */
	      if (paramname[0] == '[') 
		goto end_section;

	      /* we read a new value */
	      if (!(s = ParamScanToken(s,value)))  
		{
		  PRINTERROR2(1,"Syntax Error on line %d in configuration file\n",number_line);
		  continue;
		}
	      
	      
	      /* we have one parameter name and one parameter value */

	      local_ptr_to_section = params;
	      
	      /* we search the parameter name in the structure */
#ifdef WIN32
		  while (local_ptr_to_section->name && StrCmpI(local_ptr_to_section->name, paramname)) 
#else
		  while (local_ptr_to_section->name && strcasecmp(local_ptr_to_section->name, paramname)) 
#endif
			  local_ptr_to_section++;
	      
	      /* we have found the pointer to store the value */
	      vptr = local_ptr_to_section->ptr;
	      

	      /* the parameter name is wrong */
	      if (local_ptr_to_section->name == NULL) 
		{
		  PRINTERROR4(1,"Unknown parameter name \"%s\" on line %d in configuration file for section %s\n",
			      paramname,number_line,sectname);	
		  continue;
		}
	      
	      /* {{{ decode one value */
	      switch (local_ptr_to_section->type) 
		{
		case TYPE_INT:
		  /* {{{ single integer value */
		  *(int *)vptr = strtoul(value,NULL,0);
		  /* }}} */
		  break;
		  
		case TYPE_INT_ARRAY:
		  {
		    /* {{{ multiple integer values */
		    int loop_count;
		    int *pointer_int;

		    pointer_int= (int *)vptr;
		    for(loop_count=0;loop_count<(local_ptr_to_section->number_par-1);loop_count++)
		    {
		      *pointer_int = strtoul(value,NULL,0);
		      
		      /* we read the new value */
		      if (!(s = ParamScanToken(s,value)))  
			  {
			  PRINTERROR2(1,"Syntax Error on line %d Reading INT_ARRAY in configuration file\n",number_line);
			  continue;
			  }
		      
		      pointer_int++;
		    }
		    *pointer_int = strtoul(value,NULL,0);

		    /* }}} */
		  }
		  break;
		  
		case TYPE_DOUBLE:
		  {
		    /* {{{ single double value */
		    sscanf(value,"%lf",(double *)vptr);
		    /* }}} */
		  }
		  break;

		case TYPE_DOUBLE_ARRAY:
		  {
		    /* {{{ multiple double values */
		    int loop_count;
		    double *pointer_double;

		    pointer_double= (double *)vptr;
		    for(loop_count=0;loop_count<(local_ptr_to_section->number_par-1);loop_count++)
		    {
		      sscanf(value,"%lf",pointer_double);
		      
		      /* we read the new value */
		      if (!(s = ParamScanToken(s,value)))  
			{
			  PRINTERROR2(1,"Syntax Error on line %d Reading INT_ARRAY in configuration file\n",number_line);
			  continue;
			}
		      
		      pointer_double++;
		    }
		    sscanf(value,"%lf",pointer_double);

		    /* }}} */
		  }
		  break;
		  
		case TYPE_BOOL:
		  {
		    /* {{{ boolean value */
#ifndef WIN32
			  if (strcasecmp(value,"FALSE") == 0)
		      *((int *)vptr) = 0;
		    else if (strcasecmp(value,"TRUE") == 0)
		      *((int *)vptr) = 1;
#else
			  if (StrCmpI(value,"FALSE") == 0)
		      *((int *)vptr) = 0;
		    else if (StrCmpI(value,"TRUE") == 0)
		      *((int *)vptr) = 1;
#endif
			else
		      {
			PRINTERROR2(1,"Syntax Error on line %d Reading BOOL in configuration file\n",number_line);
			continue;
		      }

		    /* }}} */
		  }
		  break;
		  
		case  TYPE_STRING:
		  {
		    /* {{{ string value */
		    char *output_string = (char *)vptr;
		    
		    if (output_string)
		    {
				if(pointer_for_string[0]!='"')
				{
				PRINTERROR4(1,"Internal error: the string doesn't start with \"  for parameter '%s' at line %d in configuration file for section [%s]\n",paramname,number_line,sectname);
				}
				
				pointer_for_string++;
				
				while(pointer_for_string[0]!='"')
				{
					if(pointer_for_string[0]=='\0')
					{
						PRINTERROR4(1,"Internal error: the string doesn't end with \"  for parameter '%s' at line %d in configuration file for section [%s]\n",paramname,number_line,sectname);
						break;
					}
				
					*output_string = *pointer_for_string;
					output_string++;
					pointer_for_string++;			    
				}
#ifndef WIN32
				*output_string = '\0';
#endif
		      }
		    else 
		      PRINTERROR4(1,"Internal error: invalid NULL pointer for a TYPE_STRING value for parameter '%s' at line %d in configuration file for section [%s]\n",paramname,number_line,sectname);
		    /* }}} */
		  }
		  break;
		  
		default :	  
		  PRINTERROR4(1,"Internal error: Unknown parameter type for name \"%s\" on line %d in configuration file for section [%s]\n",
			      paramname,number_line,sectname);	
		 }
	      /* }}} */
	      
	    }
	}
    }
  
 end_section:	;
  
}

/* }}} */

/* {{{ DumpDefaultConfigFile */
/*******************************************************************************
*
*   Name:		DumpDefaultConfigFile(char *out_file,
*                                            name_pointer_config_section *matrix_configuration_section)
*
*   Author:		Subarna Tripathi
*   Purpose:		This procedure dumps the configuration file with the standard values
*			
*   In: 		out_file                    : path of the file where to dump the standard
*                                                     configuration file
*                       matrix_configuration_section: matrix which contains the pointer to all the 
*                                                     section structures and all the section names
*                       
*
*   Out:		None.
*						
*********************************************************************************/
void DumpDefaultConfigFile(char *out_file,name_pointer_config_section *matrix_configuration_section)
{
  /* {{{ local variables */
  FILE *fid_output;

# ifndef WIN32
  int total_number_char;
# endif
  /* }}} */
   
  /* {{{ open output config file */
  fid_output = fopen(out_file,"w");
  
  if(!fid_output)
    {
      PRINTERROR3(1,"\nfile %s line %d\n",__FILE__,__LINE__);
      PRINTERROR2(1,"Couldn't open output configuration file %s\n",out_file);
      exit(-1);
    }
  /* }}} */
# ifndef WIN32
  /* {{{ dump config files */
  {
    int i,j;
    one_config_param *pointer;
    i=0;

    /* A very very very DUMB Header Printing */
    TRACKER_info(fid_output);

    while(matrix_configuration_section[i].pointer!=NULL)
      {
	j=0;
	pointer = matrix_configuration_section[i].pointer;
	
	/* section name */
	fprintf(fid_output,"\n\n[%s]\n",matrix_configuration_section[i].name);

	while(pointer[j].name!=NULL)
	  {
	    fprintf(fid_output,"%-30s ",pointer[j].name);
	    
	    total_number_char =0;
	    switch (pointer[j].type) 
	      {
		case TYPE_INT:
		  /* {{{ single integer value */		  
		  total_number_char+=fprintf(fid_output,"%d ", *(int *)pointer[j].ptr);
		  /* }}} */
		  break;
		  
		case TYPE_INT_ARRAY:
		  {
		    /* {{{ multiple integer values */
		    int loop_count;
		    int *pointer_int;

		    pointer_int= (int *)pointer[j].ptr;

		    for(loop_count=0;loop_count<(pointer[j].number_par);loop_count++)
		    {
		      total_number_char+=fprintf(fid_output,"%d ",*pointer_int);
		      
		    
		      pointer_int++;
		    }
		    /* }}} */
		  }
		  break;
		  
		case TYPE_DOUBLE:
		  {
		    /* {{{ single double value */
		    total_number_char+=fprintf(fid_output,"%f ",*(double *)pointer[j].ptr);
		    /* }}} */
		  }
		  break;
 
		case TYPE_DOUBLE_ARRAY:
		  {
		    /* {{{ multiple double values */
		    int loop_count;
		    double *pointer_double;

		    pointer_double= (double *)pointer[j].ptr;
		    for(loop_count=0;loop_count<(pointer[j].number_par);loop_count++)
		    {
		      total_number_char+=fprintf(fid_output,"%f ",*pointer_double);
		      
		      pointer_double++;
		    }
		    /* }}} */
		  }
		  break;
		  
		case TYPE_BOOL:
		  {
		    /* {{{ boolean value */
		    if((*(int *)pointer[j].ptr)==1)
		      total_number_char+=fprintf(fid_output,"%s ","TRUE");
		    else
		      total_number_char+=fprintf(fid_output,"%s ","FALSE");
		    /* }}} */
		  }
		  break;
		  
		case  TYPE_STRING:
		  {
		    /* {{{ string value */
		    total_number_char+=fprintf(fid_output,"\"%s\"", (char *)pointer[j].ptr);
		    /* }}} */
		  }
		  break;
		}

	    /* {{{ add spaces for allining */
	    {
	     
	      int i;
	      for(i=0;i<(34-total_number_char);i++)
		fprintf(fid_output,"%c",' ');
	    }
	    /* }}} */

	    fprintf(fid_output,"%s\n",pointer[j].comment);

	    j++;
	  }
	i++;
      }
  }
# endif
  /* }}} */

  /* {{{ Footer Printing */
  Footer_Printing(fid_output);
  /* }}} */

  /* {{{ close output config file */
  fclose(fid_output);
  /* }}} */
}
/* }}} */

/* {{{ ParseConfigFile */
/*******************************************************************************
*
*   Name:		ParseConfigFile(char *in_file,name_pointer_config_section *matrix_configuration_section)
*
*   Author:		Subarna Tripathi
*   Purpose:		It is used to parse the configuration file
*			
*   In: 		out_file                    : path of the file where to load
*                                                     configuration file
*                       matrix_configuration_section: matrix which contains the pointer to all the 
*                                                     section structures and all the section names
*
*   Out:		None.
*						
*********************************************************************************/
void ParseConfigFile(char *in_file,name_pointer_config_section *matrix_configuration_section)
{
  /* {{{ local variables */
  FILE *fid_input;
  /* }}} */

  /* {{{ open input config file */
  fid_input = fopen(in_file,"r");

  if(!fid_input)
    {
      PRINTERROR3(1,"\nfile %s line %d\n",__FILE__,__LINE__);
      PRINTERROR2(1,"Couldn't open input configuration file %s\n",in_file);
      exit(-1);
    }
  /* }}} */


  /* {{{ parse config files */
  {
    int i=0;
    while(matrix_configuration_section[i].pointer!=NULL)
      {
       ParseOneSection(matrix_configuration_section[i].pointer,matrix_configuration_section[i].name,fid_input);
       i++;
      }
  }
  /* }}} */
 
  /* {{{ close input config file */
  fclose(fid_input);
  /* }}} */

}
/* }}} */

/* {{{ A very very very DUMB Footer Printer */
void Footer_Printing(FILE *f_output)
{
  
    /* A very very very DUMB Header Printing */
    fprintf(f_output,"\n\n\n\n\n\n");
    fprintf(f_output,"__________________________________________________________________________________________________________\n");
    fprintf(f_output,"   COPYRIGHT:\n");
    fprintf(f_output,"      This program is property of ST,\n");
    fprintf(f_output,"      from STMicroelectronics S.r.l. It should not be communicated \n");
    fprintf(f_output,"      outside STMicroelectronics without authorization.\n");
    fprintf(f_output,"__________________________________________________________________________________________________________\n");
    fprintf(f_output,"[EOF]\n");
    
}
/* }}} */
