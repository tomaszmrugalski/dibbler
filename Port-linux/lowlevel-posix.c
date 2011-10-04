/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "Portable.h"
#include <unistd.h>
#include <sys/wait.h>

int execute(const char *filename, char * argv[], char *env[])
{

#ifdef LOWLEVEL_DEBUG
    char ** x = argv;
    printf("Execute script: %s\n", filename);
    while (*x) {
        printf("Param: %s\n", *x);
        ++x;
    }

    x = env;
    while (*x) {
        printf("env: %s\n", *x);
        ++x;
    }
#endif

    pid_t pid;
    int status = 0;

    pid = fork();
    if (!pid) {
        execve(filename, argv, env);
    } else {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        else
            return LOWLEVEL_ERROR_UNSPEC;
    }

    return LOWLEVEL_NO_ERROR;
}
