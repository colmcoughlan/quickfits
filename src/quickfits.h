#include <string.h>
#include <fitsio.h>
#include <stdbool.h>
#include <math.h>



int quickfits_write_map(const char* filename , double* array , int imsize , double cell , double ra , double dec , double* centre_shift , double* rotations , double freq , double freq_delta , int stokes , char* object , char* observer , char* telescope , double equinox , char* date_obs , char* history , double bmaj , double bmin , double bpa , int niter , bool jy_per_beam);
int quickfits_read_uv_header(const char* filename, double* ra, double* dec, char* key_string, double* freq, int* nvis, int* nchan, int* central_chan, double* chan_width, int* nif);
int quickfits_read_uv_data(const char* filename, int nvis, int nchan, int nif, double* u_array, double* v_array, double* tvis, double* if_array);
int quickfits_overwrite_uv_data(const char* filename, int nvis, int nchan, int nif, double* tvis);
int quickfits_replace_ant_info(const char* filename, double* rdterm, double* ldterm);
int quickfits_read_cc_table(const char* filename , double* cc_xarray, double* cc_yarray, double* cc_varray , int ncc, int cc_table_version);
int quickfits_read_map_header(const char* filename , int* dim , double* cell , double* ra , double* dec , double* centre_shift , double* rotations , double* freq , double* freq_delta , int* stokes , char* object , char* observer , char* telescope , double* equinox , char* date_obs ,  double* bmaj , double* bmin , double* bpa , int* ncc, int cc_table_version);
int quickfits_read_map(const char* filename, double* tarr, int dim2 , double* cc_xarray, double* cc_yarray, double* cc_varray , int ncc, int cc_table_version);


