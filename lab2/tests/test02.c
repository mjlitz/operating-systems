/*makes sure malloc can't be run on objects over the cap*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main() {
  char *p = malloc (2049);
  printf("Points to :%p\n", p);
  return 2;
}
