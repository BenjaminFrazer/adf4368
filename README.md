# ADF4368

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

// configure state struct
adf4368_state state = {
  .constants= {
        .D = 1, 
        .O=1, 
        .R = 1, 
        .band_hz = {6000000000, 7000000000}, 
        .ref_clk = 100000000},
    },
  .read_fcn = &spi_read,
  .write_fcn = &spi_write,
  .adf4368_wait_fcn = &wait_us,
    // custom parameter overwrites
  .init_param_overwrites={
      {ADF4368_P_SINGLE_INSTRUCTION,    ADF4368_STREAMING_EN},
      {ADF4368_P_CMOS_OV,               ADF4368_OV_3V3},
      {ADF4368_P_SDO_ACTIVE,            ADF4368_SPI_MODE_4WIRE},
      {ADF4368_P_SDO_ACTIVE_R,            ADF4368_SPI_MODE_4WIRE},
      {ADF4368_P_MAIN_READBACK_CONTROL, ADF4368_RBC_SUBORINATE_REG},
    }
};
// set initialisation frequency
uint64_t f_target = 6000000000;
// cal init function
adf4368_err err = adf4368_standard_initialisation(&state, f_target);

// check return value
if (err!=ADF4368_ERR_OK){
    printf(adf_str_err.val);
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
