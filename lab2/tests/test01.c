/*makes sure malloc runs without crashing*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

int main() {
  void * p = malloc(12);
  printf("Points to : %p\n", p);
  return errno;
}
