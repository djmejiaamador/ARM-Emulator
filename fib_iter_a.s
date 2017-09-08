	.global fib_iter_a
	.func fib_iter_a
	/* ----making array-----*/
	.data
	.balign 4

fib_iter_a:
	// r0 takes n
	// r1 = i
	// r2 = size +1
	// r3 = sum i.e. holder for  rv

	// initial comparisons
	cmp r0, #0 // if n = 0 return 0;
	beq end_it_all
	cmp r0 , #1 // if n = 1 return 1
	beq end_it_all


	//store n  in r4
	//mov r4, r0
	//sub sp, sp,4
	//str r4, [sp]

	//allocate space for array.
	mov r1, #0
	mov r2, #4
	mul r1 ,r0 ,r2
	sub sp, sp, r1

	mov r1, sp // loads base address of array
	mov r2, #0
	str r2, [r1] // store 0 in array[0]
	add r1, r1, #4


	// at array[1]
	mov r2, #1
	str r2, [r1]// store 1 in array[1]
	add r1, r1, #4 // move i to array[2]


	mov r2, r0 // save n to reg 2 // this will be size
	add r2 ,r0, #1 // make size to be used in limit
	mov r3 , #0 // holder value
	mov r0, #2 // i  = 2

loop:


	cmp r0, r2// if i(2) = n leave
	beq pre_end


	// add sum by arrayp[i-1]
	ldr r4, [r1, #-4]
	add r3, r4, #0

	// add sum by array[i-2]
	ldr r4, [r1, #-8]
	add r3, r3, r4


	str r3, [r1]
	add r1, r1, #4 // i++
	add r0, r0, #1
	//mov r3, #0
	b loop


	//estores all space back to stack
pre_end:
	//get value at n
	ldr r0, [r1,#-4]

	mov r3, #4
	sub r2,r2 ,#1 // to get n

	mul r1, r2, r3 // to get size of stack
	mov r2, r1

	add sp, sp, r2 // 4xn

end_it_all:
	bx lr 
