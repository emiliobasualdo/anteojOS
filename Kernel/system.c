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

int kernelIsDigit(char c)
{
    return c>='0' && c<='9' ? 1 : 0 ;
}

char * kernelToInt(char * string, int * k, int * resp)
{
    *k = 0;
    int sgn = 1, c;
    *resp = 1;
    if (!(*string == '-' || kernelIsDigit(*string)))
    {
        *resp = 0;
        return string;
    }
    if(*string == '-')
    {
        if (kernelIsDigit(*(string + 1)))
        {
            string++;
            sgn = -1;
            *k = (*string - '0') * sgn;
            string++;
        }
        else
        {
            return string;

        }
    }
    while (kernelIsDigit(c = *string))
    {
        *k = (*k) * 10 + (c-'0') * sgn;
        string++;
    }
    return string;
}