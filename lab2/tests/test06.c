/*makes sure malloc and free run without crashing*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

int main() {
  void * p = malloc(35);
  free(p);
  return errno;
}
