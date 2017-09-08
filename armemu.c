#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int add( int a, int b);
int sum_array_a(int *array, int i);
int find_max_a(int *array, int n);
int fib_rec_a(int n);
int fib_iter_a(int n);
int find_str_a(char *s, char *sub);

#define NREGS 16
#define STACK_SIZE 1024
#define SP 13
#define LR 14
#define PC 15

// Condition flags;

#define EQ = 0
#define LT = 11
#define Al = 14



struct arm_state {
  unsigned int regs[NREGS];
  unsigned int cpsr;

  unsigned int eq;
  unsigned int lt;
  unsigned int ne;

  unsigned char stack[STACK_SIZE];
  unsigned int stack_pos;
  int num_inst;
  int data_inst;
  int branch_inst;
  int memory_inst;
};

void init_arm_state(struct arm_state *as, unsigned int *func,
		    unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
  int i;

  /* zero out all arm state */
  for (i = 0; i < NREGS; i++) {
    as->regs[i] = 0;
  }

  as->cpsr = 0;

  for (i = 0; i < STACK_SIZE; i++) {
    as->stack[i] = 0;
  }

  as->regs[PC] = (unsigned int) func;
  as->regs[SP] = (unsigned int) &as->stack[STACK_SIZE];
  as->regs[LR] = 0;
  as->stack_pos = 0;

  as->regs[0] = arg0;
  as->regs[1] = arg1;
  as->regs[2] = arg2;
  as->regs[3] = arg3;

  as->eq = 0;
  as->lt = 0;
  as->ne = 0;

  as->num_inst = 0;
  as->data_inst = 0;
  as->memory_inst = 0;
  as->branch_inst = 0;
}

bool is_add_inst(unsigned int iw)
{
  unsigned int op;
  unsigned int opcode;

  op = (iw >> 26) & 0b11;
  opcode = (iw >> 21) & 0b1111;

  return (op == 0) && (opcode == 0b0100);
}

bool is_sub_inst(unsigned int iw){

  unsigned int op;
  unsigned int opcode;

  op = ( iw >> 26 ) & 0b11;
  opcode = ( iw >> 21 ) & 0b1111;

  return ( (op == 0 ) && ( opcode == 0b0010) );
}

bool is_mov_inst(unsigned int iw){

  unsigned int op;
  unsigned int opcode;

  op = ( iw >> 26 ) & 0b11;
  opcode = ( iw >> 21 ) & 0b1111;

  return ( (op == 0b00 ) && (  (opcode == 0b1101) || (opcode == 0b1111) ) );
}

bool is_cmp_inst(unsigned int iw){

  unsigned int op;
  unsigned int opcode;

  op = ( iw >> 26 ) & 0b11;
  opcode = ( iw >> 21 ) & 0b1111;
  return ( (op == 0 ) && ( opcode == 0b1010 )  );
}

bool is_mul_inst( unsigned int iw){
  unsigned int six_zeros, second_marker;
  six_zeros = (iw >> 22) & 0b111111;
  second_marker = (iw >> 4) & 0xF;

  return ( (six_zeros == 0b000000) && (second_marker == 0b1001) );
}

// branches check

bool is_branch_inst(unsigned int iw){
  unsigned int opcode = (iw >> 25) & 0b111;
  unsigned int linkBit = ( iw >> 24) & 0b1;
  return ( (opcode == 0b101) && (linkBit == 0b0) );
}


bool is_branch_l_inst(unsigned int iw){
  unsigned int opcode = (iw >> 25) & 0b111;
  unsigned int linkBit = ( iw >> 24) & 0b1;
  return ( (opcode == 0b101) && (linkBit == 0b1) );
}

bool is_ldr_inst(unsigned int iw){
  unsigned int opcode = (iw >> 26) & 0b11;
  unsigned int l_bit = (iw >> 20) & 0b1;
  unsigned int b_bit =  (iw >> 22) & 0b1;
  return ( (opcode == 01) && ( l_bit == 1)  && ( b_bit == 0));
}

bool is_str_inst(unsigned int iw){
  unsigned int opcode = (iw >> 26) & 0b11;
  unsigned int l_bit = (iw >> 20) & 0b1;
  return ( (opcode == 01) && ( l_bit == 0) );
}

bool is_ldbr_inst(unsigned int iw){
  unsigned int opcode = (iw >> 26) & 0b11;
  unsigned int l_bit = (iw >> 20 ) & 0b1;
  unsigned int b_bit = (iw >> 22 ) & 0b1;
  return ( (opcode == 01) && ( l_bit == 1)  && ( b_bit == 1) );
}





void armemu_add(struct arm_state *state)
{

  state->num_inst++;
  state->data_inst++;
  unsigned int iw;
  unsigned int rd, rn,source, i;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;

  i = (iw >> 25) & 0b1;


  // i = 0 dealing with reg
  // i = 1 dealing with imme value
  if( i == 0 ) {
    source  = iw & 0xF;
    state->regs[rd] = state->regs[rn] +  state->regs[source];
    if(rd = SP){

    }
  }else if( i == 1 ){
    source = iw & 0xFF;
    state->regs[rd] = state->regs[rn] + source;
  }


  // to update pc
  if(rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_sub(struct arm_state *state)
{
  state->num_inst++;
  state->data_inst++;
  unsigned int iw;
  unsigned int rd, rn,source, i;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;

  i = (iw >> 25) & 0b1;


  // i = 0 dealing with reg
  // i = 1 dealing with imme value
  if( i == 0 ) {
    source  = iw & 0xF;
    state->regs[rd] = state->regs[rn] - state->regs[source];
  }else if( i == 1 ){
    source = iw & 0xFF;
    state->regs[rd] = state->regs[rn] - source;
  }
  // to update pc
  if(rd != PC) {
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_mov(struct arm_state *state){
  bool proceed = false;
  unsigned int iw; // intstruction word
  unsigned int rd, rn, i, cond, source,opcode;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  i = (iw >> 25) & 0b1;
  cond = (iw >> 28) & 0xF;
  opcode = (iw >> 21) & 0xF;




  if( cond == 0b1110){

    proceed = true;
  }else if ( (cond == 0b0000) && (state->eq == 1) ){
    proceed = true;

  }else if ( (cond == 0b0001) && (state->ne == 1) ){

    proceed = true;
  }else if ( (cond == 0b1011) && (state->lt == 1) ){
    proceed = true;
  }

  if(proceed == true){
    state->num_inst++;
    state->data_inst++;
    // i = 0 dealing with reg
    // i = 1 dealing with imm
    if( i == 0 ){
      source = iw & 0xF;

      state->regs[rd] = state->regs[source];
      //}
    }else if ( i == 1 ){

      source  = iw & 0xFF;

      if(opcode == 0b1111){
	state->regs[rd] = ~source;
      }else{
	state->regs[rd] = source;
      }
    }
  }

  //update pc
  if(rd != PC){
    state->regs[PC] = state->regs[PC] + 4;
  }
}

// cmp
void armemu_cmp(struct arm_state *state){
  state->num_inst++;
  state->data_inst++;
  unsigned int iw; // intstruction word
  unsigned int rd, rn, i,source, result;
  int opOne,opTwo;
  state->eq = 0;
  state->ne = 0;
  state->lt = 0;

  iw = *((unsigned int *) state->regs[PC]);

  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  i = (iw >> 25) & 0b1;
  // i = 0 dealing with reg
  // i = 1 dealing with imm
  if( i == 0){
    source = iw & 0xF;
    result = state->regs[rn] - state->regs[source];
    opOne = state->regs[rn];
    opTwo = state->regs[source];
  }else if ( i == 1){
    source = iw & 0xFF;
    result = state->regs[rn] - source;
    opOne = state->regs[rn];
    opTwo = source;
  }

  // set conditions;
  if(result == 0){
    state->eq = 1;
  }else{
    state->ne = 1;
  }
  
  if( opOne < opTwo ){
    state->lt = 1;
  }else{
    state->lt = 0;
  }

  //update pc
  if(rd != PC){
    state->regs[PC] = state->regs[PC] + 4;
  }
}


void armemu_mul( struct arm_state *state){
  state->num_inst++;
  state->data_inst++;
  unsigned int iw, rd, rn, rm;
  iw = *((unsigned int *) state->regs[PC]);
  rd = (iw >> 16) & 0xF;
  rn = (iw & 0xF);
  rm = (iw >> 8) & 0xF;

  state->regs[rd] = state->regs[rn] * state->regs[rm];

  if(rd != PC){
    state->regs[PC] = state->regs[PC] + 4;
  }
}

void armemu_branch(struct arm_state *state){
  bool proceed = false;
  unsigned int  thirty_two_bits;
  unsigned int iw;
  unsigned int offset;
  unsigned int cond;

  iw = *((unsigned int *) state->regs[PC]);
  offset = (iw & 0xFFFFFF);
  cond = (iw >> 28) & 0xF;

  if( cond == 0b1110){

    proceed = true;
  }else if ( (cond == 0b0000) && (state->eq == 1) ){
    proceed = true;

  }else if ( (cond == 0b0001) && (state->ne == 1) ){

    proceed = true;
  }

  if(proceed == true){
    state->num_inst++;
    state->branch_inst++;
    if( offset & 0x800000){

      thirty_two_bits = 0xFF000000 + offset;

    }else{

      thirty_two_bits = offset;
    }
    thirty_two_bits  = thirty_two_bits <<  2;
    state->regs[PC] = state->regs[PC] + 8;

    state->regs[PC] = state->regs[PC] + thirty_two_bits;
  }else{
    state->regs[PC] = state->regs[PC] + 4;
  }

}

void armemu_branch_l(struct arm_state *state){
  bool proceed = false;
  unsigned int  thirty_two_bits;
  unsigned int iw;
  unsigned int offset;
  unsigned int cond;

  iw = *((unsigned int *) state->regs[PC]);
  offset = (iw & 0xFFFFFF);
  cond = (iw >> 28) & 0xF;


  if( cond == 0b1110){
    proceed = true;
  }else if ( (cond == 0b0000) && (state->eq == 1) ){
    proceed = true;
  }else if ( (cond == 0b0001) && (state->ne == 1) ){
    printf("conditiong is NE \n");
    proceed = true;
  }

  if(proceed == true){
    state->num_inst++;
    state->branch_inst++;
    if( offset & 0x800000){

      thirty_two_bits = 0xFF000000 + offset;
    }else{

      thirty_two_bits = offset;
    }
    thirty_two_bits  = thirty_two_bits <<  2;
    state->regs[PC] = state->regs[PC] + 8;
    state->regs[LR] = state->regs[PC] - 4;
    state->regs[PC] = state->regs[PC] + thirty_two_bits;
  }else{
    state->regs[PC] = state->regs[PC] + 4;
  }

}



void armemu_ldr(struct arm_state *state){
  unsigned int  iw, i_bit, rd ,rn, offset;
  state->num_inst++;
  state->memory_inst++;
  iw = *((unsigned int *) state->regs[PC]);
  i_bit = (iw >> 25) &0b1;
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;
  offset = iw & 0xFFF;


  unsigned int *p =  (unsigned int *)(state->regs[rn]-offset);
  state->regs[rd] = *p;

  if(rd != PC){
    state->regs[PC] = state->regs[PC] + 4;
  }

}


void armemu_ldrb(struct arm_state *state){
  unsigned int  iw, i_bit, rd ,rn, offset, meh;
  state->num_inst++;
  state->memory_inst++;
  iw = *((unsigned int *) state->regs[PC]);
  i_bit = (iw >> 25) &0b1;
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;

  if( i_bit == 0){
    offset = iw & 0xFF;

    meh = offset;

    unsigned int  *p =  (unsigned int  *)(state->regs[rn] +  meh);
    state->regs[rd] = (unsigned char )*p;
  }else{
    offset = iw & 0xF;

    unsigned int  *p =  (unsigned int  *)(state->regs[rn] +  state->regs[offset]);
    state->regs[rd] = (unsigned char )*p;


  }

  if(rd != PC){
    state->regs[PC] = state->regs[PC] + 4;
  }

}

void armemu_str(struct arm_state *state){
  state->num_inst++;
  state->memory_inst++;
  unsigned int  iw, i_bit, rd ,rn;
  iw = *((unsigned int *) state->regs[PC]);
  i_bit = (iw >> 25) &0b1;
  rd = (iw >> 12) & 0xF;
  rn = (iw >> 16) & 0xF;

  unsigned int *p =  (unsigned int *)state->regs[rn];
  *p  =  state->regs[rd];
  if(rd != PC){

    state->regs[PC] = state->regs[PC] + 4;
  }
}


bool is_bx_inst(unsigned int iw)
{
  unsigned int bx_code;

  bx_code = (iw >> 4) & 0x00FFFFFF;

  return (bx_code == 0b000100101111111111110001);
}

void armemu_bx(struct arm_state *state)
{
  unsigned int iw;
  unsigned int rn;
  state->num_inst++;
  state->branch_inst++;
  iw = *((unsigned int *) state->regs[PC]);
  rn = iw & 0b1111;

  state->regs[PC] = state->regs[rn];
}

void armemu_one(struct arm_state *state)
{
  unsigned int iw;

  iw = *((unsigned int *) state->regs[PC]);

  if(is_bx_inst(iw)) {

    armemu_bx(state);
  } else if (is_add_inst(iw)) {
    armemu_add(state);
  } else if( is_sub_inst(iw)) {
    armemu_sub(state);
  }else if( is_mov_inst(iw)){
    armemu_mov(state);
  }else if( is_cmp_inst(iw)){
    armemu_cmp(state);
  }else if( is_mul_inst(iw)){
    armemu_mul(state);
  }else if( is_ldr_inst(iw)){
    armemu_ldr(state);
  }else if(is_str_inst(iw)){
    armemu_str(state);
  }else if( is_branch_inst(iw)){
    armemu_branch(state);
  }else if( is_branch_l_inst(iw)){
    armemu_branch_l(state);
  }else if( is_ldbr_inst(iw) ){
    armemu_ldrb(state);
  }
}


unsigned int armemu(struct arm_state *state)
{

  while (state->regs[PC] != 0) {
    armemu_one(state);
  }

  return state->regs[0];
}


int main(int argc, char **argv)
{
  struct arm_state find_max, fib_iter, fib_rec, sum_array, find_str;
  unsigned int r;

  printf("-----------------emulating sum_array.s --------------------\n");
  int test[6] ={ 1, 2, 6, 4 ,54 ,3};
  int i;

  printf("Input list: { ");
  for (i = 0; i<6; i++){
    printf("%d, ", test[i] );
  }
  printf("}\n");

  init_arm_state(&sum_array, (unsigned int *)sum_array_a, (unsigned int)test, 6, 0, 0);
  r = armemu(&sum_array);
  printf("Total instructions executed  %d \n", sum_array.num_inst);
  printf("Data processiong instructions exected  %d \n", sum_array.data_inst);
  printf("Memory processiong intructions executed %d\n", sum_array.memory_inst);
  printf("Branch instructions exected  %d \n",sum_array.branch_inst);
  printf("result when running arm code: %d\n", sum_array_a(test,6) );
  printf("emulated result = %d\n", r);
  printf("-----------------Done emulating sum_array.------------------\n");

  printf("\n\n");

  printf("-----------------emulating find_max.s --------------------\n");
  int test2[7] ={ 1,2,6,4,54,3,540};

  printf("Input list: { ");
  for (i = 0; i<7; i++){
    printf("%d, ", test2[i] );
  }
  printf("}\n");
  //rintf("regs[rn] is: %d\n",state->regs[rn]);
  //printf("regs[source] is: %d\n", state->regs[source]);
  init_arm_state(&find_max, (unsigned int *)find_max_a, (int)test2, 7, 0, 0);
  r = armemu(&find_max);
  printf("Total instructions executed  %d \n", find_max.num_inst);
  printf("Data processiong instructions exected  %d \n", find_max.data_inst);
  printf("Memory processiong intructions executed %d\n", find_max.memory_inst);
  printf("Branch instructions exected  %d \n",find_max.branch_inst);
  printf("\nresult when running arm code: %d\n", find_max_a(test2,7) );
  printf("emulated result = %d\n", r);

  printf("-----------------Done emulating find_max.s------------------\n");


  printf("\n\n");

  printf("-----------------emulating fib_iter.s --------------------\n");
  int test3 = 9;
  printf( "input value: %d\n",test3);
  init_arm_state(&fib_iter, (unsigned int *)fib_iter_a, (unsigned int)test3, 0, 0, 0);
  r = armemu(&fib_iter);
  printf("Total instructions executed  %d \n", fib_iter.num_inst);
  printf("Data processiong instructions exected  %d \n", fib_iter.data_inst);
  printf("Memory processiong intructions executed %d\n", fib_iter.memory_inst);
  printf("Branch instructions exected  %d \n",fib_iter.branch_inst);
  printf("\nresult when running arm code: %d\n", fib_iter_a(test3) );
  printf("emulated result = %d\n", r);
  printf("-----------------Done emulating fib_iter.s------------------\n");


  printf("\n\n");

  printf("-----------------emulating fib_rec.s --------------------\n");
  int test4 = 10;
  printf("Input: %d\n",test4);
  init_arm_state(&fib_rec, (unsigned int *)fib_rec_a, (unsigned int)test4, 0, 0, 0);
  r = armemu(&fib_rec);
  printf("Total instructions executed  %d \n", fib_rec.num_inst);
  printf("Data processiong instructions exected  %d \n", fib_rec.data_inst);
  printf("Memory processiong intructions executed %d\n", fib_rec.memory_inst);
  printf("Branch instructions exected  %d \n",fib_rec.branch_inst);
  printf("\nresult when running arm code: %d\n", fib_rec_a(test4) );
  printf("emulated result = %d\n", r);
  printf("-----------------Done emulating fib_rec.s------------------\n");


  printf("\n\n");

  printf("-----------------emulating find_str.s --------------------\n");
  char* str = "cheeseAbout";
  char * sub = "Abouts";
  printf("input string: %s  sub: %s\n",str,sub);
  init_arm_state(&find_str, (unsigned int *)find_str_a, (unsigned int)str,(unsigned int)sub, 0, 0);
  r = armemu(&find_str);
  printf("Total instructions executed  %d \n", find_str.num_inst);
  printf("Data processiong instructions exected  %d \n", find_str.data_inst);
  printf("Memory processiong intructions executed %d\n", find_str.memory_inst);
  printf("Branch instructions exected  %d \n",find_str.branch_inst);
  printf("\nresult when running arm code: %d\n", find_str_a(str,sub) );
  printf("emulated result = %d\n", r);
  printf("-----------------Done emulating find_str.s------------------\n");


  return 0;
}
