/*mallocs a seriously of increasingly large numbers*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  int size = 2048;
  int i, n;
  void * addr[63];
  for (i = 0, n = 0; n < size ; i++, n += 1 * i) {
    addr[i] = malloc(n);
    printf("Malloc %d: %p\n", i,addr[i]);
  }
  return (errno);
}
