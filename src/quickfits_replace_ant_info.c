/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_replace_ant_info(const char* filename, double* rdterm, double* ldterm)
{
/*
    Replace antenna d term info in a UV FITS file produced by FITAB in AIPS.
 
	INPUTS:
		const char* tfilename : c string = name of FITS file to be read
        double* rdterm : Right D terms of antennas
        double* ldterm : Left D term of antennas
*/
	fitsfile *fptr;

	int status, i, j;
	char extname[]="AIPS AN ";
//	char comment[FLEN_VALUE];
	char comment[]="UVFILL";
	char poltype[]="VLBI    ";
	char rdtermname[]="POLCALA";	// assumes column a is the r term and column b is the l term
	char ldtermname[]="POLCALB";
	double d_null=0;
	int colnum;
	double temp;

	status = 0;	// for error processing

	if ( fits_open_file(&fptr,filename, READWRITE, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_replace_ant_info --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : quickfits_replace_ant_info --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : quickfits_replace_ant_info --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}
	// write out d-term data to correct columns

	fits_get_colnum(fptr,CASEINSEN,rdtermname,&colnum,&status);
	if(status!=0)
	{
		printf("ERROR : quickfits_replace_ant_info --> Error finding r dterm column to write to, error = %d\n",status);
	}


	fits_write_col(fptr, TDOUBLE, colnum, 1, 1, 10*2,  rdterm, &status);
	if(status!=0)
	{
		printf("ERROR : quickfits_replace_ant_info --> Error writing keywords, error = %d\n",status);
	}

	fits_get_colnum(fptr,CASEINSEN,ldtermname,&colnum,&status);
	if(status!=0)
	{
		printf("ERROR : quickfits_replace_ant_info --> Error finding l dterm column to write to, error = %d\n",status);
	}

	fits_write_col(fptr, TDOUBLE, colnum, 1, 1, 10*2,  ldterm, &status);
	if(status!=0)
	{
		printf("ERROR : quickfits_replace_ant_info --> Error writing new antenna information, error = %d\n",status);
	}

	fits_update_key(fptr, TSTRING, "POLTYPE", &poltype,comment, &status);
	if(status!=0)
	{
		printf("ERROR : quickfits_replace_ant_info --> Error writing polarisation type to header file, error = %d\n",status);
	}



	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_replace_ant_info --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
