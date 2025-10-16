.data
FVector2 struct
	x dd ?
	y dd ?
FVector2 ends
SIZE_OF_PTR equ 8
SIZE_OF_FLOAT equ sizeof real4
.code
ASMDot proc
	movss xmm0, [rbx].FVector2.x
	mulss xmm0, [rdx].FVector2.x
	movss xmm1, [rbx].FVector2.y
	mulss xmm1, [rdx].FVector2.y
	addss xmm0, xmm1
	ret
ASMDot endp
;(FVector2 *vertices, FVector2 *axis, uint numVertices, float *min, float *max)<---- signature
ASMSetProjection proc
	push rbp
	mov rbp, rsp
	push rbx
	mov rbx, rcx; we're using rbx for first arg cuz loop uses rcx for counter
	call ASMDot
	movss real4 ptr[r9], xmm0; min = projection
	mov rax, qword ptr [rbp + 48]; init rax to max from stack (32 for shadow stack, add 8 for sizeof float ptr operand, add another 8 for pushed rbp)
	movss real4 ptr [rax], xmm0; max = projection
	mov ecx, r8d; initialize counter to numVertices from r8d
loopHead:
	cmp ecx, r8d; check if counter is r8d
	je testL
	add rbx, SIZE_OF_PTR; rbx indexes into next element of vertices array
	call ASMDot
testL:
	comiss xmm0, real4 ptr[r9]; compare 'projection' and 'min'
	jnc testG; jump if less or equal
	movss real4 ptr[r9], xmm0; min = projection
testG:
	comiss xmm0, real4 ptr [rax]; compare 'projection' and 'max'
	jc endLoop; jump if more or equal
setMaxProj:
	movss real4 ptr [rax], xmm0; rax, which is max = projection
endLoop:
	loop loopHead
	pop rbx
	mov rsp, rbp
	pop rbp
	ret
ASMSetProjection endp
end