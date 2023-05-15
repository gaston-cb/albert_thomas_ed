/**
*   Este programa muestra la configuración del modo normal del CTC 
*/ 

volatile uint16_t sistyck_timer_2  = 0 ;  

uint8_t state_led = LOW ; 



void setup() {
  Serial.begin(9600) ; 
  pinMode(13,OUTPUT) ; 
  digitalWrite(13,state_led) ; 
  Serial.print("config timer") ; 
//// configuración del timer en modo normal 
  SREG = (SREG & 0b01111111);   ///deshabilitar interrupciones  
  TCNT2 = 0 ; 
  TIMSK2 =TIMSK2|0b00000001;  
  TCCR2B = 0b00000011 ;  //FCLK = 16M/32 = 500k -> sistyck = (1/500k)*255  
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones


}

void loop() {
  if (sistyck_timer_2 >= 2000){
    Serial.println("han pasado x seg") ;     
    sistyck_timer_2 = 0 ; 
    state_led = state_led == LOW?HIGH:LOW ; 
    digitalWrite(13,state_led) ; 
    
  } 


}


ISR(TIMER2_OVF_vect)
{ 
    sistyck_timer_2++ ; 
}