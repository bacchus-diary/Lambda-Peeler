#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

using namespace std;

void mkParents(char *path, mode_t mode) {
    char dm[sizeof(path)];
    strcpy(dm, path);
    char *dir = dirname(dm);

    struct stat info;
    if (stat(dir, &info) != 0) {
        mkParents(dir, mode);
        mkdir(dir, mode);
        return;
    } else if (S_ISDIR(info.st_mode)) {
        return;
    } else {
        cout << "Already exists as a file: " << dir << endl;
        return;
    }
}
