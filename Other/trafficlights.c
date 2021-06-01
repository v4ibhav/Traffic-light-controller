#define SENSOR (*((volatile unsigned long*)0x4002400C))
#define LIGHT   (*((volatile unsigned long*)0x400050FC))
//create the data structure and assign it with matrix
Struct State{
    unsigned long out;
    unsigned long time;
    unsigned long Next[4];
};
typedef Struct State Styp;
// define states
#define goN 0;
#define waitN 1;
#define goE 2;
#define waitE 3;
//assigning 
Styp FSM[4] = {
    {0x21,3000,{goN,waitN,goN,waitN}},//ox21 represnt the output of port 
    {0x22,50,{goE,goE,goE,goE}},
    {0x0C,3000,{goE,goE,waitE,waitE}},
    {0x14,50,{goN,goN,goN,goN}}
    }
unsigned long s;
unsigned long INPUT;

int main(void){
    PLL_Init();
    Systik_Init();
    // defining direction an other thing for ports
    //portE
    SYSCTL_RCGC2_R |= 0x12;
    delay = SYSCTL_RCGC2_R;
    // GPIO_PORTE_LOCK_R = 0x4C4F434B;
    GPIO_PORTE_AMSEL_R &= ~0X03;
    GPIO_PORTE_PCTL_R &= ~0x000000FF;
    GPIO_PORTE_DIR_R &= ~0x03;
    GPIO_PORTE_AFSEL_R &= ~0x03;
    GPIO_PORTE_DEN_R |= 0x03;
    GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
    GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
    GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
    GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
    GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0
    s = goN;
    while(1){
        Light = FSM[s].out;
        SysTick_wait10ms(FSM[s].time);
        INPUT = SENSOR;
        s = FSM[s].next[INPUT];
         }

}
