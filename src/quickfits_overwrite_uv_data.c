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
		printf("ERROR : cfits_overwrite_uvdata --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : cfits_overwrite_uvdata --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : cfits_overwrite_uvdata --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
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
				printf("ERROR : cfits_overwrite_uvdata --> Error writing keywords, custom error = %d\n",err);
			}
		}

		i++;
	}
	status = 0;



	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : cfits_overwrite_uvdata --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
