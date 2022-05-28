#ifndef UTILS_H
#define UTILS_H

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


#endif // UTILS_H
