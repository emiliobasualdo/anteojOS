#include "stdio.h"

char * reaDLine(char * line, unsigned int size)
{
    int i=0;
    int character;
    while (i < size && ( character=getChar() )  != '\n')
    {
        if (character == '\b')
        {
            if (i < size && i != 0)
            {
                i--;
            }
        }
        else if (character != 0)
        {
            if (i < size && i < MAX_BUFFER)
            {
                line[i++] = character;
            }
        }
    }
    line[i]='\n';
    return line;
}

int scanF(char * buffer, unsigned int bufferSize, const char * fmt,...)
{
    va_list args;
    va_start(args, fmt);
    int i = 0;
    char * line = reaDLine(buffer, bufferSize);
    char * c;
    while((*fmt) != '\0')
    {
        if((*fmt) != '%')
        {
            if((*fmt) != (*line))
            {
                va_end(args);
                return i;
            }
            else
            {
                fmt++;
                line++;
            }
        }
        else
        {
            switch(*(++fmt))
            {
                case '%':
                    if(*line != '%')
                    {
                        va_end(args);
                        return 0;
                    }
                    line++;
                    break;
                case 'd':
                    line = toInt(line, va_arg(args,int *), &i);
                    if (i == 0)
                    {
                        va_end(args);
                        return i;
                    }
                    break;
                case 'c':
                    c = va_arg(args, char *);
                    *c = *line++;
                    i++;
                    break;
                case 's':
                    c = va_arg(args, char *);
                    while(*line != '\0')
                    {
                        *c++ = *line++;

                    }
                    i++;
                    break;
            }
            fmt++;
        }
    }
    va_end(args);
    return i;
}

char * toInt(char * string, int * k, int * resp)
{
    *k = 0;
    int sgn = 1, c;
    *resp = 1;
    if (!(*string == '-' || isDigit(*string)))
    {
        *resp = 0;
        return string;
    }
    if(*string == '-')
    {
        if (isDigit(*(string + 1)))
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
    while (isDigit(c = *string))
    {
        *k = (*k) * 10 + (c-'0') * sgn;
        string++;
    }
    return string;
}

int getNum()
{
    int c;
    char buffer[MAX_INT_DIGIT];
    int bufferIndex = 0;
    while((c=getChar()) != '\n')
    {
        if(isDigit(c))
        {
            buffer[bufferIndex++] = c ;
            putChar(c);
        }
        else if(c == '\b')
        {
            if (bufferIndex > 0)
            {
                removeChar();
                bufferIndex--;
            }
        }
    }
    buffer[bufferIndex] = 0;
    int num = 0;
    int flag;
    toInt(buffer,&num, &flag);
    return num;
}

void strncpy(char *to, const char * from, int n)
{
    while (n >= 0)
    {
        to[n] = from[n];
        n--;
    }
}
