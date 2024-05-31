#include "unity_config.h"
#include "adf4368.h"
#include "unity.h"
#include <stdint.h>
#include <stdio.h>


#define VERBOSE
#define K(val) ((uint64_t)(val) *1000)
#define M(val) ((uint64_t)(val) *1000000)
#define G(val) ((uint64_t)(val) *1000000000)

/* sometimes you may want to get at local data in a module.
 * for example: If you plan to pass by reference, this could be useful
 * however, it should often be avoided */

extern error_msg adf4368_error_msg;

void setUp(void)
{
  /* This is run before EACH TEST */
}

void tearDown(void)
{
    /* This is run after testing is complete */
}

void test_frac_value_calculation(void)
{
  /* All of these should pass */

  typedef struct {
    _freq_params p;
    uint64_t f_band[100];
    adf4368_err fail_code;
  } _val;

  #define BAND {1, G(6)+M(1),G(7)+999+K(10)+1,G(8)+M(8),G(9)+K(3)+42,G(10)+K(1)+1,G(11)+M(3)}

  _val vals[10] = {
    (_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(01)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND},
    (_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(10)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND},
    (_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(20)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND},
    (_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(40)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND},
    (_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(80)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND},
  };

  int n_vals = sizeof(vals)/sizeof(_val);
  char testcase_info[300];

/*Loop through all vals entries.**********************************************/
  for (int i = 0; i < n_vals; i++){
    _val val = vals[i];

/*Loop through a selection of frequencies.************************************/
    for (int ii = 0; ii < 100; ii++){
      uint64_t freq = val.f_band[ii];
      _freq_params p = val.p;
      if (freq != 0){
        adf4368_err err = _calc_frac_values(freq, &p);
        double f_act = _calc_freq_hz(p);
        sprintf(testcase_info, "i=%u, ii=%u, f_req%eMhz, f_act%eMhz, err=%s\n",
                i, ii, (float)freq, (float)f_act, adf4368_error_msg[err]);
        #ifdef VERBOSE
          TEST_MESSAGE(testcase_info);
        #endif
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(freq, f_act, testcase_info);
        TEST_ASSERT_MESSAGE(err==ADF4368_ERR_OK, testcase_info);
      }
    }

  }
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_frac_value_calculation);
    return UNITY_END();
}
