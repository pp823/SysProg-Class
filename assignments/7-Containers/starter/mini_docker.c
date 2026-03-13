#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define STACK_SIZE (1024 * 1024)
char child_stack[STACK_SIZE];

// --- Configuration Constants ---
const char *new_hostname = "cs503-container";
const char *container_rootfs = "./nginx-rootfs";  // Must contain a working nginx install
const char *container_cmd[] = { "/usr/sbin/nginx", "-g", "daemon off;", NULL };

// --- Provided: Sets the container's hostname using sethostname() ---
void setup_hostname() {
    sethostname(new_hostname, strlen(new_hostname));
}

// --- TODO: Mount procfs inside the container's root filesystem ---
void setup_mounts() {
    // Mount proc filesystem inside the container
    if (mount("proc", "/proc", "proc", 0, "") != 0) {
        perror("mount proc failed");
        exit(1);
    }
    
    printf("Proc filesystem mounted at /proc\n");
}

// --- TODO: Use chroot() to change root filesystem to container_rootfs ---
void setup_rootfs() {
    // Change to the new root directory
    if (chdir(container_rootfs) != 0) {
        perror("chdir failed");
        exit(1);
    }
    
    // Change root filesystem
    if (chroot(".") != 0) {
        perror("chroot failed");
        exit(1);
    }
    
    printf("Root filesystem changed to %s\n", container_rootfs);
}

// --- Provided: This is the child process entrypoint ---
int child_fn(void *arg) {
    printf("Child [%d] - inside the container\n", getpid());

    setup_hostname();
    setup_rootfs();    // TODO: Implement this
    setup_mounts();    // TODO: Implement this

    printf("Launching nginx...\n");
    execvp(container_cmd[0], (char *const *)container_cmd);
    perror("execvp failed");
    return 1;
}

// --- Provided: This is the parent process ---
int main() {
    printf("Parent [%d] - launching container\n", getpid());

    int flags = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD;
    pid_t child_pid = clone(child_fn, child_stack + STACK_SIZE, flags, NULL);

    if (child_pid == -1) {
        perror("clone failed");
        exit(1);
    }

    waitpid(child_pid, NULL, 0);
    printf("Parent - container stopped\n");
    return 0;
}
