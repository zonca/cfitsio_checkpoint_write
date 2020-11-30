#include <stdio.h>
#include "fitsio.h"

void printerror( int status)
{
    /*****************************************************/
    /* Print out cfitsio error messages and exit program */
    /*****************************************************/


    if (status)
    {
       fits_report_error(stderr, status); /* print error report */

       exit( status );    /* terminate the program, returning error status */
    }
    return;
}

static int create_empty_hdu(fitsfile * fptr)
{
    int status=0;
    int bitpix=SHORT_IMG;
    int naxis=0;
    long* naxes=NULL;
    if (fits_create_img(fptr, bitpix, naxis, naxes, &status)) {
        printerror(status);
        return 1;
    }

    return 0;
}

int main()
{
	printf("Hello World!\n");

    fitsfile *fptr;       /* pointer to the FITS file, defined in fitsio.h */
    int status, hdutype;
    long firstrow, firstelem;

    int tfields   = 3;       /* table will have 3 columns */
    long nrows    = 6;       /* table will have 6 rows    */

    char filename[] = "test_cube.fits";           /* name for new FITS file */
    char extname[] = "PLANETS_Binary";           /* extension name */

    /* define the name, datatype, and physical units for the 3 columns */
    char *ttype[] = { "Planet", "Diameter", "Density" };
    char *tform[] = { "8a",     "1J",       "1E"    };
    char *tunit[] = { "\0",      "km",       "g/cm"    };

    /* define the name diameter, and density of each planet */
    char *planet[] = {"Mercury", "Venus", "Earth", "Mars","Jupiter","Saturn"};
    long  diameter[] = {4880,     12112,   12742,   6800,  143000,   121000};
    float density[]  = { 5.1f,      5.3f,     5.52f,   3.94f,   1.33f,     0.69f};

    status=0;
	
    remove(filename);               /* Delete old file if it already exists */

    /* open the FITS file containing a primary array and an ASCII table */
    if ( fits_create_file(&fptr, filename, &status) ) 
         printerror( status );
	
	create_empty_hdu(fptr);

    if ( fits_movabs_hdu(fptr, 1, &hdutype, &status) ) /* move to 1st HDU */
         printerror( status );

    /* append a new empty binary table onto the FITS file */
    if ( fits_create_tbl( fptr, BINARY_TBL, nrows, tfields, ttype, tform,
                tunit, extname, &status) )
         printerror( status );

    firstrow  = 1;  /* first row in table to write   */
    firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

    /* write names to the first column (character strings) */
    /* write diameters to the second column (longs) */
    /* write density to the third column (floats) */

    fits_write_col(fptr, TSTRING, 1, firstrow, firstelem, nrows, planet,
                   &status);
    fits_write_col(fptr, TLONG, 2, firstrow, firstelem, nrows, diameter,
                   &status);
    fits_write_col(fptr, TFLOAT, 3, firstrow, firstelem, nrows, density,
                   &status);

    if ( fits_close_file(fptr, &status) )       /* close the FITS file */
         printerror( status );
    return;

	return (0);
}