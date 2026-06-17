MaIn500:	add r3, r5
	prn #48
	macr m_macr
	cmp r3, #-6
	bne END
	endmacr
	lea STR, r6
	inc r6
	mov *r6, K
	sub r1, r4
	m_macr
	dec K
END:	stop
STR:	.string "ab"cd"
	.data 6, -9,+10 , -0500
		.data -100
K:		.data 31
