/*
    This program is called quickfits_read_uv_data. It is part of the quickfits library interface to CFITSIO and reads in UV data from a FITS file.
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

int quickfits_read_uv_data(const char* filename, int nvis, int nchan, int nif, double* u_array, double* v_array, double* tvis, double* if_array)
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
		
		if( !strncmp(key_type,"UU---SIN",8) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, nvis, &d_null,  u_array, &anynull, &status);
			err+=status;
		}
		if( !strncmp(key_type,"VV---SIN",8) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, nvis, &d_null,  v_array, &anynull, &status);
			err+=status;
		}
		if( !strncmp(key_type,"VISIBILITIES",12) )
		{
			fits_read_col(fptr, TDOUBLE, i, 1, 1, nvis*12.0*nif*nchan, &d_null,  tvis, &anynull, &status);
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
				fits_read_col(fptr, TDOUBLE, i, 1, 1, nif, &d_null,  if_array, &anynull, &status);
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
