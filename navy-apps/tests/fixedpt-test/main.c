#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <fixedptc.h>

int main() {

  assert(fixedpt_mul(fixedpt_rconst(1.1), fixedpt_rconst(1.1)) == fixedpt_rconst(1.20));

  

  return 0;
}
