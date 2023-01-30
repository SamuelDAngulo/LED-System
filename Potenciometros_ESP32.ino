//Ubicación de los LEDs
const int LED1 = 5;   //Verde
const int LED2 = 18;  //Azul
const int LED3 = 19;  //Infrarojo
const int LED4 = 21;  //Rojo

//Ubicación potenciometros
const int POT1 = 13;  //Verde
const int POT2 = 12;  //Azul
const int POT3 = 14;  //Infrarrojo
const int POT4 = 27;  //Rojo

//Pin del Switch
const int Switch = 23; 

//Ciclo de trabajo anterior
int DC1L;
int DC2L;
int DC3L;
int DC4L;

//Propiedades PWM
const int C1 = 0;     //Canal PWM
const int F1 = 5000;  //Frecuencia PWM
const int R1 = 8;     //Resolución PWM

const int C2 = 1;     //Canal 2PWM
const int F2 = 5000;  //Frecuencia 2PWM
const int R2 = 8;     //Resolución 2PWM

const int C3 = 2;     //Canal 3PWM
const int F3 = 5000;  //Frecuencia 3PWM
const int R3 = 8;     //Resolución 3PWM

const int C4 = 3;     //Canal 4PWM
const int F4 = 5000;  //Frecuencia 4PWM
const int R4 = 8;     //Resolución 4PWM

//Duty Cycle
int DC1 = 250;       //Verde
int DC2 = 127;       //Azul
int DC3 = 250;       //Infrarojo
int DC4 = 250;       //Rojo

//Variable timer interrupciones
hw_timer_t * timer = NULL;
bool a = 0;

//Función de interrupción
void IRAM_ATTR onTimer(){
  if(a){
    //Ciclo día
    DC1 = DC1L;
    DC2 = DC2L;
    DC3 = DC3L;
    DC4 = DC4L;
    a = 0;
  }
  else{
    //Ciclo noche
    timerAlarmWrite(timer,86400000,true); //Se activa cada 12 horas
    DC1L = DC1;
    DC2L = DC2;
    DC3L = DC3;
    DC4L = DC4;
    DC1 = 0;
    DC2 = 0;
    DC3 = 0;
    DC4 = 0;
    a = 1;
  }
}
void setup() {
  Serial.begin(115200);
  //Configuración de las PWM
  ledcSetup(C1, F1, R1);
  ledcSetup(C2, F2, R2);
  ledcSetup(C3, F3, R3);
  ledcSetup(C4, F4, R4);
  
  //Acoplar pines de salida para la PWM
  ledcAttachPin(LED1, C1);
  ledcAttachPin(LED2, C2);
  ledcAttachPin(LED3, C3);
  ledcAttachPin(LED4, C4);

  //Switch para activar o desactivar los potenciometros
  pinMode(26,INPUT);
  
  //Inicializa el timer
  timer = timerBegin(1,40000,true); //Cuenta cada 500uS --> 1S = 2000 ticks
  timerAttachInterrupt(timer,&onTimer,true);
  timerAlarmWrite(timer,70200000,true); //Se activa cada 9 horas y 45 min
  timerAlarmEnable(timer);
  }

void loop() {
  //Activar potenciometros
  if (digitalRead(Switch) == HIGH && a == 0){
    DC1 = map(analogRead(POT1),0,4095,0,255); 
    DC2 = map(analogRead(POT2),0,4095,0,255);
    DC3 = map(analogRead(POT3),0,4095,0,255);
    DC4 = map(analogRead(POT4),0,4095,0,255);
  }
  //Genera la PWM
  ledcWrite(C1, DC1);
  ledcWrite(C2, DC2);
  ledcWrite(C3, DC3);
  ledcWrite(C4, DC4);
  
  Serial.print("DC1: ");
  Serial.print(DC1);
  Serial.print(" DC2: ");
  Serial.print(DC2);
  Serial.print(" DC3: ");
  Serial.print(DC3);
  Serial.print(" DC4: ");
  Serial.println(DC4);
  
  delay(50);
}
