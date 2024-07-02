#ifndef _UTILS_H
#define _UTILS_H
#include "adf4368.h"
#include "adf4368_priv.h"
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include "unity.h"

#define N_LOG_ELEMS 200

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define CUSTOM_ASSERT(_err, _state) \
  if ((_err) != ADF4368_ERR_OK) {\
    TEST_PRINTF("%s", adf4368_str_freq_parms(_get_freq_params(&(_state))).val);\
    TEST_FAIL_MESSAGE(adf4368_str_err().val);\
  }\

uint8_t regs[ADF4368_N_CACHE_REGS];
adf4368_state state;

char msgbuff[200];

enum event_t {
  NO_EVENT = 0,
  WRITE_EVENT,
  READ_EVENT,
};

void set_regs(uint8_t val) { memset(&regs, val, sizeof(regs)); };

struct event_log {
  enum event_t type;
  uint8_t reg_val;
  uint8_t reg_addr;
};

struct event_log _log[N_LOG_ELEMS] = {0};
int event_count = 0;

void clear_event_log(){
  for (int i = 0; i<N_LOG_ELEMS; i++){
    _log[i].reg_addr = 0;
    _log[i].reg_val = 0;
    _log[i].type = NO_EVENT;
  }
};

adf4368_err dummy_spi_write(uint8_t reg, uint8_t val) {
  regs[reg] = val;
  _log[event_count].type = WRITE_EVENT;
  _log[event_count].reg_val = val;
  _log[event_count].reg_addr = reg;
  event_count++;
  return ADF4368_ERR_OK;
};

adf4368_err dummy_spi_read(uint8_t reg, uint8_t *val) {
  *val = regs[reg];
  _log[event_count].type = READ_EVENT;
  _log[event_count].reg_val = *val;
  _log[event_count].reg_addr = reg;
  event_count++;
  return ADF4368_ERR_OK;
};

void write_log(const char *filename) {

  const char write_string[] = "W, %2u, %2X, %2X\n";
  const char read_string[] = "R, %2u, %2X, %2X\n";
  const char *fmt;

  FILE *fpt;
  fpt = fopen(filename, "w+");

  sprintf(msgbuff, "R/W, addr, val\n");
  fprintf(fpt, "R/W, dec, hex, val\n");
  for (int i = 0; i < sizeof(_log) / sizeof(struct event_log); i++) {
    if (_log[i].type != NO_EVENT) {
      if (_log[i].type == WRITE_EVENT) {
        fmt = write_string;
      } else if (_log[i].type == READ_EVENT) {
        fmt = read_string;
      } else {
        TEST_FAIL_MESSAGE("unexpected condition");
      }
      sprintf(msgbuff, fmt, _log[i].reg_addr, _log[i].reg_addr, _log[i].reg_val);
      fprintf(fpt, fmt, _log[i].reg_addr, _log[i].reg_addr, _log[i].reg_val);
      // TEST_PRINTF("%s", buff);
    }
  }
  fclose(fpt);
};

int comp (const void *elem1, const void *elem2){
  adf4368_param param1 = _param_lut[*((int*)elem1)];
  adf4368_param param2 = _param_lut[*((int*)elem2)];
    return (param1.reg*8+param1.offset) - (param2.reg*8+param2.offset);
};

int* sort_params(){
  static int params_sorted[ADF4368_N_PARAMS];
  for (int i = 0; i<ADF4368_N_PARAMS; i++){
       params_sorted[i] = i;
  }
  qsort(params_sorted, ADF4368_N_PARAMS, sizeof(int), comp);
  return &params_sorted[0];
};

void param_dump(const char* filename){
  FILE *fpt;
  fpt = fopen(filename, "w+");
  uint32_t val;
  int* param_ptr = sort_params();
  for (int i = 0; i<ADF4368_N_PARAMS; i++){
    adf4368_param param = _param_lut[param_ptr[i]];
    if(param.type != ADF4368_TYPE_NOT_SET){
      _get_staged_param(&regs, param_ptr[i], &val);
      fprintf(fpt, "%3d, %22s, %10lu\n", i, param.desc, 
              (unsigned long)val);
    }
  }
  fclose(fpt);
};

#endif
