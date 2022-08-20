#ifndef ERRORCODE_H
#define ERRORCODE_H

enum ERROR_CODE {
    NORMAL = 0,
    VERSION_MISMATCH = 1,
    CANCELED_AUTH_CODE_DIALOG = 2,
    AUTH_CODE_INVALID = 3,
    SERVER_RESTARTING = 4,
    NOT_ACCEPTABLE_CONFIG = 5
};

#endif // ERRORCODE_H
