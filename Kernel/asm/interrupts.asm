GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL rip
GLOBAL stack
GLOBAL getStack
GLOBAL switchTo

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN getMyStack
EXTERN dispatcher
EXTERN printtt


SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0 ; todo me faltan dos registrto pelotdudo
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

    endOfInt

	popState
	iretq
%endmacro


%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp
	call exceptionDispatcher

	popState

	mov rdi, [rip]
	mov qword [rsp], rdi ; pedro, esto no se que es pero lo ideal es que lo pongas como una macro todo
	mov rdi, [stack]
	mov qword [rsp+ 3*8], rdi
	iretq

%endmacro

%macro endOfInt 0
    ; signal pic EOI (End of Interrupt)
    mov al, 20h
    out 20h, al

%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler: ; irqHandlerMaster 0 ; meto aca el que lleva la cuenta de cuantums todo
    pushState

    mov rax, rsp    ; Lo guardamos aca para despues pushearlo en el nuevo stack
    mov rbx, stack
    sub rbx, 100
    mov rsp, rbx  ; Cambiamos de stack por uno auxiliar para no pisar el del proceso con lo que viene ahora
    push rax        ; Pushiamos el RSP

    mov rdi, 0      ; pasaje de parametro
    call irqDispatcher ; llamamos al irqDispatcher para que haga lo que quiera con el tick, ej: tick++
    pop rdi         ; Popeamos el RSP y se lo pasamos como parametro al dispatcher

    ;mov rdi, rsp    ;<------ esto es SOLO para la version sin el stack auxiliar
    call dispatcher ; llamamos al dispatcher, para que guarde el RSP y nos mande el nuevo RSP
    mov rsp, rax    ; colocamos el nuevo RPS

    popState        ; levantamos el estado de registros anterior del proceso que queremos correr

    endOfInt        ; notificamos el end of interrupt


    iretq           ; levantamos lo que pusheó la int cuando se cortó el proceso que queremos ejecutar


;Keyboard
_irq01Handler:
    ;mov rdi,34
    ;call printtt
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6
;n

haltcpu:
	cli
	hlt
	ret

getStack:
 mov rax, rsp
 ret

switchTo:

    mov rsp, rdi    ; levantamos el stack pointer
    popState        ; porque nuestro stack tiene registros a pesar de que no nos importan
    iretq           ; mentimos que hay una interrupcion




SECTION .bss
	aux: resq 1
	rip: resq 1
	stack: resb 100 ; suficiente para 100 quads todo chequear esto
