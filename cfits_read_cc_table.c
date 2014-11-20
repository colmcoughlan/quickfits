/*
    This program is called cfits_write. It interacts with the CFITSIO library to read in a FITS file to an array of doubles
    Copyright (C) 2012  Colm Coughlan

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

	INPUTS:
		char* filename : c string = name of FITS file to be read from
		array : map of doubles to be read from FITS file
		various metadata
	OUTPUTS:
		Reads a FITS file from disk

	NB : see FITS file format documentation to see what each parameter means (though AIPS may interpret it differently...)
*/

#include "error_map_heads.hpp"

int cfits_read_cc_table(const char* filename , double* cc_xarray, double* cc_yarray, double* cc_varray , int ncc, int cc_table_version)
{
/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
		int ncc : number of clean components to be read (0 if no cc table expected/needed)
	OUTPUTS:
		cc_xarray : 1D fp array containing x coords of clean components in degrees
		cc_yarray : 1D fp array containing y coords of clean components in degrees
		cc_varray : 1D fp array containing values of clean components in degrees
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
		printf("ERROR : cfits_read_map --> Error opening FITS file, error = %d\n",status);
		return(status);
	}


	if (fits_movnam_hdu(fptr,BINARY_TBL,cchdu,cc_table_version,&status))		// move to main AIPS image hdu
	{
		printf("ERROR : cfits_read_map --> Error locating AIPS clean component extension, error = %d\n",status);
		return(status);
	}
	else
	{
		fits_get_colnum(fptr,CASEINSEN,xname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error locating CC x position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,ncc,&double_null,cc_xarray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error reading CC x position information, error = %d\n",status);
		}

		fits_get_colnum(fptr,CASEINSEN,yname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error locating CC y position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,ncc,&double_null,cc_yarray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error reading CC y position information, error = %d\n",status);
		}

		fits_get_colnum(fptr,CASEINSEN,fluxname,&colnum,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error locating CC flux position information, error = %d\n",status);
		}
		fits_read_col(fptr,TDOUBLE,colnum,1,1,ncc,&double_null,cc_varray,&int_null,&status);
		if(status!=0)
		{
			printf("ERROR : cfits_read_map -->  Error reading CC flux position information, error = %d\n",status);
		}
	}

	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : cfits_read_map --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
