// this code shows intialisation process for systik clock in cortex m4
// define registers first 

#define NVIC_ST_CTRL_R  (*( (volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
void systik(void){
    // 1) disable SysTick during setup
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_CTRL_R = 0x00000005; 
}

void Systik_timer(unsigned long delay){
	NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000) == 0){

	}
}
void Systik_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_timer(800000);  // wait 10ms
  }
}

// #define STCTRL_R    (*((volatile unsigned long *)0xE000E010))
// #define STRELOAD_R  (*((volatile unsigned long *)0xE000E014))
// #define STCURRENT   (*((volatile unsigned long *)0xE000E018))

// //intialisation function
// void systik(void){
//     STCTRL_R = 0;
//     STRELOAD_R = 0x00FFFFFF;
//     STCTRL_R = 0;
//     STCTRL_R = 0x00000005;

// }