	.global fib_rec_a
	.func fib_rec_a

fib_rec_a:
	// r0 = value passed into funtion call (n).

	sub sp, sp, #4 // makes space on stack
	str lr, [sp] // saves lr on stack-4

	// because ill be using r4, I need to save it on stack. therefore,

	sub sp, sp, #4 //makes more space on stack
	str r0, [sp]
	//str r0, [sp] // stores initial value of r0/n  on stack


	cmp r0, #0 // makes initital comparison: n and 0 (i.e. first base case n == 0)
	beq fib_exit // go to end

	cmp r0, #1 //makes another comparison: n and 1 (i.e. second base case n ==1)
	beq fib_exit // since n == 1 make return 1

fib_rec:

	ldr r0,[sp]
	sub r0, r0, #1

	bl fib_rec_a

	mov r5, r0
	ldr r0, [sp]

	sub sp, sp, #4 //allocate space for r5
	str r5, [sp] // store r5
	//sub r0, r0, #1
	sub r0, r0, #2

	bl fib_rec_a

	ldr r5, [sp] // restore r5 from stack
	add sp, sp, #4 // restores space used for r5

	//---------
	mov r6, r0
	sub sp, sp, #4 //allocate space for r6
	str r6, [sp] // stores r6

	add r0, r5, r6

	ldr r5, [sp]
	add sp, sp, #4

	b end_it

fib_exit:
	ldr r0, [sp]
end_it:
	add sp, sp, #4
	ldr lr, [sp]
	add sp, sp, #4
	bx lr
