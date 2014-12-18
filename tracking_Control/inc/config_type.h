/***************************************************************************************/ 
/*! \b Project component: 
 *  \file   config_type.h
 *  \brief  Purpose:  Parameters type definitions
 *
 *  \author <a href="mailto: subarna.tripathi@gmail.com"> Subarna Tripathi (subarna.tripathi@gmail.com) </a>
 *  \date   2009
 ***************************************************************************************
 *
 *  \author
 *    Subarna Tripathi (2009)
 *
 *  \note
 *     tags are used for document system "doxygen"
 *     available at http://www.doxygen.org
 *
 ***************************************************************************************
 *
 *
 * \b History:
 *  - 30 November  2009 GF: Creation
 *
 ***************************************************************************************
 ***************************************************************************************/
#ifndef HCONFIGTYPEH
#define HCONFIGTYPEH

typedef enum 
{
  TYPE_INT,
  TYPE_INT_ARRAY,
  TYPE_DOUBLE,
  TYPE_DOUBLE_ARRAY,
  TYPE_BOOL,
  TYPE_STRING
} config_paramtype;

typedef struct 
{
  char *name;
  void *ptr;
  config_paramtype type;
  int number_par;
  char *comment;
} one_config_param;

typedef struct 
{
  one_config_param *pointer;
  char *name;
} name_pointer_config_section;

extern char *ParamSkip(char *buf);
extern char *ParamScanToken(char *buf, char *tok);
extern void ParseOneSection(one_config_param *params , char *name,FILE *fid_input);
extern void DumpDefaultConfigFile(char *out_file,name_pointer_config_section *matrix_configuration_section);
extern void ParseConfigFile(char *in_file,name_pointer_config_section *matrix_configuration_section);

#endif /* HCONFIGTYPEH */
