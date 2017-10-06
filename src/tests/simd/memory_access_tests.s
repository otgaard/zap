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
	.globl	__Z6lerp_cfff
	.p2align	4, 0x90
__Z6lerp_cfff:                          ## @_Z6lerp_cfff
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
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
LCPI3_2:
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
	mov	r14, rdx
	mov	r15d, esi
	mov	ebx, edi
	mov	eax, r15d
	imul	eax, ebx
	movsxd	rsi, eax
	shl	rsi, 2
	mov	rdi, r14
	call	___bzero
	test	r15d, r15d
	je	LBB3_10
## BB#1:
	test	ebx, ebx
	jle	LBB3_10
## BB#2:
	lea	eax, [rbx - 1]
	cvtsi2ss	xmm1, eax
	movss	xmm0, dword ptr [rip + LCPI3_0] ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, xmm1
	movaps	xmm1, xmm0
	shufps	xmm1, xmm1, 0           ## xmm1 = xmm1[0,0,0,0]
	mulps	xmm1, xmmword ptr [rip + __ZL6seq_vf]
	movsxd	rax, ebx
	mov	r8d, r15d
	lea	r9, [rax - 1]
	shr	r9, 2
	mov	r10d, r9d
	and	r10d, 1
	xorps	xmm2, xmm2
	mulss	xmm2, xmm0
	shufps	xmm2, xmm2, 0           ## xmm2 = xmm2[0,0,0,0]
	addps	xmm2, xmm1
	movaps	xmm3, xmmword ptr [rip + LCPI3_1] ## xmm3 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	movaps	xmm5, xmm3
	subps	xmm5, xmm2
	xorps	xmm4, xmm4
	mulps	xmm5, xmm4
	mulps	xmm2, xmmword ptr [rip + LCPI3_2]
	addps	xmm2, xmm5
	lea	r11, [4*rax]
	xor	ebx, ebx
	movaps	xmm5, xmmword ptr [rip + LCPI3_2] ## xmm5 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	mov	r15, r14
	.p2align	4, 0x90
LBB3_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB3_8 Depth 2
	test	r10, r10
	jne	LBB3_4
## BB#5:                                ##   in Loop: Header=BB3_3 Depth=1
	mov	rdx, rbx
	imul	rdx, rax
	movaps	xmmword ptr [r14 + 4*rdx], xmm2
	mov	edx, 4
	jmp	LBB3_6
	.p2align	4, 0x90
LBB3_4:                                 ##   in Loop: Header=BB3_3 Depth=1
	xor	edx, edx
LBB3_6:                                 ##   in Loop: Header=BB3_3 Depth=1
	test	r9, r9
	je	LBB3_9
## BB#7:                                ##   in Loop: Header=BB3_3 Depth=1
	lea	rsi, [r15 + 4*rdx]
	xor	edi, edi
	.p2align	4, 0x90
LBB3_8:                                 ##   Parent Loop BB3_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	lea	rcx, [rdx + rdi]
	xorps	xmm6, xmm6
	cvtsi2ss	xmm6, ecx
	mulss	xmm6, xmm0
	shufps	xmm6, xmm6, 0           ## xmm6 = xmm6[0,0,0,0]
	addps	xmm6, xmm1
	movaps	xmm7, xmm3
	subps	xmm7, xmm6
	mulps	xmm7, xmm4
	mulps	xmm6, xmm5
	addps	xmm6, xmm7
	movaps	xmmword ptr [rsi + 4*rdi], xmm6
	add	ecx, 4
	xorps	xmm6, xmm6
	cvtsi2ss	xmm6, ecx
	mulss	xmm6, xmm0
	shufps	xmm6, xmm6, 0           ## xmm6 = xmm6[0,0,0,0]
	addps	xmm6, xmm1
	movaps	xmm7, xmm3
	subps	xmm7, xmm6
	mulps	xmm7, xmm4
	mulps	xmm6, xmm5
	addps	xmm6, xmm7
	movaps	xmmword ptr [rsi + 4*rdi + 16], xmm6
	lea	rcx, [rdx + rdi + 8]
	add	rdi, 8
	cmp	rcx, rax
	jl	LBB3_8
LBB3_9:                                 ##   in Loop: Header=BB3_3 Depth=1
	inc	rbx
	add	r15, r11
	cmp	rbx, r8
	jne	LBB3_3
LBB3_10:
	add	rsp, 8
	pop	rbx
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI4_0:
	.long	1065353216              ## float 1
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z13render_noiseAiiPf
	.p2align	4, 0x90
__Z13render_noiseAiiPf:                 ## @_Z13render_noiseAiiPf
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
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
Ltmp21:
	.cfi_offset rbx, -56
Ltmp22:
	.cfi_offset r12, -48
Ltmp23:
	.cfi_offset r13, -40
Ltmp24:
	.cfi_offset r14, -32
Ltmp25:
	.cfi_offset r15, -24
                                        ## kill: %EDI<def> %EDI<kill> %RDI<def>
	test	esi, esi
	je	LBB4_6
## BB#1:
	test	edi, edi
	je	LBB4_6
## BB#2:
	cvtsi2ss	xmm2, edi
	movss	xmm9, dword ptr [rip + LCPI4_0] ## xmm9 = mem[0],zero,zero,zero
	movaps	xmm1, xmm9
	divss	xmm1, xmm2
	cvtsi2ss	xmm3, esi
	movaps	xmm8, xmm9
	divss	xmm8, xmm3
	lea	eax, [rdi - 1]
	lea	rax, [4*rax + 4]
	mov	qword ptr [rbp - 48], rax ## 8-byte Spill
	xor	r9d, r9d
	xorps	xmm3, xmm3
	lea	r14, [rip + __ZL12global_state]
	.p2align	4, 0x90
LBB4_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB4_4 Depth 2
	xorps	xmm4, xmm4
	cvtsi2ss	xmm4, r9d
	mulss	xmm4, xmm8
	cvttss2si	eax, xmm4
	ucomiss	xmm3, xmm4
	seta	r10b
	xorps	xmm5, xmm5
	cvtsi2ss	xmm5, eax
	ucomiss	xmm5, xmm4
	setp	bl
	setne	cl
	or	cl, bl
	and	cl, r10b
	movzx	ecx, cl
	mov	ebx, eax
	sub	ebx, ecx
	neg	ecx
	xorps	xmm5, xmm5
	cvtsi2ss	xmm5, ebx
	subss	xmm4, xmm5
	movzx	r10d, bl
	lea	eax, [rax + rcx + 1]
	movzx	r11d, al
	movaps	xmm5, xmm9
	subss	xmm5, xmm4
	mov	r12, rdx
	xor	r13d, r13d
	.p2align	4, 0x90
LBB4_4:                                 ##   Parent Loop BB4_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	xorps	xmm6, xmm6
	cvtsi2ss	xmm6, r13d
	mulss	xmm6, xmm1
	cvttss2si	eax, xmm6
	ucomiss	xmm3, xmm6
	seta	r15b
	xorps	xmm7, xmm7
	cvtsi2ss	xmm7, eax
	ucomiss	xmm7, xmm6
	setp	cl
	setne	bl
	or	bl, cl
	and	bl, r15b
	movzx	ecx, bl
	sub	eax, ecx
	xorps	xmm7, xmm7
	cvtsi2ss	xmm7, eax
	subss	xmm6, xmm7
	movzx	ecx, byte ptr [r10 + r14 + 1]
	lea	ebx, [rcx + rax]
	movzx	ebx, bl
	movzx	r15d, byte ptr [rbx + r14 + 1]
	lea	ecx, [rcx + rax + 1]
	movzx	ecx, cl
	movzx	ecx, byte ptr [rcx + r14 + 1]
	movzx	ebx, byte ptr [r11 + r14 + 1]
	lea	r8d, [rbx + rax + 1]
                                        ## kill: %EAX<def> %EAX<kill> %RAX<kill>
	add	eax, ebx
	movzx	eax, al
	movzx	eax, byte ptr [rax + r14 + 1]
	movzx	ebx, r8b
	movzx	ebx, byte ptr [rbx + r14 + 1]
	movaps	xmm7, xmm9
	subss	xmm7, xmm6
	movss	xmm2, dword ptr [r14 + 4*r15 + 272] ## xmm2 = mem[0],zero,zero,zero
	mulss	xmm2, xmm7
	movss	xmm0, dword ptr [r14 + 4*rcx + 272] ## xmm0 = mem[0],zero,zero,zero
	mulss	xmm0, xmm6
	addss	xmm0, xmm2
	mulss	xmm7, dword ptr [r14 + 4*rax + 272]
	mulss	xmm6, dword ptr [r14 + 4*rbx + 272]
	addss	xmm6, xmm7
	mulss	xmm0, xmm5
	mulss	xmm6, xmm4
	addss	xmm6, xmm0
	movss	dword ptr [r12], xmm6
	inc	r13d
	add	r12, 4
	cmp	edi, r13d
	jne	LBB4_4
## BB#5:                                ##   in Loop: Header=BB4_3 Depth=1
	inc	r9d
	add	rdx, qword ptr [rbp - 48] ## 8-byte Folded Reload
	cmp	r9d, esi
	jne	LBB4_3
LBB4_6:
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI5_0:
	.long	1065353216              ## float 1
LCPI5_3:
	.long	0                       ## float 0
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI5_1:
	.long	0                       ## float 0
	.long	1065353216              ## float 1
	.long	1073741824              ## float 2
	.long	1077936128              ## float 3
LCPI5_2:
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z13render_noiseBiiPf
	.p2align	4, 0x90
__Z13render_noiseBiiPf:                 ## @_Z13render_noiseBiiPf
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp26:
	.cfi_def_cfa_offset 16
Ltmp27:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp28:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
Ltmp29:
	.cfi_offset rbx, -56
Ltmp30:
	.cfi_offset r12, -48
Ltmp31:
	.cfi_offset r13, -40
Ltmp32:
	.cfi_offset r14, -32
Ltmp33:
	.cfi_offset r15, -24
	mov	qword ptr [rbp - 56], rdx ## 8-byte Spill
	stmxcsr	dword ptr [rbp - 44]
	mov	ecx, -24577
	mov	eax, dword ptr [rbp - 44]
	and	eax, ecx
	or	eax, 8192
	mov	dword ptr [rbp - 44], eax
	ldmxcsr	dword ptr [rbp - 44]
	test	esi, esi
	je	LBB5_6
## BB#1:
	cvtsi2ss	xmm0, edi
	movss	xmm9, dword ptr [rip + LCPI5_0] ## xmm9 = mem[0],zero,zero,zero
	movaps	xmm11, xmm9
	divss	xmm11, xmm0
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, esi
	divss	xmm9, xmm0
	shufps	xmm11, xmm11, 0         ## xmm11 = xmm11[0,0,0,0]
	movaps	xmm12, xmmword ptr [rip + LCPI5_1] ## xmm12 = [0.000000e+00,1.000000e+00,2.000000e+00,3.000000e+00]
	mulps	xmm12, xmm11
	mov	eax, edi
	sar	eax, 31
	shr	eax, 30
	add	eax, edi
	sar	eax, 2
	mov	qword ptr [rbp - 64], rax ## 8-byte Spill
	movsxd	rax, edi
	add	edi, 3
	mov	dword ptr [rbp - 48], edi ## 4-byte Spill
	mov	ecx, esi
	mov	qword ptr [rbp - 72], rcx ## 8-byte Spill
	shl	rax, 2
	mov	qword ptr [rbp - 80], rax ## 8-byte Spill
	xor	ecx, ecx
	movaps	xmm10, xmmword ptr [rip + LCPI5_2] ## xmm10 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	lea	r15, [rip + __ZL12global_state]
	.p2align	4, 0x90
LBB5_2:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB5_4 Depth 2
	mov	qword ptr [rbp - 88], rcx ## 8-byte Spill
	xorps	xmm5, xmm5
	cvtsi2ss	xmm5, ecx
	mulss	xmm5, xmm9
	cvttss2si	eax, xmm5
	xorps	xmm0, xmm0
	ucomiss	xmm0, xmm5
	seta	cl
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, eax
	ucomiss	xmm0, xmm5
	setp	bl
	setne	dl
	cmp	dword ptr [rbp - 48], 7 ## 4-byte Folded Reload
	jb	LBB5_5
## BB#3:                                ##   in Loop: Header=BB5_2 Depth=1
	or	dl, bl
	and	cl, dl
	movzx	ecx, cl
	mov	edx, eax
	sub	edx, ecx
	neg	ecx
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, edx
	subss	xmm5, xmm0
	shufps	xmm5, xmm5, 0           ## xmm5 = xmm5[0,0,0,0]
	movd	xmm0, edx
	pshufd	xmm13, xmm0, 0          ## xmm13 = xmm0[0,0,0,0]
	lea	eax, [rax + rcx + 1]
	movd	xmm0, eax
	pshufd	xmm14, xmm0, 0          ## xmm14 = xmm0[0,0,0,0]
	movaps	xmm15, xmm10
	subps	xmm15, xmm5
	xor	r11d, r11d
	mov	r12, qword ptr [rbp - 64] ## 8-byte Reload
	mov	r14, qword ptr [rbp - 56] ## 8-byte Reload
	.p2align	4, 0x90
LBB5_4:                                 ##   Parent Loop BB5_2 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movd	xmm0, r11d
	pshufd	xmm0, xmm0, 0           ## xmm0 = xmm0[0,0,0,0]
	cvtdq2ps	xmm0, xmm0
	mulps	xmm0, xmm11
	addps	xmm0, xmm12
	movaps	xmm1, xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_abs_maskE]
	andps	xmm1, xmm0
	movdqa	xmm3, xmmword ptr [rip + __ZN3zap5maths4simdL16vecm_no_fractionE]
	cvttps2dq	xmm2, xmm0
	pcmpgtd	xmm3, xmm1
	cvtdq2ps	xmm1, xmm2
	movaps	xmm2, xmm0
	cmpltps	xmm2, xmm1
	cvtdq2ps	xmm4, xmm2
	addps	xmm4, xmm1
	movdqa	xmm1, xmmword ptr [rip + __ZN3zap5maths4simdL8veci_oneE]
	movdqa	xmm2, xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_rnd_maskE]
	movdqa	xmm6, xmm2
	pand	xmm6, xmm13
	andps	xmm4, xmm3
	pextrq	rax, xmm6, 1
	movq	rcx, xmm6
	movsxd	rdx, ecx
	andnps	xmm3, xmm0
	movzx	edx, byte ptr [rdx + r15 + 1]
	sar	rcx, 32
	movzx	ecx, byte ptr [rcx + r15 + 1]
	orps	xmm3, xmm4
	movsxd	rbx, eax
	movzx	ebx, byte ptr [rbx + r15 + 1]
	sar	rax, 32
	cvtps2dq	xmm6, xmm3
	movzx	eax, byte ptr [rax + r15 + 1]
	movd	xmm7, eax
	pinsrd	xmm7, ebx, 1
	paddd	xmm1, xmm6
	pinsrd	xmm7, ecx, 2
	pinsrd	xmm7, edx, 3
	movdqa	xmm4, xmm7
	paddd	xmm4, xmm6
	pand	xmm4, xmm2
	pextrq	rax, xmm4, 1
	movq	rbx, xmm4
	movsxd	rsi, ebx
	sar	rbx, 32
	movsxd	r13, eax
	sar	rax, 32
	paddd	xmm7, xmm1
	movzx	r10d, byte ptr [rax + r15 + 1]
	pand	xmm7, xmm2
	pextrq	r9, xmm7, 1
	movq	r8, xmm7
	movdqa	xmm4, xmm2
	movzx	r13d, byte ptr [r13 + r15 + 1]
	pand	xmm4, xmm14
	pextrq	rcx, xmm4, 1
	movsxd	rdx, ecx
	movzx	ebx, byte ptr [rbx + r15 + 1]
	sar	rcx, 32
	movzx	ecx, byte ptr [rcx + r15 + 1]
	movd	xmm7, ecx
	movzx	ecx, byte ptr [rsi + r15 + 1]
	movq	rdi, xmm4
	movzx	edx, byte ptr [rdx + r15 + 1]
	pinsrd	xmm7, edx, 1
	mov	r10d, dword ptr [r15 + 4*r10 + 272]
	movsxd	rax, edi
	sar	rdi, 32
	movzx	edi, byte ptr [rdi + r15 + 1]
	mov	edx, dword ptr [r15 + 4*r13 + 272]
	pinsrd	xmm7, edi, 2
	movsxd	rdi, r8d
	sar	r8, 32
	mov	r13d, dword ptr [r15 + 4*rbx + 272]
	movzx	eax, byte ptr [rax + r15 + 1]
	pinsrd	xmm7, eax, 3
	movsxd	rax, r9d
	mov	ecx, dword ptr [r15 + 4*rcx + 272]
	sar	r9, 32
	movzx	ebx, byte ptr [r9 + r15 + 1]
	movzx	eax, byte ptr [rax + r15 + 1]
	shl	rdx, 32
	movzx	esi, byte ptr [r8 + r15 + 1]
	movzx	edi, byte ptr [rdi + r15 + 1]
	mov	r8d, dword ptr [r15 + 4*rbx + 272]
	shl	rcx, 32
	mov	ebx, dword ptr [r15 + 4*rax + 272]
	mov	r9d, dword ptr [r15 + 4*rsi + 272]
	mov	eax, dword ptr [r15 + 4*rdi + 272]
	or	rdx, r10
	paddd	xmm6, xmm7
	pand	xmm6, xmm2
	pextrq	rdi, xmm6, 1
	or	rcx, r13
	movsxd	rsi, edi
	sar	rdi, 32
	movzx	edi, byte ptr [rdi + r15 + 1]
	shl	rbx, 32
	movzx	esi, byte ptr [rsi + r15 + 1]
	mov	edi, dword ptr [r15 + 4*rdi + 272]
	mov	esi, dword ptr [r15 + 4*rsi + 272]
	shl	rax, 32
	shl	rsi, 32
	or	rsi, rdi
	movq	rdi, xmm6
	movq	xmm4, rcx
	movsxd	rcx, edi
	sar	rdi, 32
	movzx	edi, byte ptr [rdi + r15 + 1]
	movq	xmm6, rdx
	movzx	ecx, byte ptr [rcx + r15 + 1]
	mov	edx, dword ptr [r15 + 4*rdi + 272]
	mov	ecx, dword ptr [r15 + 4*rcx + 272]
	or	rbx, r8
	shl	rcx, 32
	or	rcx, rdx
	paddd	xmm7, xmm1
	or	rax, r9
	pand	xmm7, xmm2
	pextrq	rdx, xmm7, 1
	movq	rdi, xmm7
	movq	xmm2, rcx
	movsxd	rcx, edx
	sar	rdx, 32
	movzx	edx, byte ptr [rdx + r15 + 1]
	movq	xmm1, rsi
	movzx	ecx, byte ptr [rcx + r15 + 1]
	mov	edx, dword ptr [r15 + 4*rdx + 272]
	mov	ecx, dword ptr [r15 + 4*rcx + 272]
	movq	xmm7, rax
	shl	rcx, 32
	or	rcx, rdx
	movsxd	rax, edi
	punpcklqdq	xmm6, xmm4      ## xmm6 = xmm6[0],xmm4[0]
	sar	rdi, 32
	movzx	edx, byte ptr [rdi + r15 + 1]
	movzx	eax, byte ptr [rax + r15 + 1]
	movq	xmm4, rbx
	mov	edx, dword ptr [r15 + 4*rdx + 272]
	mov	eax, dword ptr [r15 + 4*rax + 272]
	shl	rax, 32
	punpcklqdq	xmm1, xmm2      ## xmm1 = xmm1[0],xmm2[0]
	or	rax, rdx
	movq	xmm2, rax
	movq	xmm8, rcx
	punpcklqdq	xmm4, xmm7      ## xmm4 = xmm4[0],xmm7[0]
	punpcklqdq	xmm8, xmm2      ## xmm8 = xmm8[0],xmm2[0]
	subps	xmm0, xmm3
	movaps	xmm2, xmm10
	subps	xmm2, xmm0
	mulps	xmm6, xmm2
	mulps	xmm4, xmm0
	mulps	xmm2, xmm1
	mulps	xmm0, xmm8
	addps	xmm4, xmm6
	addps	xmm0, xmm2
	mulps	xmm4, xmm15
	mulps	xmm0, xmm5
	addps	xmm0, xmm4
	movaps	xmmword ptr [r14], xmm0
	add	r14, 16
	add	r11d, 4
	dec	r12
	jne	LBB5_4
LBB5_5:                                 ##   in Loop: Header=BB5_2 Depth=1
	mov	rcx, qword ptr [rbp - 88] ## 8-byte Reload
	inc	rcx
	mov	rax, qword ptr [rbp - 80] ## 8-byte Reload
	add	qword ptr [rbp - 56], rax ## 8-byte Folded Spill
	cmp	rcx, qword ptr [rbp - 72] ## 8-byte Folded Reload
	jne	LBB5_2
LBB5_6:
	stmxcsr	dword ptr [rbp - 44]
	mov	eax, -24577
	and	eax, dword ptr [rbp - 44]
	mov	dword ptr [rbp - 44], eax
	ldmxcsr	dword ptr [rbp - 44]
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI6_0:
	.quad	42949672968             ## 0xa00000008
	.quad	60129542156             ## 0xe0000000c
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z5testAv
	.p2align	4, 0x90
__Z5testAv:                             ## @_Z5testAv
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp34:
	.cfi_def_cfa_offset 16
Ltmp35:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp36:
	.cfi_def_cfa_register rbp
	movaps	xmm0, xmmword ptr [rip + LCPI6_0] ## xmm0 = [42949672968,60129542156]
	pop	rbp
	ret
	.cfi_endproc

	.globl	__Z5testBv
	.p2align	4, 0x90
__Z5testBv:                             ## @_Z5testBv
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp37:
	.cfi_def_cfa_offset 16
Ltmp38:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp39:
	.cfi_def_cfa_register rbp
	movabs	rax, 42949672968
	movabs	rdx, 60129542156
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI8_0:
	.long	0                       ## 0x0
	.long	1                       ## 0x1
	.long	2                       ## 0x2
	.long	3                       ## 0x3
LCPI8_1:
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
	.long	981475336               ## float 9.77517105E-4
LCPI8_2:
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
	.long	1065353216              ## float 1
LCPI8_3:
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
	.long	1092616192              ## float 10
LCPI8_4:
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
	.long	4                       ## 0x4
LCPI8_5:
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.long	8                       ## 0x8
	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI8_6:
	.long	1315859240              ## float 1.0E+9
LCPI8_7:
	.long	981475336               ## float 9.77517105E-4
LCPI8_8:
	.long	4286578687              ## float -3.40282347E+38
LCPI8_9:
	.long	2139095039              ## float 3.40282347E+38
LCPI8_10:
	.long	1176256512              ## float 1.0E+4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z8test_onev
	.p2align	4, 0x90
__Z8test_onev:                          ## @_Z8test_onev
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
## BB#0:
	push	rbp
Ltmp130:
	.cfi_def_cfa_offset 16
Ltmp131:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp132:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 88
Ltmp133:
	.cfi_offset rbx, -56
Ltmp134:
	.cfi_offset r12, -48
Ltmp135:
	.cfi_offset r13, -40
Ltmp136:
	.cfi_offset r14, -32
Ltmp137:
	.cfi_offset r15, -24
	mov	edi, 4194304
	call	__Znam
	mov	r15, rax
	mov	edi, 4194304
	call	__Znam
	mov	rbx, rax
	mov	edi, 40000
	call	__Znwm
	mov	r12, rax
	mov	esi, 40000
	mov	rdi, r12
	call	___bzero
Ltmp40:
	mov	edi, 40000
	call	__Znwm
Ltmp41:
## BB#1:
	mov	qword ptr [rbp - 72], r12 ## 8-byte Spill
	mov	r14, r12
	add	r14, 40000
	mov	rcx, rax
	add	rcx, 40000
	mov	qword ptr [rbp - 128], rcx ## 8-byte Spill
	mov	esi, 40000
	mov	qword ptr [rbp - 64], rax ## 8-byte Spill
	mov	rdi, rax
	call	___bzero
	call	__ZNSt3__16chrono12steady_clock3nowEv
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	qword ptr [rbp - 88], r15 ## 8-byte Spill
	add	r15, 16
	mov	qword ptr [rbp - 80], rbx ## 8-byte Spill
	mov	r12, rbx
	add	r12, 16
	xor	r13d, r13d
	.p2align	4, 0x90
LBB8_2:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB8_3 Depth 2
                                        ##       Child Loop BB8_4 Depth 3
                                        ##     Child Loop BB8_7 Depth 2
                                        ##       Child Loop BB8_8 Depth 3
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	rbx, rax
	mov	esi, 4194304
	mov	rdi, qword ptr [rbp - 88] ## 8-byte Reload
	call	___bzero
	mov	rax, r15
	xor	ecx, ecx
	movaps	xmm3, xmmword ptr [rip + LCPI8_0] ## xmm3 = [0,1,2,3]
	movaps	xmm8, xmm3
	movaps	xmm4, xmmword ptr [rip + LCPI8_1] ## xmm4 = [9.775171e-04,9.775171e-04,9.775171e-04,9.775171e-04]
	movaps	xmm5, xmmword ptr [rip + LCPI8_2] ## xmm5 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	xorps	xmm6, xmm6
	movaps	xmm7, xmmword ptr [rip + LCPI8_3] ## xmm7 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	movdqa	xmm3, xmmword ptr [rip + LCPI8_4] ## xmm3 = [4,4,4,4]
	movdqa	xmm9, xmm3
	movdqa	xmm3, xmmword ptr [rip + LCPI8_5] ## xmm3 = [8,8,8,8]
	.p2align	4, 0x90
LBB8_3:                                 ##   Parent Loop BB8_2 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB8_4 Depth 3
	mov	rdx, rax
	mov	esi, 1024
	movaps	xmm0, xmm8
	.p2align	4, 0x90
LBB8_4:                                 ##   Parent Loop BB8_2 Depth=1
                                        ##     Parent Loop BB8_3 Depth=2
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
	jne	LBB8_4
## BB#5:                                ##   in Loop: Header=BB8_3 Depth=2
	inc	rcx
	add	rax, 4096
	cmp	rcx, 1024
	jne	LBB8_3
## BB#6:                                ##   in Loop: Header=BB8_2 Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	sub	rax, rbx
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, dword ptr [rip + LCPI8_6]
	mov	rax, qword ptr [rbp - 72] ## 8-byte Reload
	movss	dword ptr [rax + 4*r13], xmm0
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	rbx, rax
	mov	esi, 4194304
	mov	rdi, qword ptr [rbp - 80] ## 8-byte Reload
	call	___bzero
	movaps	xmm5, xmmword ptr [rip + LCPI8_3] ## xmm5 = [1.000000e+01,1.000000e+01,1.000000e+01,1.000000e+01]
	xorps	xmm4, xmm4
	movaps	xmm3, xmmword ptr [rip + LCPI8_2] ## xmm3 = [1.000000e+00,1.000000e+00,1.000000e+00,1.000000e+00]
	movaps	xmm0, xmmword ptr [rip + __ZL6seq_vf]
	mulps	xmm0, xmmword ptr [rip + LCPI8_1]
	mov	rax, r12
	xor	ecx, ecx
	movss	xmm6, dword ptr [rip + LCPI8_7] ## xmm6 = mem[0],zero,zero,zero
	.p2align	4, 0x90
LBB8_7:                                 ##   Parent Loop BB8_2 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB8_8 Depth 3
	mov	rdx, rax
	xor	esi, esi
	.p2align	4, 0x90
LBB8_8:                                 ##   Parent Loop BB8_2 Depth=1
                                        ##     Parent Loop BB8_7 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	xorps	xmm1, xmm1
	cvtsi2ss	xmm1, esi
	mulss	xmm1, xmm6
	shufps	xmm1, xmm1, 0           ## xmm1 = xmm1[0,0,0,0]
	addps	xmm1, xmm0
	movaps	xmm2, xmm3
	subps	xmm2, xmm1
	mulps	xmm2, xmm4
	mulps	xmm1, xmm5
	addps	xmm1, xmm2
	movaps	xmmword ptr [rdx - 16], xmm1
	mov	edi, esi
	or	edi, 4
	xorps	xmm1, xmm1
	cvtsi2ss	xmm1, edi
	mulss	xmm1, xmm6
	shufps	xmm1, xmm1, 0           ## xmm1 = xmm1[0,0,0,0]
	addps	xmm1, xmm0
	movaps	xmm2, xmm3
	subps	xmm2, xmm1
	mulps	xmm2, xmm4
	mulps	xmm1, xmm5
	addps	xmm1, xmm2
	movaps	xmmword ptr [rdx], xmm1
	add	rsi, 8
	add	rdx, 32
	cmp	rsi, 1024
	jl	LBB8_8
## BB#9:                                ##   in Loop: Header=BB8_7 Depth=2
	inc	rcx
	add	rax, 4096
	cmp	rcx, 1024
	jne	LBB8_7
## BB#10:                               ##   in Loop: Header=BB8_2 Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	sub	rax, rbx
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, dword ptr [rip + LCPI8_6]
	mov	rax, qword ptr [rbp - 64] ## 8-byte Reload
	movss	dword ptr [rax + 4*r13], xmm0
	inc	r13
	cmp	r13, 10000
	jne	LBB8_2
## BB#11:
	movss	xmm3, dword ptr [rip + LCPI8_8] ## xmm3 = mem[0],zero,zero,zero
	xorps	xmm5, xmm5
	movss	xmm4, dword ptr [rip + LCPI8_9] ## xmm4 = mem[0],zero,zero,zero
	mov	r12, qword ptr [rbp - 72] ## 8-byte Reload
	mov	rax, r12
	.p2align	4, 0x90
LBB8_12:                                ## =>This Inner Loop Header: Depth=1
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
	jne	LBB8_12
## BB#13:
Ltmp43:
	movss	dword ptr [rbp - 44], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 52], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str]
	mov	edx, 30
	mov	r13, qword ptr [rbp - 64] ## 8-byte Reload
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp44:
## BB#14:
Ltmp45:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp46:
## BB#15:
Ltmp47:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp48:
## BB#16:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI8_10]
Ltmp49:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp50:
## BB#17:
Ltmp51:
	lea	rsi, [rip + L_.str.22]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp52:
## BB#18:
Ltmp53:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp54:
## BB#19:
Ltmp55:
	lea	rsi, [rip + L_.str.23]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp56:
## BB#20:
Ltmp57:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp58:
## BB#21:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp59:
	lea	rdi, [rbp - 104]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp60:
## BB#22:
Ltmp61:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 104]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp62:
## BB#23:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp63:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r14d, eax
Ltmp64:
## BB#24:
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
Ltmp66:
	movsx	esi, r14b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp67:
## BB#25:
Ltmp68:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp69:
## BB#26:
	movss	xmm3, dword ptr [rip + LCPI8_8] ## xmm3 = mem[0],zero,zero,zero
	xorps	xmm5, xmm5
	movss	xmm4, dword ptr [rip + LCPI8_9] ## xmm4 = mem[0],zero,zero,zero
	mov	rax, r13
	mov	rcx, qword ptr [rbp - 128] ## 8-byte Reload
	.p2align	4, 0x90
LBB8_27:                                ## =>This Inner Loop Header: Depth=1
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
	jne	LBB8_27
## BB#28:
Ltmp70:
	movss	dword ptr [rbp - 44], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 52], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str.24]
	mov	edx, 31
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp71:
## BB#29:
Ltmp72:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp73:
## BB#30:
Ltmp74:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp75:
## BB#31:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI8_10]
Ltmp76:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp77:
## BB#32:
Ltmp78:
	lea	rsi, [rip + L_.str.22]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp79:
## BB#33:
Ltmp80:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp81:
## BB#34:
Ltmp82:
	lea	rsi, [rip + L_.str.23]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp83:
## BB#35:
Ltmp84:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp85:
## BB#36:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp86:
	lea	rdi, [rbp - 112]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp87:
## BB#37:
Ltmp88:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 112]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp89:
## BB#38:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp90:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r14d, eax
Ltmp91:
## BB#39:
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
Ltmp93:
	movsx	esi, r14b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp94:
## BB#40:
Ltmp95:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp96:
## BB#41:
	xor	ebx, ebx
	mov	r15, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r14, [rip + L_.str.25]
	.p2align	4, 0x90
LBB8_42:                                ## =>This Inner Loop Header: Depth=1
	mov	rax, qword ptr [rbp - 88] ## 8-byte Reload
	movss	xmm0, dword ptr [rax + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp97:
	mov	rdi, r15
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp98:
## BB#43:                               ##   in Loop: Header=BB8_42 Depth=1
Ltmp99:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r14
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp100:
## BB#44:                               ##   in Loop: Header=BB8_42 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB8_42
## BB#45:
	mov	rax, qword ptr [r15]
	add	r15, qword ptr [rax - 24]
Ltmp102:
	lea	rdi, [rbp - 120]
	mov	rsi, r15
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp103:
## BB#46:
Ltmp104:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 120]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp105:
## BB#47:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp106:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp107:
## BB#48:
	lea	rdi, [rbp - 120]
	call	__ZNSt3__16localeD1Ev
Ltmp109:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp110:
## BB#49:
Ltmp111:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp112:
## BB#50:
	xor	ebx, ebx
	mov	r15, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r14, [rip + L_.str.25]
	.p2align	4, 0x90
LBB8_51:                                ## =>This Inner Loop Header: Depth=1
	mov	rax, qword ptr [rbp - 80] ## 8-byte Reload
	movss	xmm0, dword ptr [rax + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp113:
	mov	rdi, r15
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp114:
## BB#52:                               ##   in Loop: Header=BB8_51 Depth=1
Ltmp115:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r14
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp116:
## BB#53:                               ##   in Loop: Header=BB8_51 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB8_51
## BB#54:
	mov	rax, qword ptr [r15]
	add	r15, qword ptr [rax - 24]
Ltmp118:
	lea	rdi, [rbp - 96]
	mov	rsi, r15
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp119:
## BB#55:
Ltmp120:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 96]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp121:
## BB#56:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp122:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp123:
## BB#57:
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
Ltmp125:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp126:
## BB#58:
Ltmp127:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp128:
## BB#59:
	mov	rdi, qword ptr [rbp - 88] ## 8-byte Reload
	call	__ZdaPv
	mov	rdi, qword ptr [rbp - 80] ## 8-byte Reload
	call	__ZdaPv
	mov	rdi, r13
	call	__ZdlPv
	mov	rdi, r12
	call	__ZdlPv
	add	rsp, 88
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB8_66:
Ltmp42:
	mov	rbx, rax
	jmp	LBB8_65
LBB8_70:
Ltmp124:
	mov	rbx, rax
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB8_61
LBB8_69:
Ltmp108:
	mov	rbx, rax
	lea	rdi, [rbp - 120]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB8_61
LBB8_68:
Ltmp92:
	mov	rbx, rax
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB8_61
LBB8_67:
Ltmp65:
	mov	rbx, rax
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB8_61
LBB8_60:
Ltmp129:
	mov	rbx, rax
LBB8_61:
	mov	r12, qword ptr [rbp - 72] ## 8-byte Reload
	mov	r13, qword ptr [rbp - 64] ## 8-byte Reload
	jmp	LBB8_64
LBB8_71:
Ltmp117:
	jmp	LBB8_63
LBB8_62:
Ltmp101:
LBB8_63:
	mov	rbx, rax
LBB8_64:
	mov	rdi, r13
	call	__ZdlPv
LBB8_65:
	mov	rdi, r12
	call	__ZdlPv
	mov	rdi, rbx
	call	__Unwind_Resume
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table8:
Lexception0:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.ascii	"\340\001"              ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.ascii	"\335\001"              ## Call site table length
Lset0 = Lfunc_begin0-Lfunc_begin0       ## >> Call Site 1 <<
	.long	Lset0
Lset1 = Ltmp40-Lfunc_begin0             ##   Call between Lfunc_begin0 and Ltmp40
	.long	Lset1
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset2 = Ltmp40-Lfunc_begin0             ## >> Call Site 2 <<
	.long	Lset2
Lset3 = Ltmp41-Ltmp40                   ##   Call between Ltmp40 and Ltmp41
	.long	Lset3
Lset4 = Ltmp42-Lfunc_begin0             ##     jumps to Ltmp42
	.long	Lset4
	.byte	0                       ##   On action: cleanup
Lset5 = Ltmp41-Lfunc_begin0             ## >> Call Site 3 <<
	.long	Lset5
Lset6 = Ltmp43-Ltmp41                   ##   Call between Ltmp41 and Ltmp43
	.long	Lset6
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset7 = Ltmp43-Lfunc_begin0             ## >> Call Site 4 <<
	.long	Lset7
Lset8 = Ltmp60-Ltmp43                   ##   Call between Ltmp43 and Ltmp60
	.long	Lset8
Lset9 = Ltmp129-Lfunc_begin0            ##     jumps to Ltmp129
	.long	Lset9
	.byte	0                       ##   On action: cleanup
Lset10 = Ltmp61-Lfunc_begin0            ## >> Call Site 5 <<
	.long	Lset10
Lset11 = Ltmp64-Ltmp61                  ##   Call between Ltmp61 and Ltmp64
	.long	Lset11
Lset12 = Ltmp65-Lfunc_begin0            ##     jumps to Ltmp65
	.long	Lset12
	.byte	0                       ##   On action: cleanup
Lset13 = Ltmp66-Lfunc_begin0            ## >> Call Site 6 <<
	.long	Lset13
Lset14 = Ltmp87-Ltmp66                  ##   Call between Ltmp66 and Ltmp87
	.long	Lset14
Lset15 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset15
	.byte	0                       ##   On action: cleanup
Lset16 = Ltmp88-Lfunc_begin0            ## >> Call Site 7 <<
	.long	Lset16
Lset17 = Ltmp91-Ltmp88                  ##   Call between Ltmp88 and Ltmp91
	.long	Lset17
Lset18 = Ltmp92-Lfunc_begin0            ##     jumps to Ltmp92
	.long	Lset18
	.byte	0                       ##   On action: cleanup
Lset19 = Ltmp93-Lfunc_begin0            ## >> Call Site 8 <<
	.long	Lset19
Lset20 = Ltmp96-Ltmp93                  ##   Call between Ltmp93 and Ltmp96
	.long	Lset20
Lset21 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset21
	.byte	0                       ##   On action: cleanup
Lset22 = Ltmp97-Lfunc_begin0            ## >> Call Site 9 <<
	.long	Lset22
Lset23 = Ltmp100-Ltmp97                 ##   Call between Ltmp97 and Ltmp100
	.long	Lset23
Lset24 = Ltmp101-Lfunc_begin0           ##     jumps to Ltmp101
	.long	Lset24
	.byte	0                       ##   On action: cleanup
Lset25 = Ltmp102-Lfunc_begin0           ## >> Call Site 10 <<
	.long	Lset25
Lset26 = Ltmp103-Ltmp102                ##   Call between Ltmp102 and Ltmp103
	.long	Lset26
Lset27 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset27
	.byte	0                       ##   On action: cleanup
Lset28 = Ltmp104-Lfunc_begin0           ## >> Call Site 11 <<
	.long	Lset28
Lset29 = Ltmp107-Ltmp104                ##   Call between Ltmp104 and Ltmp107
	.long	Lset29
Lset30 = Ltmp108-Lfunc_begin0           ##     jumps to Ltmp108
	.long	Lset30
	.byte	0                       ##   On action: cleanup
Lset31 = Ltmp109-Lfunc_begin0           ## >> Call Site 12 <<
	.long	Lset31
Lset32 = Ltmp112-Ltmp109                ##   Call between Ltmp109 and Ltmp112
	.long	Lset32
Lset33 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset33
	.byte	0                       ##   On action: cleanup
Lset34 = Ltmp113-Lfunc_begin0           ## >> Call Site 13 <<
	.long	Lset34
Lset35 = Ltmp116-Ltmp113                ##   Call between Ltmp113 and Ltmp116
	.long	Lset35
Lset36 = Ltmp117-Lfunc_begin0           ##     jumps to Ltmp117
	.long	Lset36
	.byte	0                       ##   On action: cleanup
Lset37 = Ltmp118-Lfunc_begin0           ## >> Call Site 14 <<
	.long	Lset37
Lset38 = Ltmp119-Ltmp118                ##   Call between Ltmp118 and Ltmp119
	.long	Lset38
Lset39 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset39
	.byte	0                       ##   On action: cleanup
Lset40 = Ltmp120-Lfunc_begin0           ## >> Call Site 15 <<
	.long	Lset40
Lset41 = Ltmp123-Ltmp120                ##   Call between Ltmp120 and Ltmp123
	.long	Lset41
Lset42 = Ltmp124-Lfunc_begin0           ##     jumps to Ltmp124
	.long	Lset42
	.byte	0                       ##   On action: cleanup
Lset43 = Ltmp125-Lfunc_begin0           ## >> Call Site 16 <<
	.long	Lset43
Lset44 = Ltmp128-Ltmp125                ##   Call between Ltmp125 and Ltmp128
	.long	Lset44
Lset45 = Ltmp129-Lfunc_begin0           ##     jumps to Ltmp129
	.long	Lset45
	.byte	0                       ##   On action: cleanup
Lset46 = Ltmp128-Lfunc_begin0           ## >> Call Site 17 <<
	.long	Lset46
Lset47 = Lfunc_end0-Ltmp128             ##   Call between Ltmp128 and Lfunc_end0
	.long	Lset47
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI9_0:
	.long	1315859240              ## float 1.0E+9
LCPI9_1:
	.long	4286578687              ## float -3.40282347E+38
LCPI9_2:
	.long	2139095039              ## float 3.40282347E+38
LCPI9_3:
	.long	1176256512              ## float 1.0E+4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
## BB#0:
	push	rbp
Ltmp227:
	.cfi_def_cfa_offset 16
Ltmp228:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp229:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 72
Ltmp230:
	.cfi_offset rbx, -56
Ltmp231:
	.cfi_offset r12, -48
Ltmp232:
	.cfi_offset r13, -40
Ltmp233:
	.cfi_offset r14, -32
Ltmp234:
	.cfi_offset r15, -24
	mov	edi, 4194304
	call	__Znam
	mov	r14, rax
	call	__ZNSt3__16chrono12steady_clock3nowEv
	call	__ZNSt3__16chrono12steady_clock3nowEv
	xorps	xmm0, xmm0
	movaps	xmmword ptr [rbp - 80], xmm0
	mov	qword ptr [rbp - 64], 0
	mov	edi, 4000
	call	__Znwm
	mov	r13, rax
	mov	qword ptr [rbp - 80], r13
	lea	r12, [r13 + 4000]
	mov	qword ptr [rbp - 64], r12
	mov	esi, 4000
	mov	rdi, r13
	call	___bzero
	mov	qword ptr [rbp - 72], r12
	xor	ebx, ebx
	.p2align	4, 0x90
LBB9_1:                                 ## =>This Inner Loop Header: Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	r15, rax
	mov	edi, 1024
	mov	esi, 1024
	mov	rdx, r14
	call	__Z13render_noiseAiiPf
	call	__ZNSt3__16chrono12steady_clock3nowEv
	movss	xmm0, dword ptr [rip + LCPI9_0] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm1, xmm0
	sub	rax, r15
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, xmm1
	movss	dword ptr [r13 + 4*rbx], xmm0
	inc	rbx
	cmp	rbx, 1000
	jne	LBB9_1
## BB#2:
	xorps	xmm5, xmm5
	movss	xmm3, dword ptr [rip + LCPI9_1] ## xmm3 = mem[0],zero,zero,zero
	movss	xmm4, dword ptr [rip + LCPI9_2] ## xmm4 = mem[0],zero,zero,zero
	.p2align	4, 0x90
LBB9_3:                                 ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [r13]   ## xmm0 = mem[0],zero,zero,zero
	movss	xmm1, dword ptr [r13 + 4] ## xmm1 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm4
	addss	xmm5, xmm0
	maxss	xmm0, xmm3
	movaps	xmm3, xmm1
	minss	xmm3, xmm2
	addss	xmm5, xmm1
	maxss	xmm1, xmm0
	movss	xmm0, dword ptr [r13 + 8] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm3
	addss	xmm5, xmm0
	maxss	xmm0, xmm1
	movss	xmm3, dword ptr [r13 + 12] ## xmm3 = mem[0],zero,zero,zero
	movaps	xmm4, xmm3
	minss	xmm4, xmm2
	addss	xmm5, xmm3
	maxss	xmm3, xmm0
	add	r13, 16
	cmp	r13, r12
	jne	LBB9_3
## BB#4:
Ltmp138:
	movss	dword ptr [rbp - 44], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 52], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str]
	mov	edx, 30
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp139:
## BB#5:
Ltmp140:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp141:
## BB#6:
Ltmp142:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp143:
## BB#7:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI9_3]
Ltmp144:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp145:
## BB#8:
Ltmp146:
	lea	rsi, [rip + L_.str.22]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp147:
## BB#9:
Ltmp148:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp149:
## BB#10:
Ltmp150:
	lea	rsi, [rip + L_.str.23]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp151:
## BB#11:
Ltmp152:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp153:
## BB#12:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp154:
	lea	rdi, [rbp - 96]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp155:
## BB#13:
Ltmp156:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 96]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp157:
## BB#14:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp158:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r15d, eax
Ltmp159:
## BB#15:
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
Ltmp161:
	movsx	esi, r15b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp162:
## BB#16:
Ltmp163:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp164:
## BB#17:
	xor	ebx, ebx
	mov	r12, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r15, [rip + L_.str.25]
	.p2align	4, 0x90
LBB9_18:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [r14 + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp165:
	mov	rdi, r12
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp166:
## BB#19:                               ##   in Loop: Header=BB9_18 Depth=1
Ltmp167:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r15
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp168:
## BB#20:                               ##   in Loop: Header=BB9_18 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB9_18
## BB#21:
	mov	rax, qword ptr [r12]
	add	r12, qword ptr [rax - 24]
Ltmp170:
	lea	rdi, [rbp - 104]
	mov	rsi, r12
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp171:
## BB#22:
Ltmp172:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 104]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp173:
## BB#23:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp174:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp175:
## BB#24:
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
Ltmp177:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp178:
## BB#25:
Ltmp179:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp180:
## BB#26:
	mov	rcx, qword ptr [rbp - 80]
	mov	rax, qword ptr [rbp - 72]
	cmp	rax, rcx
	je	LBB9_28
## BB#27:
	lea	rdx, [rax - 4]
	sub	rdx, rcx
	not	rdx
	and	rdx, -4
	add	rax, rdx
	mov	qword ptr [rbp - 72], rax
LBB9_28:
	mov	rdx, rax
	sub	rdx, rcx
	sar	rdx, 2
	cmp	rdx, 999
	ja	LBB9_31
## BB#29:
	mov	esi, 1000
	sub	rsi, rdx
Ltmp181:
	lea	rdi, [rbp - 80]
	call	__ZNSt3__16vectorIfNS_9allocatorIfEEE8__appendEm
Ltmp182:
	jmp	LBB9_32
LBB9_31:
	cmp	rdx, 1000
	je	LBB9_32
## BB#36:
	add	rcx, 4000
	cmp	rax, rcx
	je	LBB9_32
## BB#37:
	lea	rdx, [rax - 4]
	sub	rdx, rcx
	not	rdx
	and	rdx, -4
	add	rdx, rax
	mov	qword ptr [rbp - 72], rdx
LBB9_32:
	xor	ebx, ebx
	.p2align	4, 0x90
LBB9_33:                                ## =>This Inner Loop Header: Depth=1
	call	__ZNSt3__16chrono12steady_clock3nowEv
	mov	r15, rax
	mov	edi, 1024
	mov	esi, 1024
	mov	rdx, r14
	call	__Z13render_noiseBiiPf
	call	__ZNSt3__16chrono12steady_clock3nowEv
	sub	rax, r15
	xorps	xmm0, xmm0
	cvtsi2ss	xmm0, rax
	divss	xmm0, dword ptr [rip + LCPI9_0]
	mov	rax, qword ptr [rbp - 80]
	movss	dword ptr [rax + 4*rbx], xmm0
	inc	rbx
	cmp	rbx, 1000
	jne	LBB9_33
## BB#34:
	mov	rcx, qword ptr [rbp - 72]
	cmp	rax, rcx
	je	LBB9_35
## BB#38:
	lea	rdx, [rcx - 4]
	sub	rdx, rax
	mov	esi, edx
	shr	esi, 2
	inc	esi
	and	rsi, 3
	je	LBB9_39
## BB#40:
	neg	rsi
	xorps	xmm4, xmm4
	movss	xmm0, dword ptr [rip + LCPI9_1] ## xmm0 = mem[0],zero,zero,zero
	movss	xmm1, dword ptr [rip + LCPI9_2] ## xmm1 = mem[0],zero,zero,zero
	.p2align	4, 0x90
LBB9_41:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm3, dword ptr [rax]   ## xmm3 = mem[0],zero,zero,zero
	movaps	xmm5, xmm3
	minss	xmm5, xmm1
	addss	xmm4, xmm3
	maxss	xmm3, xmm0
	add	rax, 4
	inc	rsi
	movaps	xmm1, xmm5
	movaps	xmm0, xmm3
	jne	LBB9_41
	jmp	LBB9_42
LBB9_35:
	movss	xmm5, dword ptr [rip + LCPI9_2] ## xmm5 = mem[0],zero,zero,zero
	movss	xmm3, dword ptr [rip + LCPI9_1] ## xmm3 = mem[0],zero,zero,zero
	xorps	xmm4, xmm4
	jmp	LBB9_43
LBB9_39:
	xorps	xmm4, xmm4
	movss	xmm3, dword ptr [rip + LCPI9_1] ## xmm3 = mem[0],zero,zero,zero
	movss	xmm5, dword ptr [rip + LCPI9_2] ## xmm5 = mem[0],zero,zero,zero
LBB9_42:
	cmp	rdx, 12
	jb	LBB9_43
	.p2align	4, 0x90
LBB9_79:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [rax]   ## xmm0 = mem[0],zero,zero,zero
	movss	xmm1, dword ptr [rax + 4] ## xmm1 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm5
	addss	xmm4, xmm0
	maxss	xmm0, xmm3
	movaps	xmm3, xmm1
	minss	xmm3, xmm2
	addss	xmm4, xmm1
	maxss	xmm1, xmm0
	movss	xmm0, dword ptr [rax + 8] ## xmm0 = mem[0],zero,zero,zero
	movaps	xmm2, xmm0
	minss	xmm2, xmm3
	addss	xmm4, xmm0
	maxss	xmm0, xmm1
	movss	xmm3, dword ptr [rax + 12] ## xmm3 = mem[0],zero,zero,zero
	movaps	xmm5, xmm3
	minss	xmm5, xmm2
	addss	xmm4, xmm3
	maxss	xmm3, xmm0
	add	rax, 16
	cmp	rax, rcx
	jne	LBB9_79
LBB9_43:
Ltmp183:
	movss	dword ptr [rbp - 52], xmm5 ## 4-byte Spill
	movss	dword ptr [rbp - 44], xmm4 ## 4-byte Spill
	movss	dword ptr [rbp - 48], xmm3 ## 4-byte Spill
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	rsi, [rip + L_.str.24]
	mov	edx, 31
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp184:
## BB#44:
Ltmp185:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp186:
## BB#45:
Ltmp187:
	lea	rsi, [rip + L_.str.21]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp188:
## BB#46:
	movss	xmm0, dword ptr [rbp - 44] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	divss	xmm0, dword ptr [rip + LCPI9_3]
Ltmp189:
	mov	rdi, rax
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp190:
## BB#47:
Ltmp191:
	lea	rsi, [rip + L_.str.22]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp192:
## BB#48:
Ltmp193:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 52] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp194:
## BB#49:
Ltmp195:
	lea	rsi, [rip + L_.str.23]
	mov	edx, 7
	mov	rdi, rax
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp196:
## BB#50:
Ltmp197:
	mov	rdi, rax
	movss	xmm0, dword ptr [rbp - 48] ## 4-byte Reload
                                        ## xmm0 = mem[0],zero,zero,zero
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
	mov	rbx, rax
Ltmp198:
## BB#51:
	mov	rax, qword ptr [rbx]
	mov	rsi, qword ptr [rax - 24]
	add	rsi, rbx
Ltmp199:
	lea	rdi, [rbp - 112]
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp200:
## BB#52:
Ltmp201:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 112]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp202:
## BB#53:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp203:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	r15d, eax
Ltmp204:
## BB#54:
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
Ltmp206:
	movsx	esi, r15b
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp207:
## BB#55:
Ltmp208:
	mov	rdi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp209:
## BB#56:
	xor	ebx, ebx
	mov	r12, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	lea	r15, [rip + L_.str.25]
	.p2align	4, 0x90
LBB9_57:                                ## =>This Inner Loop Header: Depth=1
	movss	xmm0, dword ptr [r14 + 4*rbx] ## xmm0 = mem[0],zero,zero,zero
Ltmp210:
	mov	rdi, r12
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEElsEf
Ltmp211:
## BB#58:                               ##   in Loop: Header=BB9_57 Depth=1
Ltmp212:
	mov	edx, 2
	mov	rdi, rax
	mov	rsi, r15
	call	__ZNSt3__124__put_character_sequenceIcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp213:
## BB#59:                               ##   in Loop: Header=BB9_57 Depth=1
	inc	rbx
	cmp	rbx, 1024
	jne	LBB9_57
## BB#60:
	mov	rax, qword ptr [r12]
	add	r12, qword ptr [rax - 24]
Ltmp215:
	lea	rdi, [rbp - 88]
	mov	rsi, r12
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp216:
## BB#61:
Ltmp217:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 88]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp218:
## BB#62:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp219:
	mov	esi, 10
	mov	rdi, rax
	call	rcx
	mov	ebx, eax
Ltmp220:
## BB#63:
	lea	rdi, [rbp - 88]
	call	__ZNSt3__16localeD1Ev
Ltmp222:
	movsx	esi, bl
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp223:
## BB#64:
Ltmp224:
	mov	rdi, qword ptr [rip + __ZNSt3__14coutE@GOTPCREL]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp225:
## BB#65:
	mov	rdi, r14
	call	__ZdaPv
	mov	rdi, qword ptr [rbp - 80]
	test	rdi, rdi
	je	LBB9_69
## BB#66:
	mov	rax, qword ptr [rbp - 72]
	cmp	rax, rdi
	je	LBB9_68
## BB#67:
	lea	rcx, [rax - 4]
	sub	rcx, rdi
	not	rcx
	and	rcx, -4
	add	rcx, rax
	mov	qword ptr [rbp - 72], rcx
LBB9_68:
	call	__ZdlPv
LBB9_69:
	xor	eax, eax
	add	rsp, 72
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB9_81:
Ltmp221:
	mov	rbx, rax
	lea	rdi, [rbp - 88]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB9_72
LBB9_80:
Ltmp205:
	mov	rbx, rax
	lea	rdi, [rbp - 112]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB9_72
LBB9_78:
Ltmp176:
	mov	rbx, rax
	lea	rdi, [rbp - 104]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB9_72
LBB9_77:
Ltmp160:
	mov	rbx, rax
	lea	rdi, [rbp - 96]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB9_72
LBB9_30:
Ltmp226:
	jmp	LBB9_71
LBB9_82:
Ltmp214:
	jmp	LBB9_71
LBB9_70:
Ltmp169:
LBB9_71:
	mov	rbx, rax
LBB9_72:
	mov	rdi, qword ptr [rbp - 80]
	test	rdi, rdi
	je	LBB9_76
## BB#73:
	mov	rax, qword ptr [rbp - 72]
	cmp	rax, rdi
	je	LBB9_75
## BB#74:
	lea	rcx, [rax - 4]
	sub	rcx, rdi
	not	rcx
	and	rcx, -4
	add	rcx, rax
	mov	qword ptr [rbp - 72], rcx
LBB9_75:
	call	__ZdlPv
LBB9_76:
	mov	rdi, rbx
	call	__Unwind_Resume
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table9:
Lexception1:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.asciz	"\306\201\200"          ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.ascii	"\303\001"              ## Call site table length
Lset48 = Lfunc_begin1-Lfunc_begin1      ## >> Call Site 1 <<
	.long	Lset48
Lset49 = Ltmp138-Lfunc_begin1           ##   Call between Lfunc_begin1 and Ltmp138
	.long	Lset49
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset50 = Ltmp138-Lfunc_begin1           ## >> Call Site 2 <<
	.long	Lset50
Lset51 = Ltmp155-Ltmp138                ##   Call between Ltmp138 and Ltmp155
	.long	Lset51
Lset52 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset52
	.byte	0                       ##   On action: cleanup
Lset53 = Ltmp156-Lfunc_begin1           ## >> Call Site 3 <<
	.long	Lset53
Lset54 = Ltmp159-Ltmp156                ##   Call between Ltmp156 and Ltmp159
	.long	Lset54
Lset55 = Ltmp160-Lfunc_begin1           ##     jumps to Ltmp160
	.long	Lset55
	.byte	0                       ##   On action: cleanup
Lset56 = Ltmp161-Lfunc_begin1           ## >> Call Site 4 <<
	.long	Lset56
Lset57 = Ltmp164-Ltmp161                ##   Call between Ltmp161 and Ltmp164
	.long	Lset57
Lset58 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset58
	.byte	0                       ##   On action: cleanup
Lset59 = Ltmp165-Lfunc_begin1           ## >> Call Site 5 <<
	.long	Lset59
Lset60 = Ltmp168-Ltmp165                ##   Call between Ltmp165 and Ltmp168
	.long	Lset60
Lset61 = Ltmp169-Lfunc_begin1           ##     jumps to Ltmp169
	.long	Lset61
	.byte	0                       ##   On action: cleanup
Lset62 = Ltmp170-Lfunc_begin1           ## >> Call Site 6 <<
	.long	Lset62
Lset63 = Ltmp171-Ltmp170                ##   Call between Ltmp170 and Ltmp171
	.long	Lset63
Lset64 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset64
	.byte	0                       ##   On action: cleanup
Lset65 = Ltmp172-Lfunc_begin1           ## >> Call Site 7 <<
	.long	Lset65
Lset66 = Ltmp175-Ltmp172                ##   Call between Ltmp172 and Ltmp175
	.long	Lset66
Lset67 = Ltmp176-Lfunc_begin1           ##     jumps to Ltmp176
	.long	Lset67
	.byte	0                       ##   On action: cleanup
Lset68 = Ltmp177-Lfunc_begin1           ## >> Call Site 8 <<
	.long	Lset68
Lset69 = Ltmp200-Ltmp177                ##   Call between Ltmp177 and Ltmp200
	.long	Lset69
Lset70 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset70
	.byte	0                       ##   On action: cleanup
Lset71 = Ltmp201-Lfunc_begin1           ## >> Call Site 9 <<
	.long	Lset71
Lset72 = Ltmp204-Ltmp201                ##   Call between Ltmp201 and Ltmp204
	.long	Lset72
Lset73 = Ltmp205-Lfunc_begin1           ##     jumps to Ltmp205
	.long	Lset73
	.byte	0                       ##   On action: cleanup
Lset74 = Ltmp206-Lfunc_begin1           ## >> Call Site 10 <<
	.long	Lset74
Lset75 = Ltmp209-Ltmp206                ##   Call between Ltmp206 and Ltmp209
	.long	Lset75
Lset76 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset76
	.byte	0                       ##   On action: cleanup
Lset77 = Ltmp210-Lfunc_begin1           ## >> Call Site 11 <<
	.long	Lset77
Lset78 = Ltmp213-Ltmp210                ##   Call between Ltmp210 and Ltmp213
	.long	Lset78
Lset79 = Ltmp214-Lfunc_begin1           ##     jumps to Ltmp214
	.long	Lset79
	.byte	0                       ##   On action: cleanup
Lset80 = Ltmp215-Lfunc_begin1           ## >> Call Site 12 <<
	.long	Lset80
Lset81 = Ltmp216-Ltmp215                ##   Call between Ltmp215 and Ltmp216
	.long	Lset81
Lset82 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset82
	.byte	0                       ##   On action: cleanup
Lset83 = Ltmp217-Lfunc_begin1           ## >> Call Site 13 <<
	.long	Lset83
Lset84 = Ltmp220-Ltmp217                ##   Call between Ltmp217 and Ltmp220
	.long	Lset84
Lset85 = Ltmp221-Lfunc_begin1           ##     jumps to Ltmp221
	.long	Lset85
	.byte	0                       ##   On action: cleanup
Lset86 = Ltmp222-Lfunc_begin1           ## >> Call Site 14 <<
	.long	Lset86
Lset87 = Ltmp225-Ltmp222                ##   Call between Ltmp222 and Ltmp225
	.long	Lset87
Lset88 = Ltmp226-Lfunc_begin1           ##     jumps to Ltmp226
	.long	Lset88
	.byte	0                       ##   On action: cleanup
Lset89 = Ltmp225-Lfunc_begin1           ## >> Call Site 15 <<
	.long	Lset89
Lset90 = Lfunc_end1-Ltmp225             ##   Call between Ltmp225 and Lfunc_end1
	.long	Lset90
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI10_0:
	.quad	64                      ## 0x40
	.quad	32                      ## 0x20
LCPI10_1:
	.quad	2                       ## 0x2
	.quad	2                       ## 0x2
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZNSt3__124uniform_int_distributionIlEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEElRT_RKNS1_10param_typeE
	.weak_def_can_be_hidden	__ZNSt3__124uniform_int_distributionIlEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEElRT_RKNS1_10param_typeE
	.p2align	4, 0x90
__ZNSt3__124uniform_int_distributionIlEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEElRT_RKNS1_10param_typeE: ## @_ZNSt3__124uniform_int_distributionIlEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEElRT_RKNS1_10param_typeE
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp235:
	.cfi_def_cfa_offset 16
Ltmp236:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp237:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	rbx
	sub	rsp, 136
Ltmp238:
	.cfi_offset rbx, -40
Ltmp239:
	.cfi_offset r14, -32
Ltmp240:
	.cfi_offset r15, -24
	mov	r14, rdx
	mov	rax, qword ptr [r14]
	mov	r15, qword ptr [r14 + 8]
	sub	r15, rax
	je	LBB10_14
## BB#1:
	inc	r15
	je	LBB10_7
## BB#2:
	movabs	r8, 4294967296
	bsr	rbx, r15
	xor	rbx, -64
	add	rbx, 65
	mov	ecx, 65
	sub	ecx, ebx
	mov	rax, -1
                                        ## kill: %CL<def> %CL<kill> %ECX<kill>
	shr	rax, cl
	and	rax, r15
	cmp	rax, 1
	sbb	rbx, 0
	mov	qword ptr [rbp - 88], rsi
	mov	qword ptr [rbp - 80], rbx
	mov	rdi, rbx
	shr	rdi, 5
	mov	eax, ebx
	and	eax, 31
	cmp	rax, 1
	sbb	rdi, -1
	mov	qword ptr [rbp - 64], rdi
	xor	edx, edx
	mov	rax, rbx
	div	rdi
	mov	rsi, rax
	mov	qword ptr [rbp - 72], rsi
	mov	rdx, r8
	mov	ecx, esi
	shr	rdx, cl
	shl	rdx, cl
	xor	eax, eax
	cmp	rsi, 64
	cmovb	rax, rdx
	mov	qword ptr [rbp - 48], rax
	mov	rcx, r8
	sub	rcx, rax
	xor	edx, edx
	div	rdi
	cmp	rcx, rax
	jbe	LBB10_5
## BB#3:
	inc	rdi
	mov	qword ptr [rbp - 64], rdi
	xor	edx, edx
	mov	rax, rbx
	div	rdi
	mov	rsi, rax
	mov	qword ptr [rbp - 72], rsi
	cmp	rsi, 63
	ja	LBB10_9
## BB#4:
	mov	rax, r8
	mov	ecx, esi
	shr	rax, cl
	shl	rax, cl
	mov	qword ptr [rbp - 48], rax
LBB10_5:
	xor	edx, edx
	mov	rax, rbx
	div	rdi
	sub	rdi, rdx
	mov	qword ptr [rbp - 56], rdi
	cmp	rsi, 62
	ja	LBB10_10
## BB#6:
	lea	ecx, [rsi + 1]
	shr	r8, cl
                                        ## kill: %CL<def> %CL<kill> %ECX<kill>
	shl	r8, cl
	jmp	LBB10_11
LBB10_7:
	mov	qword ptr [rbp - 152], rsi
	movaps	xmm0, xmmword ptr [rip + LCPI10_0] ## xmm0 = [64,32]
	movups	xmmword ptr [rbp - 144], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI10_1] ## xmm0 = [2,2]
	movups	xmmword ptr [rbp - 128], xmm0
	movabs	rax, 4294967296
	movq	xmm0, rax
	movups	xmmword ptr [rbp - 112], xmm0
	mov	qword ptr [rbp - 96], -1
	lea	rdi, [rbp - 152]
	call	__ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE
	jmp	LBB10_14
LBB10_9:
	mov	qword ptr [rbp - 48], 0
	xor	edx, edx
	mov	rax, rbx
	div	rdi
	sub	rdi, rdx
	mov	qword ptr [rbp - 56], rdi
LBB10_10:
	xor	r8d, r8d
LBB10_11:
	mov	qword ptr [rbp - 40], r8
	mov	ecx, 32
	sub	ecx, esi
	mov	eax, -1
	mov	edx, -1
                                        ## kill: %CL<def> %CL<kill> %ECX<kill>
	shr	edx, cl
	xor	ecx, ecx
	test	rsi, rsi
	cmovne	ecx, edx
	mov	dword ptr [rbp - 32], ecx
	mov	ecx, 31
	sub	ecx, esi
	mov	edx, -1
                                        ## kill: %CL<def> %CL<kill> %ECX<kill>
	shr	edx, cl
	cmp	rsi, 31
	cmovae	edx, eax
	mov	dword ptr [rbp - 28], edx
	lea	rbx, [rbp - 88]
	.p2align	4, 0x90
LBB10_12:                               ## =>This Inner Loop Header: Depth=1
	mov	rdi, rbx
	call	__ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE
	cmp	rax, r15
	jae	LBB10_12
## BB#13:
	add	rax, qword ptr [r14]
LBB10_14:
	add	rsp, 136
	pop	rbx
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.globl	__ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE
	.weak_def_can_be_hidden	__ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE
	.p2align	4, 0x90
__ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE: ## @_ZNSt3__125__independent_bits_engineINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEyE6__evalENS_17integral_constantIbLb1EEE
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp241:
	.cfi_def_cfa_offset 16
Ltmp242:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp243:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
Ltmp244:
	.cfi_offset rbx, -56
Ltmp245:
	.cfi_offset r12, -48
Ltmp246:
	.cfi_offset r13, -40
Ltmp247:
	.cfi_offset r14, -32
Ltmp248:
	.cfi_offset r15, -24
	mov	r12, qword ptr [rdi + 32]
	test	r12, r12
	je	LBB11_1
## BB#2:
	mov	rcx, qword ptr [rdi + 16]
	mov	r9, qword ptr [rdi + 40]
	cmp	rcx, 64
	jae	LBB11_3
## BB#7:
	xor	r13d, r13d
	movabs	r14, 945986875574848801
	mov	r10d, -2147483648
	xor	r11d, r11d
	mov	qword ptr [rbp - 48], r12 ## 8-byte Spill
	.p2align	4, 0x90
LBB11_8:                                ## =>This Inner Loop Header: Depth=1
	mov	rbx, qword ptr [rdi]
	mov	r15, qword ptr [rbx + 2496]
	lea	rax, [r15 + 1]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	r12, [r15 + rax + 1]
	mov	eax, dword ptr [rbx + 4*r15]
	and	eax, r10d
	mov	esi, dword ptr [rbx + 4*r12]
	mov	r8d, esi
	and	r8d, 2147483646
	or	r8d, eax
	lea	rax, [r15 + 397]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	rax, [r15 + rax + 397]
	shr	r8d
	and	esi, 1
	neg	esi
	and	esi, -1727483681
	xor	esi, dword ptr [rbx + 4*rax]
	xor	esi, r8d
	mov	dword ptr [rbx + 4*r15], esi
	mov	rax, qword ptr [rbx + 2496]
	mov	eax, dword ptr [rbx + 4*rax]
	mov	edx, eax
	shr	edx, 11
	xor	edx, eax
	mov	qword ptr [rbx + 2496], r12
	mov	eax, edx
	shl	eax, 7
	and	eax, -1658038656
	xor	eax, edx
	mov	edx, eax
	shl	edx, 15
	and	edx, -272236544
	xor	edx, eax
	mov	eax, edx
	shr	eax, 18
	xor	eax, edx
	cmp	rax, r9
	jae	LBB11_8
## BB#9:                                ##   in Loop: Header=BB11_8 Depth=1
	mov	rdx, r13
	shl	rdx, cl
	and	eax, dword ptr [rdi + 56]
	mov	r13, rax
	add	r13, rdx
	inc	r11
	mov	r12, qword ptr [rbp - 48] ## 8-byte Reload
	cmp	r11, r12
	jb	LBB11_8
	jmp	LBB11_10
LBB11_1:
	xor	r13d, r13d
	xor	r12d, r12d
	jmp	LBB11_10
LBB11_3:
	xor	r10d, r10d
	movabs	r14, 945986875574848801
	mov	r11d, -2147483648
	.p2align	4, 0x90
LBB11_4:                                ## =>This Inner Loop Header: Depth=1
	mov	rsi, qword ptr [rdi]
	mov	r15, qword ptr [rsi + 2496]
	lea	rax, [r15 + 1]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	r8, [r15 + rax + 1]
	mov	eax, dword ptr [rsi + 4*r15]
	and	eax, r11d
	mov	ebx, dword ptr [rsi + 4*r8]
	mov	ecx, ebx
	and	ecx, 2147483646
	or	ecx, eax
	lea	rax, [r15 + 397]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	rax, [r15 + rax + 397]
	shr	ecx
	and	ebx, 1
	neg	ebx
	and	ebx, -1727483681
	xor	ebx, dword ptr [rsi + 4*rax]
	xor	ebx, ecx
	mov	dword ptr [rsi + 4*r15], ebx
	mov	rax, qword ptr [rsi + 2496]
	mov	eax, dword ptr [rsi + 4*rax]
	mov	ecx, eax
	shr	ecx, 11
	xor	ecx, eax
	mov	qword ptr [rsi + 2496], r8
	mov	eax, ecx
	shl	eax, 7
	and	eax, -1658038656
	xor	eax, ecx
	mov	ecx, eax
	shl	ecx, 15
	and	ecx, -272236544
	xor	ecx, eax
	mov	r13d, ecx
	shr	r13d, 18
	xor	r13d, ecx
	cmp	r13, r9
	jae	LBB11_4
## BB#5:                                ##   in Loop: Header=BB11_4 Depth=1
	inc	r10
	cmp	r10, r12
	jb	LBB11_4
## BB#6:
	and	r13d, dword ptr [rdi + 56]
LBB11_10:
	mov	r9, qword ptr [rdi + 24]
	cmp	r12, r9
	jae	LBB11_20
## BB#11:
	mov	rcx, qword ptr [rdi + 16]
	mov	r10, qword ptr [rdi + 48]
	cmp	rcx, 63
	jae	LBB11_12
## BB#16:
	inc	rcx
	movabs	r11, 945986875574848801
	mov	r14d, -2147483648
	mov	qword ptr [rbp - 48], r9 ## 8-byte Spill
	.p2align	4, 0x90
LBB11_17:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB11_18 Depth 2
	mov	r9, r12
	.p2align	4, 0x90
LBB11_18:                               ##   Parent Loop BB11_17 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	mov	r8, qword ptr [rdi]
	mov	r15, qword ptr [r8 + 2496]
	lea	rax, [r15 + 1]
	shr	rax, 4
	mul	r11
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	r12, [r15 + rax + 1]
	mov	eax, dword ptr [r8 + 4*r15]
	and	eax, r14d
	mov	esi, dword ptr [r8 + 4*r12]
	mov	ebx, esi
	and	ebx, 2147483646
	or	ebx, eax
	lea	rax, [r15 + 397]
	shr	rax, 4
	mul	r11
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	rax, [r15 + rax + 397]
	shr	ebx
	and	esi, 1
	neg	esi
	and	esi, -1727483681
	xor	esi, dword ptr [r8 + 4*rax]
	xor	esi, ebx
	mov	dword ptr [r8 + 4*r15], esi
	mov	rax, qword ptr [r8 + 2496]
	mov	eax, dword ptr [r8 + 4*rax]
	mov	edx, eax
	shr	edx, 11
	xor	edx, eax
	mov	qword ptr [r8 + 2496], r12
	mov	eax, edx
	shl	eax, 7
	and	eax, -1658038656
	xor	eax, edx
	mov	edx, eax
	shl	edx, 15
	and	edx, -272236544
	xor	edx, eax
	mov	eax, edx
	shr	eax, 18
	xor	eax, edx
	cmp	rax, r10
	jae	LBB11_18
## BB#19:                               ##   in Loop: Header=BB11_17 Depth=1
	shl	r13, cl
	and	eax, dword ptr [rdi + 60]
	add	r13, rax
	mov	r12, r9
	inc	r12
	mov	r9, qword ptr [rbp - 48] ## 8-byte Reload
	cmp	r12, r9
	jb	LBB11_17
	jmp	LBB11_20
LBB11_12:
	movabs	r14, 945986875574848801
	mov	r11d, -2147483648
	.p2align	4, 0x90
LBB11_13:                               ## =>This Inner Loop Header: Depth=1
	mov	rsi, qword ptr [rdi]
	mov	r15, qword ptr [rsi + 2496]
	lea	rax, [r15 + 1]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	r8, [r15 + rax + 1]
	mov	eax, dword ptr [rsi + 4*r15]
	and	eax, r11d
	mov	ecx, dword ptr [rsi + 4*r8]
	mov	ebx, ecx
	and	ebx, 2147483646
	or	ebx, eax
	lea	rax, [r15 + 397]
	shr	rax, 4
	mul	r14
	shr	rdx
	imul	rax, rdx, 624
	neg	rax
	lea	rax, [r15 + rax + 397]
	shr	ebx
	and	ecx, 1
	neg	ecx
	and	ecx, -1727483681
	xor	ecx, dword ptr [rsi + 4*rax]
	xor	ecx, ebx
	mov	dword ptr [rsi + 4*r15], ecx
	mov	rax, qword ptr [rsi + 2496]
	mov	eax, dword ptr [rsi + 4*rax]
	mov	ecx, eax
	shr	ecx, 11
	xor	ecx, eax
	mov	qword ptr [rsi + 2496], r8
	mov	eax, ecx
	shl	eax, 7
	and	eax, -1658038656
	xor	eax, ecx
	mov	ecx, eax
	shl	ecx, 15
	and	ecx, -272236544
	xor	ecx, eax
	mov	r13d, ecx
	shr	r13d, 18
	xor	r13d, ecx
	cmp	r13, r10
	jae	LBB11_13
## BB#14:                               ##   in Loop: Header=BB11_13 Depth=1
	inc	r12
	cmp	r12, r9
	jb	LBB11_13
## BB#15:
	and	r13d, dword ptr [rdi + 60]
LBB11_20:
	mov	rax, r13
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

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
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
## BB#0:
	push	rbp
Ltmp270:
	.cfi_def_cfa_offset 16
Ltmp271:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp272:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 40
Ltmp273:
	.cfi_offset rbx, -56
Ltmp274:
	.cfi_offset r12, -48
Ltmp275:
	.cfi_offset r13, -40
Ltmp276:
	.cfi_offset r14, -32
Ltmp277:
	.cfi_offset r15, -24
	mov	r14, rdx
	mov	r15, rsi
	mov	rbx, rdi
Ltmp249:
	lea	rdi, [rbp - 80]
	mov	rsi, rbx
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp250:
## BB#1:
	cmp	byte ptr [rbp - 80], 0
	je	LBB13_10
## BB#2:
	mov	rax, qword ptr [rbx]
	mov	rax, qword ptr [rax - 24]
	lea	r12, [rbx + rax]
	mov	rdi, qword ptr [rbx + rax + 40]
	mov	r13d, dword ptr [rbx + rax + 8]
	mov	eax, dword ptr [rbx + rax + 144]
	cmp	eax, -1
	jne	LBB13_7
## BB#3:
Ltmp252:
	mov	qword ptr [rbp - 64], rdi ## 8-byte Spill
	lea	rdi, [rbp - 56]
	mov	rsi, r12
	call	__ZNKSt3__18ios_base6getlocEv
Ltmp253:
## BB#4:
Ltmp254:
	mov	rsi, qword ptr [rip + __ZNSt3__15ctypeIcE2idE@GOTPCREL]
	lea	rdi, [rbp - 56]
	call	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp255:
## BB#5:
	mov	rcx, qword ptr [rax]
	mov	rcx, qword ptr [rcx + 56]
Ltmp256:
	mov	esi, 32
	mov	rdi, rax
	call	rcx
	mov	byte ptr [rbp - 41], al ## 1-byte Spill
Ltmp257:
## BB#6:
	lea	rdi, [rbp - 56]
	call	__ZNSt3__16localeD1Ev
	movsx	eax, byte ptr [rbp - 41] ## 1-byte Folded Reload
	mov	dword ptr [r12 + 144], eax
	mov	rdi, qword ptr [rbp - 64] ## 8-byte Reload
LBB13_7:
	add	r14, r15
	and	r13d, 176
	cmp	r13d, 32
	mov	rdx, r15
	cmove	rdx, r14
Ltmp259:
	movsx	r9d, al
	mov	rsi, r15
	mov	rcx, r14
	mov	r8, r12
	call	__ZNSt3__116__pad_and_outputIcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp260:
## BB#8:
	test	rax, rax
	jne	LBB13_10
## BB#9:
	mov	rax, qword ptr [rbx]
	mov	rax, qword ptr [rax - 24]
	lea	rdi, [rbx + rax]
	mov	esi, dword ptr [rbx + rax + 32]
	or	esi, 5
Ltmp261:
	call	__ZNSt3__18ios_base5clearEj
Ltmp262:
LBB13_10:
	lea	rdi, [rbp - 80]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB13_15:
	mov	rax, rbx
	add	rsp, 40
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB13_19:
Ltmp258:
	mov	r14, rax
	lea	rdi, [rbp - 56]
	call	__ZNSt3__16localeD1Ev
	jmp	LBB13_12
LBB13_20:
Ltmp251:
	mov	r14, rax
	jmp	LBB13_13
LBB13_11:
Ltmp263:
	mov	r14, rax
LBB13_12:
	lea	rdi, [rbp - 80]
	call	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB13_13:
	mov	r15, rbx
	mov	rdi, r14
	call	___cxa_begin_catch
	mov	rax, qword ptr [rbx]
	add	r15, qword ptr [rax - 24]
Ltmp264:
	mov	rdi, r15
	call	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp265:
## BB#14:
	call	___cxa_end_catch
	jmp	LBB13_15
LBB13_16:
Ltmp266:
	mov	rbx, rax
Ltmp267:
	call	___cxa_end_catch
Ltmp268:
## BB#17:
	mov	rdi, rbx
	call	__Unwind_Resume
LBB13_18:
Ltmp269:
	mov	rdi, rax
	call	___clang_call_terminate
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table13:
Lexception2:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.byte	125                     ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	117                     ## Call site table length
Lset91 = Ltmp249-Lfunc_begin2           ## >> Call Site 1 <<
	.long	Lset91
Lset92 = Ltmp250-Ltmp249                ##   Call between Ltmp249 and Ltmp250
	.long	Lset92
Lset93 = Ltmp251-Lfunc_begin2           ##     jumps to Ltmp251
	.long	Lset93
	.byte	1                       ##   On action: 1
Lset94 = Ltmp252-Lfunc_begin2           ## >> Call Site 2 <<
	.long	Lset94
Lset95 = Ltmp253-Ltmp252                ##   Call between Ltmp252 and Ltmp253
	.long	Lset95
Lset96 = Ltmp263-Lfunc_begin2           ##     jumps to Ltmp263
	.long	Lset96
	.byte	1                       ##   On action: 1
Lset97 = Ltmp254-Lfunc_begin2           ## >> Call Site 3 <<
	.long	Lset97
Lset98 = Ltmp257-Ltmp254                ##   Call between Ltmp254 and Ltmp257
	.long	Lset98
Lset99 = Ltmp258-Lfunc_begin2           ##     jumps to Ltmp258
	.long	Lset99
	.byte	1                       ##   On action: 1
Lset100 = Ltmp259-Lfunc_begin2          ## >> Call Site 4 <<
	.long	Lset100
Lset101 = Ltmp262-Ltmp259               ##   Call between Ltmp259 and Ltmp262
	.long	Lset101
Lset102 = Ltmp263-Lfunc_begin2          ##     jumps to Ltmp263
	.long	Lset102
	.byte	1                       ##   On action: 1
Lset103 = Ltmp262-Lfunc_begin2          ## >> Call Site 5 <<
	.long	Lset103
Lset104 = Ltmp264-Ltmp262               ##   Call between Ltmp262 and Ltmp264
	.long	Lset104
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset105 = Ltmp264-Lfunc_begin2          ## >> Call Site 6 <<
	.long	Lset105
Lset106 = Ltmp265-Ltmp264               ##   Call between Ltmp264 and Ltmp265
	.long	Lset106
Lset107 = Ltmp266-Lfunc_begin2          ##     jumps to Ltmp266
	.long	Lset107
	.byte	0                       ##   On action: cleanup
Lset108 = Ltmp265-Lfunc_begin2          ## >> Call Site 7 <<
	.long	Lset108
Lset109 = Ltmp267-Ltmp265               ##   Call between Ltmp265 and Ltmp267
	.long	Lset109
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset110 = Ltmp267-Lfunc_begin2          ## >> Call Site 8 <<
	.long	Lset110
Lset111 = Ltmp268-Ltmp267               ##   Call between Ltmp267 and Ltmp268
	.long	Lset111
Lset112 = Ltmp269-Lfunc_begin2          ##     jumps to Ltmp269
	.long	Lset112
	.byte	1                       ##   On action: 1
Lset113 = Ltmp268-Lfunc_begin2          ## >> Call Site 9 <<
	.long	Lset113
Lset114 = Lfunc_end2-Ltmp268            ##   Call between Ltmp268 and Lfunc_end2
	.long	Lset114
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
Lfunc_begin3:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception3
## BB#0:
	push	rbp
Ltmp281:
	.cfi_def_cfa_offset 16
Ltmp282:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp283:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 40
Ltmp284:
	.cfi_offset rbx, -56
Ltmp285:
	.cfi_offset r12, -48
Ltmp286:
	.cfi_offset r13, -40
Ltmp287:
	.cfi_offset r14, -32
Ltmp288:
	.cfi_offset r15, -24
	mov	r15, rcx
	mov	r12, rdx
	mov	rbx, rdi
	test	rbx, rbx
	je	LBB14_12
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
	jle	LBB14_3
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
	jne	LBB14_12
LBB14_3:
	test	r13, r13
	jle	LBB14_6
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
Ltmp278:
	mov	rdi, rbx
	mov	rdx, r13
	call	rax
	mov	r14, rax
Ltmp279:
## BB#5:
	lea	rdi, [rbp - 64]
	call	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev
	cmp	r14, r13
	jne	LBB14_12
LBB14_6:
	sub	r15, r12
	test	r15, r15
	jle	LBB14_8
## BB#7:
	mov	rax, qword ptr [rbx]
	mov	rdi, rbx
	mov	rsi, r12
	mov	rdx, r15
	call	qword ptr [rax + 96]
	cmp	rax, r15
	jne	LBB14_12
LBB14_8:
	mov	rax, qword ptr [rbp - 80] ## 8-byte Reload
	mov	qword ptr [rax + 24], 0
	jmp	LBB14_13
LBB14_12:
	xor	ebx, ebx
LBB14_13:
	mov	rax, rbx
	add	rsp, 40
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
LBB14_14:
Ltmp280:
	mov	rbx, rax
	lea	rdi, [rbp - 64]
	call	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED1Ev
	mov	rdi, rbx
	call	__Unwind_Resume
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table14:
Lexception3:
	.byte	255                     ## @LPStart Encoding = omit
	.byte	155                     ## @TType Encoding = indirect pcrel sdata4
	.byte	41                      ## @TType base offset
	.byte	3                       ## Call site Encoding = udata4
	.byte	39                      ## Call site table length
Lset115 = Lfunc_begin3-Lfunc_begin3     ## >> Call Site 1 <<
	.long	Lset115
Lset116 = Ltmp278-Lfunc_begin3          ##   Call between Lfunc_begin3 and Ltmp278
	.long	Lset116
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
Lset117 = Ltmp278-Lfunc_begin3          ## >> Call Site 2 <<
	.long	Lset117
Lset118 = Ltmp279-Ltmp278               ##   Call between Ltmp278 and Ltmp279
	.long	Lset118
Lset119 = Ltmp280-Lfunc_begin3          ##     jumps to Ltmp280
	.long	Lset119
	.byte	0                       ##   On action: cleanup
Lset120 = Ltmp279-Lfunc_begin3          ## >> Call Site 3 <<
	.long	Lset120
Lset121 = Lfunc_end3-Ltmp279            ##   Call between Ltmp279 and Lfunc_end3
	.long	Lset121
	.long	0                       ##     has no landing pad
	.byte	0                       ##   On action: cleanup
	.p2align	2

	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZNSt3__16vectorIfNS_9allocatorIfEEE8__appendEm
	.weak_def_can_be_hidden	__ZNSt3__16vectorIfNS_9allocatorIfEEE8__appendEm
	.p2align	4, 0x90
__ZNSt3__16vectorIfNS_9allocatorIfEEE8__appendEm: ## @_ZNSt3__16vectorIfNS_9allocatorIfEEE8__appendEm
	.cfi_startproc
## BB#0:
	push	rbp
Ltmp289:
	.cfi_def_cfa_offset 16
Ltmp290:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp291:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	push	rax
Ltmp292:
	.cfi_offset rbx, -56
Ltmp293:
	.cfi_offset r12, -48
Ltmp294:
	.cfi_offset r13, -40
Ltmp295:
	.cfi_offset r14, -32
Ltmp296:
	.cfi_offset r15, -24
	mov	r14, rsi
	mov	r13, rdi
	mov	rbx, qword ptr [r13 + 8]
	mov	r15, qword ptr [r13 + 16]
	mov	rax, r15
	sub	rax, rbx
	sar	rax, 2
	cmp	rax, r14
	jae	LBB15_1
## BB#2:
	mov	rdx, qword ptr [r13]
	sub	rbx, rdx
	sar	rbx, 2
	add	rbx, r14
	mov	rax, rbx
	shr	rax, 62
	je	LBB15_4
## BB#3:
	mov	rdi, r13
	call	__ZNKSt3__120__vector_base_commonILb1EE20__throw_length_errorEv
	mov	rdx, qword ptr [r13]
	mov	r15, qword ptr [r13 + 16]
LBB15_4:
	sub	r15, rdx
	mov	rax, r15
	sar	rax, 2
	movabs	rcx, 2305843009213693951
	cmp	rax, rcx
	mov	qword ptr [rbp - 48], rdx ## 8-byte Spill
	jae	LBB15_5
## BB#6:
	sar	r15
	cmp	r15, rbx
	cmovb	r15, rbx
	mov	r12, qword ptr [r13 + 8]
	mov	rbx, r12
	sub	rbx, rdx
	sar	rbx, 2
	xor	ecx, ecx
	test	r15, r15
	mov	eax, 0
	jne	LBB15_7
	jmp	LBB15_8
LBB15_1:
	lea	rsi, [4*r14]
	mov	rdi, rbx
	call	___bzero
	lea	rax, [rbx + 4*r14]
	mov	qword ptr [r13 + 8], rax
	jmp	LBB15_11
LBB15_5:
	movabs	r15, 4611686018427387903
	mov	r12, qword ptr [r13 + 8]
	mov	rbx, r12
	sub	rbx, rdx
	sar	rbx, 2
LBB15_7:
	lea	rdi, [4*r15]
	call	__Znwm
	mov	rcx, r15
LBB15_8:
	lea	rbx, [rax + 4*rbx]
	lea	r15, [rax + 4*rcx]
	lea	rsi, [4*r14]
	mov	rdi, rbx
	call	___bzero
	lea	r14, [rbx + 4*r14]
	mov	rdi, qword ptr [rbp - 48] ## 8-byte Reload
	sub	r12, rdi
	sub	rbx, r12
	test	r12, r12
	jle	LBB15_10
## BB#9:
	mov	rdi, rbx
	mov	rsi, qword ptr [rbp - 48] ## 8-byte Reload
	mov	rdx, r12
	call	_memcpy
	mov	rdi, qword ptr [rbp - 48] ## 8-byte Reload
LBB15_10:
	mov	qword ptr [r13], rbx
	mov	qword ptr [r13 + 8], r14
	mov	qword ptr [r13 + 16], r15
	test	rdi, rdi
	je	LBB15_11
## BB#12:
	add	rsp, 8
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	jmp	__ZdlPv                 ## TAILCALL
LBB15_11:
	add	rsp, 8
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	pop	rbp
	ret
	.cfi_endproc

	.section	__TEXT,__literal16,16byte_literals
	.p2align	4
LCPI16_0:
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
	.long	2147483647              ## 0x7fffffff
LCPI16_1:
	.long	255                     ## 0xff
	.long	255                     ## 0xff
	.long	255                     ## 0xff
	.long	255                     ## 0xff
LCPI16_2:
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
	.long	1258291200              ## 0x4b000000
LCPI16_3:
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
	.long	1065353216              ## 0x3f800000
LCPI16_4:
	.long	3                       ## 0x3
	.long	2                       ## 0x2
	.long	1                       ## 0x1
	.long	0                       ## 0x0
LCPI16_5:
	.long	0                       ## 0x0
	.long	1065353216              ## 0x3f800000
	.long	1073741824              ## 0x40000000
	.long	1077936128              ## 0x40400000
LCPI16_6:
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.long	1                       ## 0x1
	.section	__TEXT,__literal4,4byte_literals
	.p2align	2
LCPI16_7:
	.long	3217031168              ## float -1.5
	.section	__TEXT,__StaticInit,regular,pure_instructions
	.p2align	4, 0x90
__GLOBAL__sub_I_memory_access_tests.cpp: ## @_GLOBAL__sub_I_memory_access_tests.cpp
	.cfi_startproc
## BB#0:
	movaps	xmm0, xmmword ptr [rip + LCPI16_0] ## xmm0 = [2147483647,2147483647,2147483647,2147483647]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_abs_maskE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI16_1] ## xmm0 = [255,255,255,255]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL13vecm_rnd_maskE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI16_2] ## xmm0 = [1258291200,1258291200,1258291200,1258291200]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL16vecm_no_fractionE], xmm0
	movaps	xmm0, xmmword ptr [rip + LCPI16_3] ## xmm0 = [1065353216,1065353216,1065353216,1065353216]
	movaps	xmmword ptr [rip + __ZN3zap5maths4simdL8vecm_oneE], xmm0
	cvtps2dq	xmm1, xmmword ptr [rip + LCPI16_3]
	movapd	xmmword ptr [rip + __ZN3zap5maths4simdL8veci_oneE], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI16_4] ## xmm1 = [3,2,1,0]
	movaps	xmmword ptr [rip + __ZL6seq_vi], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI16_5] ## xmm1 = [0,1065353216,1073741824,1077936128]
	movaps	xmmword ptr [rip + __ZL6seq_vf], xmm1
	xorps	xmm1, xmm1
	movaps	xmmword ptr [rip + __ZL7zero_vi], xmm1
	movaps	xmm1, xmmword ptr [rip + LCPI16_6] ## xmm1 = [1,1,1,1]
	movaps	xmmword ptr [rip + __ZL6one_vi], xmm1
	movaps	xmmword ptr [rip + __ZL6one_vf], xmm0
	mov	byte ptr [rip + __ZL12global_state], 0
	movabs	rax, 2862933555777941757
	mov	ecx, 1
	xor	edx, edx
	lea	rsi, [rip + __ZL12global_state]
	movss	xmm0, dword ptr [rip + LCPI16_7] ## xmm0 = mem[0],zero,zero,zero
	.p2align	4, 0x90
LBB16_1:                                ## =>This Inner Loop Header: Depth=1
	mov	byte ptr [rdx + rsi + 1], dl
	imul	rcx, rax
	inc	rcx
	mov	rdi, rcx
	shr	rdi, 41
	or	edi, 1065353216
	movd	xmm1, edi
	addss	xmm1, xmm0
	addss	xmm1, xmm1
	movss	dword ptr [rsi + 4*rdx + 272], xmm1
	lea	edi, [rdx + 1]
	mov	byte ptr [rdx + rsi + 2], dil
	imul	rcx, rax
	inc	rcx
	mov	rdi, rcx
	shr	rdi, 41
	or	edi, 1065353216
	movd	xmm1, edi
	addss	xmm1, xmm0
	addss	xmm1, xmm1
	movss	dword ptr [rsi + 4*rdx + 276], xmm1
	add	rdx, 2
	cmp	rdx, 256
	jne	LBB16_1
## BB#2:
	push	rbp
Ltmp297:
	.cfi_def_cfa_offset 16
Ltmp298:
	.cfi_offset rbp, -16
	mov	rbp, rsp
Ltmp299:
	.cfi_def_cfa_register rbp
	push	r15
	push	r14
	push	r13
	push	r12
	push	rbx
	sub	rsp, 2552
Ltmp300:
	.cfi_offset rbx, -56
Ltmp301:
	.cfi_offset r12, -48
Ltmp302:
	.cfi_offset r13, -40
Ltmp303:
	.cfi_offset r14, -32
Ltmp304:
	.cfi_offset r15, -24
	mov	dword ptr [rbp - 2584], 48271
	mov	edx, 48271
	mov	eax, 1
	xor	ecx, ecx
	jmp	LBB16_3
	.p2align	4, 0x90
LBB16_9:                                ##   in Loop: Header=BB16_3 Depth=1
	lea	rdx, [rax + 3]
	mov	edi, esi
	shr	edi, 30
	xor	edi, esi
	imul	esi, edi, 1812433253
	add	edx, esi
	lea	esi, [rcx + rsi + 4]
	mov	dword ptr [rbp + 4*rcx - 2568], esi
	add	rax, 4
	add	rcx, 4
LBB16_3:                                ## =>This Inner Loop Header: Depth=1
	mov	esi, edx
	shr	esi, 30
	xor	esi, edx
	imul	edx, esi, 1812433253
	lea	edx, [rcx + rdx + 1]
	mov	dword ptr [rbp + 4*rcx - 2580], edx
	mov	esi, edx
	shr	esi, 30
	xor	esi, edx
	imul	edx, esi, 1812433253
	lea	edx, [rcx + rdx + 2]
	mov	dword ptr [rbp + 4*rcx - 2576], edx
	mov	esi, edx
	shr	esi, 30
	xor	esi, edx
	imul	edx, esi, 1812433253
	lea	rsi, [rcx + rdx + 3]
	mov	dword ptr [rbp + 4*rcx - 2572], esi
	cmp	rcx, 620
	jne	LBB16_9
## BB#4:
	mov	qword ptr [rbp - 88], 0
	movabs	rax, 9223372036854775807
	movq	xmm0, rax
	pslldq	xmm0, 8                 ## xmm0 = zero,zero,zero,zero,zero,zero,zero,zero,xmm0[0,1,2,3,4,5,6,7]
	movdqa	xmmword ptr [rbp - 80], xmm0
	lea	rbx, [rip + __ZL12global_state+1]
	mov	r13d, 255
	lea	r14, [rbp - 80]
	lea	r15, [rbp - 2584]
	lea	r12, [rbp - 56]
	.p2align	4, 0x90
LBB16_5:                                ## =>This Inner Loop Header: Depth=1
	mov	qword ptr [rbp - 56], 0
	mov	qword ptr [rbp - 48], r13
	mov	rdi, r14
	mov	rsi, r15
	mov	rdx, r12
	call	__ZNSt3__124uniform_int_distributionIlEclINS_23mersenne_twister_engineIjLm32ELm624ELm397ELm31ELj2567483615ELm11ELj4294967295ELm7ELj2636928640ELm15ELj4022730752ELm18ELj1812433253EEEEElRT_RKNS1_10param_typeE
	test	rax, rax
	je	LBB16_7
## BB#6:                                ##   in Loop: Header=BB16_5 Depth=1
	movzx	ecx, byte ptr [rbx]
	movzx	edx, byte ptr [rbx + rax]
	mov	byte ptr [rbx], dl
	mov	byte ptr [rbx + rax], cl
LBB16_7:                                ##   in Loop: Header=BB16_5 Depth=1
	inc	rbx
	dec	r13
	jne	LBB16_5
## BB#8:
	mov	byte ptr [rip + __ZL12global_state], 1
	add	rsp, 2552
	pop	rbx
	pop	r12
	pop	r13
	pop	r14
	pop	r15
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
.zerofill __DATA,__bss,__ZL12global_state,1296,4 ## @_ZL12global_state
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Time measured for CPU, total: "

L_.str.21:                              ## @.str.21
	.asciz	", avg: "

L_.str.22:                              ## @.str.22
	.asciz	", min: "

L_.str.23:                              ## @.str.23
	.asciz	", max: "

L_.str.24:                              ## @.str.24
	.asciz	"Time measured for SIMD, total: "

L_.str.25:                              ## @.str.25
	.asciz	", "

	.section	__DATA,__mod_init_func,mod_init_funcs
	.p2align	3
	.quad	__GLOBAL__sub_I_memory_access_tests.cpp

.subsections_via_symbols
