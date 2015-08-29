/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_read_uv_data(const char* filename, fitsinfo_uv fitsi, double* u_array, double* v_array, double* tvis, double* if_array)
{
/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
		int nvis : number of visibilities to be read
	OUTPUTS:
		u_array : nvis u coords, converted to physical units by fitsio
		v_array : nvis v coords, converted to physical units by fitsio
		tvis : (nvis*12 nvis*4(Stokes)*3(Re,Im,weight)*nif*nchan) visibilities, in Jy
*/
	fitsfile *fptr;

	int status, i, j;
	int err;
	char extname[]="AIPS UV ";
	char freq_extname[]="AIPS FQ ";
	char comment[FLEN_VALUE];
	char key_name[FLEN_VALUE];
	char key_type[FLEN_VALUE];
	double d_null=0;
	int anynull;
	double temp;

	status = 0;	// for error processing
	err=0;

	if ( fits_open_file(&fptr,filename, READONLY, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_read_uv_data --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : quickfits_read_uv_data --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : quickfits_read_uv_data --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
	}
	
	
	// read in data - first find where U, V and visibility columns are - then read them in
	
	i=1;
	status=0;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"TTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);
		
		if( !strncmp(key_type,"UU",2) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, fitsi.nvis, &d_null,  u_array, &anynull, &status);
			err+=status;
		}
		if( !strncmp(key_type,"VV",2) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, fitsi.nvis, &d_null,  v_array, &anynull, &status);
			err+=status;
		}
		if( !strncmp(key_type,"VISIBILITIES",12) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, fitsi.nvis*12.0*fitsi.nif*fitsi.nchan, &d_null,  tvis, &anynull, &status);
			err+=status;
		}

		i++;
	}

	if(err!=0)
	{
		printf("ERROR : quickfits_read_uv_data --> Error reading keywords, custom error = %d\n",err);
	}
	

	status=0;
	if (fits_movnam_hdu(fptr,BINARY_TBL,freq_extname,0,&status))		// move to frequency information hdu
	{
		printf("ERROR : quickfits_read_uv_data --> Error finding frequency table, error = %d\n",status);
	}
	else
	{
		i=1;
		status=0;
		while(status!=KEY_NO_EXIST)
		{
			sprintf(key_name,"TTYPE%d",i);
			fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);	// read in IF frequency offsets
		
			if( !strncmp(key_type,"IF FREQ",7) )
			{
				fits_read_col(fptr, TDOUBLE, i, 1, 1, fitsi.nif, &d_null,  if_array, &anynull, &status);
				err+=status;
			}

			i++;
		}
	}
	status=0;
	
	
	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_read_uv_data --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
