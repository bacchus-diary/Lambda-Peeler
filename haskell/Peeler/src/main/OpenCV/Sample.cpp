#include <stdio.h>

extern "C" {
    void sampleOut(char *text) {
        printf("Stdout from C++: %s", text);
    }
}
