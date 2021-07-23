/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/

.globl coctx_swap
#if !defined( __APPLE__ )
.type  coctx_swap, @function
#endif
coctx_swap:
/*
#if defined(__i386__)
    movl 4(%esp), %eax
    movl %esp,  28(%eax)
    movl %ebp, 24(%eax)
    movl %esi, 20(%eax)
    movl %edi, 16(%eax)
    movl %edx, 12(%eax)
    movl %ecx, 8(%eax)
    movl %ebx, 4(%eax)


    movl 8(%esp), %eax
    movl 4(%eax), %ebx
    movl 8(%eax), %ecx
    movl 12(%eax), %edx
    movl 16(%eax), %edi
    movl 20(%eax), %esi
    movl 24(%eax), %ebp
    movl 28(%eax), %esp

	ret

#elif defined(__x86_64__)*/
	/* 父函数栈帧中除返回地址外栈帧顶的位置
	rdi寄存器存的是第一个参数的地址，其112个字节后，是regs数组后的位置。
	难道是要把ss_size的首地址放进去？
	显然不是的，因为栈的地址从高位到低位
    所以，再度push的时候，将会首先填充regs[13] ，一直填充到reg[0]
	设置栈顶指针为
	*/
	leaq (%rsp),%rax
    /*将寄存器保存到入栈,因为此时栈的地址指向数组，因此实际上就是讲各个寄存器填充到数组中*/
    movq %rax, 104(%rdi)/* rax -> regs[13]，也就是当前的rsp -> regs[13]*/
    movq %rbx, 96(%rdi)	/* rbx -> regs[12]*/
    movq %rcx, 88(%rdi)	/* rcx -> regs[11]*/
    movq %rdx, 80(%rdi)	/* rdx -> regs[10]*/
	  movq 0(%rax), %rax	/* ret func addr  返回地址 -> regs[9]*/
	  movq %rax, 72(%rdi) 
    movq %rsi, 64(%rdi)		/* rsi -> regs[8]*/
	  movq %rdi, 56(%rdi)	/* rdi -> regs[7]*/
    movq %rbp, 48(%rdi)		/* rbp -> regs[6]*/
    movq %r8, 40(%rdi)		/* r8 -> regs[5]*/
    movq %r9, 32(%rdi)		/* r9 -> regs[4]*/
    movq %r12, 24(%rdi)		/* r12 -> regs[3]*/
    movq %r13, 16(%rdi)		/* r13 -> regs[2]*/
    movq %r14, 8(%rdi)		/* r14 -> regs[1]*/
    movq %r15, (%rdi)		/* r15 -> regs[0]  r15是程序计数器*/
	  xorq %rax, %rax

	/* 截止到此，所有的协程上下文保存成功
	   rsi中是第二个参数，我们需要将第二个参数的上下文载入到寄存器和栈里面
	   rsi的首地址就是整个regs[]参数的地址,从0开始，所以当pop的时候，将从0将参数pop出来。*/
    movq 48(%rsi), %rbp
    movq 104(%rsi), %rsp

	/* 以下为倒序还原*/
    movq (%rsi), %r15
    movq 8(%rsi), %r14
    movq 16(%rsi), %r13
    movq 24(%rsi), %r12
    movq 32(%rsi), %r9
    movq 40(%rsi), %r8
    movq 56(%rsi), %rdi
    movq 80(%rsi), %rdx
    movq 88(%rsi), %rcx
    movq 96(%rsi), %rbx
		leaq 8(%rsp), %rsp
		pushq 72(%rsi)

    movq 64(%rsi), %rsi
	ret
//#endif
