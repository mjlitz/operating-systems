/*calls free multiple times on the same pointer to
ensure it doesn't crash*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main() {
  void * p = malloc(888);
  free(p);
  free(p);
  printf("I didn't crash\n");
  return errno;
}
