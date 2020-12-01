from astropy.io import fits
import numpy as np

filename = "test_cube.fits"

readout_shape = (800,700)
num_readouts = 500
with fits.open(filename) as f:
    print("shape", f[1].data.shape)
    assert f[1].data.shape == (num_readouts,) + readout_shape
    for i_readout in range(10):
        np.testing.assert_array_equal(
            f[1].data[i_readout, :, :],
            np.arange(i_readout*10000, i_readout*10000 + readout_shape[0]*readout_shape[1]).reshape(readout_shape)
        )