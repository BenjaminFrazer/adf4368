#include "adf4368.h"
#include "adf4368_debug.h"
#include <stdint.h>
#include "unity.h"

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

void setUp(void)
{
}


void tearDown(void)
{
}

void test_init(void){

  // set initialisation frequency
  uint64_t f_target = 10000000000;

  // call init function
  adf4368_err err = adf4368_standard_initialisation(&state, f_target);

  // check return value
  if (err!=ADF4368_ERR_OK){
      TEST_PRINTF(adf4368_str_err().val);
  }
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init);
    return UNITY_END();
}
