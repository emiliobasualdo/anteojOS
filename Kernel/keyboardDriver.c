#include <keyboardDriver.h>

/* The following 2 arrays were taken from an osdev tutorial */
char keyboardList[128] = {0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i',
                          'o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\', 'z','x','c','v','b',
                          'n','m',',','.','/',0, '*', 0, ' ', 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,
                          '+',0,0,0,0,0,0,0,0,0,0,0};

char keyboardShiftList[128]= {0,27,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R','T','Y','U','I',
                              'O','P','{','}','\n',0,'A','S','D','F','G','H','J','K','L',':','\"','~',0,'|', 'Z','X','C','V','B',
                              'N','M','<','>','?',0, '*', 0, ' ', 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'-',0,0,0,
                              '+',0,0,0,0,0,0,0,0,0,0,0};

int capsLock = 0;
int shift = 0;
pipe_t * myStdin;

int initKeyboardDriver()
{
    myStdin = getPipeFromPipeList(STDIN);
    return 1;
}

int keyboardInterpreter()
{
    unsigned char key = (unsigned char) getKey();
    if (key & 0x80)
    {
        if(key == 0xAA || key == 0xB6)
            shift = 0;
        return UP;
    }
    else
    {
        if (key == 58)
        {
            capsLock = !capsLock;
        }
        if(key == 0x2A || key == 0x36)
        {
            shift = 1;
        }
        char c = keyboardList[key];
        if (c>='a' && c <= 'z')
        {
            if ( (capsLock && !shift) || (!capsLock && shift) )
            {
                c = (char) (c - ('a' - 'A'));
            }
        }
        else if(shift)
        {
            c = keyboardShiftList[key];
        }
        charToBuffer((unsigned char) c);
        return DOWN;
    }
}

void charToBuffer(unsigned char c)
{
    if (c != 0)
    {
        writePipeK(myStdin, (char *) (&c), 1);
    }
}

char getNextChar()
{
    char nextChar;
    readPipeK(myStdin, &nextChar, 1);
    return nextChar;
}
