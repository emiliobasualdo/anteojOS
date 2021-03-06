#ifndef SHELLCOMMANDS_H
#define SHELLCOMMANDS_H

#include "colour.h"
#include <stdint.h>
#include "shell.h"
#include "scLib.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "userPrintf.h"
#include "prodcons.h"
#include "messageTest.h"
#include "philosophers.h"
#include "mutexTest.h"

#define MAX_BUFFER_SIZE 1000
#define MAX_ARGS 10
#define MAX_ARG_LENGTH ((MAX_BUFFER_SIZE - (MAX_ARGS) )/MAX_ARGS)  // por los espacios

#define CERO_ARGUMENTS_ERROR      "Error: Too many arguments passed, function takes 0 arguments."
#define SET_FONT_MSSG             "Choose a colour by typing a number, or press 'q' exit."
#define SET_FONT_EX               "Colours reflect your personality!"
#define TIMEZONE_ERROR_MSG        "Error: Only numbers and '-' are allowed in timezone format. Please re-enter your timezone."
#define TIMEZONE_SUCCES_MSG       "Timezone has been set to:"
#define ARGUMENTS_AMOUNT_ERROR(x) "Error: Function takes "x" argument"
#define PRODCONS_ERROR_MSG        "Error: arguments must be numbers"

#define FONT 2
#define BACK 1

typedef char argVector[MAX_ARGS][MAX_ARG_LENGTH];

typedef struct
{
    char * name;
    char * description;
    int (* fn) (int argc, argVector argv);
} command;

/*
** returns a boolean if the given string corresponds to the name
** of an existing command
*/
int commandExists(const char *);

/* executes a given command (the name is on argVector[0][]) */
int executeCommand(int argc, argVector argv);

/* executes a command in background*/
int execProcInBackground(char *name, uint64_t intstruction, int argc, argVector argv);

/* changes a determined colour, background or font */
int changeColour(void(*f)(Colour), int flag);

/* executes the help command */
int help (int argc, argVector argv);

/* executes the echo command */
int echo (int argc, argVector argv);

/* executes the time command */
int time (int argc, argVector argv);

/* executes the clear command */
int clear(int argc, argVector argv);

/* executes the beep command */
int beep (int argc, argVector argv);

/* executes the exit command */
int exitShell (int argc, argVector argv);

/* executes the change of font colour command */
int font_colour (int argc, argVector argv);

/* executes the change of background colour command */
int background_colour(int argc, argVector argv);

/* executes the time zone command */
int timezone(int argc, argVector argv);

/* executes the screen saver command */
int screen_saver(int argc, argVector argv);

/* executes the exception tests */
int exceptionTester(int argc, argVector argv);

/* calls a zero Division Exception */
extern void zeroDivisionException();

/* calls an Invalid Opcode Exception*/
extern void invalidOpcodeException();

int printPs(int argc, argVector argv);

int procBomb(int argc, argVector argv);

int backgroundTest(int argc, argVector argv);

int kill(int argc, argVector argv);

int prodCons(int argc, argVector argv);

int multiProcTest(int count, argVector argv);

int allocatorTest(int count, argVector argv);

int nice(int count, argVector argv);

int setQuantum(int argc, argVector argv);

/* Funciones auxiliares para las funciones de usuario*/

int multiTest(int count, argVector argv);

int endLessLoop();

int messageTesting();

int columnTest(int argc, argVector argv);

int getQuantum(int argc, argVector argv);

int philoTest(int argc, argVector argv);

int mutTest(int argc, argVector argv);

int producerP(int argc, argVector argv);

int consumerP(int argc, argVector argv);

#endif
