
#include "Wire.h"                      //libreria para comunicacion I2C
#include "Adafruit_INA219.h"           //Libreria del ina219

// La direccion de cada INA219 se ajusta soldando los pines A0 y A1, revisar hoja de datos del INA219

Adafruit_INA219 ina219Rojo;            //INA219 con direccion por defecto 0x40
Adafruit_INA219 ina219Azul(0x41);      //INA219 con direccion 0x41
Adafruit_INA219 ina219Verde(0x41);     //INA219 con direccion 0x44
Adafruit_INA219 ina219Infrarrojo(0x41);//INA219 con direccion 0x45

//Variables para el muestreo
unsigned int Ts=5000;//periodo de muestreo en [us]
unsigned int t, t0;

//Corrientes Maxima en [mA], no mayores al valor tipico de operacion
float CorrienteMaximaRojo=150;
float CorrienteMaximaAzul=150;
float CorrienteMaximaVerde=350;
float CorrienteMaximaInfrarrojo=150;

//Variables para almacenar valor de Corrientes deseadas en [mA], iniciados en el valor tipico de operacion
float CorrienteReferenciaRojo=150;
float CorrienteReferenciaAzul=150;
float CorrienteReferenciaVerde=350;
float CorrienteReferenciaInfrarrojo=150;

// Valores iniciales de PWM
float PWMRojo=65535;
float PWMAzul=65535;
float PWMVerde=65535;
float PWMInfrarrojo=65535;

//Pin donde sale la PWM
const int ledPinRojo = 17;
const int ledPinAzul = 18;
const int ledPinVerde = 5;
const int ledPinInfrarrojo = 19;

//Canales PWM
const int ledChannelRojo = 0;
const int ledChannelAzul = 1;
const int ledChannelVerde = 2;
const int ledChannelInfrarrojo = 3;

//PWM caracteristicas
const int freq = 5000;
const int resolution = 16;

//Variables para almacenar la corriente medida
float CorrienteMedidaRojo;    
float CorrienteMedidaAzul;
float CorrienteMedidaVerde;
float CorrienteMedidaInfrarrojo;

//pin lectura potenciometros
const int POT1 = 13;  //Verde
const int POT2 = 12;  //Azul
const int POT3 = 14;  //Infrarrojo
const int POT4 = 27;  //Rojo

//Pin del Switch
const int Switch = 23; 

//Variable para almacenar Corriente de referencia anterior
int CorrienteReferenciaRojoAnterior;
int CorrienteReferenciaAzulAnterior;
int CorrienteReferenciaVerdeAnterior;
int CorrienteReferenciaInfrarrojoAnterior;

//Variable timer interrupciones
hw_timer_t * timer = NULL;
bool a = 0;

//Función de interrupción
void IRAM_ATTR onTimer(){
  if(a){
    //Ciclo día
  CorrienteReferenciaRojo=CorrienteReferenciaRojoAnterior;
  CorrienteReferenciaAzul=CorrienteReferenciaAzulAnterior;
  CorrienteReferenciaVerde=CorrienteReferenciaVerdeAnterior;
  CorrienteReferenciaInfrarrojo=CorrienteReferenciaInfrarrojoAnterior;
    a = 0;
  }
  else{
    //Ciclo noche
    timerAlarmWrite(timer,86400000,true); //Se activa cada 12 horas
    CorrienteReferenciaRojoAnterior = CorrienteReferenciaRojo;
    CorrienteReferenciaAzulAnterior = CorrienteReferenciaAzul;
    CorrienteReferenciaVerdeAnterior = CorrienteReferenciaVerde;
    CorrienteReferenciaInfrarrojoAnterior = CorrienteReferenciaInfrarrojo;
    CorrienteReferenciaRojo = 0;
    CorrienteReferenciaAzul = 0;
    CorrienteReferenciaVerde = 0;
    CorrienteReferenciaInfrarrojo = 0;
    a = 1;
  }
}
void setup() {
  Serial.begin(115200);
  ina219Rojo.begin();       //Inicializa sensor con direccion 0x40
  ina219Azul.begin();       //Inicializa sensor con direccion 0x41
  ina219Verde.begin();      //Inicializa sensor con direccion 0x44
  ina219Infrarrojo.begin(); //Inicializa sensor con direccion 0x45

  //Ajusta el rango de medicion de los INA219 ya que no vamos a medir mas de 1A
  ina219Rojo.setCalibration_32V_1A();
  ina219Azul.setCalibration_32V_1A();
  ina219Verde.setCalibration_32V_1A();
  ina219Infrarrojo.setCalibration_32V_1A();
  
  t0=micros(); // inicia el contador

  //Ajuste de caracteristicas de PWM
  ledcSetup(ledChannelRojo, freq, resolution);
  ledcSetup(ledChannelAzul, freq, resolution);
  ledcSetup(ledChannelVerde, freq, resolution);
  ledcSetup(ledChannelInfrarrojo, freq, resolution);

  //Asignacion de canal a los pines
  ledcAttachPin(ledPinRojo, ledChannelRojo);
  ledcAttachPin(ledPinAzul, ledChannelAzul);
  ledcAttachPin(ledPinVerde, ledChannelVerde);
  ledcAttachPin(ledPinInfrarrojo, ledChannelInfrarrojo);

  //Switch para activar o desactivar los potenciometros
  pinMode(23,INPUT);
  
  //Inicializa el timer
  timer = timerBegin(1,40000,true); //Cuenta cada 500uS --> 1S = 2000 ticks
  timerAttachInterrupt(timer,&onTimer,true);
  timerAlarmWrite(timer,70200000,true); //Se activa cada 9 horas y 45 min
  timerAlarmEnable(timer);
  }

void loop() {
  //Activar potenciometros
  if (digitalRead(Switch) == HIGH && a == 0){
    CorrienteReferenciaRojo = map(analogRead(POT1),0,4095,0,CorrienteMaximaRojo); 
    CorrienteReferenciaAzul = map(analogRead(POT2),0,4095,0,CorrienteMaximaAzul);
    CorrienteReferenciaVerde = map(analogRead(POT3),0,4095,0,CorrienteMaximaVerde);
    CorrienteReferenciaInfrarrojo = map(analogRead(POT4),0,4095,0,CorrienteMaximaInfrarrojo);
  }

    t=micros();
  if(t-t0>=Ts){

  CorrienteMedidaRojo = ina219Rojo.getCurrent_mA();
  PWMRojo= AjustarPWM(CorrienteReferenciaRojo,CorrienteMedidaRojo,PWMRojo);
  ledcWrite(ledChannelRojo,PWMRojo);
  Serial.print("Corriente Rojo:       "); Serial.print(CorrienteMedidaRojo); Serial.println(" mA");
  
  CorrienteMedidaAzul = ina219Azul.getCurrent_mA();
  PWMAzul= AjustarPWM(CorrienteReferenciaAzul,CorrienteMedidaAzul,PWMAzul);
  ledcWrite(ledChannelAzul,PWMAzul);
  Serial.print("Corriente Azul:       "); Serial.print(CorrienteMedidaAzul); Serial.println(" mA");
  
  CorrienteMedidaVerde = ina219Verde.getCurrent_mA();
  PWMVerde= AjustarPWM(CorrienteReferenciaVerde,CorrienteMedidaVerde,PWMVerde);
  ledcWrite(ledChannelVerde,PWMVerde);
  Serial.print("Corriente Verde:       "); Serial.print(CorrienteMedidaVerde); Serial.println(" mA");
  
  CorrienteMedidaInfrarrojo = ina219Infrarrojo.getCurrent_mA();
  PWMInfrarrojo= AjustarPWM(CorrienteReferenciaInfrarrojo,CorrienteMedidaInfrarrojo,PWMInfrarrojo);
  ledcWrite(ledChannelInfrarrojo,PWMInfrarrojo);
  Serial.print("Corriente Infrarrojo:       "); Serial.print(CorrienteMedidaInfrarrojo); Serial.println(" mA");
       
  t0=t;
  }
    
}

float AjustarPWM(float CorrienteReferencia, float CorrienteMedida, float PWM){

  float e=CorrienteReferencia-CorrienteMedida;
  float kp = 100;  
  if(e!=0){ PWM=PWM-kp*e; }
  return PWM;
  
}
