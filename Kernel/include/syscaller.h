#ifndef SYSCALLER_H
#define SYSCALLER_H

#include <stdint.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <timer.h>
#include <beepDriver.h>
#include <sleep.h>
#include <shellTests.h>
#include "ipc.h"
#include "pageAllocator.h"
#include "allocatorTest.h"
#include "pipes.h"

/* eax = 1
** draws a string with a given string in rdi
*/
uint64_t writeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 2
** returns the next character to consume from the buffer
*/
uint64_t readK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 3
** return the hour
*/
uint64_t getHour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 4
** return the minutes
*/
uint64_t getMin(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 5
** return the seconds
*/
uint64_t getSec(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 6
** makes a beep sound from the motherload
*/
uint64_t beep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 7
** suspends the system for a determined time
*/
uint64_t sleep(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 8
** draws a pixel in coordinates rdi (x) and rsi (y) with a given colour (r=rdx g=rcx b=r8)
*/
uint64_t userDrawPixel(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 9
** returns the resolutions in given pointers (rdi for width resolution and rsi for height resolution)
*/
uint64_t getResolutions(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 10
** changes the font colour with a given colour (r=rdi g=rsi b=rdx)
*/
uint64_t changeFontColour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 11
** makes a beep sound and exits
*/
uint64_t myExit(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 12
** draws a character
*/
uint64_t putChar(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 13
** makes a backSpace
*/
uint64_t removeChar(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 14
** sets a new background colour (r=rdi g=rsi b=rdx)
*/
uint64_t changeBackgroundColour(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 15
** sets current video driver coordinates to the given ones (x=rdi, y=rsi)
*/
uint64_t setCoordinates(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 16
** returns an address to the first page for a given size in rdi
*/
uint64_t sysMalloc(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 17
** frees a given page (or pages) on address given in rdi
*/
uint64_t sysFree (uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 18
** prints all process queues
*/
uint64_t printProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 19
** create and executes a process
*/
uint64_t kernelCreateAndExProcess(uint64_t rdi, uint64_t rsi, uint64_t fore, uint64_t argv, uint64_t argc);

/* eax = 20
** kills a proces process state
*/
uint64_t kill(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 21
** makes a process bomb
*/
uint64_t procBomb(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 22
 ** returns the current pid
 */
 uint64_t getCurrentPid(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 23
 ** sends a message to a process
 */
uint64_t send(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 24
 ** receives a message to a process
 */
uint64_t receive(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 25
 ** creates a mute5
 */
uint64_t createMutex(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 26
 ** locks mutex
 */
uint64_t kernelLock(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 27
 ** unlocks a mutex
 */
uint64_t kernelUnlock(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 28
 ** destroys a mutex
 */
uint64_t destroyMutexKernel(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 29
 ** runs a test to prove functionality of the allocator
 */
uint64_t sysAllocatorTest(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 30
 ** changes the priority of a process
 */
uint64_t nice(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 31
 ** executes a test
 */
uint64_t kernelColumnTest(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 32
 ** kill all descendentts of pid rdi
 */
uint64_t kernelKillAllDescendants(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 33
 ** creates a semaphore
 */
uint64_t kernelCreateSemaphore(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 34
 ** Try to access space through a semaphore
 */
uint64_t kernelSemWait(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 35
 **  Frees a semaphore
 */
uint64_t kernelSemPost(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 36
 ** Try to destroy space through a semaphore
 */
uint64_t kernelSemDestroy(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 37
 **
 */
uint64_t kernelGetQuantum(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 38
 **
 */
uint64_t kernelSetQuantum(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 39
 **  Opens Pipe
 */
uint64_t openPipe(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);


/* eax = 40
 **  Closes a pipe
 */
uint64_t closeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 41
 **  Sets a pipe from left to right
 */
uint64_t pipeK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 42
 */
uint64_t kernelCreateProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 43
 */
uint64_t kernelStartProcess(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 44
 */
uint64_t pipesToStdsK(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

/* eax = 45
 */
uint64_t drawCharInULND(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8);

#endif
