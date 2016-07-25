quickfits v1.101
12.12.2014
Colm Coughlan

Changes

v1.101: June 2016. Fixed bug reading EQUINOX keyword from UV FITS file

v1.1: Now allows non-square images and different cellsizes in RA and DEC directions

v1.0: Initial release

=============

General C interfaces to the CFITSIO used across multiple programs

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
