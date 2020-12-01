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
    int status, ii, jj, kk, hdutype;
    long  nelements;
    long fpixel[3];
    /* initialize FITS image parameters */
    int bitpix   =  ULONG_IMG; /* 32-bit unsigned short pixel values       */
    long naxis    =   3;  /* 3-dimensional image                            */
    static long num_readouts = 500;
    static long xsize = 700;
    static long ysize = 800;
    long naxes[3] = { xsize, ysize, num_readouts };
    int readout[ysize][xsize];

    long num_readouts_written = 10;
    /* allocate memory for the whole image */
    // readout[0] = (unsigned int *)malloc( naxes[0] * naxes[1]
    //                                    * sizeof( unsigned int ) );

    remove(filename);               /* Delete old file if it already exists */

    status = 0;         /* initialize status before calling fitsio routines */

    if (fits_create_file(&fptr, filename, &status)) /* create new FITS file */
         printerror( status );           /* call printerror if error occurs */

    create_empty_hdu(fptr);
    if ( fits_movabs_hdu(fptr, 1, &hdutype, &status) ) /* move to 1nd HDU */
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
    kk = 0;
    for (jj = 0; jj < naxes[1]; jj++)
    {   for (ii = 0; ii < naxes[0]; ii++)
        {
            readout[jj][ii] = kk;
            kk++;
        }
    }

    nelements = xsize * ysize;          /* number of pixels to write */

    /* write the array of unsigned integers to the FITS file */
    for (kk = 0; kk < num_readouts_written; kk++) {
        fpixel[0] = 1;    /* first pixel to write      */
        fpixel[1] = 1;
        fpixel[2] = kk + 1;

        if ( fits_write_pix(fptr, TUINT, fpixel, nelements, readout[0], &status) )
            printerror( status );

          for (jj = 0; jj < naxes[1]; jj++)
        {   for (ii = 0; ii < naxes[0]; ii++)
            {
                readout[jj][ii] += 10000;
            }
        }
        if (kk == 0) {
            if ( fits_flush_file(fptr, &status) ) // first time we need to flush_file
                printerror( status );
        }
    }
    if ( fits_flush_buffer(fptr, 0, &status) )  // flush buffer is a lot quicker
        printerror( status );
    *(int*)0 = 0;

    //free( readout[0] );  /* free previously allocated memory */


    if ( fits_close_file(fptr, &status) )       /* close the FITS file */
         printerror( status );

    return (0);
}
