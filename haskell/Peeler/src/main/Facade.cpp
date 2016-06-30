#include <stdio.h>
#include <HsFFI.h>

#include "./OpenCV/Video.hpp"

extern "C" {
    char *start(char *filepath) {
        char *argv[] = { (char *)"libHSPeeler", NULL }, **argv_ = argv;
        int argc = 1;
        printf("Entering Haskell Runtime\n");
        hs_init(&argc, &argv_);

        loadVideo(filepath);

        printf("Leaving Haskell Runtime\n");
        hs_exit();
        return filepath;
    }
}
