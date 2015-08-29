/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_read_cc_table(const char* filename , fitsinfo_map fitsi , double* cc_xarray, double* cc_yarray, double* cc_varray)
{
/*
	INPUTS:
		const char* tfilename : c string = name of FITS file to be read
		int ncc : number of clean components to be read (0 if no cc table expected/needed)
        int cc_table_version : Version of CC table to read
	OUTPUTS:
		cc_xarray : 1D fp array containing x coords of clean components in degrees
		cc_yarray : 1D fp array containing y coords of clean components in degrees
		cc_varray : 1D fp array containing values of clean components in degrees
 
    RETURN:
        0 on success.
*/
	fitsfile *fptr;

	int status;
	char comment[FLEN_VALUE];
	float nullval=666.0;
	int int_null=0;
	double double_null=0;
	long fpixel=1;
	char cchdu[]="AIPS CC ";
	char fluxname[]="FLUX";
	char xname[]="DELTAX";
	char yname[]="DELTAY";
	int colnum;
	int i;


	status = 0;	// for error processing


	if ( fits_open_file(&fptr,filename, READONLY, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_read_cc_table --> Error opening FITS file, error = %d\n",status);
		return(status);
	}


	if (fits_movnam_hdu(fptr,BINARY_TBL,cchdu,fitsi.cc_table_version,&status))		// move to main AIPS image hdu
	{
		printf("ERROR : quickfits_read_cc_table --> Error locating AIPS clean component extension, error = %d\n",status);
		return(status);
	}
	else
	{
		fits_get_colnum(fptr,CASEINSEN,xname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error locating CC x position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,fitsi.ncc,&double_null,cc_xarray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error reading CC x position information, error = %d\n",status);
		}

		fits_get_colnum(fptr,CASEINSEN,yname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error locating CC y position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,fitsi.ncc,&double_null,cc_yarray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error reading CC y position information, error = %d\n",status);
		}

		fits_get_colnum(fptr,CASEINSEN,fluxname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error locating CC flux position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,fitsi.ncc,&double_null,cc_varray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : quickfits_read_cc_table -->  Error reading CC flux position information, error = %d\n",status);
		}
	}

	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_read_cc_table --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
