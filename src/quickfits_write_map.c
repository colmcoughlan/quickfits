/*
 Copyright (c) 2014, Colm Coughlan
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quickfits.h"


int quickfits_write_map(const char* filename , double* array, fitsinfo_map fitsi, char* history)
{
    /*
     Write out FITS map.
     
     inputs: (contained in fisinfo_map structure)
        filename = name of file to write out
        array = image to write out
        imsize = dimension of image
        cell = cellsize
        ra,dec = Right ascension and declination
        centre_shift = any shift applied to the centre of the map
        rotations = any rotation to be applied to the map
        freq = frequency of map
        freq_delta = change in frequency
        stokes = Stokes parameter of map
        object = name of object in map
        observer = observer name
        telescope = observing telescope
        equinox = Equinox
        data_obs = observing date
        history = history comments
        bmaj = restoring beam major axis (degrees)
        bmin = restoring beam minor axis (degrees)
        bpa = restoring beam angle (degrees)
        niter = number of iterations
        jy_per_beam = boolean - are the units Jy/Beam or Jy?
     
     returns:
        0 if no errors occur.
     */

	fitsfile *fptr;	// pointer to fits file
	int status, i, j;
	long fpixel = 1, naxis = 4, nelements;	// fpixel is the coordinate of the first pixel to be read
	long naxes[4] = { fitsi.imsize_ra, fitsi.imsize_dec , 1 , 1 };
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


	fits_update_key(fptr, TSTRING, "OBJECT", fitsi.object , comment , &status);
	fits_update_key(fptr, TSTRING, "OBSERVER", fitsi.observer , comment , &status);	// write out information about the source
	fits_update_key(fptr, TSTRING, "TELESCOP", fitsi.telescope , comment , &status);
	fits_update_key(fptr, TDOUBLE, "EQUINOX", &fitsi.equinox , comment, &status);
	fits_update_key(fptr, TSTRING, "DATE-OBS", fitsi.date_obs , comment , &status);
	fits_update_key(fptr, TDOUBLE, "OBSRA", &fitsi.ra , comment , &status);
	fits_update_key(fptr, TDOUBLE, "OBSDEC", &fitsi.dec , comment , &status);

	temp=1.0;
	fits_update_key(fptr, TDOUBLE, "BSCALE", &temp,comment, &status);

	temp=0.0;
	fits_update_key(fptr, TDOUBLE, "BZERO", &temp,comment, &status);


	if(fitsi.have_beam)
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

	fits_update_key(fptr, TDOUBLE, "CRVAL1", &fitsi.ra , comment , &status);

	temp = -fitsi.cell_ra;
	fits_update_key(fptr, TDOUBLE, "CDELT1", &temp , comment , &status);

	temp = fitsi.centre_shift[0];
	fits_update_key(fptr, TDOUBLE, "CRPIX1", &temp , comment , &status);

	temp = fitsi.rotations[0];
	fits_update_key(fptr, TDOUBLE, "CROTA1", &temp , comment , &status);





	sprintf(tstring,"DEC--SIN");
	fits_update_key(fptr, TSTRING, "CTYPE2", tstring , comment , &status);	// write out information about the second axis, Declination

	fits_update_key(fptr, TDOUBLE, "CRVAL2", &fitsi.dec , comment , &status);

	fits_update_key(fptr, TDOUBLE, "CDELT2", &fitsi.cell_dec , comment , &status);

	temp = fitsi.centre_shift[1];
	fits_update_key(fptr, TDOUBLE, "CRPIX2", &temp , comment , &status);

	temp = fitsi.rotations[1];
	fits_update_key(fptr, TDOUBLE, "CROTA2", &temp , comment , &status);





	sprintf(tstring,"FREQ");
	fits_update_key(fptr, TSTRING, "CTYPE3", tstring , comment , &status);	// write out information about the third axis, Frequency

	fits_update_key(fptr, TDOUBLE, "CRVAL3", &fitsi.freq , comment , &status);

	fits_update_key(fptr, TDOUBLE, "CDELT3", &fitsi.freq_delta , comment , &status);

	temp=1.0;
	fits_update_key(fptr, TDOUBLE, "CRPIX3", &temp , comment , &status);

	temp=0.0;
	fits_update_key(fptr, TDOUBLE, "CROTA3", &temp , comment , &status);



	sprintf(tstring,"STOKES");
	fits_update_key(fptr, TSTRING, "CTYPE4" , tstring , comment , &status);	// write out information about the fourth axis, Stokes parameter

	temp = (double)(fitsi.stokes);
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

	if(fitsi.have_beam)	// write out beam information for AIPS if necessary. note AIPS CG --> AIPS CLEAN gaussian, which is not true in this case, but is used for compatability with AIPS
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
		sprintf(tstring,"AIPS   CLEAN BMAJ=  %.8lf BMIN=  %.8lf BPA=   %.8lf" , fitsi.bmaj , fitsi.bmin , fitsi.bpa );
		fits_write_history(fptr , tstring , &status);

		if(fitsi.niter>0)
		{
			sprintf(tstring,"COMMENT : NITER BELOW NOT NECESSARILY FROM CLEAN");	// write beam as AIPS-style note
			fits_write_history(fptr , tstring , &status);


			sprintf(tstring,"AIPS   CLEAN NITER=     %d PRODUCT=1",fitsi.niter);
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

		fits_write_col(fptr, TDOUBLE , 1 , 1 , 1 , 1 , &fitsi.freq , &status);
		fits_write_col(fptr, TDOUBLE , 2 , 1 , 1 , 1 , &fitsi.bmaj , &status);	// write out values (beam info in degrees)
		fits_write_col(fptr, TDOUBLE , 3 , 1 , 1 , 1 , &fitsi.bmin , &status);
		fits_write_col(fptr, TDOUBLE , 4 , 1 , 1 , 1 , &fitsi.bpa , &status);

	}

	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	return(status);

	fits_close_file(fptr, &status);
	fits_report_error(stderr, status);

	return(status);
}

