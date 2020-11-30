from astropy.io import fits

filename = "test_cube.fits"

with fits.open(filename) as f:
    print("shape", f[1].data.shape)
    assert f[1].data.shape == (20,30,500)
    print("data", f[1].data.flat[:5], f[1].data.flat[-5:])
