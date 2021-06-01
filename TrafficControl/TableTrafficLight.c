// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

//clock preprocessors
#define STCTRL_R (*((volatile unsigned long *) 0xE000E010))
#define STRELOAD_R (*((volatile unsigned long *) 0xE000E014))
#define STCURRENT_R   (*((volatile unsigned long *)0xE000E018))
//input port initiation
#define Input (*((volatile unsigned long *) 0x40004070)) //input PA4-2
//output define 
#define StreetLights (*((volatile unsigned long *) 0x400050FC)) //outputs are PB 0-5
#define PedLights   (*((volatile unsigned long *)0x40025028))	//accesses PF3 and PF1
//Registers for port A
#define GPIO_PORTA_IN           (*((volatile unsigned long *)0x40004070)) // bits 2-0
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))	
	
//Registers for port B
#define GPIO_PORTB_DIR_R        (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile unsigned long *)0x4000552C))
//Registers for port F
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))

#define GoW    0
#define WaitW  1
#define GoS 2
#define WaitS 3
#define GoPed 4
#define B1 5
#define B2 6
#define B3 7
#define B4 8
 


//FSM Modelling
struct State{
	unsigned long Streetlights;
	unsigned long Pedlights;
	unsigned long time;
	unsigned long next_State[8];
};


// ***** 2. Global Declarations Section *****
unsigned long Cstate;
unsigned long In;

// FUNCTION PROTOTYPES: Each subroutine defined
void Systik_Init(void);
void Ports_Init(void);
void Systik_timer(unsigned long delay);
void Systik_10mstimer(unsigned long delay);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****
typedef const struct State SType;
SType FSM[9] = {
	{0x0C,0x02,300,{0,0,1,1,1,1,1,1}},
	{0x14,0x02,200,{2,2,2,2,4,4,2,2}},
	{0x21,0x02,300,{2,3,2,3,3,3,3,3}},
	{0x22,0x02,200,{4,0,4,0,4,4,4,4,}},
	{0x24,0x08,300,{4,5,5,5,4,5,5,5}},
	{0x24,0x02,50,{6,6,6,6,4,6,6,6}},
	{0x24,0x00,50,{7,7,7,7,4,7,7,7}},
	{0x24,0x02,50,{8,8,8,8,4,8,8,8}},
	{0x24,0x00,50,{0,0,2,0,4,0,2,0}}};
int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  Systik_Init();
  Ports_Init();
  Cstate = GoW;  
  EnableInterrupts();
  while(1){
  	StreetLights = FSM[Cstate].Streetlights;
  	PedLights = FSM[Cstate].Pedlights;
  	Systik_10mstimer(FSM[Cstate].time);
  	In = Input>>2;
  	Cstate = FSM[Cstate].next_State[In]; 
  }
}






void Systik_Init(void){
	STCTRL_R = 0;
	STCTRL_R = 0x00000005; //clock source and enable clock frequency is 80 Mhz

}

void Systik_timer(unsigned long delay){
	STRELOAD_R = delay-1;
	STCURRENT_R = 0;
while((STCTRL_R&0x00010000) == 0){

	}
}
void Systik_10mstimer(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    Systik_timer(800000);  // wait 10ms
  }
}

// Port Initalizers
void Ports_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x23;      // 1) A B F ports
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTA_AMSEL_R &= ~0x1C; // 3) disable analog function on PA4-2
  GPIO_PORTA_PCTL_R &= ~0x000FFF00; // 4) enable regular GPIO
  GPIO_PORTA_DIR_R &= ~0x1C;   // 5) inputs on PA4-2
  GPIO_PORTA_AFSEL_R &= ~0x1C; // 6) regular function on PA4-2
  GPIO_PORTA_DEN_R |= 0x1C;    // 7) enable digital on PA4-2
  
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
	
  GPIO_PORTF_AMSEL_R &= ~0x0A;        // 3) disable analog function PF1 and PF3
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0;   // 4) GPIO clear bit PCTL  PF1 and PF3
  GPIO_PORTF_DIR_R |= 0x0A;          // 5.2) PF3,PF1 as outputs  
  GPIO_PORTF_AFSEL_R &= ~0x0A;        // 6) no alternate function PF1 and PF3
  GPIO_PORTF_DEN_R |= 0x0A;    // 7) enable digital on PF1 and PF3
}


