#include <Arduino.h> 
#include "timers2.h"


void initport(led_state_t led){ 
  pinMode(led.number_led, OUTPUT) ; 
  setStateLed(led) ; 
}

void setStateLed(led_state_t led){
  digitalWrite(led.number_led, led.state) ;   
}



