/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_read_map_header(const char* filename , int* dim , double* cell , double* ra , double* dec , double* centre_shift , double* rotations , double* freq , double* freq_delta , int* stokes , char* object , char* observer , char* telescope , double* equinox , char* date_obs ,  double* bmaj , double* bmin , double* bpa , int* ncc, int cc_table_version)
{

/*
    Read in map header information
 
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
	OUTPUTS:
		ra = right ascention
		dec = declination
		object = name of source
		freq = frequency
		cell =  cellsize (degrees)
		dim = image size
		bmaj, bmin, bpa = beam information (degrees)
*/
	fitsfile *fptr;

	int status,i,j;
	int err;
	char comment[FLEN_VALUE];
	char key_name[FLEN_VALUE];
	char key_type[FLEN_VALUE];
	char beamhdu[]="AIPS CG ";
	char cchdu[]="AIPS CC ";
	char bmajname[]="BMAJ";
	char bminname[]="BMIN";
	char bpaname[]="BPA";
	int colnum;
	double temp;
	float floatbuff;
	float float_null=0;
	int int_null=0;
	long longbuff;


	status = 0;	// for error processing
	err=0;



	if ( fits_open_file(&fptr,filename, READONLY, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_read_map_header --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movabs_hdu(fptr,1,&i,&status))		// move to main AIPS image HDU (assuming it's the first one)
	{
		printf("ERROR : quickfits_read_map_header --> Error locating AIPS ACSII table extension, error = %d\n",status);
		printf("ERROR : quickfits_read_map_header --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}

	// read in some optional keys (these may fail on strange files - but are not that important)

	fits_read_key(fptr,TSTRING,"OBJECT",object,comment,&status);
	fits_read_key(fptr,TSTRING,"OBSERVER",observer,comment,&status);
	fits_read_key(fptr,TSTRING,"TELESCOP",telescope,comment,&status);
	fits_read_key(fptr,TDOUBLE,"EQUINOX",equinox,comment,&status);
	fits_read_key(fptr,TSTRING,"DATE-OBS",date_obs,comment,&status);
	
	//	Now iterate through CTYPE coords to get important information about RA, DEC, cellsize etc.
	//  Most of this data is important - at least notify the user if some is missing
	
	i=1;
	status=0;
	j=0;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"CTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);

		if( !strncmp(key_type,"RA---SIN",8) )
		{
			j++;
			
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,ra,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing RA information %s\n",key_name);
				status= 0;
			}
			
			sprintf(key_name,"CDELT%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing RA information %s\n",key_name);
				status= 0;
			}
			else
			{
				cell[0]=fabs(temp);
			}
			
			sprintf(key_name,"CRPIX%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing RA information %s\n",key_name);
				status= 0;
			}
			else
			{
				centre_shift[0]=temp;
			}
			
			sprintf(key_name,"CROTA%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing RA information %s\n",key_name);
				status= 0;
			}
			else
			{
				rotations[0]=temp;
			}
		}

		if( !strncmp(key_type,"DEC--SIN",8) )
		{
			j++;
			
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,dec,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing DEC information %s\n",key_name);
				status = 0;
			}

			sprintf(key_name,"CRPIX%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing DEC information %s\n",key_name);
				status = 0;
			}
			else
			{
				centre_shift[1]=temp;
			}
			
			sprintf(key_name,"CROTA%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);			
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing DEC information %s\n",key_name);
				status = 0;
			}
			else
			{
				rotations[1]=temp;
			}
		}
		
		if( !strncmp(key_type,"FREQ",4) )
		{
			j++;
			
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,freq,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing FREQ information %s\n",key_name);
				status = 0;
			}

			sprintf(key_name,"CDELT%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,freq_delta,comment,&status);
			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing FREQ information %s\n",key_name);
				status = 0;
			}
		}

		if( !strncmp(key_type,"STOKES",6) )
		{
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);

			if(status==KEY_NO_EXIST)
			{
				printf("WARNING : quickfits_read_map_header --> Missing Stokes information %s\n",key_name);
				status = 0;
			}
			else
			{
				stokes[0]=(int)(temp);
			}
		}
		i++;
	}
	status=0;
	if(j!=3)
	{
		printf("WARNING : quickfits_read_map_header --> Error reading RA, DEC, FREQ information\n",err);
		printf("\t Only %d out of 3 read.\n",j);
	}


	fits_read_key(fptr,TDOUBLE,"NAXIS1",&temp,comment,&status);
	dim[0]=(int)(temp);	// dim is stored as a double in the FITS file

	if(status!=0)
	{
		printf("ERROR : quickfits_read_map_header --> Error reading image size from NAXIS1, error = %d\n",err);
		return(err);
	}

	fits_read_key(fptr,TDOUBLE,"BMAJ",bmaj,comment,&status);
	err += status;
	fits_read_key(fptr,TDOUBLE,"BMIN",bmin,comment,&status);
	err += status;
	fits_read_key(fptr,TDOUBLE,"BPA",bpa,comment,&status);
	err += status;
	
	if(err!=0)	// if the beam information isn't in the main header, move to AIPS CG HDU for beam information
	{
		status=0;
		if (fits_movnam_hdu(fptr,BINARY_TBL,beamhdu,0,&status))		// move to beam information hdu
		{
			printf("WARNING : quickfits_read_map_header --> No beam information found.\n");
			bmaj[0] = 0.0;
			bmin[0] = 0.0;	// changed this because model files don't have any beam information. Should check to make sure beam info is valid in other code
			bpa[0] = 0.0;
		}
		else
		{
			fits_get_colnum(fptr,CASEINSEN,bmajname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error locating BMAJ information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error reading BMAJ information, error = %d\n",status);
				return(err);
			}
			else
			{
				bmaj[0]=(double)(floatbuff);
			}

			fits_get_colnum(fptr,CASEINSEN,bminname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error locating BMIN information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error reading BMIN information, error = %d\n",status);
				return(err);
			}
			else
			{
				bmin[0]=(double)(floatbuff);
			}

			fits_get_colnum(fptr,CASEINSEN,bpaname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error locating BPA information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error reading BPA information, error = %d\n",status);
				return(err);
			}
			else
			{
				bpa[0]=(double)(floatbuff);
			}
		}
	}






	// move to AIPS CC HDU for clean component information if requested. Read, or give error if a failure occurs. Allow for possibility of the outdated "A3DTABLE" table type 6 as well as normal BINARY_TBL

	if (cc_table_version >=0 )
	{
		fits_movnam_hdu(fptr,ANY_HDU,cchdu,cc_table_version,&status);
		if (status==0)		// move to main AIPS UV hdu
		{
			fits_get_num_rows(fptr,&longbuff,&status);
			ncc[0]=longbuff;
			if(status!=0)
			{
				printf("ERROR : quickfits_read_map_header -->  Error reading number of clean components, error = %d\n",status);
				return(status);
			}
		}
		else
		{
			printf("WARNING : quickfits_read_map_header -->  No clean component table detected.\n");
			ncc[0]=0;
			return(status);
		}
	}
	else
	{
		ncc[0]=0;
	}


	

	status=0;
	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_read_map_header --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
