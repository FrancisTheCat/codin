#include "syscalls.h"

.intel_syntax 
.text
.globl _start, __syscall0, __get_fs_register, __call_main
.globl __syscall1, __syscall2, __syscall3, __syscall4, __syscall5, __syscall6, __clone3, __go_unmap_urself


_start:
        xor %rbp, %rbp
        pop %rdi
        mov %rsi, %rsp
        and %rsp, -16
        call __codin_main
        mov %rdi, %rax
        mov %rax, SYS_exit
        syscall
        ret

__syscall0:
        mov %rax, %rdi
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

__syscall1:
        mov %rax, %rdi
        mov %rdi, %rsi
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

__syscall2:
        mov %rax, %rdi
        mov %rdi, %rsi
        mov %rsi, %rdx
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

__syscall3:
        mov %rax, %rdi
        mov %rdi, %rsi
        mov %rsi, %rdx
        mov %rdx, %rcx
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

__syscall4:
        mov %rax, %rdi
        mov %rdi, %rsi
        mov %rsi, %rdx
        mov %rdx, %rcx
        mov %r10, %r8
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

__syscall5:
        mov %rax, %rdi
        mov %rdi, %rsi
        mov %rsi, %rdx
        mov %rdx, %rcx
        mov %r10, %r8
        mov %r8,  %r9
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11

        ret

__syscall6:
        mov %rax, %rdi
        mov %rdi, %rsi
        mov %rsi, %rdx
        mov %rdx, %rcx
        mov %r10, %r8
        mov %r8,  %r9
        mov %r9,  0
        push %r11
        push %rcx
        syscall
        pop %rcx
        pop %r11
        ret

/* The userland implementation is:
   int clone3 (struct clone_args *cl_args, size_t size,
	       int (*func)(void *arg), void *arg);
   the kernel entry is:
   int clone3 (struct clone_args *cl_args, size_t size);

   The parameters are passed in registers from userland:
   rdi: cl_args
   rsi: size
   rdx: func
   rcx: arg

   The kernel expects:
   rax: system call number
   rdi: cl_args
   rsi: size  */

__clone3:
        mov %rax, SYS_clone3
        mov %r8, %rcx
        syscall
        
        cmp %rax, 0
        je __clone_child
__clone_parent:
        ret
__clone_child:
        xor  %ebp, %ebp
        mov  %rdi, %r8
        call %rdx
        mov  %rdi, %rax
        mov  %rax, SYS_exit
        syscall
        
        ret 

// rdi: stack pointer
// rsi: stack size
// rdx: exit code
__go_unmap_urself:
        mov  %rax, SYS_munmap
        syscall
        
        mov  %rdi, %rdx
        mov  %rax, SYS_exit
        syscall

__get_fs_register:
        mov %rax, %fs:0
        ret
