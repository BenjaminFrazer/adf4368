
//#define BREAK_ON_ERR

#include "adf4368.h"
#include "adf4368_priv.h"
#include "adf4368_debug.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <byteswap.h>
#include <signal.h>


#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
//#define VERBOSE

/* sometimes you may want to get at local data in a module.
 * for example: If you plan to pass by reference, this could be useful
 * however, it should often be avoided */


typedef struct {
    adf4360_freq_params p;
    uint64_t f_band[10];
    adf4368_err fail_code;
} _val;


uint8_t regs[ADF4368_N_CACHE_REGS];

adf4368_read_fcn dummy_spi_read;
adf4368_write_fcn dummy_spi_write;

adf4368_err dummy_spi_write(uint8_t reg, uint8_t val){
  regs[reg] = val;
  return ADF4368_ERR_OK;
};

adf4368_err dummy_spi_read(uint8_t reg, uint8_t* val){
  *val = regs[reg];
  return ADF4368_ERR_OK;
};

void set_regs(uint8_t val){
  memset(&regs, val, sizeof(regs));
};

adf4368_state state;

void setUp(void)
{
  state.read_fcn = dummy_spi_read;
  state.write_fcn = dummy_spi_write;
  info.err = ADF4368_ERR_OK;
  set_regs(0);
}

void tearDown(void)
{
    /* This is run after testing is complete */
}

void test_number_of_params(void){
TEST_ASSERT_EQUAL_MESSAGE(ADF4368_N_PARAMS, ADF4368_P_PARAM_END+1, "Incorrect number of params defined");
};

void test_param_n_regs(void)
{
  TEST_ASSERT_EQUAL(0, N_REGS(_param_lut[ADF4368_P_PARAM_NOT_DEF]));
  TEST_ASSERT_EQUAL(4, N_REGS(_param_lut[ADF4368_P_FRAC1WORD]));
  TEST_ASSERT_EQUAL(2, N_REGS(_param_lut[ADF4368_P_NINT]));
  TEST_ASSERT_EQUAL(2, N_REGS(_param_lut[ADF4368_P_BLEED_I]));
  TEST_ASSERT_EQUAL(2, N_REGS(_param_lut[ADF4368_P_PRODUCT_ID]));
  TEST_ASSERT_EQUAL(3, N_REGS(_param_lut[ADF4368_P_RESYNC_WAIT]));
  TEST_ASSERT_EQUAL(1, N_REGS(_param_lut[ADF4368_P_SOFT_RESET]));

};

adf4368_err _assert_tester(adf4368_err err)
{
  _ASSERT_ERR(err)
  return ADF4368_ERR_OK;
};


void test_test_err_assert(void)
{

  TEST_ASSERT_EQUAL(ADF4368_ERR_OK, _assert_tester(ADF4368_ERR_OK));
  TEST_ASSERT_EQUAL(ADF4368_ERR_N_LOW, _assert_tester(ADF4368_ERR_N_LOW));
  //TEST_PRINTF("%s",adf4368_str_err().val);
  info.err = ADF4368_ERR_OK; // clear the error
  TEST_ASSERT_EQUAL(ADF4368_ERR_UNKOWN, _assert_tester(ADF4368_ERR_UNKOWN));
  //TEST_PRINTF("%s",adf4368_str_err().val);
}

void test_frac_value_calculation(void)
{
  /* All of these should pass */

#define BAND0 {1, G(6)+M(1),G(7)+999+K(10)+1,G(8)+M(8),G(9)+K(3)+42,G(10)+K(1)+1,G(11)+M(3)}

  _val vals[10] = {
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(01)}}, .fail_code=ADF4368_ERR_OK, .f_band=BAND0},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(10)}}, .fail_code=ADF4368_ERR_OK, .f_band=BAND0},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(20)}}, .fail_code=ADF4368_ERR_OK, .f_band=BAND0},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(40)}}, .fail_code=ADF4368_ERR_OK, .f_band=BAND0},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(80)}}, .fail_code=ADF4368_ERR_OK, .f_band=BAND0},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(40)}}, .fail_code=ADF4368_ERR_OK, .f_band={G(6.897394)}},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(40)}}, .fail_code=ADF4368_ERR_OK, .f_band={M(7328)}},
  };

  int n_vals = sizeof(vals)/sizeof(_val);
  char testcase_info[300];

/*Loop through all vals entries.**********************************************/
  for (int i = 0; i < n_vals; i++){
    _val val = vals[i];

/*Loop through a selection of frequencies.************************************/
    for (int ii = 0; ii < 10 ; ii++){
      uint64_t freq = val.f_band[ii];
      adf4360_freq_params p = val.p;
      if (p.c.ref_clk == 0){

      }
      if (freq != 0){
        fracn f;
        ad4368_constants c = p.c;
        adf4368_err err = _calc_frac_values(freq, c, &f);
        double f_act = _calc_freq_hz(c, f);
        sprintf(testcase_info, "i=%u, ii=%u, f_req=%ehz, f_act=%ehz, err=%s\n",
                i, ii, (float)freq, (float)f_act, adf4368_error_msg[err]);
        //  TEST_MESSAGE(testcase_info);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(freq, f_act, testcase_info);
        TEST_ASSERT_MESSAGE(err==ADF4368_ERR_OK, testcase_info);
      }
    }

  }
}

void test_freq_param_validation(void){
  #define BAND1 {G(7), G(11)}
  #define BAND2 {G(6), G(10)}
  #define BAND3 {M(800), G(4)}
  //adf4368_validate_freq_settings
  _val vals[] = {
/*Test passing values*********************************************************/
    //(_val){.p={.D=1, .R=1, .O=1, .ref_clk=M(20)}, .fail_code=ADF4368_ERR_OK, .f_band=BAND1},
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(10),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(40),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(80),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(100),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=1, .O=1, .ref_clk=M(200),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=2, .O=1, .ref_clk=M(400),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=4, .O=1, .ref_clk=M(800),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=8, .O=1, .ref_clk=G(1.6),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=16, .O=1, .ref_clk=G(3.2), .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=1, .R=32, .O=1, .ref_clk=G(4),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_OK,         },
/*Test VCO value**************************************                 ****************/
    (_val){.p={.c={.D=2, .R=1, .O=4, .ref_clk=M(40),   .band_hz={G(2), G(2)}   }}, .fail_code=ADF4368_ERR_OK,         },
    (_val){.p={.c={.D=2, .R=1, .O=8, .ref_clk=M(40),   .band_hz={G(2), G(2)}   }}, .fail_code=ADF4368_ERR_VCO_F_HIGH, },
    (_val){.p={.c={.D=2, .R=1, .O=2, .ref_clk=M(40),   .band_hz={G(2), G(2)}   }}, .fail_code=ADF4368_ERR_VCO_F_LOW,  },
/*Test failing{.c= invalid O value*******************}       z          *****************/
    (_val){.p={.c={.D=1, .R=1, .O=3, .ref_clk=M(20),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_O_INVALID,  },
    (_val){.p={.c={.D=1, .R=1, .O=16, .ref_clk=M(20),  .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_O_INVALID,  },
/*Test invalid{.c= R value****************************       z          ****************/
    (_val){.p={.c={.D=2, .R=64, .O=1, .ref_clk=M(200), .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_R_HIGH,     },
/*Test invalid{.c= D value****************************       z          ****************/
    (_val){.p={.c={.D=3, .R=1, .O=1, .ref_clk=M(20),   .band_hz=BAND1    }}, .fail_code=ADF4368_ERR_D_HIGH,     },
/*Test invalid{.c= N value***************************}       z          *****************/
    (_val){.p={.c={.D=1, .R=1, .O=4, .ref_clk=M(100),   .band_hz={G(1.9), G(1.9)} }}, .fail_code=ADF4368_ERR_OK,         }, //TODO
    (_val){.p={.c={.D=1, .R=1, .O=4, .ref_clk=M(100),   .band_hz={G(1.8), G(1.8)} }}, .fail_code=ADF4368_ERR_N_LOW,      }, //TODO
    (_val){.p={.c={.D=1, .R=20, .O=1, .ref_clk=M(10), .band_hz={G(10), G(10)}}}, .fail_code=ADF4368_ERR_N_HIGH,     }, //TODO
/*Test Low Ref{.c=erence Frequency********************       z          ****************/
    (_val){.p={.c={.D=2, .R=1, .O=1, .ref_clk=M(10),   .band_hz=BAND1   }}, .fail_code=ADF4368_ERR_OK,          },
    (_val){.p={.c={.D=2, .R=1, .O=1, .ref_clk=M(9),    .band_hz=BAND1   }}, .fail_code=ADF4368_ERR_F_REF_LOW,   },
    (_val){.p={.c={.D=2, .R=37, .O=2, .ref_clk=G(4),   .band_hz={G(4.4), G(4.4)} }}, .fail_code=ADF4368_ERR_OK,         }, //TODO
    (_val){.p={.c={.D=2, .R=37, .O=2, .ref_clk=G(4.1), .band_hz={G(4.4), G(4.4)} }}, .fail_code=ADF4368_ERR_F_REF_HIGH, }, //TODO
  };

  int n_vals = sizeof(vals) / sizeof(_val);
  adf4368_err err;

  for (int i = 0; i < n_vals; i++) {
    _val val = vals[i];
    val.p.dclk_div.dclk_div = 1;
    val.p.dclk_div.dclk_mode= 1;
    state.constants = val.p.c;


    for (int ii = 0; ii < 2; ii++) {
      info.err = ADF4368_ERR_OK;
      _calc_frac_values(val.p.c.band_hz[ii], val.p.c, &val.p.f);
      str freq_param_str = adf4368_str_freq_parms(val.p);
      str internal_freqs = adf4368_str_internal_feqs(val.p);
      err = _check_freq_settings(val.p);
      if (err!=val.fail_code){
        str err_str = adf4368_str_err();
        TEST_PRINTF("\nTestcase [i=%d, ii=%d] [%s].\nerr=%s,\n  %s,\n  %s",
                i, ii, err_str.val,
                adf4368_error_msg[err],
                freq_param_str.val, internal_freqs.val);
        TEST_FAIL_MESSAGE("Incorrect Error Code");
      }
    }
#ifdef VERBOSE
#endif
  }
}

void test_timout_calcs(void)
{
  adf4360_freq_params params;
  params.c.D = 1;
  params.c.R = 1;
  params.c.O = 1;
  params.c.ref_clk = M(100); // set high to maximise chance of integer overflow
  params.dclk_div.dclk_div = 1;
  params.dclk_div.dclk_mode = 1;
  adf4368_err err = _calc_frac_values(G(12), params.c, &params.f);
  TEST_ASSERT_EQUAL(ADF4368_ERR_OK, err);
  //with the above settings f_pdf = refclk
  params.dclk_div.dclk_div = 1; params.dclk_div.dclk_mode = 0; // f_PFD/1
  TEST_ASSERT_EQUAL_MESSAGE(params.c.ref_clk, _calc_fdiv_rclk(params.c, params.dclk_div), "_calc_fdiv_rclk");
  params.dclk_div.dclk_div = 2; params.dclk_div.dclk_mode = 1; // f_PFD/4
  TEST_ASSERT_EQUAL_MESSAGE(params.c.ref_clk/4, _calc_fdiv_rclk(params.c, params.dclk_div), "_calc_fdiv_rclk");
  params.dclk_div.dclk_div = 4; params.dclk_div.dclk_mode = 1; // f_PFD//8
  TEST_ASSERT_EQUAL_MESSAGE(params.c.ref_clk/8, _calc_fdiv_rclk(params.c, params.dclk_div), "_calc_fdiv_rclk");

  params.dclk_div.dclk_div = 1; params.dclk_div.dclk_mode = 1; // f_PFD/2

  TEST_ASSERT_EQUAL_MESSAGE(K(10), _calc_synth_lock_timeout(params.c, params.dclk_div), "_calc_synth_lock_timeout");
  TEST_ASSERT_EQUAL_MESSAGE(2500, _calc_vco_alc_timeout(params.c, params.dclk_div), "_calc_vco_alc_timeout");
  TEST_ASSERT_EQUAL_MESSAGE(24, _calc_vco_band_div(params.c, params.dclk_div), "_calc_vco_band_div");
  TEST_ASSERT_EQUAL_MESSAGE(31, _calc_adc_clk_div(params.c, params.dclk_div), "_calc_adc_clk_div");
};


int debug_dump_regs(adf4368_state* s, int n_rows){
    #define _ROW_WIDTH 16
    #define _N_ROWS (25+2)
    #define _BUFF_SIZE (_ROW_WIDTH*_N_ROWS + 1) // for null termination
    char row_buff[_ROW_WIDTH+1];
    //"OFF 00 01 02 03\n"
    //"-----------------"
    //"000 DE AD BE EF\n"
    //"003 DE AD BE EF\n"
    /*Populate table header.**************************************************/
    TEST_PRINTF("---------------");
    TEST_PRINTF("OFF 00 01 02 03");
    TEST_PRINTF("---------------");
    /*Populate the table.*****************************************************/
    for (unsigned int i = 0; i<n_rows; i++){
        sprintf(row_buff, "%03u %02X %02X %02X %02X", i*4,
                s->registers[i*4],
                s->registers[i*4+1],
                s->registers[i*4+2],
                s->registers[i*4+3]);
        TEST_PRINTF("%s", row_buff);
    }
    return 0;
}

int debug_disp_param_info(adf4368_param_hdl p){
  adf4368_param param = _param_lut[p];
    TEST_PRINTF("REG[%u], Type=%u, Offset=%u, Width=%u",param.reg, param.type, param.offset, param.width);
    return 0;
};

adf4368_param_hdl test_params[] = {
    ADF4368_P_TST_1,
    ADF4368_P_TST_2,
    ADF4368_P_TST_3,
    ADF4368_P_TST_4,
    ADF4368_P_TST_5,
    ADF4368_P_TST_6_DEADBEEF,
    ADF4368_P_TST_7_DEAF,
};

void test_param_write_filled(void){
  set_regs(0xFFu);
  uint8_t filled[ADF4368_N_CACHE_REGS] = {[0 ... ADF4368_N_CACHE_REGS-1] = 0xFF};
  adf4368_err err;
  /*Test with filled table to prove correct masking behaviour.****************/
  for(int i = 0; i< sizeof(test_params)/sizeof(adf4368_param_hdl); i++){
    adf4368_param param = _param_lut[test_params[i]];
    uint64_t val_to_fill = ((uint64_t)1u << param.width) - 1;
    err = adf4368_write_param(&state, test_params[i], val_to_fill);
    TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, err, adf4368_error_msg[err]);
    if (memcmp(&regs, &filled, sizeof(filled))){
      TEST_PRINTF("TEST PARAM[%d]", i +1); // width of 1
      debug_dump_regs(&state, 5);
      TEST_FAIL_MESSAGE("Modified unexpected register.");
      }
  }
}

void test_read_param(void){
  uint8_t registers[] = {
      0xFF, 0x00, 0xFF, 0xFF, //0
      0xF0, 0xEE, 0xDB, 0xEA, //4
      0x0D, 0xF0, 0xEA, 0x0D};//8
  memcpy(&regs[ADF4368_TEST_REG_OFFSET], &registers, sizeof(registers));
  uint32_t val1;
  char buff[30];
  adf4368_read_uint_param(&state, ADF4368_P_TST_6_DEADBEEF, &val1);
  sprintf(buff, "0xDEADBEEF not found %x", val1);
  TEST_ASSERT_EQUAL_MESSAGE(0xDEADBEEF, val1, buff);
  val1 = 0; // clear
  adf4368_read_uint_param(&state, ADF4368_P_TST_7_DEAF, &val1);
  sprintf(buff, "0xDEAF not found %x", val1);
  TEST_ASSERT_EQUAL_MESSAGE(0xDEAF, val1, buff);
}

void test_read_real_param(void){
  regs[0x12u] = 0xDE;
  regs[0x13u] = 0xAD;
  regs[0x14u] = 0xBE;
  regs[0x15u] = 0x01;
  uint32_t val, val2;
  adf4368_read_uint_param(&state, ADF4368_P_FRAC1WORD, &val);
  TEST_ASSERT_EQUAL(29273566, val);
  adf4368_read_uint_param(&state, ADF4368_P_CMOS_OV, &val2);
  TEST_ASSERT_EQUAL(0, val2);
  adf4368_write_param(&state, ADF4368_P_CMOS_OV, 1);
  adf4368_read_uint_param(&state, ADF4368_P_CMOS_OV, &val2);
  TEST_ASSERT_EQUAL(1, val2);
  adf4368_read_uint_param(&state, ADF4368_P_FRAC1WORD, &val);
  TEST_ASSERT_EQUAL(29273566, val); // check this param not affected
  TEST_ASSERT_EQUAL(0x3, regs[0x15]); // check this param not affected
}

void test_back_2_back_read(void){
  uint32_t val;
  char buff[100];
  for (int i=0; i< ADF4368_N_PARAMS; i++){
    adf4368_param param = _param_lut[i];
    if(param.type==ADF4368_T_UINT){
      for (int ii=0; ii<param.width; ii++ ){
        uint32_t val2write = 1u<<ii;
        TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK,adf4368_write_param(&state, i, val2write), "Write Error");
        TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK,adf4368_read_uint_param(&state, i, &val), "Read Error");
        sprintf(buff, "i=%d, ii=%d [%s].",
                i, ii, adf4368_str_param_info(i).val);
        TEST_ASSERT_EQUAL_MESSAGE(val2write, val, buff);
      }
      //adf4368_write_uint_param(&s, i, val);
    }
  }
};

/******************************************************************************************************************************************************************************************************************************************************************************************************************/
/* Loop through all of the paramers and for each toggle all bits.
 *      This means that if any overlap then the bit's will be toggled
 *      more than once and parts of the param will be zero. This can
 *      then be checked by reading out all of the params and asserting
 *      it's value to be all 1's.                                            */
/******************************************************************************************************************************************************************************************************************************************************************************************************************/
struct param {
  int start;
  int end;
  adf4368_param_hdl hdl;
};


int comp (const void *elem1, const void *elem2){
    return (((struct param*)elem1)->start - ((struct param*)elem2)->start);
};

void test_print_all_params(void){
  for (int i = 0; i<ADF4368_N_PARAMS; i++){
    str info = adf4368_str_param_info(i);
    TEST_PRINTF("%s", info.val);
  }
}

int intersection(int start1, int end1, int start2, int end2){
  int start = MAX(start1, start2);
  int end = MIN(end1+1, end2+1);
  int intersection = end-start;

  if (intersection<0)
    return 0;
  return intersection;
}

void test_intersection(void){
  TEST_ASSERT_EQUAL(0, intersection(1, 2, 3, 4));
  TEST_ASSERT_EQUAL(1, intersection(1, 2, 2, 4));
  TEST_ASSERT_EQUAL(0, intersection(1, 1, 2, 2));
  TEST_ASSERT_EQUAL(1, intersection(1, 1, 1, 2));
  TEST_ASSERT_EQUAL(0, intersection(1, 0, 1, 2));
}

/*Make sure that parm n and param n+1 don't overlap.***************************/
void test_param_overlaps(void){
  struct param plist[ADF4368_N_PARAMS];
  char msg[100];

  for (int i=0; i < ADF4368_N_PARAMS; i++){
    plist[i].start = (_param_lut[i].reg)*8+_param_lut[i].offset;
    plist[i].end = (plist[i].start)+_param_lut[i].width - 1;
    plist[i].hdl = i;
  }
  // get all of the parameters in ascending order (by bit offset + reg*8) so
  // start and end of each ajoining param can be tested against overlap
  qsort(&plist, sizeof(plist)/sizeof(struct param), sizeof(struct param), comp);
  str info;
  for (int i = 1; i<ADF4368_N_PARAMS; i++){
    if (intersection(plist[i].start,  plist[i].end, plist[i-1].start, plist[i-1].end)>0){
      info = adf4368_str_param_info(plist[i-1].hdl);
      TEST_PRINTF("%s", info.val);
      info = adf4368_str_param_info(plist[i].hdl);
      TEST_PRINTF("%s", info.val);
        sprintf(msg, "Overlapping Param[%s] with Param[%s]",
                _param_lut[plist[i].hdl].desc,
                _param_lut[plist[i-1].hdl].desc);
        TEST_FAIL_MESSAGE(msg);
    }
  }
};

void test_toggling_writes(void){
  adf4368_err err;
  uint32_t val;
  char msg[100];
  // loop through all params and toggle them
  int i = 0;
  while (i < ADF4368_P_PARAM_END){
    uint32_t param_mask = ((uint64_t)1u << _param_lut[i].width) - 1;
    val = 0;
    err = adf4368_read_uint_param(&state, i, &val);
    sprintf(msg, "%s, %s", adf4368_error_msg[err], _param_lut[i].desc);
    if (err!=ADF4368_ERR_OK)
        raise(SIGTRAP);
    TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, err, msg);
    val = (~val) & param_mask; // toggle the value
    err = adf4368_write_param(&state, i, val);
    sprintf(msg, "%s, %s", adf4368_error_msg[err], _param_lut[i].desc);
    TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, err, msg);
    i++;
  }
  // loop through all params again and check they havn't been toggled
  // more than once...
  i = 0;
  while (i < ADF4368_P_PARAM_END){
    char msg[100];
    sprintf(msg, "Param[%s]",
            _param_lut[i].desc);
    val = 0;
    err = adf4368_read_uint_param(&state, i, &val);
    uint32_t expected = ((uint64_t)1u<<(_param_lut[i].width))-1;
    if (_param_lut[i].type != ADF4368_TYPE_NOT_SET){
      if (val!=expected)
        raise(SIGTRAP);
      TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, err, msg);
      TEST_ASSERT_EQUAL_MESSAGE(expected, val, msg);
    }
  i++;
  }
};


void test_known_values(void){
  regs[0x2b] = 0b00001001;
  regs[0x1D] = 0x33u; // bleed I
  regs[0x1E] = 0x20u; // bleed I
  regs[0x31] = 0x69u; // adc clock
  regs[0x38] = 0xA0u; // adc clock
  regs[0x39] = 0xFu; // adc clock
  uint32_t val;
  adf4368_read_uint_param(&state, ADF4368_P_PD_ALL, &val);
  TEST_ASSERT_EQUAL_MESSAGE(0, val, "ADF4368_P_PD_ALL");
  adf4368_read_uint_param(&state, ADF4368_P_PD_LD, &val);
  TEST_ASSERT_EQUAL_MESSAGE(1, val, "ADF4368_P_PD_LD");
  adf4368_read_uint_param(&state, ADF4368_P_PD_CLKOUT1, &val);
  TEST_ASSERT_EQUAL_MESSAGE(0, val, "ADF4368_P_PD_CLKOUT1");
  adf4368_read_uint_param(&state, ADF4368_P_PD_CLKOUT2, &val);
  TEST_ASSERT_EQUAL_MESSAGE(1, val, "ADF4368_P_PD_CLKOUT2");
  val = 0;
  adf4368_read_uint_param(&state, ADF4368_P_BLEED_I, &val);
  TEST_ASSERT_EQUAL_MESSAGE(51, val, "BLEED_I");
  val = 0;
  adf4368_read_uint_param(&state, ADF4368_P_EN_ADC_CLK, &val);
  TEST_ASSERT_EQUAL_MESSAGE(1, val, "EN_ADC_CLK");
  val = 0;
  adf4368_read_uint_param(&state, ADF4368_P_SYNC_DEL, &val);
  TEST_ASSERT_EQUAL_MESSAGE(3, val, "SYNC_DEL");
  val = 0;
  adf4368_read_uint_param(&state, ADF4368_P_SYNTH_LOCK_TIMEOUT, &val);
  TEST_ASSERT_EQUAL_MESSAGE(4000, val, "SYNTH_LOCK_TIMEOUT");
};

void test_write_read_freq_params()
{
  char buff[100];

  adf4360_freq_params p = {
    .c        = {.O=4,.D=2,.R=42,.ref_clk=400000},
    .dclk_div = {.dclk_div=2,.dclk_mode=1},
    .f        = {.n_int = 23,.mod2=67,.n_frac1=89,.n_frac2=105},
  };

  TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, _stage_freq_settings(&state, p), "Stage failed");

  adf4360_freq_params p2 = _get_freq_params(&state);
  TEST_ASSERT_EQUAL_MESSAGE(ADF4368_ERR_OK, info.err, "Load failed");

  int pass = memcmp(&p, &p2, sizeof(p));
  if (!pass){
    sprintf(buff, "Read %s, Written %s", adf4368_str_freq_parms(p).val, adf4368_str_freq_parms(p2).val);
    TEST_PRINTF("%s", buff);
    TEST_FAIL_MESSAGE("READ!=WRITTEN");
  }
};

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_number_of_params);
  #ifndef BREAK_ON_ERR
    RUN_TEST(test_test_err_assert);
  #endif
    RUN_TEST(test_param_n_regs);
    RUN_TEST(test_frac_value_calculation);
  #ifndef BREAK_ON_ERR
    RUN_TEST(test_freq_param_validation);
  #endif
    RUN_TEST(test_timout_calcs);
    RUN_TEST(test_param_write_filled);
    RUN_TEST(test_read_param);
    RUN_TEST(test_read_real_param);
    RUN_TEST(test_back_2_back_read);
    RUN_TEST(test_param_overlaps);
    RUN_TEST(test_toggling_writes);
    RUN_TEST(test_known_values);
    RUN_TEST(test_intersection);
    RUN_TEST(test_write_read_freq_params);
    return UNITY_END();
}
