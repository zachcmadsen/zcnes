#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "tharte.h"

// TODO:
//  * Accept more than one input file?
int main(int argc, char *argv[]) {
    if (argc < 2) {
        zc_log(zc_log_error, "no input file\n");
        return EXIT_FAILURE;
    }

    char *filename = argv[1];
    if (tharte_run(filename)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
