/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_overwrite_uv_data(const char* filename, fitsinfo_uv fitsi, double* u, double* v, double* tvis)
{
/*
    Overwrite UV data in a UV FITS file produced by FITAB in AIPS.
 
	INPUTS:
		const char* tfilename : c string = name of FITS file to be read
		int nvis : number of visibilities to be read
        int nchan : number of channels to be read
        int nif : number of IFs to be read
	OUTPUTS:
		tvis : (nvis*12*nif*nchan) visibilities, in Jy. 12 = 4 Stokes * (Re,Im, Weight)
*/
	fitsfile *fptr;

	int status, i, j;
	int err;
	char extname[]="AIPS UV ";
	char comment[FLEN_VALUE];
	char key_name[FLEN_VALUE];
	char key_type[FLEN_VALUE];
	double d_null=0;
	int anynull;
	double temp;

	status = 0;	// for error processing
	err=0;

	if ( fits_open_file(&fptr,filename, READWRITE, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_overwrite_uv_data --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : quickfits_overwrite_uv_data --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : quickfits_overwrite_uv_data --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}


	fits_write_col(fptr, TDOUBLE, 1, 1, 1, fitsi.nvis,  u, &status);
	err+=status;
	if(err!=0)
	{
		printf("ERROR : cfits_overwrite_uvdata --> Error writing uarray, custom error = %d\n",err);
	}

	fits_write_col(fptr, TDOUBLE, 2, 1, 1, fitsi.nvis,  v, &status);
	err+=status;


	i=1;
	status=0;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"TTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);
		if( !strncmp(key_type,"VISIBILITIES",12) )
		{
			fits_write_col(fptr, TDOUBLE, i, 1, 1, fitsi.nvis*12.0*fitsi.nif*fitsi.nchan,  tvis, &status);
			err+=status;
			if(err!=0)
			{
				printf("ERROR : quickfits_overwrite_uv_data --> Error writing keywords, custom error = %d\n",err);
			}
		}

		i++;
	}
	status = 0;



	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_overwrite_uv_data --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
