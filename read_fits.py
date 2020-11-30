from astropy.io import fits

filename = "test_cube.fits"

with fits.open(filename) as f:
    print("shape", f[1].data.shape)
    print("data", f[1].data)
