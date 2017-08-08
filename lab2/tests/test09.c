/*fills an array with malloc and frees some of them*/

/*mjlitz and kiweber have neither given nor
received unauthorized aid*/


#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  int size = 1024;
  int i = 0;
  int n = 9;
  void *addr[9];
  for ( ; i < n ; i++) {
    addr[i] = malloc(size);
    printf("Malloc %d: %p\n", i,addr[i]);
  }

  for (i = 3; i < 6; i++){
    free(addr[i]);
    printf("Freed %d: %p\n", i,addr[i]);
  }
  return (errno);
}
