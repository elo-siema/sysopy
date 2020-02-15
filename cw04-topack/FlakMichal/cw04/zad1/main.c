#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int childpid = 0;
int sigstp_toggle = 0;

void handle_sigint()
{
    printf("\nOtrzymalem signal SIGINT\n");
    fflush(stdout);
    kill(childpid, SIGINT);
    exit(0);
}

void handle_sigtstp()
{
    sigstp_toggle = !sigstp_toggle;
    if (sigstp_toggle)
    {
        printf("\nOczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
        fflush(stdout);
        kill(childpid, SIGINT);
    }
    else
    {
        launch_child();
    }
}

void launch_child()
{
    childpid = fork();
    if (!childpid)
    {
        execl("./date.sh", NULL);
    }
}

int main(int argc, char **argv)
{
    launch_child();

    /*begin set sigint handling*/
    struct sigaction act;
    act.sa_handler = handle_sigint;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    /*end set sigint handling*/

    /*begin set sigtstp handling*/
    signal(SIGTSTP, handle_sigtstp);
    /*end set sigtstp handling*/

    for (;;)
    {
        pause();
    }

    return 0;
}