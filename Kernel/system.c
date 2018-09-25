//
// Created by Emilio Basualdo on 9/4/18.
//

#include <system.h>
/**https://stackoverflow.com/questions/14159625/implementation-of-strncpy*/
char *myStrncpy(char *dst, const char*src, unsigned int n)
{
    char *temp = dst;
    while (n-- && (*dst++ = *src++))
        ;
    return temp;
}
