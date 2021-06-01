// implementing time delay using systik

#define NVIC_ST_CTRL_R  (*((volatile unsigned long*)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long*)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long*))0xE000E018))

// systik intialisation
void systik_init(void){
    NVIC_ST_CTRL_R = 0; //disable the sytik
    NVIC_ST_CTRL_R = 0x00000005;

}
//the clk source has frequency of 80mhz max delay with this function will be 2^24*12.5ns= 200 ms
void systik_wait(unsigned long  delay){
    NVIC_ST_RELOAD_R =delay-1;
    NVIC_ST_CURRENT_R = 0;
    while((NVIC_ST_CTRL_R&0x00010000) == 0){

    }
}
void systik_wait_10ms(unsigned long  delay){
    for(unsigned long i=0;i<delay;i++){
        systik_wait(800000);
    }
}