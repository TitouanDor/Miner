#include <stdio.h>
#include "log/log.h"

int main() {
    if (init_log() != 0) {
        printf("[ERROR] : Log initialization failed.\n");
        return 1;
    }
    
    return 0;
}