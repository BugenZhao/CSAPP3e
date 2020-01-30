.text
.globl _add
_add:
	addq %rsi, %rdi
	movq %rdi, %rax
	ret
