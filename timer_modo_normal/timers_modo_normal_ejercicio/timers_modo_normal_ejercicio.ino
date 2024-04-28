#define PORT_1 2
#define PORT_2 3
#define PORT_3 4 

#define PORT_PISO_1 13 
#define PORT_PISO_2 12 
#define PORT_PISO_3 11 



volatile uint16_t sistyck_timer_1  = 0 ;  
volatile uint16_t sistyck_timer_2  = 0 ;  
volatile uint16_t sistyck_timer_3  = 0 ;  


uint8_t state_led_1 = LOW ; 
uint8_t state_led_2 = LOW ; 
uint8_t state_led_3 = LOW ; 

uint8_t state_piso_1 = HIGH ; 
uint8_t state_piso_2 = HIGH ; 
uint8_t state_piso_3 = HIGH ; 



void setup() {
  Serial.begin(9600) ; 
  pinMode(PORT_1,OUTPUT) ; 
  pinMode(PORT_2,OUTPUT) ; 
  pinMode(PORT_3,OUTPUT) ; 
  pinMode(PORT_PISO_1,OUTPUT) ; 
  pinMode(PORT_PISO_2,OUTPUT) ; 
  pinMode(PORT_PISO_3,OUTPUT) ; 

  /// CONFIGURAR DE FORMA FÁCIL LOS PUERTOS DE SALIDA
  digitalWrite(PORT_1,state_led_1) ; 
  digitalWrite(PORT_2,state_led_1) ; 
  digitalWrite(PORT_3,state_led_1) ; 
  digitalWrite(PORT_PISO_1,state_piso_1) ; 
  digitalWrite(PORT_PISO_2,state_piso_2) ; 
  digitalWrite(PORT_PISO_3,state_piso_3) ; 
  

  
  Serial.print("config timer") ; 

//// configuración del timer en modo normal 

  
  SREG = (SREG & 0b01111111);   ///deshabilitar interrupciones  
  TCNT2 = 0 ; 
  TIMSK2 =TIMSK2|0b00000001;  
  TCCR2B = 0b00000101 ;   
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones
}


void loop() {
  if (sistyck_timer_1 >= 20){
    Serial.println("han pasado x1 seg") ;     
    sistyck_timer_1 = 0 ; 
    state_led_1 = state_led_1 == LOW?HIGH:LOW ; 
    digitalWrite(PORT_1,state_led_1) ;     
  } 

  if (sistyck_timer_2 >= 60){
    Serial.println("han pasado x2 seg") ;     
    sistyck_timer_2 = 0 ; 
    state_led_2 = state_led_2 == LOW?HIGH:LOW ; 
    digitalWrite(PORT_2,state_led_2) ;     
  } 
  if (sistyck_timer_3 >= 70){
    Serial.println("han pasado x3 seg") ;     
    sistyck_timer_3 = 0 ; 
    state_led_3 = state_led_3 == LOW?HIGH:LOW ; 
    digitalWrite(PORT_3,state_led_3) ;     
  } 




}


ISR(TIMER2_OVF_vect)
{ 
    sistyck_timer_1++ ; 
    sistyck_timer_2++ ; 
    sistyck_timer_3++ ; 
}
