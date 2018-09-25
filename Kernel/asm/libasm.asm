GLOBAL cpuVendor
GLOBAL cmpandSwap
GLOBAL unlockMutexASM
GLOBAL lockMutexASM

section .text

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

cmpandSwap: ;POR COMO LO ESTABAMOS HACIENDO ANTES
	push rbp
    mov rbp, rsp

    mov rax, 0
    mov rbx, 1
    mov rcx, 0
    mov rdx, 0

    lock cmpxchg8b [rdi]

    je lockAquired

    mov rax, 0
    jmp final

lockAquired:
    mov rax, 1

final:
    mov rsp, rbp
    pop rbp
    ret

lockMutexASM:
	push rbp
	mov rbp, rsp
	xor rax, rax
	mov rax, 1
	xchg rax, [rdi]
	jmp final

unlockMutexASM:
	push rbp
	mov rbp, rsp
	xor rax, rax
	mov rax, 0
	xchg rax, [rdi]
	jmp final
