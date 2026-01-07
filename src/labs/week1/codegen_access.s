	.file	"codegen_access.cpp"
	.text
	.p2align 4
	.globl	_Z13access_unsafeRKSt6vectorIiSaIiEEm
	.type	_Z13access_unsafeRKSt6vectorIiSaIiEEm, @function
_Z13access_unsafeRKSt6vectorIiSaIiEEm:
.LFB1259:
	.cfi_startproc
	endbr64
	movq	(%rdi), %rax
	movl	(%rax,%rsi,4), %eax
	ret
	.cfi_endproc
.LFE1259:
	.size	_Z13access_unsafeRKSt6vectorIiSaIiEEm, .-_Z13access_unsafeRKSt6vectorIiSaIiEEm
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"vector::_M_range_check: __n (which is %zu) >= this->size() (which is %zu)"
	.text
	.p2align 4
	.globl	_Z11access_safeRKSt6vectorIiSaIiEEm
	.type	_Z11access_safeRKSt6vectorIiSaIiEEm, @function
_Z11access_safeRKSt6vectorIiSaIiEEm:
.LFB1260:
	.cfi_startproc
	endbr64
	movq	(%rdi), %rax
	movq	8(%rdi), %rdx
	subq	%rax, %rdx
	sarq	$2, %rdx
	cmpq	%rdx, %rsi
	jnb	.L8
	movl	(%rax,%rsi,4), %eax
	ret
.L8:
	pushq	%rax
	.cfi_def_cfa_offset 16
	leaq	.LC0(%rip), %rdi
	xorl	%eax, %eax
	call	_ZSt24__throw_out_of_range_fmtPKcz@PLT
	.cfi_endproc
.LFE1260:
	.size	_Z11access_safeRKSt6vectorIiSaIiEEm, .-_Z11access_safeRKSt6vectorIiSaIiEEm
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
