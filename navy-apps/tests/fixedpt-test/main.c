#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <fixedptc.h>

int main() {

  assert(fixedpt_mul(fixedpt_rconst(1.1), fixedpt_rconst(1.1)) == fixedpt_rconst(1.21));
  assert(fixedpt_mul(fixedpt_rconst(-1.1), fixedpt_rconst(-1.1)) == fixedpt_rconst(1.21));
  assert(fixedpt_mul(fixedpt_rconst(-0.5), fixedpt_rconst(0.5)) == fixedpt_rconst(-0.25));
  assert(fixedpt_muli(fixedpt_rconst(1.21), 2) == fixedpt_rconst(2.42));

  assert(fixedpt_div(fixedpt_rconst(0.25), fixedpt_rconst(0.5)) == fixedpt_rconst(0.5));
  assert(fixedpt_div(fixedpt_rconst(0.0625), fixedpt_rconst(-0.25)) == fixedpt_rconst(-0.25));
  assert(fixedpt_div(fixedpt_rconst(-0.25), fixedpt_rconst(-0.5)) == fixedpt_rconst(0.5));
  assert(fixedpt_div(fixedpt_rconst(0.25), fixedpt_rconst(-0.5)) == fixedpt_rconst(-0.5));
  //assert(fixedpt_div(fixedpt_rconst(1.21), fixedpt_rconst(1.1)) == fixedpt_rconst(1.1));
  assert(fixedpt_divi(fixedpt_rconst(1.21), 2) == fixedpt_rconst(0.605));

  assert(fixedpt_floor(fixedpt_rconst(0)) == fixedpt_rconst(0)); 
  assert(fixedpt_floor(fixedpt_rconst(-0.5)) == fixedpt_rconst(-1)); 
  assert(fixedpt_floor(fixedpt_rconst(-127.625)) == fixedpt_rconst(-128)); 
  assert(fixedpt_floor(fixedpt_rconst(0.5)) == fixedpt_rconst(0)); 
  assert(fixedpt_floor(fixedpt_rconst(128.625)) == fixedpt_rconst(128)); 
  assert(fixedpt_floor(fixedpt_rconst(-128)) == fixedpt_rconst(-128)); 
  assert(fixedpt_floor(fixedpt_rconst(128)) == fixedpt_rconst(128)); 
  
  assert(fixedpt_ceil(fixedpt_rconst(0)) == fixedpt_rconst(0)); 
  assert(fixedpt_ceil(fixedpt_rconst(-0.5)) == fixedpt_rconst(0)); 
  assert(fixedpt_ceil(fixedpt_rconst(-128.625)) == fixedpt_rconst(-128)); 
  assert(fixedpt_ceil(fixedpt_rconst(0.5)) == fixedpt_rconst(1)); 
  assert(fixedpt_ceil(fixedpt_rconst(127.625)) == fixedpt_rconst(128)); 
  assert(fixedpt_ceil(fixedpt_rconst(-128)) == fixedpt_rconst(-128)); 
  assert(fixedpt_ceil(fixedpt_rconst(128)) == fixedpt_rconst(128)); 

  return 0;
}
