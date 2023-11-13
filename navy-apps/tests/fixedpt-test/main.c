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
  assert(fixedpt_div(fixedpt_rconst(0.001), fixedpt_rconst(0.1)) == fixedpt_rconst(0.01));
  assert(fixedpt_div(fixedpt_rconst(-0.25), fixedpt_rconst(-0.5)) == fixedpt_rconst(0.5));
  assert(fixedpt_div(fixedpt_rconst(0.25), fixedpt_rconst(-0.5)) == fixedpt_rconst(-0.5));
  //assert(fixedpt_div(fixedpt_rconst(1.21), fixedpt_rconst(1.1)) == fixedpt_rconst(1.1));
  assert(fixedpt_divi(fixedpt_rconst(1.21), 2) == fixedpt_rconst(0.605));

  

  return 0;
}
