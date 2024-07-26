#include <stdio.h>

#include <unistd.h>

#include "log.h"

#include <sys/reboot.h>

int main(int argc, char* argv[]) {
    if(getpid() != 1) {
        printf("%s must be run as PID 1\n", argv[0]);
        return -1;
    }

    log_debug("TEST", "debug");
    log_info("TEST", "info");
    log_warn("TEST", "warn");
    log_error("TEST", "error");

    reboot(RB_HALT_SYSTEM);
}