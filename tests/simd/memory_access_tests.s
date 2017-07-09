	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.intel_syntax noprefix
	.globl	__Z5totalPfm
	.p2align	4, 0x90
__Z5totalPfm:                           ## @_Z5totalPfm
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp0:
	.cfi_def_cfa_offset 16
Ltmp1:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp2:
	.cfi_def_cfa_register rbp
	xorps	xmm0, xmm0
	test	rsi, rsi
	je	LBB0_8
## BB#1:
	lea	rax, [rsi - 1]
	mov	rdx, rsi
	and	rdx, 7
	je	LBB0_2
## BB#3:
	xorps	xmm0, xmm0
	xor	ecx, ecx
	.p2align	4, 0x90
LBB0_4:                                 ## =>This Inner Loop Header: Depth=1
	addss	xmm0, dword ptr [rdi + 4*rcx]
	inc	rcx
	cmp	rdx, rcx
	jne	LBB0_4
	jmp	LBB0_5
LBB0_2:
	xor	ecx, ecx
	xorps	xmm0, xmm0
LBB0_5:
	cmp	rax, 7
	jb	LBB0_8
## BB#6:
	sub	rsi, rcx
	lea	rax, [rdi + 4*rcx + 28]
	.p2align	4, 0x90
LBB0_7:                                 ## =>This Inner Loop Header: Depth=1
	addss	xmm0, dword ptr [rax - 28]
	addss	xmm0, dword ptr [rax - 24]
	addss	xmm0, dword ptr [rax - 20]
	addss	xmm0, dword ptr [rax - 16]
	addss	xmm0, dword ptr [rax - 12]
	addss	xmm0, dword ptr [rax - 8]
	addss	xmm0, dword ptr [rax - 4]
	addss	xmm0, dword ptr [rax]
	add	rax, 32
	add	rsi, -8
	jne	LBB0_7
LBB0_8:
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI1_0:
	.long	1065353216              ## float 1
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z4lerpfff
	.p2align	4, 0x90
__Z4lerpfff:                            ## @_Z4lerpfff
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp5:
	.cfi_def_cfa_register rbp
	movss	xmm3, dword ptr [rip + LCPI1_0] ## xmm3 = mem[0],zero,zero,zero
	subss	xmm3, xmm0
	mulss	xmm3, xmm1
	mulss	xmm2, xmm0
	addss	xmm3, xmm2
	movaps	xmm0, xmm3
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI2_0:
	.long	1065353216              ## float 1
LCPI2_5:
	.long	1092616192              ## float 10
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI2_1:
	.long	0                       ## 0x0
	.long	1                       ## 0x1
	.long	2                       ## 0x2
	.long	3                       ## 0x3
LCPI2_2:
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
LCPI2_3:
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
LCPI2_4:
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z10render_cpuiiPf
	.p2align	4, 0x90
__Z10render_cpuiiPf:                    ## @_Z10render_cpuiiPf
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp6:
	.cfi_def_cfa_offset 16
Ltmp7:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp8:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	rbx
	push	rax
Ltmp9:
	.cfi_offset rbx, -40
Ltmp10:
	.cfi_offset r14, -32
Ltmp11:
	.cfi_offset r15, -24
	mov	rbx, rdx
	mov	r15d, esi
	mov	r14d, edi
	mov	eax, r15d
	imul	eax, r14d
	movsxd	rsi, eax
	shl	rsi, 2
	mov	rdi, rbx
	call	___bzero
	test	r15d, r15d
	je	LBB2_11
## BB#1:
	test	r14d, r14d
	je	LBB2_11
## BB#2:
	lea	eax, [r14 - 1]
	cvtsi2ss	xmm2, eax
	movss	xmm0, dword ptr [rip + LCPI2_0] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm1, xmm0
	divss	xmm1, xmm2
	mov	eax, r14d
	movsxd	r10, r14d
	mov	r9d, r15d
	mov	r8d, r14d
	and	r8d, 3
	mov	rcx, rax
	sub	rcx, r8
	movaps	xmm2, xmm1
	shufps	xmm2, xmm2, 0           ## xmm2 = xmm2[0,0,0,0]
	shl	r10, 2
	xor	esi, esi
	xorps	xmm3, xmm3
	movss	xmm4, dword ptr [rip + LCPI2_5] ## xmm4 = mem[0],zero,zero,zero
	movaps	xmm8, xmmword ptr [rip + LCPI2_1] ## xmm8 = [0,1,2,3]
	movaps	xmm10, xmmword ptr [rip + LCPI2_2] ## xmm10 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	xorps	xmm9, xmm9
	movaps	xmm11, xmmword ptr [rip + LCPI2_3] ## xmm11 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	movdqa	xmm12, xmmword ptr [rip + LCPI2_4] ## xmm12 = [4,4,4,4]
	.p2align	4, 0x90
LBB2_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB2_8 Depth 2
                                        ##     Child Loop BB2_5 Depth 2
	cmp	r14d, 3
	jbe	LBB2_4
## BB#6:                                ##   in Loop: Header=BB2_3 Depth=1
	test	rcx, rcx
	je	LBB2_4
## BB#7:                                ##   in Loop: Header=BB2_3 Depth=1
	mov	rdi, rbx
	mov	rdx, rcx
	movaps	xmm6, xmm8
	.p2align	4, 0x90
LBB2_8:                                 ##   Parent Loop BB2_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cvtdq2ps	xmm5, xmm6
	mulps	xmm5, xmm2
	movaps	xmm7, xmm10
	subps	xmm7, xmm5
	mulps	xmm7, xmm9
	mulps	xmm5, xmm11
	addps	xmm5, xmm7
	movups	xmmword ptr [rdi], xmm5
	paddd	xmm6, xmm12
	add	rdi, 16
	add	rdx, -4
	jne	LBB2_8
## BB#9:                                ##   in Loop: Header=BB2_3 Depth=1
	test	r8d, r8d
	mov	rdi, rcx
	jne	LBB2_5
	jmp	LBB2_10
	.p2align	4, 0x90
LBB2_4:                                 ##   in Loop: Header=BB2_3 Depth=1
	xor	edi, edi
	.p2align	4, 0x90
LBB2_5:                                 ##   Parent Loop BB2_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	xorps	xmm5, xmm5
	cvtsi2ss	xmm5, edi
	mulss	xmm5, xmm1
	movaps	xmm6, xmm0
	subss	xmm6, xmm5
	mulss	xmm6, xmm3
	mulss	xmm5, xmm4
	addss	xmm5, xmm6
	movss	dword ptr [rbx + 4*rdi], xmm5
	inc	rdi
	cmp	rax, rdi
	jne	LBB2_5
LBB2_10:                                ##   in Loop: Header=BB2_3 Depth=1
	inc	rsi
	add	rbx, r10
	cmp	rsi, r9
	jne	LBB2_3
LBB2_11:
	add	rsp, 8
	pop	rbx
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI3_0:
	.long	1065353216              ## float 1
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI3_1:
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z11render_simdiiPf
	.p2align	4, 0x90
__Z11render_simdiiPf:                   ## @_Z11render_simdiiPf
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp12:
	.cfi_def_cfa_offset 16
Ltmp13:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp14:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	rbx
	push	rax
Ltmp15:
	.cfi_offset rbx, -40
Ltmp16:
	.cfi_offset r14, -32
Ltmp17:
	.cfi_offset r15, -24
	mov	rbx, rdx
	mov	r14d, esi
	mov	r15d, edi
	mov	eax, r14d
	imul	eax, r15d
	movsxd	rsi, eax
	shl	rsi, 2
	mov	rdi, rbx
	call	___bzero
	test	r14d, r14d
	je	LBB3_6
## BB#1:
	test	r15d, r15d
	jle	LBB3_6
## BB#2:
	lea	eax, [r15 - 1]
	cvtsi2ss	xmm1, eax
	movss	xmm0, dword ptr [rip + LCPI3_0] ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, xmm1
	movaps	xmm1, xmm0
	shufps	xmm1, xmm1, 0           ## xmm1 = xmm1[0,0,0,0]
	mulps	xmm1, xmmword ptr [rip + __ZL6seq_vf]
	movsxd	rax, r15d
	mov	r8d, r14d
	lea	rdx, [4*rax]
	xor	esi, esi
	xorps	xmm2, xmm2
	movaps	xmm3, xmmword ptr [rip + LCPI3_1] ## xmm3 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	.p2align	4, 0x90
LBB3_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB3_4 Depth 2
	mov	rdi, rbx
	xor	ecx, ecx
	.p2align	4, 0x90
LBB3_4:                                 ##   Parent Loop BB3_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	xorps	xmm4, xmm4
	cvtsi2ss	xmm4, ecx
	mulss	xmm4, xmm0
	shufps	xmm4, xmm4, 0           ## xmm4 = xmm4[0,0,0,0]
	addps	xmm4, xmm1
	movaps	xmm5, xmmword ptr [rip + __ZL6one_vf]
	subps	xmm5, xmm4
	mulps	xmm5, xmm2
	mulps	xmm4, xmm3
	addps	xmm4, xmm5
	movaps	xmmword ptr [rdi], xmm4
	add	rcx, 4
	add	rdi, 16
	cmp	rcx, rax
	jl	LBB3_4
## BB#5:                                ##   in Loop: Header=BB3_3 Depth=1
	inc	rsi
	add	rbx, rdx
	cmp	rsi, r8
	jne	LBB3_3
LBB3_6:
	add	rsp, 8
	pop	rbx
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI4_0:
	.quad	42949672968             ## 0xa00000008
	.quad	60129542156             ## 0xe0000000c
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z5testAv
	.p2align	4, 0x90
__Z5testAv:                             ## @_Z5testAv
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp18:
	.cfi_def_cfa_offset 16
Ltmp19:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp20:
	.cfi_def_cfa_register rbp
	movaps	xmm0, xmmword ptr [rip + LCPI4_0] ## xmm0 = [42949672968,60129542156]
	pop	rbp
	ret
	.cfi_endproc

	.globl	__Z5testBv
	.p2align	4, 0x90
__Z5testBv:                             ## @_Z5testBv
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp21:
	.cfi_def_cfa_offset 16
Ltmp22:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp23:
	.cfi_def_cfa_register rbp
	movabs	rax, 42949672968
	movabs	rdx, 60129542156
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI6_0:
	.long	0                       ## 0x0
	.long	1                       ## 0x1
	.long	2                       ## 0x2
	.long	3                       ## 0x3
LCPI6_1:
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
LCPI6_2:
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
LCPI6_3:
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
LCPI6_4:
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
LCPI6_5:
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI6_6:
	.long	1315859240              ## float 1.0E+9
LCPI6_7:
	.long	981475336               ## float 9.77517105E-4
LCPI6_8:
	.long	2139095039              ## float 3.40282347E+38
LCPI6_9:
	.long	4286578687              ## float -3.40282347E+38
LCPI6_10:
	.long	1176256512              ## float 1.0E+4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
## BB#0:
	push	rbp
Ltmp114:
	.cfi_def_cfa_offset 16
Ltmp115:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp116:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 88
Ltmp117:
	.cfi_offset rbx, -56
Ltmp118:
	.cfi_offset r12, -48
Ltmp119:
	.cfi_offset r13, -40
Ltmp120:
	.cfi_offset r14, -32
Ltmp121:
	.cfi_offset r15, -24
	mov	edi, 4194304
	call	__Znam
	mov	r15, rax
	mov	edi, 4194304
	call	__Znam
	mov	r13, rax
	mov	edi, 40000
	call	__Znwm
	mov	r12, rax
	mov	esi, 40000
	mov	rdi, r12
	call	___bzero
Ltmp24:
	mov	edi, 40000
	call	__Znwm
Ltmp25:
## BB#1:
	mov	qword ptr [rbp - 72], r12 ## 8-byte Spill
	mov	r14, r12
	add	r14, 40000
	mov	rcx, rax
	add	rcx, 40000
	mov	qword ptr [rbp - 120], rcx ## 8-byte Spill
	mov	esi, 40000
	mov	qword ptr [rbp - 64], rax ## 8-byte Spill
	mov	rdi, rax
	call	___bzero
	call	__ZNSt3__16chrono12steady_clock3nowEv
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	qword ptr [rbp - 80], r15 ## 8-byte Spill
	add	r15, 16
	xor	r12d, r12d
	.p2align	4, 0x90
LBB6_2:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB6_3 Depth 2
                                        ##       Child Loop BB6_4 Depth 3
                                        ##     Child Loop BB6_7 Depth 2
                                        ##       Child Loop BB6_8 Depth 3
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	rbx, rax
	mov	esi, 4194304
	mov	rdi, qword ptr [rbp - 80] ## 8-byte Reload
	call	___bzero
	mov	rax, r15
	xor	ecx, ecx
	movaps	xmm3, xmmword ptr [rip + LCPI6_0] ## xmm3 = [0,1,2,3]
	movaps	xmm8, xmm3
	movaps	xmm4, xmmword ptr [rip + LCPI6_1] ## xmm4 = [9.775171e-04,9.775171e-04,9.775171e-04,9.775171e-04]
	movaps	xmm5, xmmword ptr [rip + LCPI6_2] ## xmm5 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	xorps	xmm6, xmm6
	movaps	xmm7, xmmword ptr [rip + LCPI6_3] ## xmm7 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	movdqa	xmm3, xmmword ptr [rip + LCPI6_4] ## xmm3 = [4,4,4,4]
	movdqa	xmm9, xmm3
	movdqa	xmm3, xmmword ptr [rip + LCPI6_5] ## xmm3 = [8,8,8,8]
	.p2align	4, 0x90
LBB6_3:                                 ##   Parent Loop BB6_2 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB6_4 Depth 3
	mov	rdx, rax
	mov	esi, 1024
	movaps	xmm0, xmm8
	.p2align	4, 0x90
LBB6_4:                                 ##   Parent Loop BB6_2 Depth=1
                                        ##     Parent Loop BB6_3 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	cvtdq2ps	xmm1, xmm0
	mulps	xmm1, xmm4
	movaps	xmm2, xmm5
	subps	xmm2, xmm1
	mulps	xmm2, xmm6
	mulps	xmm1, xmm7
	addps	xmm1, xmm2
	movups	xmmword ptr [rdx - 16], xmm1
	movaps	xmm1, xmm0
	paddd	xmm1, xmm9
	cvtdq2ps	xmm1, xmm1
	mulps	xmm1, xmm4
	movaps	xmm2, xmm5
	subps	xmm2, xmm1
	mulps	xmm2, xmm6
	mulps	xmm1, xmm7
	addps	xmm1, xmm2
	movups	xmmword ptr [rdx], xmm1
	paddd	xmm0, xmm3
	add	rdx, 32
	add	rsi, -8
	jne	LBB6_4
## BB#5:                                ##   in Loop: Header=BB6_3 Depth=2
	inc	rcx
	add	rax, 4096
	cmp	rcx, 1024
	jne	LBB6_3
## BB#6:                                ##   in Loop: Header=BB6_2 Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	sub	rax, rbx
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, dword ptr [rip + LCPI6_6]
	mov	rax, qword ptr [rbp - 72] ## 8-byte Reload
	movss	dword ptr [rax + 4*r12], xmm0
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	rbx, rax
	mov	esi, 4194304
	mov	rdi, r13
	call	___bzero
	movaps	xmm5, xmmword ptr [rip + LCPI6_3] ## xmm5 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	xorps	xmm4, xmm4
	movaps	xmm0, xmmword ptr [rip + __ZL6seq_vf]
	mulps	xmm0, xmmword ptr [rip + LCPI6_1]
	movaps	xmm1, xmmword ptr [rip + __ZL6one_vf]
	mov	rax, r13
	xor	ecx, ecx
	movss	xmm6, dword ptr [rip + LCPI6_7] ## xmm6 = mem[0],zero,zero,zero
	.p2align	4, 0x90
LBB6_7:                                 ##   Parent Loop BB6_2 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB6_8 Depth 3
	mov	rdx, rax
	xor	esi, esi
	.p2align	4, 0x90
LBB6_8:                                 ##   Parent Loop BB6_2 Depth=1
                                        ##     Parent Loop BB6_7 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	xorps	xmm2, xmm2
	cvtsi2ss	xmm2, esi
	mulss	xmm2, xmm6
	shufps	xmm2, xmm2, 0           ## xmm2 = xmm2[0,0,0,0]
	addps	xmm2, xmm0
	movaps	xmm3, xmm1
	subps	xmm3, xmm2
	mulps	xmm3, xmm4
	mulps	xmm2, xmm5
	addps	xmm2, xmm3
	movaps	xmmword ptr [rdx], xmm2
	add	rsi, 4
	add	rdx, 16
	cmp	rsi, 1024
	jl	LBB6_8
## BB#9:                                ##   in Loop: Header=BB6_7 Depth=2
	inc	rcx
	add	rax, 4096
	cmp	rcx, 1024
	jne	LBB6_7
## BB#10:                               ##   in Loop: Header=BB6_2 Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	sub	rax, rbx
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, dword ptr [rip + LCPI6_6]
	mov	rax, qword ptr [rbp - 64] ## 8-byte Reload
	movss	dword ptr [rax + 4*r12], xmm0
	inc	r12
	cmp	r12, 10000
	jne	LBB6_2
## BB#11:
	movss	xmm4, dword ptr [rip + LCPI6_8] ## xmm4 = mem[0],zero,zero,zero
	movss	xmm3, dword ptr [rip + LCPI6_9] ## xmm3 = mem[0],zero,zero,zero
	xorps	xmm5, xmm5
	mov	r12, qword ptr [rbp - 72] ## 8-byte Reload
	mov	rax, r12
	.p2align	4, 0x90
LBB6_12:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [rax]   ## xmm0 = mem[0],zero,zero,zero
	movss	xmm1, dword ptr [rax + 4] ## xmm1 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm4
	addss	xmm5, xmm0
	maxss	xmm0, xmm3
	movaps	xmm3, xmm1
	minss	xmm3, xmm2
	addss	xmm5, xmm1
	maxss	xmm1, xmm0
	movss	xmm0, dword ptr [rax + 8] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm3
	addss	xmm5, xmm0
	maxss	xmm0, xmm1
	movss	xmm3, dword ptr [rax + 12] ## xmm3 = mem[0],zero,zero,zero
	movaps	xmm4, xmm3
	minss	xmm4, xmm2
	addss	xmm5, xmm3
	maxss	xmm3, xmm0
	add	rax, 16
	cmp	rax, r14
	jne	LBB6_12
## BB#13:
Ltmp27:
	movss	dword ptr [rbp - 44], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 52], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str]
	mov	edx, 30
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp28:
## BB#14:
Ltmp29:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp30:
## BB#15:
Ltmp31:
	lea	rsi, [rip + L_.str.19]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp32:
## BB#16:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI6_10]
Ltmp33:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp34:
## BB#17:
Ltmp35:
	lea	rsi, [rip + L_.str.20]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp36:
## BB#18:
Ltmp37:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp38:
## BB#19:
Ltmp39:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp40:
## BB#20:
Ltmp41:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp42:
## BB#21:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp43:
	lea	rdi, [rbp - 96]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp44:
## BB#22:
Ltmp45:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 96]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp46:
## BB#23:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp47:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r14d, eax
Ltmp48:
## BB#24:
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
Ltmp50:
	movsx	esi, r14b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp51:
## BB#25:
Ltmp52:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp53:
## BB#26:
	movss	xmm4, dword ptr [rip + LCPI6_8] ## xmm4 = mem[0],zero,zero,zero
	movss	xmm3, dword ptr [rip + LCPI6_9] ## xmm3 = mem[0],zero,zero,zero
	xorps	xmm5, xmm5
	mov	rax, qword ptr [rbp - 64] ## 8-byte Reload
	mov	rcx, qword ptr [rbp - 120] ## 8-byte Reload
	.p2align	4, 0x90
LBB6_27:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [rax]   ## xmm0 = mem[0],zero,zero,zero
	movss	xmm1, dword ptr [rax + 4] ## xmm1 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm4
	addss	xmm5, xmm0
	maxss	xmm0, xmm3
	movaps	xmm3, xmm1
	minss	xmm3, xmm2
	addss	xmm5, xmm1
	maxss	xmm1, xmm0
	movss	xmm0, dword ptr [rax + 8] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm3
	addss	xmm5, xmm0
	maxss	xmm0, xmm1
	movss	xmm3, dword ptr [rax + 12] ## xmm3 = mem[0],zero,zero,zero
	movaps	xmm4, xmm3
	minss	xmm4, xmm2
	addss	xmm5, xmm3
	maxss	xmm3, xmm0
	add	rax, 16
	cmp	rax, rcx
	jne	LBB6_27
## BB#28:
Ltmp54:
	movss	dword ptr [rbp - 44], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 52], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str.22]
	mov	edx, 31
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp55:
## BB#29:
Ltmp56:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp57:
## BB#30:
Ltmp58:
	lea	rsi, [rip + L_.str.19]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp59:
## BB#31:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI6_10]
Ltmp60:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp61:
## BB#32:
Ltmp62:
	lea	rsi, [rip + L_.str.20]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp63:
## BB#33:
Ltmp64:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp65:
## BB#34:
Ltmp66:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp67:
## BB#35:
Ltmp68:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp69:
## BB#36:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp70:
	lea	rdi, [rbp - 104]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp71:
## BB#37:
Ltmp72:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 104]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp73:
## BB#38:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp74:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r14d, eax
Ltmp75:
## BB#39:
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
Ltmp77:
	movsx	esi, r14b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp78:
## BB#40:
Ltmp79:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp80:
## BB#41:
	xor	ebx, ebx
	mov	r15, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r14, [rip + L_.str.23]
	.p2align	4, 0x90
LBB6_42:                                ## =>This Inner Loop Header: Depth=1
	mov	rax, qword ptr [rbp - 80] ## 8-byte Reload
	movss	xmm0, dword ptr [rax + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp81:
	mov	rdi, r15
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp82:
## BB#43:                               ##   in Loop: Header=BB6_42 Depth=1
Ltmp83:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r14
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp84:
## BB#44:                               ##   in Loop: Header=BB6_42 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB6_42
## BB#45:
	mov	rax, qword ptr [r15]
	add	r15, qword ptr [rax - 24]
Ltmp86:
	lea	rdi, [rbp - 112]
	mov	rsi, r15
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp87:
## BB#46:
Ltmp88:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 112]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp89:
## BB#47:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp90:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp91:
## BB#48:
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
Ltmp93:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp94:
## BB#49:
Ltmp95:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp96:
## BB#50:
	xor	ebx, ebx
	mov	r15, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r14, [rip + L_.str.23]
	.p2align	4, 0x90
LBB6_51:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [r13 + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp97:
	mov	rdi, r15
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp98:
## BB#52:                               ##   in Loop: Header=BB6_51 Depth=1
Ltmp99:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r14
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp100:
## BB#53:                               ##   in Loop: Header=BB6_51 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB6_51
## BB#54:
	mov	rax, qword ptr [r15]
	add	r15, qword ptr [rax - 24]
Ltmp102:
	lea	rdi, [rbp - 88]
	mov	rsi, r15
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp103:
## BB#55:
Ltmp104:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 88]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp105:
## BB#56:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp106:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp107:
## BB#57:
	lea	rdi, [rbp - 88]
	call	__ZNSt3__16localeD1Ev
Ltmp109:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp110:
## BB#58:
Ltmp111:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp112:
## BB#59:
	mov	rdi, qword ptr [rbp - 80] ## 8-byte Reload
	call	__ZdaPv
	mov	rdi, r13
	call	__ZdaPv
	mov	rdi, qword ptr [rbp - 64] ## 8-byte Reload
	call	__ZdlPv
	mov	rdi, r12
	call	__ZdlPv
	xor	eax, eax
	add	rsp, 88
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB6_60:
Ltmp26:
	mov	rbx, rax
	jmp	LBB6_71
LBB6_61:
Ltmp108:
	mov	rbx, rax
	lea	rdi, [rbp - 88]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB6_66
LBB6_62:
Ltmp92:
	mov	rbx, rax
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB6_66
LBB6_63:
Ltmp76:
	mov	rbx, rax
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB6_66
LBB6_64:
Ltmp49:
	mov	rbx, rax
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB6_66
LBB6_65:
Ltmp113:
	mov	rbx, rax
LBB6_66:
	mov	r12, qword ptr [rbp - 72] ## 8-byte Reload
	jmp	LBB6_70
LBB6_67:
Ltmp101:
	jmp	LBB6_69
LBB6_68:
Ltmp85:
LBB6_69:
	mov	rbx, rax
LBB6_70:
	mov	rdi, qword ptr [rbp - 64] ## 8-byte Reload
	call	__ZdlPv
LBB6_71:
	mov	rdi, r12
	call	__ZdlPv
	mov	rdi, rbx
	call	__Unwind_Resume
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table6:
Lexception0:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.ascii	"\340\001"              ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.ascii	"\335\001"              ## Call site table length
Lset0 = Lfunc_begin0-Lfunc_begin0       ## >> Call Site 1 <<
	.long	Lset0
Lset1 = Ltmp24-Lfunc_begin0             ##   Call between Lfunc_begin0 and Ltmp24
	.long	Lset1
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset2 = Ltmp24-Lfunc_begin0             ## >> Call Site 2 <<
	.long	Lset2
Lset3 = Ltmp25-Ltmp24                   ##   Call between Ltmp24 and Ltmp25
	.long	Lset3
Lset4 = Ltmp26-Lfunc_begin0             ##     jumps to Ltmp26
	.long	Lset4
	.byte	0                       ##   On action: cleanup
Lset5 = Ltmp25-Lfunc_begin0             ## >> Call Site 3 <<
	.long	Lset5
Lset6 = Ltmp27-Ltmp25                   ##   Call between Ltmp25 and Ltmp27
	.long	Lset6
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset7 = Ltmp27-Lfunc_begin0             ## >> Call Site 4 <<
	.long	Lset7
Lset8 = Ltmp44-Ltmp27                   ##   Call between Ltmp27 and Ltmp44
	.long	Lset8
Lset9 = Ltmp113-Lfunc_begin0            ##     jumps to Ltmp113
	.long	Lset9
	.byte	0                       ##   On action: cleanup
Lset10 = Ltmp45-Lfunc_begin0            ## >> Call Site 5 <<
	.long	Lset10
Lset11 = Ltmp48-Ltmp45                  ##   Call between Ltmp45 and Ltmp48
	.long	Lset11
Lset12 = Ltmp49-Lfunc_begin0            ##     jumps to Ltmp49
	.long	Lset12
	.byte	0                       ##   On action: cleanup
Lset13 = Ltmp50-Lfunc_begin0            ## >> Call Site 6 <<
	.long	Lset13
Lset14 = Ltmp71-Ltmp50                  ##   Call between Ltmp50 and Ltmp71
	.long	Lset14
Lset15 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset15
	.byte	0                       ##   On action: cleanup
Lset16 = Ltmp72-Lfunc_begin0            ## >> Call Site 7 <<
	.long	Lset16
Lset17 = Ltmp75-Ltmp72                  ##   Call between Ltmp72 and Ltmp75
	.long	Lset17
Lset18 = Ltmp76-Lfunc_begin0            ##     jumps to Ltmp76
	.long	Lset18
	.byte	0                       ##   On action: cleanup
Lset19 = Ltmp77-Lfunc_begin0            ## >> Call Site 8 <<
	.long	Lset19
Lset20 = Ltmp80-Ltmp77                  ##   Call between Ltmp77 and Ltmp80
	.long	Lset20
Lset21 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset21
	.byte	0                       ##   On action: cleanup
Lset22 = Ltmp81-Lfunc_begin0            ## >> Call Site 9 <<
	.long	Lset22
Lset23 = Ltmp84-Ltmp81                  ##   Call between Ltmp81 and Ltmp84
	.long	Lset23
Lset24 = Ltmp85-Lfunc_begin0            ##     jumps to Ltmp85
	.long	Lset24
	.byte	0                       ##   On action: cleanup
Lset25 = Ltmp86-Lfunc_begin0            ## >> Call Site 10 <<
	.long	Lset25
Lset26 = Ltmp87-Ltmp86                  ##   Call between Ltmp86 and Ltmp87
	.long	Lset26
Lset27 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset27
	.byte	0                       ##   On action: cleanup
Lset28 = Ltmp88-Lfunc_begin0            ## >> Call Site 11 <<
	.long	Lset28
Lset29 = Ltmp91-Ltmp88                  ##   Call between Ltmp88 and Ltmp91
	.long	Lset29
Lset30 = Ltmp92-Lfunc_begin0            ##     jumps to Ltmp92
	.long	Lset30
	.byte	0                       ##   On action: cleanup
Lset31 = Ltmp93-Lfunc_begin0            ## >> Call Site 12 <<
	.long	Lset31
Lset32 = Ltmp96-Ltmp93                  ##   Call between Ltmp93 and Ltmp96
	.long	Lset32
Lset33 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset33
	.byte	0                       ##   On action: cleanup
Lset34 = Ltmp97-Lfunc_begin0            ## >> Call Site 13 <<
	.long	Lset34
Lset35 = Ltmp100-Ltmp97                 ##   Call between Ltmp97 and Ltmp100
	.long	Lset35
Lset36 = Ltmp101-Lfunc_begin0           ##     jumps to Ltmp101
	.long	Lset36
	.byte	0                       ##   On action: cleanup
Lset37 = Ltmp102-Lfunc_begin0           ## >> Call Site 14 <<
	.long	Lset37
Lset38 = Ltmp103-Ltmp102                ##   Call between Ltmp102 and Ltmp103
	.long	Lset38
Lset39 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset39
	.byte	0                       ##   On action: cleanup
Lset40 = Ltmp104-Lfunc_begin0           ## >> Call Site 15 <<
	.long	Lset40
Lset41 = Ltmp107-Ltmp104                ##   Call between Ltmp104 and Ltmp107
	.long	Lset41
Lset42 = Ltmp108-Lfunc_begin0           ##     jumps to Ltmp108
	.long	Lset42
	.byte	0                       ##   On action: cleanup
Lset43 = Ltmp109-Lfunc_begin0           ## >> Call Site 16 <<
	.long	Lset43
Lset44 = Ltmp112-Ltmp109                ##   Call between Ltmp109 and Ltmp112
	.long	Lset44
Lset45 = Ltmp113-Lfunc_begin0           ##     jumps to Ltmp113
	.long	Lset45
	.byte	0                       ##   On action: cleanup
Lset46 = Ltmp112-Lfunc_begin0           ## >> Call Site 17 <<
	.long	Lset46
Lset47 = Lfunc_end0-Ltmp112             ##   Call between Ltmp112 and Lfunc_end0
	.long	Lset47
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	___clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	4, 0x90
___clang_call_terminate:                ## @__clang_call_terminate
## BB#0:
	push	rax
	call	___cxa_begin_catch
	call	__ZSt9terminatev

	.globl	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.weak_def_can_be_hidden	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.p2align	4, 0x90
__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m: ## @_ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
## BB#0:
	push	rbp
Ltmp143:
	.cfi_def_cfa_offset 16
Ltmp144:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp145:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 40
Ltmp146:
	.cfi_offset rbx, -56
Ltmp147:
	.cfi_offset r12, -48
Ltmp148:
	.cfi_offset r13, -40
Ltmp149:
	.cfi_offset r14, -32
Ltmp150:
	.cfi_offset r15, -24
	mov	r14, rdx
	mov	r15, rsi
	mov	rbx, rdi
Ltmp122:
	lea	rdi, [rbp - 80]
	mov	rsi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp123:
## BB#1:
	cmp	byte ptr [rbp - 80], 0
	je	LBB8_10
## BB#2:
	mov	rax, qword ptr [rbx]
	mov	rax, qword ptr [rax - 24]
	lea	r12, [rbx + rax]
	mov	rdi, qword ptr [rbx + rax + 40]
	mov	r13d, dword ptr [rbx + rax + 8]
	mov	eax, dword ptr [rbx + rax + 144]
	cmp	eax, -1
	jne	LBB8_7
## BB#3:
Ltmp125:
	mov	qword ptr [rbp - 64], rdi ## 8-byte Spill
	lea	rdi, [rbp - 56]
	mov	rsi, r12
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp126:
## BB#4:
Ltmp127:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 56]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp128:
## BB#5:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp129:
	mov	esi, 32
	mov	rdi, rax
	call	rcx
	mov	byte ptr [rbp - 41], al ## 1-byte Spill
Ltmp130:
## BB#6:
	lea	rdi, [rbp - 56]
	call	__ZNSt3__16localeD1Ev
	movsx	eax, byte ptr [rbp - 41] ## 1-byte Folded Reload
	mov	dword ptr [r12 + 144], eax
	mov	rdi, qword ptr [rbp - 64] ## 8-byte Reload
LBB8_7:
	add	r14, r15
	and	r13d, 176
	cmp	r13d, 32
	mov	rdx, r15
	cmove	rdx, r14
Ltmp132:
	movsx	r9d, al
	mov	rsi, r15
	mov	rcx, r14
	mov	r8, r12
	call	__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp133:
## BB#8:
	test	rax, rax
	jne	LBB8_10
## BB#9:
	mov	rax, qword ptr [rbx]
	mov	rax, qword ptr [rax - 24]
	lea	rdi, [rbx + rax]
	mov	esi, dword ptr [rbx + rax + 32]
	or	esi, 5
Ltmp134:
	call	__ZNSt3__18ios_base5clearEj
Ltmp135:
LBB8_10:
	lea	rdi, [rbp - 80]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB8_15:
	mov	rax, rbx
	add	rsp, 40
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB8_19:
Ltmp131:
	mov	r14, rax
	lea	rdi, [rbp - 56]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB8_12
LBB8_20:
Ltmp124:
	mov	r14, rax
	jmp	LBB8_13
LBB8_11:
Ltmp136:
	mov	r14, rax
LBB8_12:
	lea	rdi, [rbp - 80]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB8_13:
	mov	r15, rbx
	mov	rdi, r14
	call	___cxa_begin_catch
	mov	rax, qword ptr [rbx]
	add	r15, qword ptr [rax - 24]
Ltmp137:
	mov	rdi, r15
	call	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp138:
## BB#14:
	call	___cxa_end_catch
	jmp	LBB8_15
LBB8_16:
Ltmp139:
	mov	rbx, rax
Ltmp140:
	call	___cxa_end_catch
Ltmp141:
## BB#17:
	mov	rdi, rbx
	call	__Unwind_Resume
LBB8_18:
Ltmp142:
	mov	rdi, rax
	call	___clang_call_terminate
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table8:
Lexception1:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.byte	125                     ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	117                     ## Call site table length
Lset48 = Ltmp122-Lfunc_begin1           ## >> Call Site 1 <<
	.long	Lset48
Lset49 = Ltmp123-Ltmp122                ##   Call between Ltmp122 and Ltmp123
	.long	Lset49
Lset50 = Ltmp124-Lfunc_begin1           ##     jumps to Ltmp124
	.long	Lset50
	.byte	1                       ##   On action: 1
Lset51 = Ltmp125-Lfunc_begin1           ## >> Call Site 2 <<
	.long	Lset51
Lset52 = Ltmp126-Ltmp125                ##   Call between Ltmp125 and Ltmp126
	.long	Lset52
Lset53 = Ltmp136-Lfunc_begin1           ##     jumps to Ltmp136
	.long	Lset53
	.byte	1                       ##   On action: 1
Lset54 = Ltmp127-Lfunc_begin1           ## >> Call Site 3 <<
	.long	Lset54
Lset55 = Ltmp130-Ltmp127                ##   Call between Ltmp127 and Ltmp130
	.long	Lset55
Lset56 = Ltmp131-Lfunc_begin1           ##     jumps to Ltmp131
	.long	Lset56
	.byte	1                       ##   On action: 1
Lset57 = Ltmp132-Lfunc_begin1           ## >> Call Site 4 <<
	.long	Lset57
Lset58 = Ltmp135-Ltmp132                ##   Call between Ltmp132 and Ltmp135
	.long	Lset58
Lset59 = Ltmp136-Lfunc_begin1           ##     jumps to Ltmp136
	.long	Lset59
	.byte	1                       ##   On action: 1
Lset60 = Ltmp135-Lfunc_begin1           ## >> Call Site 5 <<
	.long	Lset60
Lset61 = Ltmp137-Ltmp135                ##   Call between Ltmp135 and Ltmp137
	.long	Lset61
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset62 = Ltmp137-Lfunc_begin1           ## >> Call Site 6 <<
	.long	Lset62
Lset63 = Ltmp138-Ltmp137                ##   Call between Ltmp137 and Ltmp138
	.long	Lset63
Lset64 = Ltmp139-Lfunc_begin1           ##     jumps to Ltmp139
	.long	Lset64
	.byte	0                       ##   On action: cleanup
Lset65 = Ltmp138-Lfunc_begin1           ## >> Call Site 7 <<
	.long	Lset65
Lset66 = Ltmp140-Ltmp138                ##   Call between Ltmp138 and Ltmp140
	.long	Lset66
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset67 = Ltmp140-Lfunc_begin1           ## >> Call Site 8 <<
	.long	Lset67
Lset68 = Ltmp141-Ltmp140                ##   Call between Ltmp140 and Ltmp141
	.long	Lset68
Lset69 = Ltmp142-Lfunc_begin1           ##     jumps to Ltmp142
	.long	Lset69
	.byte	1                       ##   On action: 1
Lset70 = Ltmp141-Lfunc_begin1           ## >> Call Site 9 <<
	.long	Lset70
Lset71 = Lfunc_end1-Ltmp141             ##   Call between Ltmp141 and Lfunc_end1
	.long	Lset71
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.byte	1                       ## >> Action Record 1 <<
                                        ##   Catch TypeInfo 1
	.byte	0                       ##   No further actions
                                        ## >> Catch TypeInfos <<
	.long	0                       ## TypeInfo 1
	.p2align	2

	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.globl	__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.weak_def_can_be_hidden	__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.p2align	4, 0x90
__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_: ## @_ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
## BB#0:
	push	rbp
Ltmp154:
	.cfi_def_cfa_offset 16
Ltmp155:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp156:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 40
Ltmp157:
	.cfi_offset rbx, -56
Ltmp158:
	.cfi_offset r12, -48
Ltmp159:
	.cfi_offset r13, -40
Ltmp160:
	.cfi_offset r14, -32
Ltmp161:
	.cfi_offset r15, -24
	mov	r15, rcx
	mov	r12, rdx
	mov	rbx, rdi
	test	rbx, rbx
	je	LBB9_12
## BB#1:
	mov	rax, r15
	sub	rax, rsi
	mov	qword ptr [rbp - 80], r8 ## 8-byte Spill
	mov	rcx, qword ptr [r8 + 24]
	xor	r13d, r13d
	sub	rcx, rax
	cmovg	r13, rcx
	mov	r14, r12
	sub	r14, rsi
	test	r14, r14
	jle	LBB9_3
## BB#2:
	mov	rax, qword ptr [rbx]
	mov	rdi, rbx
	mov	rdx, r14
	mov	qword ptr [rbp - 72], r15 ## 8-byte Spill
	mov	r15, r12
	mov	r12d, r9d
	call	qword ptr [rax + 96]
	mov	r9d, r12d
	mov	r12, r15
	mov	r15, qword ptr [rbp - 72] ## 8-byte Reload
	cmp	rax, r14
	jne	LBB9_12
LBB9_3:
	test	r13, r13
	jle	LBB9_6
## BB#4:
	xorps	xmm0, xmm0
	movaps	xmmword ptr [rbp - 64], xmm0
	mov	qword ptr [rbp - 48], 0
	movsx	edx, r9b
	lea	rdi, [rbp - 64]
	mov	rsi, r13
	call	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6__initEmc
	test	byte ptr [rbp - 64], 1
	lea	rsi, [rbp - 63]
	cmovne	rsi, qword ptr [rbp - 48]
	mov	rax, qword ptr [rbx]
	mov	rax, qword ptr [rax + 96]
Ltmp151:
	mov	rdi, rbx
	mov	rdx, r13
	call	rax
	mov	r14, rax
Ltmp152:
## BB#5:
	lea	rdi, [rbp - 64]
	call	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev
	cmp	r14, r13
	jne	LBB9_12
LBB9_6:
	sub	r15, r12
	test	r15, r15
	jle	LBB9_8
## BB#7:
	mov	rax, qword ptr [rbx]
	mov	rdi, rbx
	mov	rsi, r12
	mov	rdx, r15
	call	qword ptr [rax + 96]
	cmp	rax, r15
	jne	LBB9_12
LBB9_8:
	mov	rax, qword ptr [rbp - 80] ## 8-byte Reload
	mov	qword ptr [rax + 24], 0
	jmp	LBB9_13
LBB9_12:
	xor	ebx, ebx
LBB9_13:
	mov	rax, rbx
	add	rsp, 40
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB9_14:
Ltmp153:
	mov	rbx, rax
	lea	rdi, [rbp - 64]
	call	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev
	mov	rdi, rbx
	call	__Unwind_Resume
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table9:
Lexception2:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.byte	41                      ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	39                      ## Call site table length
Lset72 = Lfunc_begin2-Lfunc_begin2      ## >> Call Site 1 <<
	.long	Lset72
Lset73 = Ltmp151-Lfunc_begin2           ##   Call between Lfunc_begin2 and Ltmp151
	.long	Lset73
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset74 = Ltmp151-Lfunc_begin2           ## >> Call Site 2 <<
	.long	Lset74
Lset75 = Ltmp152-Ltmp151                ##   Call between Ltmp151 and Ltmp152
	.long	Lset75
Lset76 = Ltmp153-Lfunc_begin2           ##     jumps to Ltmp153
	.long	Lset76
	.byte	0                       ##   On action: cleanup
Lset77 = Ltmp152-Lfunc_begin2           ## >> Call Site 3 <<
	.long	Lset77
Lset78 = Lfunc_end2-Ltmp152             ##   Call between Ltmp152 and Lfunc_end2
	.long	Lset78
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI10_0:
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
LCPI10_1:
	.long	255                     ## 0xff
	.long	255                     ## 0xff
	.long	255                     ## 0xff
	.long	255                     ## 0xff
LCPI10_2:
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
LCPI10_3:
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
LCPI10_4:
	.long	3                       ## 0x3
	.long	2                       ## 0x2
	.long	1                       ## 0x1
	.long	0                       ## 0x0
LCPI10_5:
	.long	0                       ## 0x0
	.long	1065353216              ## 0x3f800000
	.long	1073741824              ## 0x40000000
	.long	1077936128              ## 0x40400000
LCPI10_6:
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.section	__TEXT,__StaticInit,regular,pure_instructions
	.p2align	4, 0x90
__GLOBAL__sub_I_memory_access_tests.cpp: ## @_GLOBAL__sub_I_memory_access_tests.cpp
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp162:
	.cfi_def_cfa_offset 16
Ltmp163:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp164:
	.cfi_def_cfa_register rbp
	movaps	xmm0, xmmword ptr [rip + LCPI10_0] ## xmm0 = [2147483647,2147483647,2147483647,2147483647]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_abs_maskE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI10_1] ## xmm0 = [255,255,255,255]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_rnd_maskE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI10_2] ## xmm0 = [1258291200,1258291200,1258291200,1258291200]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL16vecm_no_fractionE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI10_3] ## xmm0 = [1065353216,1065353216,1065353216,1065353216]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL8vecm_oneE], xmm0
	cvtps2dq	xmm1, xmmword ptr [rip + LCPI10_3]
	movapd	xmmword ptr [rip + __ZN3zap5maths4simdL8veci_oneE], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI10_4] ## xmm1 = [3,2,1,0]
	movaps	xmmword ptr [rip + __ZL6seq_vi], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI10_5] ## xmm1 = [0,1065353216,1073741824,1077936128]
	movaps	xmmword ptr [rip + __ZL6seq_vf], xmm1
	xorps	xmm1, xmm1
	movaps	xmmword ptr [rip + __ZL7zero_vi], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI10_6] ## xmm1 = [1,1,1,1]
	movaps	xmmword ptr [rip + __ZL6one_vi], xmm1
	movaps	xmmword ptr [rip + __ZL6one_vf], xmm0
	pop	rbp
	ret
	.cfi_endproc

.zerofill __DATA,__bss,__ZN3zap5maths4simdL13vecm_abs_maskE,16,4 ## @_ZN3zap5maths4simdL13vecm_abs_maskE
.zerofill __DATA,__bss,__ZN3zap5maths4simdL13vecm_rnd_maskE,16,4 ## @_ZN3zap5maths4simdL13vecm_rnd_maskE
.zerofill __DATA,__bss,__ZN3zap5maths4simdL16vecm_no_fractionE,16,4 ## @_ZN3zap5maths4simdL16vecm_no_fractionE
.zerofill __DATA,__bss,__ZN3zap5maths4simdL8vecm_oneE,16,4 ## @_ZN3zap5maths4simdL8vecm_oneE
.zerofill __DATA,__bss,__ZN3zap5maths4simdL8veci_oneE,16,4 ## @_ZN3zap5maths4simdL8veci_oneE
.zerofill __DATA,__bss,__ZL6seq_vi,16,4 ## @_ZL6seq_vi
.zerofill __DATA,__bss,__ZL6seq_vf,16,4 ## @_ZL6seq_vf
.zerofill __DATA,__bss,__ZL7zero_vi,16,4 ## @_ZL7zero_vi
.zerofill __DATA,__bss,__ZL6one_vi,16,4 ## @_ZL6one_vi
.zerofill __DATA,__bss,__ZL6one_vf,16,4 ## @_ZL6one_vf
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Time measured for CPU, total: "

L_.str.19:                              ## @.str.19
	.asciz	", avg: "

L_.str.20:                              ## @.str.20
	.asciz	", min: "

L_.str.21:                              ## @.str.21
	.asciz	", max: "

L_.str.22:                              ## @.str.22
	.asciz	"Time measured for SIMD, total: "

L_.str.23:                              ## @.str.23
	.asciz	", "

	.section	__DATA,__mod_init_func,mod_init_funcs
	.p2align	3
	.quad	__GLOBAL__sub_I_memory_access_tests.cpp

.subsections_via_symbols
