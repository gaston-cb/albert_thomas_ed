/**
  
  Codigo de ejemplo para utilizar el timer en modo CTC 
  Ejercicio: cambiar la x time por el tiempo correspondiente. 
  Fecha de entrega 31/8/2023 
*/
#include "timers2.h"
led_state_t led1 = {
  8  ,LOW
} ; 
led_state_t led2 =  {
  9  ,LOW

}; 
led_state_t led3  {
  10  ,LOW
}; 

static uint16_t sistyck[3] = {0,0,0} ; 



void setup() {
  Serial.begin(9600) ; /// iniciio de comunicacion UART. 
  initport(led1) ; 
  initport(led2) ; 
  initport(led3) ; 
  
  SREG = (SREG & 0b01111111); // deshabilito interrupciones globales
  TCNT2 = 0 ;
  TIMSK2 = TIMSK2 | 0b00000010 ; //
  TCCR2A = 0b00000010;
  TCCR2B = 0b00000100; // 250 khZ n= 249 
  OCR2A = 249;         //valor de comparacion 
  SREG = (SREG & 0b01111111) | 0b10000000 ;// habilito interrupciones globales
  Serial.println("inicio de programa") ; 

}


void loop() {
  if (sistyck[0] >= 1000){ 
    Serial.println("task0 - x time") ; 
    led1.state = led1.state == LOW ?HIGH:LOW ; 
    setStateLed(led1) ; 

    sistyck[0] = 0 ; 
  }
  if (sistyck[1] >= 4500){ 
    Serial.println("task1  - x time") ; 
    led2.state = led2.state == LOW ?HIGH:LOW ; 
    setStateLed(led2) ; 
    sistyck[1] = 0 ; 
  }
  if (sistyck[2] >= 6500){ 
    Serial.println("task2 - x time") ; 
    led3.state = led3.state == LOW ?HIGH:LOW ;
    setStateLed(led3) ; 
    sistyck[2] = 0 ; 
  }

}


ISR(TIMER2_COMPA_vect){ 
  sistyck[0]++ ; 
  sistyck[1]++ ; 
  sistyck[2]++ ;   
}


