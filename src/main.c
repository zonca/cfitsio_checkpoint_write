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
    char filename[] = "test_cube.fits";           /* name for new FITS file */
    int status, ii, jj, hdutype;
    long  fpixel, nelements;
    unsigned int *readout[600];

    /* initialize FITS image parameters */
    int bitpix   =  ULONG_IMG; /* 32-bit unsigned short pixel values       */
    long naxis    =   3;  /* 3-dimensional image                            */
	long naxes[3] = { 500, 30, 20 };   /* image is 300 pixels wide by 200 rows */

    /* allocate memory for the whole image */ 
    readout[0] = (unsigned int *)malloc( naxes[0] * naxes[1]
                                        * sizeof( unsigned int ) );

    /* initialize pointers to the start of each row of the image */
    for( ii=1; ii<naxes[1]; ii++ )
      readout[ii] = readout[ii-1] + naxes[0];

    remove(filename);               /* Delete old file if it already exists */

    status = 0;         /* initialize status before calling fitsio routines */

    if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
         printerror( status );           /* call printerror if error occurs */

	create_empty_hdu(fptr);
 
    if ( fits_movabs_hdu(fptr, 1, &hdutype, &status) ) /* move to 2nd HDU */
         printerror( status );
    
	/* write the required keywords for the primary array image.     */
    /* Since bitpix = USHORT_IMG, this will cause cfitsio to create */
    /* a FITS image with BITPIX = 16 (signed short integers) with   */
    /* BSCALE = 1.0 and BZERO = 32768.  This is the convention that */
    /* FITS uses to store unsigned integers.  Note that the BSCALE  */
    /* and BZERO keywords will be automatically written by cfitsio  */
    /* in this case.                                                */

    if ( fits_create_img(fptr,  bitpix, naxis, naxes, &status) )
         printerror( status );          

    /* initialize the values in the image with a linear ramp function */
    for (jj = 0; jj < naxes[1]; jj++)
    {   for (ii = 0; ii < naxes[0]; ii++)
        {
            readout[jj][ii] = ii + jj;
        }
    }

    fpixel = 1;                               /* first pixel to write      */
    nelements = naxes[0] * naxes[1];          /* number of pixels to write */

    /* write the array of unsigned integers to the FITS file */
    if ( fits_write_img(fptr, TUINT, fpixel, nelements, readout[0], &status) )
        printerror( status );
      
    free( readout[0] );  /* free previously allocated memory */

    if ( fits_close_file(fptr, &status) )       /* close the FITS file */
         printerror( status );
    return;

	return (0);
}