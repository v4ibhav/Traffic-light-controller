#define GPIO_PORTB_DATA_R Output
#define GPIO_PORTE_DATA_R&0x03 Sensor

struct State{
	unsigned char out;
	unsigned short time;
	unsigned int Next[4]
};
typedef const struct State StateType;
StateType FSM[10]{
  {0x55,2,{1,4,7,3}},
  {0x66, 2, {2, 2, 2, 0} }, // S1) both forward [1]
  {0xAA, 2, {3, 3, 3, 1} }, // S2) both forward [2]
  {0x99, 2, {0, 0, 0, 2} }, // S3) both forward [3]
  {0x69, 5, {5, 5, 5, 5} }, // S4) turn left, left reverse; right forward[1] left
  {0xAA, 5, {6, 6, 6, 6} }, // S5) turn left, left reverse; right forward[2] left
  {0x96, 5, {0, 0, 0, 0} }, // S6) turn left, left reverse; right forward[3] left
  {0x96, 5, {8, 8, 8, 8} }, // S7) turn right, left forward; right reverse[1] right
  {0xAA, 5, {9, 9, 9, 9} }, // S8) turn right, left forward; right reverse[2] right
  {0x69, 5, {0, 0, 0, 0} }  // S9) turn right, left forward; right reverse[3] right
};

void PortB_Init(void){
	volatile long delay;
	SYSCTL_RCGC2_R |= 0x02;
	delay = SYSCTL_RCGC2_R;
	//no need to unlock;
	GPIO_PORTB_AMSEL_R = 0x00;
	GPIO_PORTB_PCTL_R = 0x00000000;
	GPIO_PORTB_DIR_R = 0xFF; //port pb0-7 all are outputs
	GPIO_PORTB_AFSEL_R = 0x00;
	GPIO_PORTB_DR8R_R = 0xFF;        // enable 8 mA drive on PB7-0
    GPIO_PORTB_DEN_R = 0xFF;         // 7) enable digital I/O on PB7-0
}
void PortE_Init(void){
	volatile long delay;
	SYSCTL_RCGC2_R |= 0x10;
	delay = SYSCTL_RCGC2_R;
	//PE 01;
	GPIO_PORTE_AMSEL_R &= ~0x03;
	GPIO_PORTE_PCTL_R &= ~0x03;
	GPIO_PORTE_DIR_R &= ~0x03;
	GPIO_PORTE_AFSEL_R &= ~0x03;     // 6) disable alt funct on PE1-0
    GPIO_PORTE_DEN_R |= 0x03;        // 7) enable digital I/O on PE1-0 

}
unsigned char cState;
int main(void){
	PLL_Init();
	Systick_Init();
	PortB_Init();
	PortE_Init();
	cState = 0;
	while(1){
		Output = FSM[cState].out;
		Systick_Wait10ms(FSM[cState].time);
		Input = Sensor;
		cState = FSM[cState].Next[Input];

	}
}