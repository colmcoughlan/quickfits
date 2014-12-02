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
		printf("ERROR : cfits_read_data --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : cfits_read_data --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : cfits_read_data --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
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
		printf("ERROR : cfits_read_data --> Error reading keywords, custom error = %d\n",err);
	}
	

	status=0;
	if (fits_movnam_hdu(fptr,BINARY_TBL,freq_extname,0,&status))		// move to frequency information hdu
	{
		printf("ERROR : cfits_read_data --> Error finding frequency table, error = %d\n",status);
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
		printf("ERROR : cfits_read_data --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
