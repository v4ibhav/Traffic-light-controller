#define SYSCTL_RCC2_R   (*((volatile unsigned long *) ))    //register address is written here
#define SYSCTL_RIS_R    (*((volatile unsigned long *) ))
void PLL_init(void){
    // 0) Use RCC2 because it provides more options.
    SYSCTL_RCC2_R |= 0x80000000;  //USERCC2
// 1) Bypass pll while intiating
    SYSCTL_RCC2_R |= 0x00000800;  //BYPASS2 .PLL bypass 
// 2) specify crystal value & osclattor source is cleared
    // 2.1) clear the crystal value and then configure it 
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R &~0x000007C0) + 0x00000540;
    //2.2) configure for main oscillator source that is oscsrc2
    SYSCTL_RCC2_R |= 0x00000070;
// 3)clear pwdrdn2 this will activate the pll
    SYSCTL_RCC2_R &= ~0x00002000 ;
// 4) setting up the desired system divider using sysdiv2;
    SYSCTL_RCC2_R |= 0x40000000; //this is to use 400 mhz pLL;
    SYSCTL_RCC2_R |= (SYSCTL_RCC2_R & ~0x01FC0000) //sysdiv is cleared and the put in the divider number
                        + (4<<22);
//5) wait for pll to lock or stabilize that is by polling for PLLRIS
    while((SYSCTL_RIS_R&0x00000040)==0){};
// 6) enable pll by clearing bypass2
    SYSCTL_RCC2_R  &= ~0x00000800;
    
    // this is activation sequence of the pll








}