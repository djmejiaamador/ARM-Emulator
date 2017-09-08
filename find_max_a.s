	.global find_max_a
	.func find_max_a
find_max_a:
	//r0 = base of array
	//r1 = size of array
	//r2 = i
	//r3 = current max

	mov r2, #0 // i = 0;
	ldr r3, [r0] // current_max = array[0]
loop1:
	cmp r2, r1
	beq endloop1
	ldr r4, [r0]// temp = array[0]
	cmp r3, r4 // if( current_max < temp)
	movlt r3, r4 // current_max == temp  if max < temp
	add r2,r2,#1
	add r0,r0,#4

	b loop1

endloop1:
	mov r0,r3

	bx lr
