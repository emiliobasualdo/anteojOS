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

int strcmp(char *s1, char *s2)
{
    while (*s1 && *s2)
    {
        if (*s1 != *s2)
        {
            return 0;
        }
        s1++;
        s2++;
    }
    if (*s1 == 0 && *s2!=0)
    {
        return 0;
    }
    else if (*s1!=0 && *s2==0)
    {
        return 0;
    }
    return 1;
}

int strlen(char *str)
{
    int len = 0;
    int i;

    for (i=0; str[i] != 0; i++)
    {
        len++;
    }
    return(len);
}
