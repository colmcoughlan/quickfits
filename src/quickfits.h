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



int quickfits_write_map(const char* filename , double* array , int imsize , double cell , double ra , double dec , double* centre_shift , double* rotations , double freq , double freq_delta , int stokes , char* object , char* observer , char* telescope , double equinox , char* date_obs , char* history , double bmaj , double bmin , double bpa , int niter , bool jy_per_beam);
int quickfits_read_uv_header(const char* filename, double* ra, double* dec, char* key_string, double* freq, int* nvis, int* nchan, int* central_chan, double* chan_width, int* nif);
int quickfits_read_uv_data(const char* filename, int nvis, int nchan, int nif, double* u_array, double* v_array, double* tvis, double* if_array);
int quickfits_overwrite_uv_data(const char* filename, int nvis, int nchan, int nif, double* u, double* v, double* tvis);
int quickfits_replace_ant_info(const char* filename, double* rdterm, double* ldterm);
int quickfits_read_cc_table(const char* filename , double* cc_xarray, double* cc_yarray, double* cc_varray , int ncc, int cc_table_version);
int quickfits_read_map_header(const char* filename , int* dim , double* cell , double* ra , double* dec , double* centre_shift , double* rotations , double* freq , double* freq_delta , int* stokes , char* object , char* observer , char* telescope , double* equinox , char* date_obs ,  double* bmaj , double* bmin , double* bpa , int* ncc, int cc_table_version);
int quickfits_read_map(const char* filename, double* tarr, int dim2 , double* cc_xarray, double* cc_yarray, double* cc_varray , int ncc, int cc_table_version);


