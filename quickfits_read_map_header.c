/*
    This program is called cfits_write. It interacts with the CFITSIO library to read metadata from a FITS file
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
	OUTPUTS:
		various metadata

	NB : see FITS file format documentation to see what each parameter means (though AIPS may interpret it differently...)
*/

#include "error_map_heads.hpp"

int cfits_read_header_map(const char* filename , int* dim , double* cell , double* ra , double* dec , double* centre_shift , double* rotations , double* freq , double* freq_delta , int* stokes , char* object , char* observer , char* telescope , double* equinox , char* date_obs ,  double* bmaj , double* bmin , double* bpa , int* ncc, int cc_table_version)
{

/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
	OUTPUTS:
		ra = right ascention
		dec = declination
		key_string = name of source
		freq = frequency
		cell =  cellsize (degrees)
		dim = image size
		bmaj, bmin, bpa = beam information (degrees)
*/
	fitsfile *fptr;

	int status,i;
	int err;
	char comment[FLEN_VALUE];
	char key_name[FLEN_VALUE];
	char key_type[FLEN_VALUE];
	char beamhdu[]="AIPS CG ";
	char cchdu[]="AIPS CC ";
	char bmajname[]="BMAJ";
	char bminname[]="BMIN";
	char bpaname[]="BPA";
	int colnum;
	double temp;
	float floatbuff;
	float float_null=0;
	int int_null=0;
	long longbuff;


	status = 0;	// for error processing
	err=0;



	if ( fits_open_file(&fptr,filename, READONLY, &status) )	// open file and make sure it's open
	{
		printf("ERROR : cfits_read_header_map --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movabs_hdu(fptr,1,&i,&status))		// move to main AIPS image HDU (assuming it's the first one)
	{
		printf("ERROR : cfits_read_header_map --> Error locating AIPS ACSII table extension, error = %d\n",status);
		printf("ERROR : cfits_read_header_map --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}

	// read in some optional keys

	fits_read_key(fptr,TSTRING,"OBJECT",object,comment,&status);
	fits_read_key(fptr,TSTRING,"OBSERVER",observer,comment,&status);
	fits_read_key(fptr,TSTRING,"TELESCOP",telescope,comment,&status);
	fits_read_key(fptr,TDOUBLE,"EQUINOX",equinox,comment,&status);
	fits_read_key(fptr,TSTRING,"DATE-OBS",date_obs,comment,&status);

	// read in important keys

	status = 0;

	fits_read_key(fptr,TDOUBLE,"OBSRA",ra,comment,&status);
	err+=status;
	fits_read_key(fptr,TDOUBLE,"OBSDEC",dec,comment,&status);
	err+=status;
	
	if(err!=0)
	{
		printf("ERROR : cfits_read_header_map --> Error reading RA and DEC, custom error = %d\n",err);
		return(err);
	}
	
	//	Now iterate through CTYPE coords to get important information about RA, DEC, cellsize etc.
	
	i=1;
	while(status!=KEY_NO_EXIST)
	{
		sprintf(key_name,"CTYPE%d",i);
		fits_read_key(fptr,TSTRING,key_name,key_type,comment,&status);

		if( !strcmp(key_type,"RA---SIN") )
		{
			printf("Success\n");
			sprintf(key_name,"CDELT%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			cell[0]=fabs(temp);
			sprintf(key_name,"CRPIX%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			centre_shift[0]=temp;
			sprintf(key_name,"CROTA%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			rotations[0]=temp;
		}

		if( !strcmp(key_type,"DEC--SIN") )
		{
			sprintf(key_name,"CRPIX%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			centre_shift[1]=temp;
			sprintf(key_name,"CROTA%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,&temp,comment,&status);
			rotations[1]=temp;
		}
		
		if( !strcmp(key_type,"FREQ    ") )
		{
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,freq,comment,&status);
			sprintf(key_name,"CRDELT%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,freq_delta,comment,&status);
		}
		
		if( !strcmp(key_type,"STOKES    ") )
		{
			sprintf(key_name,"CRVAL%d",i);
			fits_read_key(fptr,TDOUBLE,key_name,stokes,comment,&status);
		}

		i++;
	}
	status=0;


	fits_read_key(fptr,TDOUBLE,"NAXIS1",&temp,comment,&status);
	err+=status;
	dim[0]=int(temp);	// dim is stored as a double in the FITS file

	if(err!=0)
	{
		printf("ERROR : cfits_read_header_map --> Error reading keywords, custom error = %d\n",err);
		return(err);
	}

	fits_read_key(fptr,TDOUBLE,"BMAJ",bmaj,comment,&status);
	err += status;
	fits_read_key(fptr,TDOUBLE,"BMIN",bmin,comment,&status);
	err += status;
	fits_read_key(fptr,TDOUBLE,"BPA",bpa,comment,&status);
	err += status;
	
	if(err!=0)	// if the beam information isn't in the main header, move to AIPS CG HDU for beam information
	{
		if (fits_movnam_hdu(fptr,BINARY_TBL,beamhdu,0,&status))		// move to beam information hdu
		{
			printf("Warning : cfits_read_header_map --> No beam information found.");
			bmaj[0] = 0.0;
			bmin[0] = 0.0;	// changed this because model files don't have any beam information. Should check to make sure beam info is valid in other code
			bpa[0] = 0.0;
		}
		else
		{
			fits_get_colnum(fptr,CASEINSEN,bmajname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error locating BMAJ information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error reading BMAJ information, error = %d\n",status);
				return(err);
			}
			bmaj[0]=double(floatbuff);

			fits_get_colnum(fptr,CASEINSEN,bminname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error locating BMIN information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error reading BMIN information, error = %d\n",status);
				return(err);
			}
			bmin[0]=double(floatbuff);

			fits_get_colnum(fptr,CASEINSEN,bpaname,&colnum,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error locating BPA information, error = %d\n",status);
				return(err);
			}
			fits_read_col(fptr,TFLOAT,colnum,1,1,1,&float_null,&floatbuff,&int_null,&status);
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error reading BPA information, error = %d\n",status);
				return(err);
			}
			bpa[0]=double(floatbuff);
		}
	}






	// move to AIPS CC HDU for clean component information if requested. Read, or give error if a failure occurs. Allow for possibility of the outdated "A3DTABLE" table type 6 as well as normal BINARY_TBL

	if (cc_table_version >=0 )
	{
		if (fits_movnam_hdu(fptr,ANY_HDU,cchdu,cc_table_version,&status) != BAD_HDU_NUM)		// move to main AIPS UV hdu
		{
			fits_get_num_rows(fptr,&longbuff,&status);
			ncc[0]=longbuff;
			if(status!=0)
			{
				printf("ERROR : cfits_read_header_map -->  Error reading number of clean components, error = %d\n",status);
				return(status);
			}
		}
		else
		{
			printf("WARNING : cfits_read_header_map -->  No clean component table detected.\n");
			ncc[0]=0;
			return(status);
		}
	}
	else
	{
		ncc[0]=0;
	}


	

	status=0;
	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : cfits_read_header_map --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
