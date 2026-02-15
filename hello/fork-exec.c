// fork-exec.c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

static void child2_exec(void) {
    printf("Hello from Child #2 (pid %d, ppid %d) - about to exec ./hi\n",
           getpid(), getppid());

    execl("./hi", "hi", (char *)NULL);

    // Only runs if exec fails
    perror("execl ./hi failed");
    _exit(1);
}

static void child1_create_child2(void) {
    printf("Hello from Child #1 (pid %d, ppid %d)\n", getpid(), getppid());

    pid_t c2 = fork();
    if (c2 < 0) {
        perror("fork for Child #2 failed");
        _exit(1);
    }

    if (c2 == 0) {
        // Child #2
        child2_exec();
        // never returns if exec succeeds
    } else {
        // Child #1
        int status2 = 0;
        printf("Child #1 (pid %d) created Child #2 (pid %d) and is waiting...\n",
               getpid(), c2);

        waitpid(c2, &status2, 0);

        printf("Child #1 (pid %d) done waiting for Child #2 (pid %d)\n",
               getpid(), c2);

        if (WIFEXITED(status2)) {
            printf("Child #2 exited with status %d\n", WEXITSTATUS(status2));
        } else if (WIFSIGNALED(status2)) {
            printf("Child #2 terminated by signal %d\n", WTERMSIG(status2));
        }

        printf("Child #1 (pid %d) exiting now.\n", getpid());
        _exit(0);
    }
}

static void parent_wait(pid_t c1) {
    int status1 = 0;

    printf("Hello from Parent (pid %d). Created Child #1 (pid %d). Waiting...\n",
           getpid(), c1);

    waitpid(c1, &status1, 0);

    printf("Parent (pid %d) done waiting for Child #1 (pid %d)\n", getpid(), c1);

    if (WIFEXITED(status1)) {
        printf("Child #1 exited with status %d\n", WEXITSTATUS(status1));
    } else if (WIFSIGNALED(status1)) {
        printf("Child #1 terminated by signal %d\n", WTERMSIG(status1));
    }
}

int main(void) {
    pid_t c1 = fork();
    if (c1 < 0) {
        perror("fork for Child #1 failed");
        return 1;
    }

    if (c1 == 0) {
        // Child #1
        child1_create_child2();
        // child1 exits inside the function
    } else {
        // Parent
        parent_wait(c1);
    }

    return 0;
}
