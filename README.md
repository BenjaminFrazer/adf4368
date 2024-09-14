# ADF4368

> [!WARNING] 
> This API is still WIP and as of yet FULLY untested on actual hardware. I have done extensive testing of the parameter engine and frequency selection using host testing and I'm pretty confident it will work in auto-calibration mode. Fast hopping mode may still need a bit more work. I think the framework I've written here, critically the parameter engine, will still be useful to anyone intending to extend the capabilities of the library. I'm very open to accept pull requests and fixes.

## Overview

This is a C library providing an API for the the ADF4368 fractional n Digital synthesiser. The library if fully generic and decoupled from platform specific code by relying on the user to provide primitive SPI read and write functions. decouple it from platform specific code, relying on auses dependency injection to  

## Capabilities

- Run-time computing of fractional frequency paramers based on an integer frequency in Hz.
- Parameter system supporting read-write to every single register level setting by enumerated parameter.

## Limitations

- As yet fully un-tested on actuall hardware.

## Basic Usage

### Basic initialisation

```c
#include "adf4368.h"
#include "adf4368_debug.h"

adf4368_err spi_write(uint8_t reg, uint8_t val) {
    // your spi write code in here 
    return ADF4368_ERR_OK;
};

adf4368_err spi_read(uint8_t reg, uint8_t *val) {
    // your spi read code in here 
  return ADF4368_ERR_OK;
};

adf4368_err wait_us(unsigned long time_us){
    // your wait code in here
  return ADF4368_ERR_OK;
}

// configure state struct
adf4368_state state = {
  .constants= {
        .D = 1, 
        .O = 1, 
        .R = 1, 
        .band_hz = {6400000000, 12800000000}, // 6.4-12.8G
        .ref_clk = 100000000}, // 100M
  .read_fcn = &spi_read,
  .write_fcn = &spi_write,
  .wait_fcn = &wait_us,
    // custom parameter overwrites
  .init_param_overwrites={
      {ADF4368_P_SINGLE_INSTRUCTION,    ADF4368_STREAMING_EN},
      {ADF4368_P_CMOS_OV,               ADF4368_OV_3V3},
      {ADF4368_P_SDO_ACTIVE,            ADF4368_SPI_MODE_4WIRE},
      {ADF4368_P_SDO_ACTIVE_R,            ADF4368_SPI_MODE_4WIRE},
      {ADF4368_P_MAIN_READBACK_CONTROL, ADF4368_RBC_SUBORINATE_REG},
    }
};

int main()
{
// set initialisation frequency
uint64_t f_target = 10000000000;
// cal init function
adf4368_err err = adf4368_standard_initialisation(&state, f_target);

// check return value
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
}
}
```

### Updating parameters on the fly
```c
err = adf4368_write_param(&state, i, val);
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
}
```

### Reading parameter on-the-fly

```c
err = adf4368_read_uint_param(&state, i, &val);
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
}
```

### Frequeny hopping with auto calibration
```c
adf4368_err err = adf4368_switch_freq_autocal(&state, f_target), state);
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
}
```

### Frequency hopping with manual calibration

```c
uint64_t f_target = G(10.2289340);
adf4368_vco_cal_params cal;
adf4368_err err;

/* typically these will be retrieved from a cache generated during a seperate
   * calibration phase */
cal.vco_band = 0xFF;
cal.vco_bias = 0xF;
cal.vco_core = 0x3;

err = adf4368_switch_calibration_mode(&state, ADF4368_MODE_MANUAL), state);
// handle error
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
    return -1; 
}
err = adf4368_switch_freq_mancal(&state, f_target, cal), state);
// handle error
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
    return -1; 
}
```

## API Philosophy

### Parameter Cache (TODO)

### Self check (WIP)

- There are a number of paramters in the ADF4368 need to be configured acording to particular rules, depending on configurations such as the reference clock frequency and the target RF frequency.
- When requesting a new frequency, the API will run self check and diagnostics on the selected paramters and determin if the requested frequency is achievable. If any check fails, an error code will be passed up the call stack.

### Error Handling & Debug (WIP)

- Most functions in the library will return an enumerated error code variable.
- Tools for providing user readable debug messages are provided in an optional serperate debug library, largely for memory efficiency.
- The debug library can be used by including the `debug.h` header file.
- The debug library contains functions for printing the most recently generated error codes into human readable error strings, including line-number and file name of the occurence.
- Additional functions allow you to print the internal state of key API registers such as the frequency, calibration and clock divider paramters.

### Tests

- Host testing is conducted using the Unity C testing library.
- This is included as a sub-repo in the libs/ directory. Include this by doing a recursive clone: `git clone --recurse-submodules`.
- Tests can be run using `make test`

## ADF4368 Theory (TODO)
