#include <shell.h>

static int screenSaverStatus = FALSE;
static unsigned int saverTime = DEFAULT_SAVER_TIME;
static unsigned int inactivityCounter = 0;
int secs = 0;

char buffer[MAX_BUFFER_SIZE];
int bufferPtr;

char cmdsBuffer[MAX_HISTORY][MAX_BUFFER_SIZE];
int cmdBufferLast;
int viewIndex;

void putLastBuffer(int dir);

void shell()
{
    int run = TRUE;
    int resp = NULL_CMMD;
    int c;
    cmdBufferLast = viewIndex = 0;
    //turnOnOff();
    while(run)
    {
        c = 0;
        RESET_BUFFER;
        printShellLine();
        while(c != '\n')
        {
            c = getChar();
            if (isGraph((char) c))
            {
                buffer[bufferPtr++] = (char) c;  // bufferPtr siempre apunta a donde agregar
                putChar((char) c);
            }
            else if (c == '\b' && bufferPtr > 0)  // no tendria sentido seguir borrando
            {
                removeChar();
                bufferPtr--;
            }
            else if(c == '\n')
            {
                if (bufferPtr > 0)    //sino solamente imprimo una linea nueva pero no mando el comando
                {
                    buffer[bufferPtr] = (char) c; // para saber hasta donde leer
                    addToCmdHistory(buffer); // lo agregamos con \n;
                    NEW_LINE;
                    resp = parseAndInterpret(buffer);
                }
                else
                {
                    putChar((char) c);
                }
            }
            else if(isCmd(c))
            {
                int dir = c == C_UP ? -1 : 1;
                putLastBuffer(dir);
            }

        }
        if (resp == EXIT_CMMD)
        {
            run = FALSE;
        }
        else if(resp == NULL_CMMD)
        {
            printF("%s\n", NO_SUCH_CMMD_MSG);
        }
        else if (resp == ILLEGAL_INPUT)
        {
            printF("%s\n",ILLEGAL_INPUT_MSG);
        }
    }
    doBeforeExit();
}

void putLastBuffer(int dir)
{
    if ((viewIndex + dir) >= 0 && (viewIndex + dir) <= cmdBufferLast)
    {
        int i = 0;
        viewIndex = viewIndex + dir;

        while(bufferPtr > 0) // limpiamos el current
        {
            removeChar();
            bufferPtr--;
        }

        while(cmdsBuffer[viewIndex][i] != '\n')
        {
            buffer[bufferPtr++] = cmdsBuffer[viewIndex][i];
            putChar(cmdsBuffer[viewIndex][i++]);
        }
        buffer[bufferPtr] = '\n'; // pero no lo imprimimos porque todavía no confirmamos la acción
    }
}

void turnOnOff()
{
    int x, y;
    kernelBeep();
    newWindow();
    changeFontColour(getCurrentFontColour());
    changeBackgroundColour(getCurrentBackgroundColour());
    setPresentationImageCoordinates(&x, &y, GLASSESWIDTH, GLASSESWIDTH);
    drawImageFromHexaMap((unsigned int) x, (unsigned int) y, eyeGlassesSmall, GLASSESWIDTH, GLASSESHEIGHT);
    sleep();
    sleep();
    sleep();
    newWindow();
}

void newShell()
{
    newWindow();
}

void printShellLine()
{
    printF("%s",OS_SHELL_LINE);
}

void doBeforeExit()
{
    turnOnOff();
    notifyExitRequest();
}

int parseAndInterpret(const char *string) // se lee desde indice 0 hasta un \n
{
    int spaceFlag = 0;
    int state = INITIAL;
    int argIndex = 0;
    int letterIndex = 0;
    argVector argsVector; // vector de punteros a string
    argsVector[0][0] = 0;
    char * c = (char *) string;

    while (*c != '\n') //los strings se pasan enteros enteros ""
    {
        if (!isGraph(*c) && *c != 0)// no puede ser un nombre, tiene que ser distinto de 0 porque reseteamos el primer indice del buffer
        {
            return NULL_CMMD;
        }
        if ((argIndex+1)*(letterIndex+1) > MAX_BUFFER_SIZE)
        {
            return BUFFER_OVERFLOW;
        }
        switch (state)
        {
            case INITIAL:
                if (isSpace(*c))
                {
                    state = INITIAL;
                    spaceFlag = 1;
                }
                else
                {
                    letterIndex = 0;
                    argsVector[argIndex][letterIndex++] = *c;
                    state = IN_ARGUMENT;
                    spaceFlag = 0;
                }
                break;
            case SPACE:
                if(!isSpace(*c))      // si es espacio nos quedamos aca
                {
                    if (spaceFlag) {
                      argIndex++;
                    }
                    if (isQuote(*c))    //string, agrego " porque tal vez echo toma otros parametros
                    {
                        state = IN_STRING;
                    }
                    else
                    {
                        state = IN_ARGUMENT;
                    }
                    letterIndex = 0;         //empieza un argumento
                    argsVector[argIndex][letterIndex++] = *c;
                }
                break;
            case IN_ARGUMENT:
                if (isSpace(*c))        // tabs o espacioss etc
                {
                    argsVector[argIndex][letterIndex++] = 0;
                    spaceFlag++;
                    state = SPACE;
                }
                else
                {
                    argsVector[argIndex][letterIndex++] = *c;
                }
                break;
            case IN_STRING:
                if (isQuote(*c))
                {
                    state = SPACE;
                }
                argsVector[argIndex][letterIndex++] = *c;
            default:break;
        }

        argsVector[argIndex][letterIndex] = 0;
        c++;
    }
    if(argsVector[0][0] == 0){
        return OK;
    }
    return executeCommand(argIndex+1, argsVector);
}

void setPresentationImageCoordinates(int *x, int *y, int width, int height)
{
    unsigned int xRes, yRes;
    getResolutions(&xRes,&yRes);
    *x = xRes/2 - width/2;
    *y = yRes/2 - height/2;
}

int getSaverStatus()
{
    return screenSaverStatus;
}

unsigned int getSaverTime()
{
    return saverTime;
}

void setSaverStatus(int flag)
{
    screenSaverStatus = flag;
}

void refreshInactivityCounter()
{
    int s = getSecond();
    if(getSecond()!= secs)
    {
        inactivityCounter++;
        secs = s;
    }
}

void setSaverTime(int num)
{
    saverTime = num;
}

void addToCmdHistory(char *cmd)
{
    strncpy(cmdsBuffer[cmdBufferLast], cmd, MAX_BUFFER_SIZE);
    cmdBufferLast = (cmdBufferLast+1)% MAX_BUFFER_SIZE;
    cmdsBuffer[cmdBufferLast][0] = '\n'; // para no levantar basura en el historial
    viewIndex = cmdBufferLast;
}
