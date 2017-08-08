/*test to make sure mallocs and frees are poisoned properly*/
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main() {
  char *p = malloc (1245);
  write(1, p, 12);
  free(p);
  write(1, p, 12);
  return 0;
}
