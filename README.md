# ADF4368
> [!WARNING] 
> This API is still WIP and as of yet FULLY untested on actual hardware. I have done extensive testing of the parameter engine and frequency selection using host testing and I'm pretty confident it will work in auto-calibration mode. Fast hopping mode may still need a bit more work. I think the framework I've written here, critically the parameter engine, will still be useful to anyone intending to extend the capabilities of the library. I'm very open to accept pull requests and fixes.

## Overview

This repository provides a 

## Capabilities

- On the fly computing of fractional frequency paramers.

## Limitations

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

### Initialisation with custom parameters
```c

```

### Updating parameters on the fly
```c

```

### Frequeny hopping with auto calibration
```c

```

### Frequency hopping with manual calibration

## API Philosophy

### Parameter Cache

- State management problem
- Also refered to as staging area

### Self check

### Error Handling

- 

### Debug


### Tests

## ADF4368 Theory (TODO)
