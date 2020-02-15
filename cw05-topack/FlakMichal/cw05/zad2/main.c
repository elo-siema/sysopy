#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

int childpid = 0;
int receivedSignals = 0;
int sentSignals = 0;

int L = 0;
int type = 0;

void handle_sigint()
{
    //printf("\nOtrzymalem signal SIGINT\n");
    fflush(stdout);
    kill(childpid, SIGUSR2);
    exit(0);
}

void handle_sigusr1_parent()
{
    //printf("\nOtrzymalem signal SIGUSR1 - rodzic\n");
    receivedSignals++;
}

void handle_sigusr1_child()
{
    //printf("\nOtrzymalem signal SIGUSR1 - dziecko\n");
    receivedSignals++;
    kill(getppid(), SIGUSR1);
}

void handle_sigusr2()
{ 
    //printf("\nOtrzymalem signal SIGUSR2 - dziecko\n");
    printf("Odebrane sygnały przez potomka: %d\n", receivedSignals);
    fflush(stdout);
    exit(0);
}


void launch_child()
{
    childpid = fork();
    if (!childpid)
    {
        //block all signals:
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        //set handlers:
        signal(SIGUSR1, handle_sigusr1_child);
        signal(SIGUSR2, handle_sigusr2);

        //do nothing
        for (;;)
        {
            pause();
        }
    }
}

void send_to_child()
{
    switch(type) {
        case 1:
            for(int i = 0; i<L; i++) {
                kill(childpid, SIGUSR1);
                sentSignals++;
            }
            break;
        case 2:
            for(int i = 0; i<L; i++) {
                kill(childpid, SIGUSR1);
                pause();
                sentSignals++;
            }
            break;
        case 3:
            kill(childpid, SIGCONT);
            kill(childpid, SIGPROF);
            sentSignals += 2;
            break;
    }
    kill(childpid, SIGUSR2);
}

int main(int argc, char **argv)
{
    //handle args
    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-L") && i+1 <= argc-1) {
            L = atoi(argv[i+1]);
        }
        else if(!strcmp(argv[i], "--Type") && i+1 <= argc-1) {
            type = atoi(argv[i+1]);
        }
    }
    //printf("L: %d\n", L);
    //printf("T: %d\n", type);
    if(L <= 0) {
        printf("-L missing or invalid. Must be greater than 0.");
        exit(0);
    }
    if(type <= 0 || type > 3) {
        printf("--Type missing or invalid. Must be 1, 2 or 3.");
        exit(0);
    }
    //end handling args

    launch_child();
    sleep(1); //give child time to initialize, must be ready before first signal is sent

    signal(SIGINT, handle_sigint);
    signal(SIGUSR1, handle_sigusr1_parent);

    send_to_child();

    wait(NULL);
    printf("Wysłane sygnały: %d\n", sentSignals);
    printf("Odebrane sygnały od potomka: %d\n", receivedSignals);

    return 0;
}