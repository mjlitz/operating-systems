/*mallocs several different values, one per size block*/
/*mjlitz and kiweber have neither given nor
received unauthorized aid*/
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int main() {
  void *p;
  int value = 17, i = 0;

  for (; i < 7; i++, value *= 2){
    p = malloc(value);
    printf("At least %d bytes were mapped starting at %p\n", value, p);
  }
  return (errno);
}




