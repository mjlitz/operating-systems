/*mallocs increasingly large numbers and frees them in reverse order*/

/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  int maxsize = 2048;
  int i, n, size[63];
  void * addr[63];

  for (i = 0, n = 0; n < maxsize ; i++, n += 1 * i) {
    size[i] = n;
    addr[i] = malloc(n);
    printf("Malloc at least %d bytes at: %p\n", size[i],addr[i]);
  }

  for (i = 63; i > 0; i--){
    free(addr[i]);
    printf("Freed at least %d bytes at: %p\n", size[i], addr[i]);
  }

  return (errno);
}


