#ifndef UTILS_H
#define UTILS_H

#include <QObject>

unsigned int constexpr qConstHash(const char *string)
{
    unsigned int h = 0;

    while (*string != 0)
    {
        h = (h << 4) + *string++;
        h ^= (h & 0xf0000000) >> 23;
        h &= 0x0fffffff;
    }

    return h;
}

class Utils : public QObject
{
    Q_OBJECT
public:
    static bool isBluetoothSupported();
};

#endif // UTILS_H
