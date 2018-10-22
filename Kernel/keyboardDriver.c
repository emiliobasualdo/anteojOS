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
int multiMedia = 0;

int buffer[BUFFERSIZE] = {0};
int bufferWrite = 0;
int bufferRead = 0;
int size = 0;

int keyboardInterpreter()
{
    int key = (unsigned char) getKey();
    if(key == 0xE0) //multimedia
    {
        multiMedia = (multiMedia+1 ) % 2;
    }
    if (key & 0x80)
    {
        if(key == 0xAA || key == 0xB6)
            shift = 0;
        return UP;
    }
    else // DOWN
    {
        if(multiMedia)//si la me avisaron que se viene un multimedia
        {
            switch (key)
            {
                // cursor up
                case 0x48:
                    charToBuffer(C_UP);
                    break;
                    // cursor dowb
                case 0x50:
                    charToBuffer(C_DOWN);
                    break;
                default:break;
            }

        }
        if (key == 58)
        {
            capsLock = !capsLock;
        }
        if(key == 0x2A || key == 0x36)
        {
            shift = 1;
        }
        int c = keyboardList[key];
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
        charToBuffer(c);
        return DOWN;
    }
}

void charToBuffer(int c)
{
    if (c != 0)
    {
        buffer[bufferWrite] = c;
        bufferWrite++;
        bufferWrite = bufferWrite % BUFFERSIZE;
        size++;
    }
}

int returnNextChar()
{
    int resp = 0;
    if(size == 0)
    {
        return resp;
    }
    resp = buffer[bufferRead++];
    bufferRead = bufferRead % BUFFERSIZE;
    size--;
    return resp;
}

int newToRead()
{
    if (size == 0 || bufferRead == bufferWrite)
    {
        return 0;
    }
    return 1;
}

int getNextChar()
{
    while (!newToRead())
        setProcessState(getCurrentProc()->pid, BLOCKED, KEYBOARD);
    return returnNextChar();
}
