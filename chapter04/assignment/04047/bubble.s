	.text
	.globl _bubble_a
_bubble_a:
LFB1:
	leaq	-8(%rdi,%rsi,8), %rsi
	jmp	L2
L3:
	addq	$8, %rax
L5:
	cmpq	%rax, %rsi
	jbe	L7
	movq	8(%rax), %rdx
	movq	(%rax), %rcx
	cmpq	%rcx, %rdx
	jge	L3
	movq	%rcx, 8(%rax)
	movq	%rdx, (%rax)
	jmp	L3
L7:
	subq	$8, %rsi
L2:
	cmpq	%rdi, %rsi
	je	L8
	movq	%rdi, %rax
	jmp	L5
L8:
	ret
LFE1:
	.cstring
lC0:
	.ascii "%ld \0"
	.section __TEXT,__text_startup,regular,pure_instructions
	.globl _main
_main:
LFB2:
	pushq	%rbx
LCFI0:
	movl	$7, %esi
	leaq	_d(%rip), %rdi
	call	_bubble_a
	movl	$0, %ebx
	jmp	L10
L11:
	movslq	%ebx, %rdx
	leaq	_d(%rip), %rax
	movq	(%rax,%rdx,8), %rsi
	leaq	lC0(%rip), %rdi
	movl	$0, %eax
	call	_printf
	addl	$1, %ebx
L10:
	cmpl	$6, %ebx
	jle	L11
	movl	$0, %eax
	popq	%rbx
LCFI1:
	ret
LFE2:
	.globl _d
	.data
	.align 5
_d:
	.quad	6
	.quad	7
	.quad	3
	.quad	5
	.quad	2
	.quad	4
	.quad	1
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x1
	.ascii "zR\0"
	.byte	0x1
	.byte	0x78
	.byte	0x10
	.byte	0x1
	.byte	0x10
	.byte	0xc
	.byte	0x7
	.byte	0x8
	.byte	0x90
	.byte	0x1
	.align 3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB1-.
	.set L$set$2,LFE1-LFB1
	.quad L$set$2
	.byte	0
	.align 3
LEFDE1:
LSFDE3:
	.set L$set$3,LEFDE3-LASFDE3
	.long L$set$3
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB2-.
	.set L$set$4,LFE2-LFB2
	.quad L$set$4
	.byte	0
	.byte	0x4
	.set L$set$5,LCFI0-LFB2
	.long L$set$5
	.byte	0xe
	.byte	0x10
	.byte	0x83
	.byte	0x2
	.byte	0x4
	.set L$set$6,LCFI1-LCFI0
	.long L$set$6
	.byte	0xe
	.byte	0x8
	.align 3
LEFDE3:
	.ident	"GCC: (Homebrew GCC 9.2.0_3) 9.2.0"
	.subsections_via_symbols
