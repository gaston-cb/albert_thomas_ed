#include <Arduino.h>
/**
 * @brief I2C MASTER, for atmega328p, se va a intentar usar isr handles
 *        Ver posibilidad de usar simba  
 *      Hay cuatro modos de operacion 
 *      master transmit : un numero de bytes es transmitido a un slave receive 
 *      master receive 
 *      slave transmit 
 *      slave received 
 *   todas pueden convivir en un código 
 */

#define I2C_ADDRESS 0x09
typedef struct{ 
  uint8_t read_write_bit : 1 ; 
  uint8_t i2c_addr :7 ;  

}i2c_address_t ; 

unsigned long int test_i2c = 0 ; /// manage timer using millis for example 
unsigned long int test_i2c1 = 0 ; /// manage timer using millis for example 

i2c_address_t i2c_master = {0x00,I2C_ADDRESS } ; 

void initI2C_master() ; 
void master_transmit_test() ; 
void master_receive_test() ; 



void sendStart(){
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN) ;  ///send start 
}



void initI2C(){
  /// frecuencia de clock: 
  /// F_cpu/(16+2 x TWBR x Preescaler)) 
  /// 16 
  TWSR = (0<<TWPS0) | (0<<TWPS1) ; 
  TWBR = (uint8_t) 72 ; //BITRATE 
  TWCR |= (1<<TWEN) ; // enable twi 

}

void error_handler(){ 
  Serial.println("ERROR HANDLER IN I2C") ;   
}


void setup() {
  
  Serial.begin(9600) ; 
  initI2C() ; 
  Serial.println("Inicio de maestro software ");  

}

void loop() {
  if (millis()- test_i2c>=3000){ /// transmite cada 3 segundos 
    test_i2c = millis() ; 
    Serial.println("master transmit i2c ")  ; 
    master_receive_test() ; 
    Serial.println("end master transmit i2c ")  ; 
  }
  // if (millis()- test_i2c1>=2000){
  //   test_i2c = millis() ; 
  //   Serial.println("transmit i2c ")  ; 
  //   master_transmit_test() ; 
  //   Serial.println("end transmit i2c ")  ; 
  //   test_i2c1 = 0 ; 
  // }


}

void master_receive_test(){
  i2c_master.read_write_bit = 1 ; //mode read 
  uint8_t bytes_twdr ; // i2c_master.read_write_bit | i2c_master.i2c_addr ;   ;
  uint8_t error_code ; 
  uint8_t data_arrived[14] ; // = 0x00 ; 
  
  memcpy(&bytes_twdr, &i2c_master, sizeof(i2c_master) ) ; 
  sendStart() ; 
  while (!(TWCR & (1<<TWINT))) ; /// wait byte confirm ! 
  if (TWSR != 0x08){
    TWCR = (1<<TWINT)| (1<<TWSTO) | (1<<TWEN) ;//STOP CONDITION IF NACK
    TWCR|= (1<<TWINT) ; 
    error_handler() ; 
    return ; ///end aplication ! 
  }
  ///send sla+r
  TWDR = bytes_twdr;   
  TWCR = (1<<TWINT)| (1<<TWEN) ; 
  while (!(TWCR & (1<<TWINT))) ;  //ESPERAR ENVIO DE SLA+R 
  if (TWSR != 0x40){
    TWCR|= (1<<TWINT) ; 
    error_handler() ; 
    return ; ///end aplication ! 
  }


  for (int i = 0 ; i<14 ; i++) { 
    TWCR =(i!=13)?(1<<TWINT)| (1<<TWEN) | (1<<TWEA):(1<<TWINT)| (1<<TWEN);//: (1<<TWINT) |(1<<TWEN)   ;
    while (!(TWCR & (1<<TWINT))) ;  //ESPERO EL DATO 
    error_code = TWSR  ;     
    data_arrived[i] = TWDR ; 
    //TWCR =(1<<TWINT)| (1<<TWEN)| (1<<TWEA) ; 
//    TWCR = (i!=13)?(1<<TWINT)| (1<<TWEN)| (1<<TWEA): (1<<TWINT) |(1<<TWEN) ;//| (1<<TWEA);//| (1<<TWEN)  ;   //SEND ACK // NOT SEND ACK 
  }
  
  TWCR = (1<<TWINT)| (1<<TWSTO) | (1<<TWEN) ; //send stop

// for (int i = 0 ; i<14; i++){
  Serial.print((char *)data_arrived) ; 
  
  Serial.println()  ;
  
}

void master_transmit_test(){
  i2c_master.read_write_bit = 0 ; 
  uint8_t bytes_twdr  ; 
  memcpy(&bytes_twdr, &i2c_master, sizeof(i2c_master) ) ; 
//  sendStart() ; 
}


// master send mode o master transmit 
void initI2C_master(){
///clock_scl = F_cpu /(16 + 2*twbr x preescaler) 
///registro importante : TWDR 
  sendStart() ; 
  while (!(TWCR & (1<<TWINT))) ; 
  if (TWSR != 0x08){
    TWCR|= (1<<TWINT) ; 
    error_handler() ; 
    return ; 
  }
  uint8_t bytes_twdr  ; 
  memcpy(&bytes_twdr, &i2c_master, sizeof(i2c_master) ) ; 
  TWDR = 12 ; //bytes_twdr ; ///send sla +w 
  TWCR = (1<<TWINT)| (1<<TWEN) ; 
  while (!(TWCR & (1<<TWINT))) ;  
  if (TWSR != 0x18){
    if (TWSR == 0x20){
      Serial.println("error slave 0x20") ; 
    }else if (TWSR == 0x38){ 
      Serial.println("error slave 0x38") ; 
    }
    return ; 
  }
  uint8_t bytes_to_send[2] = {21,23}; 
  TWDR = bytes_to_send[0] ; 
  TWCR = (1<<TWINT)| (1<<TWEN) ; 
  while (!(TWCR & (1<<TWINT))) ;  
  TWCR = (1<<TWINT)| (1<<TWSTO) | (1<<TWEN) ; //send stop
  //while (!(TWCR & (1<<TWINT))) ;  
  //TWCR|= (1<<TWINT) ; 


}

/*
void master_transmit(){
  // TWCR{ TWINT  = 1  //TWCR = ()
  //        TWEA  = x 
  //        TWSTA = 1 -> transmite una condicion de start 
  //        TWSTO = 0 
  //        TWWC  = x 
  //        TWWEN = 1 -> habilita el bus i2c 
  //        - 
  //        TWIE  = x }         
  //        if ( TWSR!=0x08) ERROR_CODE 
  //        TWDR = SLA+W 
  //        TWCR = TWINT = 1 , TWEA = 'X ' , TWSTA = 0 ,TWSTO = 0, TWWC = 'X' ,TWEN = 1, - TWIE = 'X' 
  

}*/ 
 










/**
 * EJEMPLO ENVIO DE UN BYTE : PASOS PARA LA COMUNICACION I2C 
 *  1 - Transmitir la condicion de start. Se escribe un valor especifico en TWCR 
 *      Se borra TWINT(BIT DE REG ) -> SE TRANSMITE LA CONDICION DE START  
 *  2 - Cuando se ha transmitido la condicion de start, TWINT SE SETEA, Y se establece en el registro TWSR UN
 *      CODIGO DE ERROR 
 *  3 - La app debe exdaminar este valor y ver que hacer: 
 *      Si el estatus es correcto -> CARGAR sla+w en, usar instrucciones de hw para transmitir el dato.Borrar twint
 *      para iniciar el envio de informacion 
 *  4 - Cuando se envia, se genera un codigo de error en TWSR Y TWINT SE ESTABLECE 
 *  5 - Verificar que el paquete se envio correctamente y el ACK es el esperado. Si funciona todo ok 
 *      Entonces cargar un paquete de datos en twdr 
 *  6 - Cuando el paquete se transmite, se establece TWINT en TWCR, y hay un codigo de error. 
 *  7 - La app debe leer el estado del codigo de error ademas se debe tomar una accion. Si es "ok" -> enviar Stop
 *  "ejemplo en C "
 *  1 - TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN) -> SEND START CONDITION 
 *  2 - while (!(TWCR & (1<<TWINT)))            -> ESPERAR EL FLAG DE CONFIRMACION 
 *  3 - if ((TWSR & 0xF8) != START) ERROR()     -> 
 *      TWDR = SLA_W; TWCR = (1<<TWINT) | (1<<TWEN); -> cargar sla+w y borrar twint 
 *  4 - while (!(TWCR & (1<<TWINT))) -> esperar condicion 
 *  5 - if ((TWSR & 0xF8) != MT_SLA_ACK) ERROR();    ->checkear error y cargar datos 
 *      TWDR = DATA; TWCR = (1<<TWINT) | (1<<TWEN);  -> si no hubo error , cargo datos 
 *  6 - while (!(TWCR & (1<<TWINT)))          -> Espera 
 *  7 - if ((TWSR & 0xF8)!= MT_DATA_ACK) ERROR();   -> evaluar condicion de "espera"
 *         TWCR = (1<<TWINT)|(1<<TWEN)| (1<<TWSTO); -> enviar condicion de stop 
 */   
