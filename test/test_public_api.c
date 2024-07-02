#include "adf4368_params.h"
#include <stdlib.h>
#include <string.h>
#include "adf4368.h"
#include "adf4368_debug.h"
#include "unity.h"
#include "unity_internals.h"
#include <byteswap.h>
#include <stdint.h>
#include "utils.h"
#include <sys/types.h>

adf4368_cfg custom_params[] = {
  {ADF4368_P_SINGLE_INSTRUCTION,    ADF4368_STREAMING_EN},
  {ADF4368_P_CMOS_OV,               ADF4368_OV_3V3},
  {ADF4368_P_SDO_ACTIVE,            ADF4368_SPI_MODE_4WIRE},
  {ADF4368_P_SDO_ACTIVE_R,            ADF4368_SPI_MODE_4WIRE},
  {ADF4368_P_MAIN_READBACK_CONTROL, ADF4368_RBC_SUBORINATE_REG},
};

adf4368_err wait_wrapper(unsigned long time_us){
  return ADF4368_ERR_OK;
};

void setUp(void) {
  state.read_fcn = dummy_spi_read;
  state.write_fcn = dummy_spi_write;
  state.wait_fcn = wait_wrapper;
  state.constants.O = 1, state.constants.D = 1, state.constants.R = 1,
  state.constants.ref_clk = M(100), state.constants.band_hz[0] = G(6.4);
  state.constants.band_hz[1] = G(12.8);
  info.err = ADF4368_ERR_OK;
  clear_event_log();
}


size_t n_elems = sizeof(custom_params)/sizeof(adf4368_cfg);

void tearDown(void) { /* This is run after testing is complete */ }

void test_init(void) {
  uint64_t f_target = M(7328.389987);
  CUSTOM_ASSERT(adf4368_standard_initialisation(&state, f_target), state);
  CUSTOM_ASSERT(adf4368_validate_freq_settings(&state), state);
  TEST_ASSERT_EQUAL_INT64_MESSAGE((int64_t)f_target, (int64_t)adf4368_get_freq(&state), "Frequency missmatch");
  CUSTOM_ASSERT(info.err, state);
  write_log("test_init_writelog.csv");
  param_dump("test_init_param_dump.csv");
};

void test_init_custom_params(void) {
  uint64_t f_target = G(10.2);
  // populate custom params
  set_regs(0);
  memcpy(&state.init_param_overwrites, &custom_params, sizeof(custom_params));
  CUSTOM_ASSERT(adf4368_standard_initialisation(&state, f_target), state);
  CUSTOM_ASSERT(adf4368_validate_freq_settings(&state), state);
  TEST_ASSERT_EQUAL_INT64_MESSAGE((int64_t)f_target, (int64_t)adf4368_get_freq(&state), "Frequency missmatch");
  CUSTOM_ASSERT(info.err, state);
  write_log("test_init_custom_write_log.csv");
  param_dump("test_init_custom_param_dump.csv");
};

void test_hop_frequency(void){
  uint64_t f_target = G(10.2289340);
  CUSTOM_ASSERT(adf4368_switch_freq_autocal(&state, f_target), state);
  TEST_ASSERT_EQUAL_INT64_MESSAGE((int64_t)f_target, (int64_t)adf4368_get_freq(&state), "Frequency missmatch");
  write_log("test_hop_frequency_writelog.csv");
  param_dump("test_hop_frequency_param_dump.csv");
};

void test_hop_frequency_manual(void){
  uint64_t f_target = G(10.2289340);
  adf4368_vco_cal_params cal; 
  cal.vco_band = 0xFF;
  cal.vco_bias = 0xF;
  cal.vco_core = 0x3;
  CUSTOM_ASSERT(adf4368_switch_calibration_mode(&state, ADF4368_MODE_MANUAL), state);
  CUSTOM_ASSERT(adf4368_switch_freq_mancal(&state, f_target, cal), state);
  TEST_ASSERT_EQUAL_INT64_MESSAGE((int64_t)f_target, (int64_t)adf4368_get_freq(&state), "Frequency missmatch");
  write_log("test_hop_frequency_manual_writelog.csv");
  param_dump("test_hop_frequency_manual_param_dump.csv");
};


int main(void) {
  RUN_TEST(test_init);
  RUN_TEST(test_init_custom_params);
  RUN_TEST(test_hop_frequency);
  RUN_TEST(test_hop_frequency_manual);
  return UNITY_END();
}
