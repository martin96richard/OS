#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NB_CHILDS 5

void childProcess(int signal) {
    printf("Creation of child process => PPID: %d PID: %d\n", getppid(), getpid());
    sleep(10);
    exit(EXIT_SUCCESS);
}

int main(void) {
	pid_t childs[NB_CHILDS];
	pid_t waitres;

	for (int i = 0; i < NB_CHILDS; i++) {
	    childs[i] = fork();
        if (childs[i] == 0) {
            childProcess();
        } else if (childs[i] > 0) {
            printf("Parent => PID: %d\n", getpid());
        } else {
            printf("Unable to create child process.\n");
            signal(SIGTERM, childProcess)
        }
    }

	for (int i = 0; i < NB_CHILDS; i++) {
        printf("Waiting for child process to finish.\n");
        waitres = wait(NULL);
        printf("Child process finished. Wait rtn : %d\n", waitres);
	}

	return EXIT_SUCCESS;
}
