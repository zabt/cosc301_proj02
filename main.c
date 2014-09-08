#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>


void process_line(char *input_line) {
    // your code should start here;  the line below
    // is just to help you see the exact text that gets typed in from the
    // loop in main().  
    printf("You typed <%s>\n", input_line);
}


#ifndef AUTOTEST
int main(int argc, char **argv)
{
    char buffer[1024];
    for (;;) { // infinite loop
        memset(buffer, 0, 1024);
        printf(">> "); // the shell prompt
        fflush(stdout);
        if (fgets(buffer, 1024, stdin) == NULL){
            break;
        }
        process_line(buffer);
    } 

    return 0;
}
#endif // AUTOTEST

