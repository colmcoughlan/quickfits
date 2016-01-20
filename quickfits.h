/*
Copyright (c) 2014, Colm Coughlan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <fitsio.h>
#include <stdbool.h>
#include <math.h>


#ifndef fitsinfo_defined
	#define fitsinfo_defined
	
	struct fitsinfo_map_tag;	// Use a tag so you can pass stucts as fitsinfo_map etc.,  rather than struct fitsinfo_map
	typedef struct fitsinfo_map_tag{
		int imsize_ra;
		int imsize_dec;
		double cell_ra;
		double cell_dec;
		double ra;
		double dec;
		double centre_shift[2];
		double rotations[2];
		double stokes;
		double freq;
		double freq_delta;

		char object[FLEN_VALUE];
		char observer[FLEN_VALUE];	// information about the source
		char telescope[FLEN_VALUE];
		double equinox;
		char date_obs[FLEN_VALUE];
		
		double bmaj;	// Assumed in degrees
		double bmin;
		double bpa;
		bool have_beam;
		int niter;
		
		int ncc;
		int cc_table_version;
	}fitsinfo_map;
	
	struct fitsinfo_uv_tag;
	typedef struct fitsinfo_uv_tag{
		int nvis;
		double ra;
		double dec;
		int nif;
		int nchan;
		int chan_width;
		int central_chan;
		double freq;

		char object[FLEN_VALUE];
		char observer[FLEN_VALUE];	// information about the source
		char telescope[FLEN_VALUE];
		double equinox;
		char date_obs[FLEN_VALUE];
	}fitsinfo_uv;
	
	
#endif




int quickfits_write_map(const char* filename , double* array, fitsinfo_map fitsi, char* history);
int quickfits_read_uv_header(const char* filename, fitsinfo_uv* fitsi);
int quickfits_read_uv_data(const char* filename, fitsinfo_uv fitsi, double* u_array, double* v_array, double* tvis, double* if_array);
int quickfits_overwrite_uv_data(const char* filename, fitsinfo_uv fitsi, double* u, double* v, double* tvis);
int quickfits_replace_ant_info(const char* filename, double* rdterm, double* ldterm);
int quickfits_read_cc_table(const char* filename , fitsinfo_map fitsi , double* cc_xarray, double* cc_yarray, double* cc_varray);
int quickfits_read_map_header(const char* filename , fitsinfo_map* fitsi);
int quickfits_read_map(const char* filename, fitsinfo_map fitsi , double* tarr , double* cc_xarray, double* cc_yarray, double* cc_varray);


