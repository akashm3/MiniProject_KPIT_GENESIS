#include<LiquidCrystal.h> 
#define SET_BIT(PORT,BIT)  PORT|=(1<<BIT)
#define CLR_BIT(PORT,BIT)  PORT&=~(1<<BIT)

const int rs = 12, en = 11, d4 =4, d5 = 5, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int adc_val1,adc_val2,flag1=0,flag2=0,cur_speed=10;
const int pingPin = 7;
long duration, cm;

/*Main */
void setup()
{  
  int i=0;
    Serial.begin(9600);
    InputOutputs();
 	Init_EXT_INT();
  	lcd.begin(16,2);
    lcd.setCursor(2, 0);
    lcd.print("ADAS SYSTEM");
    delay(1000);
    lcd.setCursor(2, 0);
    lcd.print("   PROJECT   ");
   lcd.setCursor(2, 1);
   lcd.print( " PRESENTATION ");
     
    delay(1000);
  
   lcd.setCursor(2, 0);
    lcd.print(" SYSTEM is ON  ");
  
  lcd.setCursor(2, 1);
    lcd.print(" ENGINE is ON  ");
  delay(1000);
  
  lcd.setCursor(2, 1);
    lcd.print("SPEED(KMPH):");
    lcd.clear();
}


/* Continuesly Running Function */
void loop()
{
  	long dist_cm;
  	dist_cm=ultra();
  	Serial.println(dist_cm);
  

  /* Interrupts Monitoring */
  		if(flag1)
        {
          flag1=0;
          cur_speed=cur_speed+5;
          lcd.setCursor(14, 1);
    	  lcd.print(cur_speed);
        }
  		if(flag2)
  		{
          flag2=0;
          cur_speed=cur_speed-5;
          lcd.setCursor(14, 1);
    	  lcd.print(cur_speed);
         
  		}
  
     /* Call for 1st ADC */
       adc_val1=adc1();
  
        /*CHECK ADC1 VALUE */
        if(adc_val1 >= 750 )
        { 
           SET_BIT(PORTB,  PB2);
           CLR_BIT(PORTB,  PB1);
           CLR_BIT(PORTB,  PB0);      
          
        }
        else if(adc_val1 <= 330)
        { 
           SET_BIT(PORTB,  PB0);
           CLR_BIT(PORTB,  PB2);
           CLR_BIT(PORTB,  PB1);   
         
        } 
        else
        {
           SET_BIT(PORTB,  PB1);
           CLR_BIT(PORTB,  PB0);
           CLR_BIT(PORTB,  PB2);              
        }
  
   /* Call for 2nd ADC */
  		adc_val2=adc2();

  		if((adc_val2 >=0) && (adc_val2 < 300)) 
        {  
          
           	lcd.setCursor(2, 0);
    		lcd.print("SAFE ROAD");
   	   		lcd.setCursor(2, 1);
    		lcd.print("SPEED(KMPH):");
            lcd.setCursor(14, 1);
    	    lcd.print(cur_speed);
          
            CLR_BIT(PORTB,  PB5);  
          
        }
       else if((adc_val2 >=300) && (adc_val2 <= 750)) 
        { 
           lcd.setCursor(2, 0);
           lcd.print("SCHOOL AHEAD");
           cur_speed=20;
           lcd.setCursor(2, 1);
            lcd.print("SPEED(KMPH):");
           lcd.setCursor(14, 1);
    	   lcd.print(cur_speed);
           SET_BIT(PORTB,  PB5);
          
        }
   		else
         {
  			lcd.setCursor(2, 0);
    		lcd.print("ACCIDENT ROAD");
      		cur_speed=20;
   	   		lcd.setCursor(2, 1);
    		lcd.print("SPEED(KMPH):");
            lcd.setCursor(14, 1);
            SET_BIT(PORTB,  PB5);
    	    lcd.print(cur_speed);
  		}
  
  
  /* Code for Ultrasonic Sensor */
   if((dist_cm >0) && (dist_cm < 90) )
  {
     
     analogWrite(PD6, 20);
  }
  else if((dist_cm >= 90) && (dist_cm < 180))
  {
   analogWrite(PD6, 80);
  }
  else if(dist_cm >= 180)
  {
    analogWrite(PD6, 250);
  }
  
    
}

/* Function for ADC1 */
int adc1()
{
    ADMUX =0X00;
    ADMUX |=  (1<<REFS0); //AVCC
    ADCSRA|=   (1<<ADEN);
    ADCSRA|=   (1<<ADSC);
  
    while(ADCSRA & (1<<ADSC)); // wait until  conversion complete*
    
  return ADC;
    
}

/* Function for ADC2 */
int adc2()
{
    ADMUX =0X00;
    ADMUX |=  (1<<MUX1); 
    ADMUX |=  (1<<REFS0); //AVCC
    ADCSRA|=   (1<<ADEN);
    ADCSRA|=   (1<<ADSC);
  	ADC=0;
    while(ADCSRA & (1<<ADSC)); // wait until  conversion complete*
    
  return ADC;
  
  
}

/*Function to initialise Input and Output Pins */
void InputOutputs()
{
    SET_BIT(DDRB,  PB1);   // Output pin LED pin 0
    SET_BIT(DDRB,  PB2);   // Output pin LED pin 1
    SET_BIT(DDRB,  PB0);
    SET_BIT(DDRB,  PB5);// Output pin LED 2

    CLR_BIT(DDRC,  PC4);
    
    CLR_BIT(DDRD,  PD0);// FOR ULTRA
    CLR_BIT(DDRD,  PD1);
   
    CLR_BIT(DDRC,  PC0);   //POT1
    CLR_BIT(DDRC,  PC2);   // POT 2
  
   CLR_BIT(PORTB,  PB0); 
   CLR_BIT(PORTB,  PB1);
   CLR_BIT(PORTB,  PB2);
   CLR_BIT(PORTB,  PB5);
}

void Init_EXT_INT()
{
    int flag1=0,flag2=0;
  
    CLR_BIT(DDRD,  PD2);  // Make PD2 as EXTINT1
    SET_BIT(PORTD, PD2); 
  
    CLR_BIT(DDRD,  PD3);
    SET_BIT(PORTD, PD3);  // Make PD3 as EXTINT1
  
  /*Setting for EXTINT0 */
    EICRA|=(1<<ISC00);
    EIMSK|=(1<<INT0);


 /*Setting for EXTINT1 */
    EICRA|=(1<<ISC10);
    EIMSK|=(1<<INT1);     
}

/*Function for Ultrasonic sensor */
long ultra()
{
  long duration, cm;
  
   SET_BIT(DDRD,  PD7); 
   CLR_BIT(PORTD,  PD7);
  delayMicroseconds(2);
   SET_BIT(PORTD,  PD7);
  delayMicroseconds(5);
  CLR_BIT(PORTD,  PD7);
   pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
 

/* Speed increase Button */
ISR(INT0_vect)
{
  flag1=1;
}

/* Speed Decrease Button */
ISR(INT1_vect)
{ 
  flag2=1;
  
}
