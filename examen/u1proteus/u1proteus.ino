/**************************************PREGUNTAS **************************************************
 * Preguntas evaluatorias: 
 *  1) Diga que hace este código 
 *  2) Como esta configurado el TIMER. Indique el periodo de interrupción. 
 *  3) Cada cuanto tiempo ocurre la interrupción de lectura del adc 
 *  4) Cual es la dirección del I2C. 
 *  5) Este código corresponde al dispositivo slave o master. Si es el slave indique la dirección. 
 *  6) Sabiendo que el tamaño en bytes de un float son 4 bytes, indique la transmision i2c y quienes son los 
 *     responsables de cada señalización. 
 *  7) Que función cumple en el código la constante 5.0/1024 sabiendo que el ADC es de 10 bits y tiene una tensión de 5 V 
 *  8) Proponga un esquema de #define para que se pueda cambiar la constante hasta la que cuenta el systick. 
 *  9) Cambie el esquema para que se envie la tensión en lugar de la temperatura. 
 **************************************************************************************************************/

#include <Arduino.h>
#define ADDRESS_I2C_SLAVE 0x45  
volatile static uint16_t systick = 0 ; 
const float resolution = (0.01) ; //0.01v/ºC  o 10mv/ºC 
const float factor_conv_adc = 5.0/1024.0 ; 
uint8_t buffer_i2c[4] ; 
volatile uint8_t size_message = 0 ; 
volatile uint8_t status ;

void i2c_config(); 
void function_i2c(void); 



void setup() {
  Serial.begin(9600) ; 
  pinMode(A0,INPUT) ; 
  pinMode(12,OUTPUT) ; 
  SREG = (SREG & 0b01111111); 
  TCNT2 = 0 ;
  TIMSK2 = TIMSK2 | 0b00000010 ; 
  TCCR2A = 0b00000010;
  TCCR2B = 0b00000011;  
  OCR2A = 249;          
  SREG = (SREG & 0b01111111) | 0b10000000 ;
  i2c_config();


}


void loop() {
  float temperature  ;
  if (systick>=1300){
    temperature = (factor_conv_adc* analogRead(A0))/0.01 ; 
    memcpy(buffer_i2c,&temperature, sizeof(float)) ; /// 
  
  }

}

ISR(TIMER2_COMPA_vect){ 
  systick++ ;   
}


void i2c_config(){

  cli() ; ///disable global irq (macro microchip)
  TWAR = (ADDRESS_I2C_SLAVE)<<1 ; //<<1 ; //birtshift 9,1 // ; 
  TWCR = (1 << TWEN) | (1 << TWEA) |(1<<TWIE)  ;
  sei() ; 
}



ISR(TWI_vect){ 
  status = TWSR & 0xF8;
  if (status == 0xA8 || status == 0xB0) 
  {
    TWDR = buffer_i2c[size_message] ;      
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; 
    size_message++ ; 
  }
  if (status == 0xB8 ){
    TWDR = buffer_i2c[size_message] ; 
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; 
    size_message++ ; 
  }
  if (status == 0xC0 || status == 0xC8){
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; ; 
    size_message= 0  ; 
  }
}