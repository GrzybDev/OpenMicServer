#ifndef EXITCODE_H
#define EXITCODE_H

enum EXIT_CODE {
    NORMAL = 0,
    VERSION_MISMATCH = 1,
    CANCELED_AUTH_CODE_DIALOG = 2,
    AUTH_CODE_INVALID = 3,
    SERVER_RESTARTING = 4
};

#endif // EXITCODE_H
