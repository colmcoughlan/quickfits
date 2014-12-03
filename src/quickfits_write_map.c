/*
    This program is called quickfits_write_map. It is part of the quickfits library interface to CFITSIO and writes out FITS maps from memory.
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

	INPUTS:
		char* filename : c string = name of FITS file to be created
		array : map of doubles to be converted to a FITS file
		various metadata (source and observation information, cellsize, frequency, beam (if any) etc.)
	OUTPUTS:
		writes a FITS file to disk

	NB : see FITS file format documentation to see what each parameter means (though AIPS may interpret it differently...)
*/

#include "quickfits.h"


int quickfits_write_map(const char* filename , double* array , int imsize , double cell , double ra , double dec , double* centre_shift , double* rotations , double freq , double freq_delta , int stokes , char* object , char* observer , char* telescope , double equinox , char* date_obs , char* history , double bmaj , double bmin , double bpa , int niter , bool jy_per_beam)
{

	fitsfile *fptr;	// pointer to fits file
	int status, i, j;
	long fpixel = 1, naxis = 4, nelements;	// fpixel is the coordinate of the first pixel to be read
	long naxes[4] = { imsize, imsize , 1 , 1 };
	double temp;
	char comment[]="";
	char tstring[FLEN_VALUE];
	char fname[strlen(filename)+1];

	strcpy(fname,"!");
	strcat(fname,filename);



	status = 0;
	// status is an error variable

	fits_create_file(&fptr,fname, &status);

	// create new file

	// Create the primary array image (64-bit floating point pixels)
	fits_create_img(fptr, DOUBLE_IMG, naxis, naxes, &status);


	fits_update_key(fptr, TSTRING, "OBJECT", object , comment , &status);
	fits_update_key(fptr, TSTRING, "OBSERVER", observer , comment , &status);	// write out information about the source
	fits_update_key(fptr, TSTRING, "TELESCOP", telescope , comment , &status);
	fits_update_key(fptr, TDOUBLE, "EQUINOX", &equinox , comment, &status);
	fits_update_key(fptr, TSTRING, "DATE-OBS", date_obs , comment , &status);
	fits_update_key(fptr, TDOUBLE, "OBSRA", &ra , comment , &status);
	fits_update_key(fptr, TDOUBLE, "OBSDEC", &dec , comment , &status);

	temp=1.0;
	fits_update_key(fptr, TDOUBLE, "BSCALE", &temp,comment, &status);

	temp=0.0;
	fits_update_key(fptr, TDOUBLE, "BZERO", &temp,comment, &status);


	if(jy_per_beam)
	{
		sprintf(tstring,"JY/BEAM");
		fits_update_key(fptr, TSTRING, "BUNIT", tstring , comment , &status);
	}
	else
	{
		sprintf(tstring,"JY/PIX");
		fits_update_key(fptr, TSTRING, "BUNIT", tstring , comment , &status);
	}



	sprintf(tstring,"RA---SIN");
	fits_update_key(fptr, TSTRING, "CTYPE1", tstring , comment , &status);	// write out information about the first axis, Right Ascention

	fits_update_key(fptr, TDOUBLE, "CRVAL1", &ra , comment , &status);

	temp = -cell;
	fits_update_key(fptr, TDOUBLE, "CDELT1", &temp , comment , &status);

	temp = centre_shift[0];
	fits_update_key(fptr, TDOUBLE, "CRPIX1", &temp , comment , &status);

	temp = rotations[0];
	fits_update_key(fptr, TDOUBLE, "CROTA1", &temp , comment , &status);





	sprintf(tstring,"DEC--SIN");
	fits_update_key(fptr, TSTRING, "CTYPE2", tstring , comment , &status);	// write out information about the second axis, Declination

	fits_update_key(fptr, TDOUBLE, "CRVAL2", &dec , comment , &status);

	fits_update_key(fptr, TDOUBLE, "CDELT2", &cell , comment , &status);

	temp = centre_shift[1];
	fits_update_key(fptr, TDOUBLE, "CRPIX2", &temp , comment , &status);

	temp = rotations[1];
	fits_update_key(fptr, TDOUBLE, "CROTA2", &temp , comment , &status);





	sprintf(tstring,"FREQ");
	fits_update_key(fptr, TSTRING, "CTYPE3", tstring , comment , &status);	// write out information about the third axis, Frequency

	fits_update_key(fptr, TDOUBLE, "CRVAL3", &freq , comment , &status);

	fits_update_key(fptr, TDOUBLE, "CDELT3", &freq_delta , comment , &status);

	temp=1.0;
	fits_update_key(fptr, TDOUBLE, "CRPIX3", &temp , comment , &status);

	temp=0.0;
	fits_update_key(fptr, TDOUBLE, "CROTA3", &temp , comment , &status);



	sprintf(tstring,"STOKES");
	fits_update_key(fptr, TSTRING, "CTYPE4" , tstring , comment , &status);	// write out information about the fourth axis, Stokes parameter

	temp = (double)(stokes);
	fits_update_key(fptr, TDOUBLE, "CRVAL4", &temp , comment , &status);

	temp=1.0;
	fits_update_key(fptr, TDOUBLE, "CDELT4", &temp , comment , &status);

	fits_update_key(fptr, TDOUBLE, "CRPIX4", &temp , comment , &status);

	temp=0.0;
	fits_update_key(fptr, TDOUBLE, "CROTA4", &temp , comment , &status);


	fits_write_history(fptr , history , &status);	// write history and date

	fits_write_date(fptr , &status);



	nelements = naxes[0] * naxes[1];
	// number of pixels to write

	// Write the array of double size floating point to the image
	fits_write_img(fptr, TDOUBLE, fpixel, nelements, array, &status);

	if(jy_per_beam)	// write out beam information for AIPS if necessary. note AIPS CG --> AIPS CLEAN gaussian, which is not true in this case, but is used for compatability with AIPS
	{
		char bmaj_name[] = "BMAJ";
		char bmin_name[] = "BMIN";
		char bpa_name[] = "BPA";
		char freq_name[] = "FREQUENCY";
		char freq_units[] = "HZ";
		char beam_units[] = "DEGREES";
		char data_type[] = "1D";
		char* beaminfo_names[4] = {freq_name , bmaj_name, bmin_name, bpa_name};
		char* beaminfo_units[4] = {freq_units , beam_units , beam_units , beam_units};
		char* beaminfo_datatype[4] = {data_type , data_type , data_type , data_type};
		char tbl_name[] = "AIPS CG ";

		sprintf(tstring,"COMMENT : WRITING OUT BEAM IN AIPS FASHION (NOT REALLY FROM AIPS)");	// write beam as AIPS-style note
		fits_write_history(fptr , tstring , &status);

		// NB - do no use more than 8 places here. Make sure to use the same accuracy for BMAJ and BMIN
		sprintf(tstring,"AIPS   CLEAN BMAJ=  %.8lf BMIN=  %.8lf BPA=   %.8lf" , bmaj , bmin , bpa );
		fits_write_history(fptr , tstring , &status);

		if(niter>0)
		{
			sprintf(tstring,"COMMENT : NITER BELOW NOT NECESSARILY FROM CLEAN");	// write beam as AIPS-style note
			fits_write_history(fptr , tstring , &status);


			sprintf(tstring,"AIPS   CLEAN NITER=     %d PRODUCT=1",niter);
			fits_write_history(fptr , tstring , &status);
		}

		fits_create_tbl(fptr , BINARY_TBL , 1 , 4 , beaminfo_names , beaminfo_datatype , beaminfo_units , tbl_name , &status);	// make beam table
		if(status!=0)
		{
			printf("quickfits_write_map -->  Error creating beam information table for %s, error code %i\n",filename,status);
		}

		if(fits_movnam_hdu(fptr , BINARY_TBL , tbl_name , 0 , &status))
		{
			printf("quickfits_write_map -->  Error writing beam information to %s, error code %i\n",filename,status);
		}

		fits_write_col(fptr, TDOUBLE , 1 , 1 , 1 , 1 , &freq , &status);
		fits_write_col(fptr, TDOUBLE , 2 , 1 , 1 , 1 , &bmaj , &status);	// write out values (beam info in degrees)
		fits_write_col(fptr, TDOUBLE , 3 , 1 , 1 , 1 , &bmin , &status);
		fits_write_col(fptr, TDOUBLE , 4 , 1 , 1 , 1 , &bpa , &status);

	}

	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	return(status);

	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	return(status);
}

