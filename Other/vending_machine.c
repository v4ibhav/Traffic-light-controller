// intialisation sequence
#define T10ms  80000
#define T20ms  160000
#define SODA    (*((volatile unsigned long *) 0x40005004)) //PB0
#define CHANGE   (*((volatile unsigned long *) 0x40005008)) //PB1
#define COIN  (*((volatile unsigned long *) 0x4002400C) //PE1,PE0
void FSM_init(void){
	// initialise ports and delays
	PLL_init();
	Systik_init();
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |=0x00000012;             //port b and e
	delay = SYSCTL_RCGC2_R;d
	//now to unlock port B and E;
  GPIO_PORTE_AMSEL_R &= ~0x03; // 3) disable analog function on PE1-0
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x03;   // 5) inputs on PE1-0
  GPIO_PORTE_AFSEL_R &= ~0x03; // 6) regular function on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;    // 7) enable digital on PE1-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x000000FF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x03;    // 5) outputs on PB1-0
  GPIO_PORTB_AFSEL_R &= ~0x03; // 6) regular function on PB1-0
  GPIO_PORTB_DEN_R |= 0x03;    // 7) enable digital on PB1-0
  SODA = 0;
  CHANGE =0;
}
// funtions
void Solenoid_NONE(void){

};
unsigned long Coin_input(void){
	return COIN;
}
void Solenoid_Soda(void){
	SODA = 0x01;
	Systick(T10ms);
	SODA = 0x00;

}
void Solenoid_Change(void){
	CHANGE = 0x02;
	Systick(T10ms);
	CHANGE = 0x00;

}

// model creation
struct State{
	void(*CmdPT)(void);
	unsigned long time;
	unsigned long Next[3];

};
typedef const struct State Styp;
#define M0 0
#define W5 1
#define M5 2
#define W10 3
#define M10 4
#define W15 5
#define M15 6
#define W20 7
#define M20 8
Styp FSM[9]={
  {&Solenoid_None,  T20ms,{M0,W5,W10}},      // M0, no money
  {&Solenoid_None,  T20ms,{M5,W5,W5}},       // W5, seeing a nickel
  {&Solenoid_None,  T20ms,{M5,W10,W15}},     // M5, have 5 cents
  {&Solenoid_None,  T20ms,{M10,W10,W10}},    // W10, seeing a dime
  {&Solenoid_None,  T20ms,{M10,W15,W20}},    // M10, have 10 cents
  {&Solenoid_None,  T20ms,{M15,W15,W15}},    // W15, seeing something
  {&Solenoid_Soda,  T20ms,{M0,M0,M0}},       // M15, have 15 cents
  {&Solenoid_None,  T20ms,{M20,W20,W20}},    // W20, seeing dime
  {&Solenoid_Change,T20ms,{M15,M15,M15}}   // M20, have 20 cents
};
unsigned long S;
unsigned long Input;
int main(void){
	FSM_init();
	S = M0;
	while(1){
		(FSM[S].CmdPT)(); //calling output function of current state 
		Systick(FSM[S].time); //calling wait delay of current state
		Input = Coin_input; //getting input for the next state
		S = FSM.Next[Input]; //calling the next state from input
		}
}
