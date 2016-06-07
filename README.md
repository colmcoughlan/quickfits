quickfits v1.101
12.12.2014
Colm Coughlan

Changes

v1.101: June 2016. Fixed bug reading EQUINOX keyword from UV FITS file

v1.1: Now allows non-square images and different cellsizes in RA and DEC directions

v1.0: Initial release

Released under BSD 2-clause style licence:

Copyright (c) 2014, Colm Coughlan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
