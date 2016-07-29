#include "filesystem.hpp"

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
        std::cout << "Already exists as a file: " << dir << std::endl;
        return;
    }
}
