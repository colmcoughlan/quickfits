quickfits v1.101
12.12.2014
Colm Coughlan

# Installation instructions

Dependencies: a C compiler. You will also need cfitsio to run quickfits.

On Ubuntu cfitsio can be found in the cfitsio-dev package. On a mac the best way to install it might be through homebrew (brew install cfitsio). More mac instructions can be found at https://heasarc.gsfc.nasa.gov/fitsio/fitsio_macosx.html.

If you're using linux you probably already have a c compiler (gcc). On a mac you can install this through homebrew with "brew install gcc5", to install gcc version 5.

Once cfitsio has been installed and you know which C compiler you have, edit the makefile such that CC=\<your C compiler executable>.

You can then build the library by running "make -f makefile". If successful, you should see quickfits.a and quickfits.h in the top directory. You can move these to a location on your library path, or just add the current location to the path.

# Changes

v1.101: June 2016. Fixed bug reading EQUINOX keyword from UV FITS file

v1.1: Now allows non-square images and different cellsizes in RA and DEC directions

v1.0: Initial release

=============

# General C interfaces to the CFITSIO used across multiple programs

Contains:
	quickfits_read_map_header:
		Read the contents of a FITS map header

	quickfits_read_map:
		Read a FITS map into a double array

	quickfits_read_cc_table
		Read in only a Clean component table from a FITS map

	quickfits_write_map:
		Write a double array to a FITS map (with some metadata)

	quickfits_read_uv_header:
		Read the contents of a FITS uv header (from AIPS’s FITAB)

	quickfits_read_uv_data:
		Read a FITS uv file into a double array (from AIPS’s FITAB)

	quickfits_overwrite_uv_data:
		Replace UV data in a UV FITS file with specified double array

	quickfits_replace_ant_info:
		Replace antenna information in a FITS UV file
