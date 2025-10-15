.data
FVector2 struct
	x dd ?
	y dd ?
FVector2 ends
.code
ASMDot proc
	movss xmm0, [rbx].FVector2.x; TODO: make this SIMD with SSE2
	mulss xmm0, [rdx].FVector2.x
	movss xmm1, [rbx].FVector2.y
	mulss xmm1, [rdx].FVector2.y
	addss xmm0, xmm1
	ret
ASMDot endp
;(FVector2 *vertices, FVector2 *axis, uint numVertices, float *min, float *max)
ASMSetProjection proc
	push rbx
	mov rbx, rcx; we're using rbx for first arg cuz loop uses rcx for counter
	call ASMDot
	movss real4 ptr[r9], xmm0; min = projection
	movss real4 ptr [rax], xmm0; max = projection
	mov ecx, r8d; initialize counter to numVertices from r8d
loopHead:
	and ecx, ecx; check if counter is 0
	setnz al; set al to bool version of counter (ecx)
	jz testL
	add rbx, sizeof FVector2; rbx indexes into next element of vertices array
	call ASMDot
testL:
	comiss xmm0, real4 ptr[r9]; compare 'projection' and 'min'
	jnc testMax; jump if less or equal
	movss real4 ptr[r9], xmm0; min = projection
testMax:
	comiss xmm0, real4 ptr [rax]; compare 'projection' and 'max'
	jc endLoop; jump if more or equal
setMaxProj:
	movd real4 ptr [rax], xmm0; rbx, which is max = projection
endLoop:
	loop loopHead
	pop rbx
	ret
ASMSetProjection endp
end