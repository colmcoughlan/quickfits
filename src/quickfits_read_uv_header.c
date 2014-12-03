/*
    This program is called quickfits_read_uv_header. It is part of the quickfits library interface to CFITSIO and reads the header of a UV FITS file.
    Copyright (C) 2012  Colm Coughlan
    colmcoughlanirl <!at!> gmail.com https://github.com/colmcoughlan/quickfits

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quickfits.h"

int quickfits_read_uv_header(const char* filename, double* ra, double* dec, char* object, double* freq, int* nvis, int* nchan, int* central_chan, double* chan_width, int* nif)
{
/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
	OUTPUTS:
		ra = right ascention
		dec = declination
		key_string = name of source
		freq = frequency
		nvis = number of visibilities
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

	fits_read_key(fptr,TSTRING,"OBJECT",object,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"OBSRA",ra,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"OBSDEC",dec,comment,&status);
	err+=status;
	
	fits_read_key(fptr,TDOUBLE,"NAXIS2",&temp,comment,&status);
	err+=status;
	nvis[0]=(int)(temp);	// nvis is stored as a double in the FITS file
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
			fits_read_key(fptr,TDOUBLE,key_name,freq,comment,&status);
			sprintf(key_name,"%dCDLT%d",i,j);
			fits_read_key(fptr,TDOUBLE,key_name,chan_width,comment,&status);
			sprintf(key_name,"%dCRPX%d",i,j);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			central_chan[0] = temp; // central channel stored as double in data
			nchan[0] = vis_axes[i-1];
		}
		
		if( !strncmp(key_type,"IF",2) )
		{
			nif[0] = vis_axes[i-1];
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
