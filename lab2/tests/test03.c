/*mallocs 0 to find return value*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  void * p = malloc(0);
  printf("0 returns %p\n", p);
  return (errno);
}




