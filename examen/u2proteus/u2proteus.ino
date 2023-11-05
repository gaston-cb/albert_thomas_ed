/************************PREGUNTAS******************************************************************************
 * 1) Diga que hace este código. Indique claramente para que sirve la linea 43. 
 * 2) Indique el modo del timer, y cada cuanto tiempo se realiza la acción. ¿Que acción ocurre cada determinada cantidad de tiempo? 
 * 3) El dispositivo ¿Esta como maestro o eslavo?. En caso de estar como maestro, decir si esta como MT o MR. Idem si se encuentra en modo esclavo
 * 4) Para que se encuentra la linea 47 o que hace. 
 * 5) Indique en que lineas se reciben/envian datos desde este dispositivo hacia el otro. 
 * 6) Proponga un esquema de #define para que el buffer de recepcion o transmisión se modifique desde un solo lugar en todo el código. 
 *  
 ****************************************************************************************************************/

#include <Arduino.h>
#define SLAVE_I2C 0x45  
#define PORT_BUTTON 8

typedef struct{ 
  uint8_t read_write_bit : 1 ; 
  uint8_t i2c_addr :7 ;  

}i2c_address_t ; 

i2c_address_t i2c_master = {0x00,SLAVE_I2C } ; 
volatile static uint16_t sistyck_timer ; 
float temperature_rx ; 
uint8_t buffer_rx[4] ; 




void i2c_config(); 
void function_i2c(void); 


void setup() {
  Serial.begin(9600) ; 
  pinMode(PORT_BUTTON,INPUT_PULLUP) ;
   
  SREG = (SREG & 0b01111111);   
  TCNT2 = 0 ; 
  TIMSK2 =TIMSK2|0b00000001;  
  TCCR2B = 0b00000010 ;    
  SREG = (SREG & 0b01111111) | 0b10000000; 
  ///I2C_CONFIG 
  i2c_config() ; 


}
uint8_t state_port = LOW ; 
void loop() {
  if (digitalRead(PORT_BUTTON)==LOW){ 
    while(digitalRead(PORT_BUTTON)==LOW) ;  
    Serial.print("temperatura es: ") ; Serial.println(temperature_rx) ; 
  }
  if (sistyck_timer>=20000u){
    sistyck_timer = 0 ; 
    function_i2c() ;  
  }
}


void i2c_config(){ 
  TWSR = (0<<TWPS0) | (0<<TWPS1) ; 
  TWBR = (uint8_t) 72 ; 
  TWCR |= (1<<TWEN) ;  

}


void function_i2c(void){ 
  i2c_master.read_write_bit = 1 ;  
  uint8_t bytes_twdr ; 
  uint8_t data_arrived[4] ;
  
  memcpy(&bytes_twdr, &i2c_master, sizeof(i2c_master) ) ; 
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN) ;   
  while (!(TWCR & (1<<TWINT))) ; 
  if (TWSR != 0x08){
    TWCR = (1<<TWINT)| (1<<TWSTO) | (1<<TWEN) ;
    TWCR|= (1<<TWINT) ; 
    return ; ///end aplication ! 
  }
  TWDR = bytes_twdr;   
  TWCR = (1<<TWINT)| (1<<TWEN) ; 
  while (!(TWCR & (1<<TWINT))) ;  
  if (TWSR != 0x40){
    TWCR|= (1<<TWINT) ; 
    return ; ///end aplication ! 
  }


  for (int i = 0 ; i<4 ; i++) { 
    TWCR =(i!=3)?(1<<TWINT)| (1<<TWEN) | (1<<TWEA):(1<<TWINT)| (1<<TWEN);  ;
    while (!(TWCR & (1<<TWINT))) ;   
    data_arrived[i] = TWDR ; 
  }
  
  TWCR = (1<<TWINT)| (1<<TWSTO) | (1<<TWEN) ; 

  memcpy(&temperature_rx,data_arrived,sizeof(float) ) ; 
}

ISR(TIMER2_OVF_vect)
{ 
    sistyck_timer++ ; 
}