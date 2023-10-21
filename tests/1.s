	.file	"1.c"
	.option nopic
	.attribute arch, "rv32i2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	li	a5,-2147483648
	addi	a5,a5,240
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	li	a4,1
	sw	a4,0(a5)
	j	.L2
.L3:
	lw	a5,-20(s0)
	lw	a5,0(a5)
	addi	a4,a5,1
	lw	a5,-20(s0)
	sw	a4,0(a5)
.L2:
	lw	a5,-20(s0)
	lw	a5,0(a5)
	bne	a5,zero,.L3
	li	a5,0
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: () 13.2.0"
