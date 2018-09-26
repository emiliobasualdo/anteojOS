#include "shellCommands.h"

command commands[]={
        {"help",  "Shows the different commands available and their description.", help},
        {"echo",  "Prints on stdout the specified string/s. Strings without quotes are considered separated", echo},
        {"time",  "Prints the current system time with default Timezone. Timezone can be changed with 'timezone' command", time},
        {"clear", "Clears the screen.", clear},
        {"beep",  "Requests kernel to emit beep from motherboard.", beep},
        {"exit", "Exits the terminal.", exitShell},
        {"font_colour", "Changes the font colour.", font_colour},
        {"background_colour", "Changes the background colour.", background_colour},
        //{"digital_clock","Displays a digital clock on screen", digital_clock},
        {"timezone", "Allows the user to change the current timezone. Usage: timezone [int]",timezone},
        //{"screen_saver", "Allows user to change screen savers parameters. Input on/off to turn on/off, or a positive integer to change waiting time.", screen_saver},
        {"exceptionTester", "This command calls an exception,0 for zero division, 1 for Invalid Opcode", exceptionTester},
        {"ps", "Prints all process. Usage ps or ps <q> or ps <p pid>", printPs},
        {"proc_bomb", "Starts a process bomb", procBomb},
        {"back_test","Performs a test to prove the background functionality", backgroundTest},
        {"multi_test","Performs a test to prove the multi-processing functionality",multiProcTest},
        {"kill","Kill process. Usage: kill <pid> or kill <pidFrom pidTo>", kill},
        {NULL, "ESTO NO LO SACAMOS DALE?", NULL} // NOOO SE SACA
};

command aux_programs[]={
        {"endLessLoop",  "while(1)", endLessLoop},
        {NULL, "ESTO NO LO SACAMOS DALE?", NULL} // NOOO SE SACA
};

/** Funciones auxiliares*/
int executeCommand(int argc, argVector argv)
{
    int cmd = commandExists(argv[0]);
    if (cmd == NULL_CMMD)
    {
        return NULL_CMMD;
    }
    if (argv[argc-1][0] == '&')
    {
        execProcInBackground(commands[cmd].name, (uint64_t) commands[cmd].fn);
        return AMPRESAND_CMD;
    }
    return (*commands[cmd].fn)(argc,argv);
}

int commandExists(const char *name)
{
    for (int i=0; commands[i].name; i++)
    {
        if (strcmp((char *) name, commands[i].name) != 0)
        {
            return i;
        }
    }
    return NULL_CMMD;
}

int execProcInBackground(char *name, uint64_t intstruction)
{
    int pid = userStartProcess(name, intstruction, FALSE);
    if (pid == -1)
        printF("Error executing process in background\n");
    else
        printF("Process executed in background. Pid = %d\n", pid);
    return pid;
}

/** A partir de aca van los comandos para el usuario*/
int help (int argc, argVector argv)
{
    for (int i=0; commands[i].name; i++)
    {
        printF("%s: %s\n",commands[i].name ,commands[i].description);
    }
    return 1;
}

int echo (int argc, argVector argv)
{
    if (argc == 1)
    {
        return 0;
    }
    for (int i=1; i<argc; i++)
    {
        if (isQuote(argv[i][0]))
        {
            char prev = argv[i][1];
            int j=2;
            while (argv[i][j])
            {
                printF("%c", prev);
                prev = argv[i][j++];
            }
            if(!isQuote(prev))
            {
                printF("%c", prev);
            }
            NEW_LINE;
        }
        else
        {
            printF("%s\n", argv[i]);
        }
    }
    return 1;
}

int time (int argc, argVector argv)
{
    printF("Current time: %d:%d:%d\n", getTimezoneHour(), getMinute(), getSecond());
    return 1;
}

int clear (int argc, argVector argv)
{
    newShell();
    return 1;
}

int beep (int argc, argVector argv)
{
    kernelBeep();
    return 1;
}

int exitShell (int argc, argVector argv)
{
    return EXIT_CMMD;
}

int font_colour(int argc, argVector argv)
{
    if (argc > 1)
    {
        printF("%s\n", CERO_ARGUMENTS_ERROR);
        return 0;
    }
    changeColour(changeFontColour, FONT);
    return 1;
}

int background_colour(int argc, argVector argv)
{
    if (argc > 1)
    {
        printF("%s\n", CERO_ARGUMENTS_ERROR);
        return 0;
    }
    int changed = changeColour(changeBackgroundColour, BACK);
    if (changed)
    {
        clear(argc,argv);
    }
    return 1;
}

int changeColour(void(*f)(Colour), int flag)
{
    Colour original = getCurrentFontColour();
    printF("%s\n", SET_FONT_MSSG);
    for (int i = 1; i <= COLOURS_AMOUNT; ++i)
    {
        changeFontColour(userColours[i]);
        printF("%d) %s \n", i, SET_FONT_EX );
    }
    changeFontColour(original);
    char c;
    int ask = TRUE;
    int changed = FALSE;
    Colour c1 = getCurrentFontColour();
    Colour c2 = getCurrentBackgroundColour();
    while (ask)
    {
        c = getChar();
        if (c >= '1' && c <= '9')
        {
            Colour col = userColours[c-'0'];
            if ((equalColour(col, c1) && flag == BACK)|| (equalColour(col, c2) && flag == FONT))
            {
              printF("Choose another colour, one that makes everything better! \n");
            }
            else
            {
              (*f)(col);
              ask = FALSE;
              changed = TRUE;
            }
        }
        else if(c == 'q')
        {
            ask = FALSE;
        }
    }
    return changed;
}

int digital_clock(int argc, argVector argv)
{
    showClock(NORMAL_MODE);
    clear(argc,argv);
    return 1;
}

int timezone(int argc, argVector argv)
{
    if (argc != 2)
    {
        printF("%s\n", ARGUMENTS_AMOUNT_ERROR("1"));
        return 0;
    }
    int flag = 0;
    int num = 0;
    toInt(argv[1],&num,&flag);
    if(flag)
    {
        changeTimeZone(num);
        printF("%s %d\n", TIMEZONE_SUCCES_MSG, num);
        time(0,argv);
    }
    else
    {
        printF("%s\n", TIMEZONE_ERROR_MSG);
    }
    return 1;
}

int screen_saver(int argc, argVector argv)
{
    if (argc != 2)
    {
        printF("%s\n", ARGUMENTS_AMOUNT_ERROR("1"));
        printF("Change waiting time in seconds by passing an integer greater than %d.\n", MIN_SAVER_TIME);
        printF("Screen saver is currently %s.\n", (getSaverStatus()? "on":"off"));
        printF("Turn it %s by passing '%s'.\n", (!getSaverStatus()? "on":"off"),  (!getSaverStatus()? "on":"off"));
        return 0;
    }
    if(strcmp(argv[1],"on"))
    {
        setSaverStatus(TRUE);
        printF("Screen saver is now %s, and will wait %d seconds\n", (getSaverStatus()? "on":"off"), getSaverTime());
    }
    else if(strcmp(argv[1],"off"))
    {
        setSaverStatus(FALSE);
        printF("Screen saver is now %s.\n", (getSaverStatus()? "on":"off"));
    }
    else
    {
        int flag = 0;
        int num = 0;
        toInt(argv[1],&num,&flag);
        if(flag && num > MIN_SAVER_TIME)
        {
            setSaverTime(num);
            printF("Screen saver will now wait %d seconds\n", getSaverTime());
        }
        else
        {
            printF("Waiting time must be an integer greater than %d.\n", MIN_SAVER_TIME);
            return 0;
        }
    }
    return 1;
}

int exceptionTester(int argc, argVector argv)
{
  if (argc > 2)
  {
      printF("%s\n", CERO_ARGUMENTS_ERROR);
      return 0;
  }
  else if (strcmp(argv[1],"0"))
  {
    zeroDivisionException();
  }
  else if(strcmp(argv[1],"1"))
  {
    invalidOpcodeException();
  }
  else
  {
    printF("Wrong arguments\n");
  }
  return 1;
}

int printPs(int argc, argVector argv)
{
    int pid = 0;
    int flag;
    switch (argc)
    {
        case 1:
            userPs(0, 0);
            break;
        case 2:
            if (argv[1][0] == 'q')
                userPs('q', 0);
            else
                printF("Usage: Usage: ps or ps <q> or ps <p pid>\n");
            break;
        case 3:
            toInt(argv[2], &pid, &flag);
            if (flag)
                userPs('p',pid);
            else
                printF("Usage: Usage: ps or ps <q> or ps <p pid>\n");
            break;
        default:
            printF("Usage: Usage: ps or ps <q> or ps <p pid>\n");
    }

    return TRUE;
}

int procBomb(int argc, argVector argv)
{
    return userProcessBomb();
}

int kill(int argc, argVector argv)
{
    int fromPid, toPid;
    int flag = 0;
    if (argc < 2)
    {
        printF("Usage: kill <pid> or kill <pidFrom pidTo>\n");
        return FALSE;
    }
    toInt(argv[1],&fromPid,&flag);
    if (argc == 3)
        toInt(argv[2],&toPid,&flag);
    else
        toPid = fromPid;

    if(!flag)
    {
        printF("Usage: kill <pid> or kill <pidFrom pidTo>\n");
        return FALSE;
    }
    for (int i = fromPid; i <= toPid; ++i) {
        userKill((uint64_t) i);
    }
    return TRUE;
}

int backgroundTest(int count, argVector argv)
{
    return multiTest(1, NULL);
}

int multiProcTest(int count, argVector argv)
{
    int num;
    printF("This test will create many process and leave them running.\n");
    printF("See how execution slows down as more process are running\n");
    printF("How many process do you want to create?: ");
    num = getNum();
    printF("%d\n", num);
    return num;
    return multiTest(num, NULL);
}


/* Funciones auxiliares para los comandos de de usuario*/

int multiTest(int count, argVector argv)
{
    char *message;
    if (count == 1)
        message = "running process in background";
    else if(count < 1)
        return FALSE;
    else
        message = "multi-processing";

    printF("We will create and run %d process in background so that you see that we are %s.\n", count ,message);
    printF("The process name is %s.\n", aux_programs[0].name);
    printF("First we will print the current process list so that you see that program%s not currently running.\n\n", count==1?" is":"s are");
    printPs(1,NULL);
    printF("\nPress any key to execute the program%s\n", count==1?"":"s");
    getChar();
    for (int i = 0; i < count; ++i) {
        execProcInBackground(aux_programs[0].name, (uint64_t) aux_programs[0].fn);
    }
    printF("We will print again the process list so that you see the process %s running\n\n",count==1?"is":"are");
    printPs(1, NULL);
    printF("\nWe will leave the program%s running for you to decide what to do with it\n",count==1?"":"s");
    printF("Remember you can kill %s with the command kill <pid>\n",count==1?"it":"them");
    printF("Goodbye :)\n");
    return TRUE;
}

int endLessLoop()
{
    static long long counter = 0;
    int pid = userGetCurrentPid();
    printF("Hola! soy el proceso de pid:%d y estoy vivio!\n",pid);
    while (1)
    {
        if (counter++ % 2000000000 == 0)
        {
            printF("Soy el proceso %d y sigo vivio! Matame si te animas\n", pid);
        }
    }
}

