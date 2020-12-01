CFITSIO file writing with checkpoints
=====================================

![C/C++ CI](https://github.com/zonca/cfitsio_checkpoint_write/workflows/C/C++%20CI/badge.svg)

Prototype implementation and test of writing a FITS
file to disk and checkpointing it by flushing the contents
to disk in order to avoid having a corrupt file in case of segfault.

`fits_flush_file` allows to write a file to disk and then reopen it,
so that in case of segmentation fault the file is not corrupt.
However it can be slower, so we can just use it after writing the first
time, then we can switch to `fits_flush_buffer` which is quicker.

In this test case the software needs to write a datacube, it writes
a 2D slice (readout) at a time, therefore it only needs to explicitely allocate
memory for one 2D slice.
However, `cfitsio` keeps internal buffers, therefore, calling regularly
`fits_flush_buffer` both empties those buffers and also checkpoints the file to
disk.
We can configure independently how often we checkpoint the file but still
having a single output datacube.

In this specific example, executed via Github Actions under Ubuntu, we execute:

* we target a datacube (500x30x20)
* we allocate memory for one (30x20) readout
* we write the first readout
* we call `fits_flush_file` to create the HDU and write the first readout
* we write other 9 readouts for a total of 10 readouts
* we call `fits_flush_buffer` to checkpoint the file on disk
* we trigger a segmentation fault
* open the output file with `astropy` and check the contents are as expected

See:

* [implementation in C](src/main.c)
* [testing program in Python](read_fits.py)
* [output log of the execution on Github actions](https://github.com/zonca/cfitsio_checkpoint_write/actions?query=workflow%3A%22C%2FC%2B%2B+CI%22)
