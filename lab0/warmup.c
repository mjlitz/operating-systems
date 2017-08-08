/*Users kiweber and mjlitz swear to have
neither given nor recieved unathourized aid 
and hereby submit lab0*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void main() {
   //read entire user input, up to MAX_SIZE
   char *str_in   = malloc(160);
   char *str_out  = malloc(80*sizeof(char));
   int n_in = 0;
   int n_out = 0;
   while(fgets(str_in,160,stdin)) {
      n_in = 0;
    //  printf("Input:\n");
     // puts(str_in);
     // printf("\nOutput:\n");
      while (n_in < strlen(str_in)-1) {
         /*if (str_in[n_in] == '\0') {
            printf("/0");
            //return;
         }*/
         if (str_in[n_in] == '%' && str_in[n_in+1] == '%') {
            str_out[n_out] = '*';
            n_in += 2; n_out++;
         }// else if (str_in[n_in] == '\n') {
            //printf("/n");
            //str_out[n_out] = ' ';
            //n_in+=2; n_out++;
            //printf("%d",n_out);
         /*}*/ else {
           // printf("w");
            str_out[n_out] = str_in[n_in];
            n_in++; n_out++;
         }
         if (n_out == 80) {
           // printf("'80'");
            puts(str_out);
            n_out = 0;
            }
      }
      str_out[n_out] = ' ';
      n_out++;

   }
   free(str_in);
   free(str_out);
}

