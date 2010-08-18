/*
 Copyright (C) 2010 Quantum ESPRESSO group
 This file is distributed under the terms of the
 GNU General Public License. See the file `License'
 in the root directory of the present distribution,
 or http://www.gnu.org/copyleft/gpl.txt . 

------------------------------------------------------
*/


#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "c_defs.h"
#include "md5.h"

#define MAX_BUF 1024 

//char *strCopy( char * str ) 
//{ 
//  if( !str ) 
//  { 
//    return NULL; 
//  } 
   
//  char *n = ( char * )malloc( strlen( str ) + 1 ); 
//  memcpy( n, str, strlen( str ) + 1 ); 
   
//  return n; 
//} 

static void fatal ( const char * msg )
{
  
   fprintf( stderr , "fatal: %s" , *msg ? msg : "Oops!" ) ;
   exit( -1 ) ;

} /* fatal */

static void * xcmalloc ( size_t size )
{

  register void * ptr = malloc( size ) ;

  if ( ptr == NULL )
    fatal( "fatal: virtual memory exhausted" ) ;
  else
    memset( ptr , 0 , size ) ;

  return ptr ;

} /* xcmalloc */

char *readFile( FILE *file ) 
{ 

	char *out;
	unsigned long fileLen;

	if (!file)
	{
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	out=(char *)xcmalloc(fileLen+1);

	if (!out)
	{
	fprintf(stderr, "Memory error!");
	fclose(file);
	exit(1);
	}

	fread(out, fileLen, 1, file);

	return out; 

} 


void get_md5(const char *file, char *md5, int err)
{
 
     FILE *fp;
     char *data;
     //char THISMD5[31];
     md5_state_t state;
     md5_byte_t digest[16];
     
     //printf("     GET_MD5: check string existence -%s- ",file);
     if(file==NULL) { 
	err = 1;
        return;
     }
     //printf("...ok \n");

     //printf("     GET_MD5: check file opening -%s- ",file);
     fp=fopen(file,"rb");
     if(fp==NULL) {
	err = 2;
	return;
     }
     //printf("...ok \n");

     //printf("     GET_MD5: check file reading -%s- ",file);
     data=readFile(fp);
     if(data==NULL) {
	err = 3;
	return;
     }
     //printf("...ok \n");

     //printf("     GET_MD5: file %s \n",data);
     md5_init(&state);
     md5_append(&state,(const md5_byte_t *)data,strlen(data));
     md5_finish(&state,digest);

     int i=0;
     for(i;i<16;i++){
           //snprintf(THISMD5+i*2,sizeof(THISMD5),"%02x",digest[i]);
	   snprintf(md5+i*2,sizeof(md5),"%02x",digest[i]);
     }
     fclose(fp);

     //strcpy(md5, THISMD5);
     //md5 = &THISMD5;
     err = 0;
     return;
}

int F77_FUNC_(file_md5,FILE_MD5)( const int * f_name, const int * f_len, int * out )
{
     int i, err = -1 ;
     char * md5 = ( char * ) xcmalloc( 32 + 1 ) ;
     char * f = ( char * ) xcmalloc( (*f_len) + 1) ;

     for( i = 0; i < * f_len; i++ ) f[ i ] = (char)f_name[ i ];

     f[*f_len] = '\0' ;


     //printf("     C: md5 of file %s \n",f); 
     get_md5( f ,  md5, err) ;
     //printf("     C: md5 status %i \n",err);
     //printf("     C: md5 calculated %s \n",md5);     
     for( i = 0; i < 32; i++ ) out[ i ] = md5[ i ]; 
    
     return err;
} 
