#include <Arduino.h>
/**
 * @brief i2c slave project: receive data using isr over i2c in atmega328p 
 *        direcciones validas en 7 bits: 0x08 a 0x77 
 */
#define I2C_ADDRES_SLAVE 0x09u  
uint8_t buffer_receive [3] ; 
void clean_buffer(uint8_t *buffer_clean, size_t len_buffer ){
  memset(buffer_clean,0,len_buffer) ; //set zero to buffer receive  

}

volatile uint8_t flag_isr_twi_av[10] ;  ; 
volatile uint8_t track_twcr[10] ;  ; 
const uint8_t send_master_data = 33 ;  
volatile const char message_i2c[14] = "HOLA MUNDO\r\n" ; 
volatile uint8_t size_message = 0 ; 

void setup() {
  //TWSR = (0<<TWPS0) | (0<<TWPS1) ; 
  //TWBR = (uint8_t) 72 ; //BITRATE 
  //TWCR = 0x00 ; 
  //TWCR |= (1<<TWEN) ; // enable twi 
  //TWCR |= (1<<TWIE) ; 
  Serial.begin(9600) ; 
  clean_buffer(buffer_receive, 3) ; 
  cli() ; ///disable global irq (macro microchip)
//  TWAR = 12 ; //birtshift 9,1 //(I2C_ADDRES_SLAVE)<<1 ; 
  TWAR = (I2C_ADDRES_SLAVE)<<1 ; //<<1 ; //birtshift 9,1 // ; 

 // TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
  TWCR = (1 << TWEN) | (1 << TWEA) |(1<<TWIE)  ;
  sei() ; ///enable globar irq (macro microchip)
  Serial.print("I2C SLAVE INIT: slave ADDR ") ; 
  Serial.print(TWAR,HEX) ; 

}

  volatile uint8_t status ;

volatile int cuenta = 0 ; 
volatile uint8_t status_code_isr = 0 ; 
void loop() {
  if (status == 0xA0){
    Serial.print("rx: ") ; Serial.println(buffer_receive[0]) ;  
    Serial.println("stop received") ; 
    status = 0x00 ; 

  }
  
 
}





ISR(TWI_vect){ 
  status = TWSR & 0xF8;
  flag_isr_twi_av[cuenta] = status ; 
  track_twcr[cuenta] = TWSR & 0xF8  ;
  cuenta++ ; 
  // slave transmitter mode ...
  if (status == 0xA8 || status == 0xB0) 
  {
    TWDR = message_i2c[size_message] ;      /// SEND DATA TO SLAVE 
    //TWCR = (1 << TWINT) | (1 << TWIE) | (1 << TWEN) ;///| (1 << TWEA) | (1 << TWEN) | (1 << TWIE);   
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; ///NEXT ACTION IS A ACK 
    size_message++ ; 
   // return ; 
  }
  if (status == 0xB8 ){ //NEXT CHARACTER 
    TWDR = message_i2c[size_message] ; /// load twdr  
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; 
    size_message++ ; 
  }
  if (status == 0xC0 || status == 0xC8){
    //TWDR =(uint8_t) message_i2c[size_message] ; /// load twdr  
    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; ; 
    //TWCR = 0xC5 ; 
    size_message= 0  ; 
  }




//   if (status == 0x60){
//     TWDR =(uint8_t) message_i2c[size_message] ; /// SEND DATA TO SLAVE 
//     size_message++ ; 
//     TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWIE) ;
// //    TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA) |(1<<TWIE) ;  
//   }
  
  // if(status == 0x80 || status == 0x88 || status == 0x90)  // receive mode - master send data after SLA+W - read it
  // {
  
  //   buffer_receive[0] = TWDR ; 
  //   TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA) |(1<<TWIE) ;  
  
  // }
  // if (status == 0xA0){
  //   TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE)  ;  //TWCR |= (1<<TWINT) ;
  // }
  // TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA)|(1<<TWIE) ; /// | (1<<TWEN) ; /// OTHER CASE 
  // if (size_message == 13) {
  //   size_message = 0 ; 
  // }  
}
