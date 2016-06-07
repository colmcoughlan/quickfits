/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"

int quickfits_read_uv_header(const char* filename, fitsinfo_uv* fitsi)
{
/*
    Read useful keywords from the header of a UV FITS file produced by FITAB in AIPS.
 
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
	OUTPUTS:
		ra = right ascention
		dec = declination
		object = name of source
		freq = frequency
		nvis = number of visibilities
        nchan = number of channels
        central_chan = central channel
        chan_width = channel width
        nif = number of IFs
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
	int num_vis_axes;
	long vis_axes[8];

	status = 0;	// for error processing
	err=0;

	if ( fits_open_file(&fptr,filename, READONLY, &status) )	// open file and make sure it's open
	{
		printf("ERROR : quickfits_read_uv_header --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : quickfits_read_uv_header --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : quickfits_read_uv_header --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}

	// read in some keys

	fits_read_key(fptr,TSTRING,"OBJECT",fitsi[0].object,comment,&status);
	err+=status;
	fits_read_key(fptr,TSTRING,"OBSERVER",fitsi[0].observer,comment,&status);
	err+=status;
	fits_read_key(fptr,TSTRING,"TELESCOP",fitsi[0].telescope,comment,&status);
	err+=status;
	fits_read_key(fptr,TSTRING,"DATE-OBS",fitsi[0].date_obs,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"EQUINOX",&fitsi[0].equinox,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"OBSRA",&fitsi[0].ra,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"OBSDEC",&fitsi[0].dec,comment,&status);
	err+=status;
	
	fits_read_key(fptr,TDOUBLE,"NAXIS2",&temp,comment,&status);
	err+=status;
	fitsi[0].nvis=(int)(temp);	// nvis is stored as a double in the FITS file
	if(err!=0)
	{
		printf("ERROR : quickfits_read_uv_header --> Error reading keywords, custom error = %d\n",err);
	}
	
	i=1;
	status=0;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"TTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);
		
		if( !strncmp(key_type,"VISIBILITIES",12) )
		{
			sprintf(key_name,"TDIM%d",i);
			fits_read_tdim(fptr, i, 8, &num_vis_axes, vis_axes, &status);	// note maxdim = size of vis_axes
		}

		i++;
	}

	status=0;
	j=i-2;	// set j to point to the visibility data
	i=1;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"%dCTYP%d",i,j);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);

		if( !strncmp(key_type,"FREQ",4) )
		{
			sprintf(key_name,"%dCRVL%d",i,j);
			fits_read_key(fptr,TDOUBLE,key_name,&fitsi[0].freq,comment,&status);
			sprintf(key_name,"%dCDLT%d",i,j);
			fits_read_key(fptr,TDOUBLE,key_name,&fitsi[0].chan_width,comment,&status);
			sprintf(key_name,"%dCRPX%d",i,j);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			fitsi[0].central_chan = temp; // central channel stored as double in data
			fitsi[0].nchan = vis_axes[i-1];
		}
		
		if( !strncmp(key_type,"IF",2) )
		{
			fitsi[0].nif = vis_axes[i-1];
		}

		i++;
	}
	status=0;


	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : quickfits_read_uv_header --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
