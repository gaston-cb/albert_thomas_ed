#include <Arduino.h> 


typedef struct{
  uint8_t number_led ; 
  uint8_t state ; 
}led_state_t; 



void setStateLed(led_state_t leds); 
void initport(led_state_t led) ; 
