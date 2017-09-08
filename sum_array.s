	.global sum_array_a
	.func sum_array_a
	//array is passed in so no need to creat it or allocate space for it
sum_array_a:

	//r0 is the base address of array
	//r1 is size
	//r2 will be i
	//r3 is sum

	mov r2, #0 // int i
	mov r3, #0// int sum
loop1:
	cmp r2, r1
	beq endloop1
	ldr r4, [r0]
	add r3, r3, r4
	add r0, r0, #4
	add r2,r2,#1
	b loop1
endloop1:
	mov r0, r3

	bx lr
	 
