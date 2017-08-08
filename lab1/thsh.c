/* COMP 530: Tar Heel SHell */
//correctly parses input to argv
//currently finds file location.
//part 1 may or may not be finished. Runs ls, bugs on pico
//exit works and adding cd. Cursors has been added
//added goheels

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT 1024
static int *finished;


//declarations of methods that must be declared early
void myexit();
void mycd(char ** args);
char * find(char * cmd);
void printarray(char ** args);
int binCmd(char ** cmd);
int set(char ** cmd);
int goheels();
int launch(char ** args, int * redir, int argc);
int echo(char ** cmd);
pid_t wait();
int open(const char *filename);
int creat(const char *path);

//declarations of external variables
int debug = 0; //1 if on, 0 if off
int timeon = 0;

int main (int argc, char ** argv, char **envp) {
  //interior variables
  finished = mmap(NULL,sizeof *finished, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *finished = 0;
  char *baseprompt = "thsh> ";
  char *prompt = malloc(1024);
  char cmd[MAX_INPUT];
  char *token;
  char *path = malloc(2048);
  setenv("PREVDIR", getenv("HOME"), 1);
  int option;


 //check if in debugging mode
  if ((option = getopt(argc,argv,"dt")) != -1) {
      printf("%d ",option);
      switch(option) {
      case 'd':
        debug = 1;
        puts("Debug mode is on.");
        break;
      case 't':
        timeon = 1;
      default:
        puts("Debug mode is off.");
      }
  }

  //main program loop
  while (!*finished) {
    char *cursor;
    char last_char;
    int rv;
    int count;

    //copies the current PWD to the variable path
    strcpy(path, getenv("PATH"));

    //sets the inital prompt to include the current PWD
    strcpy(prompt, getenv("PWD"));
    prompt[0] = '[';
    strcat(prompt, "] ");
    strcat(prompt, baseprompt);

    // Print the prompt
    rv = write(1, prompt, strlen(prompt));
    if (!rv) {
      *finished = 1;
      break;
    }

    // read and parse the input
    for(rv = 1, count = 0,
	  cursor = cmd, last_char = 1;
	rv
	  && (++count < (MAX_INPUT-1))
	  && (last_char != '\n');
	cursor++) {

      rv = read(0, cursor, 1);
      last_char = *cursor;
    }
    *cursor = '\0';

    if (!rv) {
      *finished = 1;
      break;
    }

//tokenizes the input from stdin into individual words
    char *delims = " \r\n";
    int x;
    token = strtok(cmd, delims);
    char ** args = malloc(256);
    char ** args2 = malloc(256);
    int * redir = malloc(256);
    for(x = 0; token != NULL; x++){
        args2[x] = token;
	token = strtok(NULL, delims);
    }

//handle redirection between files
    int i = 0;
    int shift = 0;
    for (; i+shift < x ; i++) {
        //j is the index in args2, i in args
        int j = i+shift;
        if (args2[j][0]=='<') {
            //<filename
            if (args2[j][1] != '\0') {
                args2[j]++;
                redir[i] = 0;
                args[i] = args2[j];
            }
            //< filename
            else {
                shift++;
                j++;
                redir[i] = 0;
                args[i] = args2[j];
            }
        }
        else if (args2[j][0]=='>') {
             //>filename
            if (args2[j][1] != '\0') {
                args2[j]++;
                redir[i] = 1;
                args[i] = args2[j];
            }
            //> filename
            else {
                shift++;
                j++;
                args[i] = args2[j];
                redir[i] = 1;
            }
        } else {
            args[i] = args2[j];
            redir[i] = -1;
        }
    }


    //handle debug and time modes
    struct timespec start, end;
    if (debug) {
        printf("RUNNING: %s",args[0]);
        int i = 1;
        for ( ; i < x ; i++)
            printf(" %s",args[i]);
        puts(" ");
    }
    if (timeon)
        clock_gettime(CLOCK_REALTIME, &start);
    int status = launch(args,redir,x);
    if (debug) {
        printf("ENDED: %s",args[0]);
        int i = 1;
        for ( ; i < x ; i++)
            printf(" %s",args[i]);
        printf(" (ret = %d)",status);
        puts(" ");
    }
    if (timeon) {
        clock_gettime(CLOCK_REALTIME, &end);
        double t = end.tv_sec - start.tv_sec;
        printf("Took %e seconds to complete.",t);
        *finished = 1;
    }
  }
  return 0;
}

//begins the parsing and running of the command
int launch(char **args, int * redir,int argc){
    char * path = malloc(2048);
    strcpy(path, getenv("PWD"));
    int status;
    char *filePath;
    char * notFound = ": Command not found.\n";
    //the child process
    if(fork() == 0){//child process
        if(binCmd(args)){
            filePath = find(args[0]);
            if(filePath != NULL) {
                int i = 0;
                int in,out;
                for (; i < argc ; i++) {
                    switch (redir[i]) {
                    //input file at index i in args
                    case 0:
                        in = open(args[i]);
                        dup2(in,0);
                        close(in);
                        break;
                    //output file at index i in args
                    case 1:
                        out = creat(args[i]);
                        dup2(out,1);
                        close(out);
                        break;
                    //pipe file at index i in args
                    case 2:
                        break;
                    }
                }
                status = execvp(filePath, args);
            } else {
                char * temp = strcat(args[0], notFound);
                write(1, temp, strlen(temp));
            }
        }

    //the parent process
    } else
	wait();
    return status;
}

//searches the path for binaries that support the current command
//and returns their array index in the list of possible paths
char * find(char *cmd){
    char * path = malloc(2048);
    strcpy(path, getenv("PATH"));
    char *delims = " :=";
    cmd[strlen(cmd)]='\0';
    char *token = strtok(path, delims);
    char * temp = malloc(2048);
    struct stat fileStat;

    while(token != NULL){
        strcpy(temp, token);
	strcat(temp, "/");
	strcat(temp, cmd);
        int x = stat(temp, &fileStat);
	if(x>-1){
	    return temp;
	}
	token = strtok(NULL, delims);
    }

    return NULL;
}

//checks commands to see if we have encoded them ourselves
int binCmd(char ** cmd){
    if(strcmp(cmd[0], "exit")==0){
        *finished = 1;
        return 0;
    }else if(strcasecmp(cmd[0], "cd")==0){
        mycd(cmd);
        return 0;
    }else if(strcasecmp(cmd[0], "echo") == 0){
        echo(cmd);
        return 0;
    }else if(strcasecmp(cmd[0], "set") ==0){
        set(cmd);
        return 0;
    }else if(strcasecmp(cmd[0], "goheels")==0){
        return goheels();
    }return 1;
}

//self-explanatory
int goheels(){
    puts("          .........,8DDDDD=.....                                                ");
    puts("          ......O8888888888DD...                                                ");
    puts("          ....,D88888888888888~.                                                ");
    puts("          ...ND8888888I?O888888+....ODDDDN7.....                                ");
    puts("          ..N88888D?=~=~~=888888.=8888888D88D,...                               ");
    puts("         ..D88888~=========8D8888DD88888888888D..                               ");
    puts("     .. ..D8888D~==========~8888888D8DI?$D888888...........                     ");
    puts("     ....~88888~===========~88888888:==~=+=D8DDDD...........                    ");
    puts("     ....D8888I============~D888D8$~~======~88888~..........                    ");
    puts("     ....88888~~===========~88888D=~~======~ZD8888..:$8$:..                     ");
    puts("      ..?8888O~============~8888D~==========~888D88888888D8D.....               ");
    puts("      ..$8888Z~~===========Z8888+~~========~~8888D8888888D8D8:...               ");
    puts("      ..I8888Z==========~=~D8888~~~=========~88888888DD888DD88...               ");
    puts("     ...:D888D~========~~~I8888D~===~======~=8888887====~888888.......          ");
    puts("      ...8888D~~========~~D88888I=~=~=======888888:=~=~=~~88888?.. ...          ");
    puts("      ...888888=~====~===88888888~==~=====~~8D888~~=~~~=~=8888DD......          ");
    puts("      ...,8888D8~~=====~D8888888DD~=~====~~D88887~=~~~~==~Z8888D~... .          ");
    puts("     ....888888D8DZ7?$D888888888888?==~=~=O88888?=~=====~=888888888D8.... .     ");
    puts("     ...N8888OD888D8888D88888888888DD=~=~ND88888O========~888888888888:....     ");
    puts("     ...O8888:=~ZD888DD888D8888888888DD88D888888D~~===~==88888D8D8D888D....     ");
    puts("     ..O88888=~~====~~=+++===+?7ODD8DD88888888888O~~=~~~78888O+~==D8888D...     ");
    puts("     ..D8888==~~======~~=~~~~==~=====OD8888888888DI=~===8DDD8~~===~88888...     ");
    puts("     ..8888D~=~~=======================~=$8D8D88888D+~$88888=~=~~=~88888=..     ");
    puts("     .+88888~=~~================~~~~~~==~=~=?88888888D8D888Z=~~~~==88888+..     ");
    puts("     .78888O==================================~888D888888887===~~==D88DDDD+. ...");
    puts("     .?8888O===================================~=7888888888D~~=~===888D888DDD,..");
    puts("     .:8888D====================================~~~ODD888888~~==~~88888888888$..");
    puts("     ..88888:========================================Z88888888O$D888D8Z?O88888..");
    puts("     ..N88887======================================~~~=888888888D888~=~~~ZD888$.");
    puts("     ..?888DD~=========================================~?888888888D~=====?88888.");
    puts("     ...D88888=========================================~=~88888888~~=~===?8888D.");
    puts("     .. :88888?=========================================~~~78D8888~======Z8888Z.");
    puts("     ....$8888D7========================================~~~==888887======D8888..");
    puts("     ... .DD8888$============================================I88888~~~~=78888D..");
    puts("        . .OD888DD~=~======================================~~=IO8888I=~Z88888,..");
    puts("          ..~DDD888D~======================================~~=~8888D8D888888?...");
    puts("          ....8888D88I~====================================~~=~~888888888DD7. ..");
    puts("          .....ZD8888D8+===================================~~===78888888D8. ....");
    puts("               ..8888888D~=~====================================~88888......    ");
    puts("               ...=ND888D88~===~=================================8D888I....     ");
    puts("                .. ..NDD88887=~==================================+8888N.. .     ");
    puts("                  .  .$8DD888D~~=================================~8888D.. .     ");
    puts("                      ..8D888D8==================================~D8888.. .     ");
    puts("                     ....~888888+~===============================~88888.. .     ");
    puts("                     ......DD8888I~==============================~D888D.. .     ");
    puts("                           .D88888~==============================~8888D... .    ");
    puts("                          ...888888~==============================8888D... .    ");
    puts("                            ..D8888O=============================Z8888O....     ");
    puts("                           . .,D888D===========================~=8D888?....     ");
    puts("                          .....D8888==========================~=~D8888.....     ");
    puts("                           . ..88888?=========================~=~88888...       ");
    puts("                           . ..Z8888$=========================~~ID888D..        ");
    puts("                            ...Z8888$======================~~=~~D88887.         ");
    puts("                            ...88888I======================~~==~D8888..         ");
    puts("                           ....D8888=======================~~=~=8888D.          ");
    puts("                          .....8888D~==========================O88888.          ");
    puts("                            ..8D8888~=========================~D8888:.          ");
    puts("                          ...=88888~~=======================~=+8888D.           ");
    puts("                          ..,88888Z=~========================~888888..          ");
    puts("                     ......$88888D=~~~=====================~~:D8888...          ");
    puts("                     .....DDD888D~~=~========================?88DDD...          ");
    puts("                     ..+DDD8888Z===========================~=D88887...          ");
    puts("          ...........888888D88=============================~~88888..            ");
    puts("          .. ....O8D8888888D==~~===========================~888888....          ");
    puts("         ....IDD888888888O==~===========================~==~88888.....          ");
    puts("          ,D88D88888888+~~==~~=============================88888D.....          ");
    puts("   . ...DD8888888DD$~=~~=====~~=======================~=~=$88888..              ");
    puts("......O888888888?==~==~=======~+======================~==~88888D..              ");
    puts("....+88888888==~~=~~~=========~88~~=~~~~~===~7========~==OD8888..               ");
    puts("...88888DD+~~====~===~~~==~~=:DD8888DD88DD8DI~========~==8888DZ..               ");
    puts("..$DD8888~~=~===~~~~====~~~~O8D888888888888D===========~D8D8DD...               ");
    puts(".:88888?=~======~~7DD8888D888888888888888DD~~~=========$88888~ ..               ");
    puts(".D8888O==~=======~==Z888D88888888888888888$~==========~88888D....               ");
    puts(".88888~==~========~==+8D888888888888888888~========~~=DD8888.....               ");
    puts(",8888D~==~============8D888888888888888888~==~=======D88888,.....               ");
    puts(":88888===~===========~78888888888888888888$=~======~?88888:......               ");
    puts(",8888D=~=~============$88888888888888888888~=======+8D888I.                     ");
    puts(".8888D~==~===========~D88888888888888888888D:~==~~~DD888N...                    ");
    puts(".D8888?==~=========~~~88888888888888D88D+~~~~===~~88888D. .                     ");
    puts(".78888D==~========~=~8DD888888888888D~~===~~=~==~88888D....                     ");
    puts(" .D88887~~========~=I?:=~===~7D8888?=======~===+88888D......                    ");
    puts("..:8D888+~======================D8I===========8D888DN.......                    ");
    puts("...D8888D==================~~~===7~=~======~~D88888D..    .                     ");
    puts(" ...D8D888O++=~~=====================~~~=~=O888888=...                          ");
    puts(".....NDD888D~===========================~+8D8888D.. ..                          ");
    puts("......78D888887~=~~~=============~~~===O8888888$......                          ");
    puts("........D88888D88D?~~=============~788D8888888...... .                          ");
    puts("     ....,D88888888888DD8Z777$O88D888888888D....                                ");
    puts("     .......7D888D8888888888888888888888D+.. ...                                ");
    puts("        .......$D888D88888888888888D8D?....... .                                ");
    puts("               ......=Z8DDDDDDDDZ=...                                           ");
    puts("               ... .... ..............");
    return 0;
}

//implements cd
void mycd(char ** args){
    char * temp = malloc(strlen(getenv("PWD")));
    char * current = malloc(strlen(getenv("PWD")) + strlen(args[1]) + 8);
    strcpy(temp, getenv("PWD"));
    if(strlen(args[1]) == 1 && args[1][0] == '~'){
        setenv("PWD", getenv("HOME"), 1);
    }else if(strlen(args[1]) == 1 && args[1][0] == '-'){
        setenv("PWD", getenv("PREVDIR"), 1);;
    }else if(strlen(args[1]) == 1 && args[1][0] == '.'){
        strcpy(temp, getenv("PREVDIR"));
    }else if(strlen(args[1]) == 2 && args[1][0] == '.' && args[1][1] == '.'){
        strcpy(current, getenv("PWD"));
        char * c = strrchr(current, '/');
        *c = 0;
        setenv("PWD", current, 1);
    }else{
        strcpy(current, getenv("PWD"));
        strcat(current, "/");
        strcat(current, args[1]);
        setenv("PWD", current, 1);
    }
    chdir(getenv("PWD"));
    setenv("PREVDIR", temp, 1);
    return;
}


//sets local variables, which are added to the environment
int set(char ** cmd){
    if(cmd[2] != NULL){
        char * badCmd = "Too many parameters.\n";
        write(1, badCmd, strlen(badCmd));
        return 0;
    }
    if(cmd[1] != NULL){
        char *delims = "=";
        char * token;
        token = strtok(cmd[1], delims);
        char * var = token;
        token = strtok(NULL, delims);
        char * val = token;
        setenv(var, val, 1);
    }
    return 0;
}


//checks if echo is a valid command and returns the proper
//result or error message
int echo(char ** cmd){
    if(cmd[2] != NULL){
        char * badCmd = "Too many parameters.\n";
        write(1, badCmd, strlen(badCmd));
        return 0;
    }
    if(cmd[1][0] != '$'){
        write(1, cmd[1], strlen(cmd[1]));printf("\r\n");
        return 0;
    }
    char * c = cmd[1];
    c++;
    char *s = getenv(c);
    if(s != NULL){
        write(1, s, strlen(s));printf("\r\n");
        return 0;
    }
    else {
        char * ch = malloc(128);
        char * badCmd = ": Undefined variable.\n";
        strcat(ch, c);
        strcat(ch, badCmd);
        write(1, ch, strlen(ch));
        return 0;
    }
}


