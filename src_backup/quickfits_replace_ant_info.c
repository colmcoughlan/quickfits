#include "quickfits.h"

int quickfits_replace_ant_info(const char* filename, double* rdterm, double* ldterm)
{
/*
	INPUTS:
		char* tfilename : c string = name of FITS file to be read
		int nvis : number of visibilities to be read
	OUTPUTS:
		u_array : nvis u coords, converted to physical units by fitsio
		v_array : nvis v coords, converted to physical units by fitsio
		tvis : (nvis*24) visibilities, in Jy
*/
	fitsfile *fptr;

	int status, i, j;
	char extname[]="AIPS AN ";
//	char comment[FLEN_VALUE];
	char comment[]="UVFILL";
	char poltype[]="VLBI    ";
	char rdtermname[]="POLCALA";	// assumes column a is the r term and column b is the l term
	char ldtermname[]="POLCALB";
	double d_null=0;
	int colnum;
	double temp;

	status = 0;	// for error processing

	if ( fits_open_file(&fptr,filename, READWRITE, &status) )	// open file and make sure it's open
	{
		printf("ERROR : cfits_replace_ant_info --> Error opening FITS file, error = %d\n",status);
		return(status);
	}

	if (fits_movnam_hdu(fptr,BINARY_TBL,extname,0,&status))		// move to main AIPS UV hdu
	{
		printf("ERROR : cfits_replace_ant_info --> Error locating AIPS UV binary extension, error = %d\n",status);
		printf("ERROR : cfits_replace_ant_info --> Did you remember to use the AIPS FITAB task instead of FITTP?\n");
		return(status);
	}
	// write out d-term data to correct columns

	fits_get_colnum(fptr,CASEINSEN,rdtermname,&colnum,&status);
	if(status!=0)
	{
		printf("ERROR : cfits_replace_ant_info --> Error finding r dterm column to write to, error = %d\n",status);
	}


	fits_write_col(fptr, TDOUBLE, colnum, 1, 1, 10*2,  rdterm, &status);
	if(status!=0)
	{
		printf("ERROR : cfits_replace_ant_info --> Error writing keywords, error = %d\n",status);
	}

	fits_get_colnum(fptr,CASEINSEN,ldtermname,&colnum,&status);
	if(status!=0)
	{
		printf("ERROR : cfits_replace_ant_info --> Error finding l dterm column to write to, error = %d\n",status);
	}

	fits_write_col(fptr, TDOUBLE, colnum, 1, 1, 10*2,  ldterm, &status);
	if(status!=0)
	{
		printf("ERROR : cfits_replace_ant_info --> Error writing new antenna information, error = %d\n",status);
	}

	fits_update_key(fptr, TSTRING, "POLTYPE", &poltype,comment, &status);
	if(status!=0)
	{
		printf("ERROR : cfits_replace_ant_info --> Error writing polarisation type to header file, error = %d\n",status);
	}



	if ( fits_close_file(fptr, &status) )
	{
		printf("ERROR : cfits_replace_ant_info --> Error closing FITS file, error = %d\n",status);
		return(status);
	}

	return(status);
}
