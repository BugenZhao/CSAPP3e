.text
.globl _rfact
_rfact:
	pushq	%rbx
	movq	%rdi, %rbx
	movq	$1, %rax
	cmpq	$1, %rdi
	jle		.L1
	subq	$1, %rdi
	callq	_rfact
	imulq	%rbx, %rax
.L1:
	popq	%rbx
	ret
