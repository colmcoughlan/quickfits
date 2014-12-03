/*
    This program is called quickfits_overwrite_uv_data. It is part of the quickfits library interface to CFITSIO and overwrites data in a UV FITS file.
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

int quickfits_overwrite_uv_data(const char* filename, int nvis, int nchan, int nif, double* tvis)
{
/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
		int nvis : number of visibilities to be read
	OUTPUTS:
		u_array : nvis u coords, converted to physical units by fitsio
		v_array : nvis v coords, converted to physical units by fitsio
		tvis : (nvis*12*nif*nchan) visibilities, in Jy
*/
	fitsfile *fptr;

	int status, i, j;
	int err;
	char extname[]="AIPS UV ";
	char comment[]="UVFILL";
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
	// write out data - no need to overwrite u,v etc. as they are not changing
/*
	fits_write_col(fptr, TDOUBLE, 1, 1, 1, nvis,  u_array, &status);
	err+=status;
	if(err!=0)
	{
		printf("ERROR : cfits_overwrite_uvdata --> Error writing uarray, custom error = %d\n",err);
	}

	fits_write_col(fptr, TDOUBLE, 2, 1, 1, nvis,  v_array, &status);
	err+=status;
*/

	i=1;
	status=0;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"TTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);
		
		if( !strncmp(key_type,"VISIBILITIES",12) )
		{
			fits_write_col(fptr, TDOUBLE, i, 1, 1, nvis*12.0*nif*nchan,  tvis, &status);
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
