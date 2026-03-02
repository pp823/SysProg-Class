#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Increase tries so it doesn't exit too early
static volatile int attempt_counter = 100000; 

static void my_wise_guy_signal_handler(int sig_num){
    // DANGER: Massive printf in the handler
    if (sig_num == SIGINT){
        printf("--- SIGNAL HANDLER INTERRUPTION #%d ---\n", attempt_counter--);
        if (attempt_counter <= 0) exit(0);
    }
}

int main(){
    signal(SIGINT, my_wise_guy_signal_handler);

    while(1){
        // DANGER: Constant stream of printf in main
        printf("[MAIN] DATA_STREAM_BLOCK_CORRUPTION_TEST_DATA_STREAM_BLOCK_CORRUPTION_TEST\n");
    }
}
