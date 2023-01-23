



 void ATD_init(void);
unsigned int ATD_read(void);
unsigned int k;
unsigned char myscaledVoltage;
unsigned char mysevenseg[10]={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

unsigned int angle;
unsigned int Dcntr;
unsigned int Mcntr;
unsigned char cntr;
unsigned char HL;//High Low
unsigned int temp;

void myDelay(unsigned int x);






  //////////
sbit LCD_RS at RB1_bit;

sbit LCD_EN at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;

sbit LCD_RS_Direction at TRISB1_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;




int i;

void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(unsigned char s);
void ir_int()  ;
void interrupt(void)
{
     if(INTCON&0x02)
     {


        INTCON=INTCON&0XFD  ;
     }

     //// /////
      if(INTCON&0x04){// will get here every 1ms
    TMR0=248;
    Mcntr++;
    Dcntr++;
    if(Dcntr==500){//after 500 ms
      Dcntr=0;

    }
  INTCON = INTCON & 0xFB; //clear T0IF
}
 if(PIR1&0x04){//CCP1 interrupt
   if(HL){ //high
     CCPR1H= angle >>8;
     CCPR1L= angle;
     HL=0;//next time low
     CCP1CON=0x09;//next time Falling edge
     TMR1H=0;
     TMR1L=0;
   }
   else{  //low
     CCPR1H= (40000 - angle) >>8;
     CCPR1L= (40000 - angle);
     CCP1CON=0x08; //next time rising edge
     HL=1; //next time High
     TMR1H=0;
     TMR1L=0;

   }

 PIR1=PIR1&0xFB;
 }
 if(PIR1&0x01){//TMR1 ovwerflow


   PIR1=PIR1&0xFE;
 }






}

void main() {

     TRISB=0x01  ;
     TRISC=0x00  ;
     TRISD=0X03;
     INTCON=0X90    ;
      OPTION_REG=OPTION_REG&0XBF  ;



    Lcd_Init();
    ////////////////////////////////////////////////////
      PORTD=0x00;
ATD_init();
PORTA=0x08;//enable 4th seven segment display
TMR1H=0;
TMR1L=0;
TMR0=248;

HL=1; //start high
CCP1CON=0x08;//

OPTION_REG = 0x87;//Fosc/4 with 256 prescaler => incremetn every 0.5us*256=128us ==> overflow 8count*128us=1ms to overflow

T1CON=0x01;//TMR1 On Fosc/4 (inc 0.5uS) with 0 prescaler (TMR1 overflow after 0xFFFF counts ==65535)==> 32.767ms

INTCON=0xF0;//enable TMR0 overflow, TMR1 overflow, External interrupts and peripheral interrupts;
PIE1=PIE1|0x04;// Enable CCP1 interrupts
CCPR1H=2000>>8;
CCPR1L=2000;

//angle=600; //600us initially == 1000*0.5=500


     Lcd_cmd(0xc0);
            Lcd_Out_CP("STARTING  THE    CRANE SYSTEM        ");

      myDelay(2500);




     ///////////////////////////////////////////////////
    while(1)
    {





      while(PORTD&0X01)
        {

         PORTD=0X30;
       if(PORTB&0X01==0) {
            Lcd_Cmd(_LCD_CLEAR);
            while(PORTB&0X01==0){
            PORTD=0X04;
            Lcd_cmd(0xc0);
            Lcd_Out_CP("Obstacle Detcted");
            myDelay(3000);
                         }
        }
         else {


            Lcd_cmd(0xc0);
          Lcd_Out_CP("Landing  area     is clear          ");
        }
        }

        while(PORTD&0X02)
        {
         PORTD=0X90;
       if(PORTB&0X01==0) {


            Lcd_Cmd(_LCD_CLEAR);
            while(PORTB&0X01==0){
            PORTC=0X03;
            PORTD=0X04;
            Lcd_cmd(0xc0);
            Lcd_Out_CP("Obstacle Detcted");
            myDelay(3000);
                         }
        }
         else {

            Lcd_cmd(0xc0);
          Lcd_Out_CP("Landing area is clear             ");
        }


        }

          PORTD=0X00;

           Lcd_cmd(0xc0);
            Lcd_Out_CP("THE   SYSTEM    IS READY         ");
         ///////////////////
         k = ATD_read();
      myscaledVoltage= ((k*5)/1023);

      k=k>>2;
      angle= 1000 + ((k*25)/2.55);//angle= 1000 + ((k*2500)/255);//1000count=500uS to 3500count =1750us
      if(angle>3500) angle=3500;
      if(angle<1000) angle=1000;


     }

}

void myDelay(unsigned int x){
     Mcntr=0;
     while(Mcntr<x);
}
void ATD_init(void){
      ADCON0=0x41;//ON, Channel 0, Fosc/16== 500KHz, Dont Go
      ADCON1=0xCE;// RA0 Analog, others are Digital, Right Allignment,
      TRISA=0x01;
}
unsigned int ATD_read(void){
      ADCON0=ADCON0 | 0x04;//GO
      while(ADCON0&0x04);//wait until DONE
      return (ADRESH<<8)|ADRESL;

}
