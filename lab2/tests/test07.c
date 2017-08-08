/*calls malloc and free multiple times, and
into a new superblock*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  int size = 512;
  int i = 0;
  void *x[16];

  for(; i < 16 ; i++){
    x[i] = malloc(size);
    printf("Malloc %d: %p\n", i,x[i]);
  }

  for (i = 0 ; i < 16 ; i++){
    free(x[i]);
    printf("Freed %p\n",x[i]);
  }
  return 0;
}
