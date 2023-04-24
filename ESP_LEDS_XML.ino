#include <WiFi.h>       // Librería estandar
#include <WebServer.h>  // Librería estandar
#include "WebPage.h"   // Archivo .h que almacena el código de la página con HTML

// Si se desea utilizar una WiFi ya establecida quitar el comentario de "#define USE_INTRANET"
//#define USE_INTRANET

// Reemplazar con las credenciales de la red WiFi a la que desea conectarse
#define LOCAL_SSID "*******"
#define LOCAL_PASS "*******"

// Nombre y contraseña de la red generada por la ESP32
#define AP_SSID "TestWebSite"
#define AP_PASS "12345678"

// Define los pines utilizados por los LEDs
#define PIN_LEDV 5  //Pin 5 LED verde
#define PIN_LEDA 18  //Pin 18 LED azul
#define PIN_LEDIR 19 //Pin 19 LED ingrarojo
#define PIN_LEDR 21  //Pin 21 LED rojo

// Variables si se desea sensar
bool SomeOutput = false;
uint32_t SensorUpdate = 0;

// variables para los LEDs
int LEDV = 0, LEDA = 0, LEDIR = 0, LEDR = 0;
int LEDV_percent = 0, LEDA_percent = 0, LEDIR_percent = 0, LEDR_percent = 0;
int t_hora = 0, t_min = 0;

//Variable timer interrupciones
hw_timer_t * timer = NULL;
bool LED_status = 0;
int t_on = 0, t_off = 0;

//Función de interrupción
void IRAM_ATTR onTimer(){
  if(LED_status){
    //Ciclo día
    //Serial.println("Encendido");
    timerAlarmWrite(timer,t_on,true); //Dura activo t_on horas.
    ledcWrite(0,LEDV);
    ledcWrite(1,LEDA);
    ledcWrite(2,LEDIR);
    ledcWrite(3,LEDR);
    LED_status = 0;
  }
  else{
    //Ciclo noche
    //Serial.println("Apagado");
    timerAlarmWrite(timer,t_off,true); //Dura activo t_off horas
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    LED_status = 1;
  }
}

// Variable para enviar el archivo XML
char XML[2048];

// Buffer para variables char
char buf[32];

// Variable para mostrar la dirección IP al conectarse
IPAddress Actual_IP;

// Dirección IP generada por la ESP32
IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ip;

// Genera el servidor
WebServer server(80);

void setup() {

  Serial.begin(115200);

  // Configuración pines de los LEDs
  pinMode(PIN_LEDV, OUTPUT);
  pinMode(PIN_LEDA, OUTPUT);
  pinMode(PIN_LEDIR, OUTPUT);
  pinMode(PIN_LEDR, OUTPUT);

  // Configuración del LED verde a una señal PWM
  ledcSetup(0, 10000, 12);
  ledcAttachPin(PIN_LEDV, 0);
  ledcWrite(0, LEDV);

  // Configuración del LED azul a una señal PWM
  ledcSetup(1, 10000, 12);
  ledcAttachPin(PIN_LEDA, 1);
  ledcWrite(1, LEDA);

  // Configuración del LED infrarojo a una señal PWM
  ledcSetup(2, 10000, 12);
  ledcAttachPin(PIN_LEDIR, 2);
  ledcWrite(2, LEDIR);

  // Configuración del LED rojo a una señal PWM
  ledcSetup(3, 10000, 12);
  ledcAttachPin(PIN_LEDR, 3);
  ledcWrite(3, LEDR);

  // Desactiva el perro guardian del núcleo 0 para evitar errores al utilizar el módulo WiFi
  disableCore0WDT();

  // Desactiva el perro guardian del núcleo 1
  //disableCore1WDT();

  // Indica que se está iniciando el servidor
  Serial.println("starting server");

  // Se conecta a una red WiFi
  #ifdef USE_INTRANET
    WiFi.begin(LOCAL_SSID, LOCAL_PASS);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    Actual_IP = WiFi.localIP();
  #endif

    // Crea el acces point
  #ifndef USE_INTRANET
    WiFi.softAP(AP_SSID, AP_PASS);
    delay(100);
    WiFi.softAPConfig(PageIP, gateway, subnet);
    delay(100);
    Actual_IP = WiFi.softAPIP();
    Serial.print("IP address: "); Serial.println(Actual_IP);
  #endif

  printWifiStatus();

  // Envía la página web al dispositivo
  server.on("/", SendWebsite);

  // Envía el archivo XML
  server.on("/xml", SendXML);

  // Se encarga de recibir las intrucciones que se envian desde la página web
  // por ejemplo "/SLIDER_V" y ejecuta su respectiva función.
  server.on("/SLIDER_V", UpdateSliderV);
  server.on("/SLIDER_A", UpdateSliderA);
  server.on("/SLIDER_IR", UpdateSliderIR);
  server.on("/SLIDER_R", UpdateSliderR);
  server.on("/HORA", ProcessButton);

  // Inicia el servidor
  server.begin();

  // Se configura el temporizador que se utilizará en los ciclos de encendido
  // y apagado de los LEDs.
  timer = timerBegin(1,40000,true); //Cuenta cada 500uS --> 1S = 2000 ticks
  timerAttachInterrupt(timer,&onTimer,true);
}

void loop() {

  // Si se necesita sensar algo desde la ESP se puede añadir acá
  if ((millis() - SensorUpdate) >= 50) {
    //Serial.println("Reading Sensors");
    SensorUpdate = millis();
  }

  // Llama constantemente al servidor para realizar sus funciones
  server.handleClient();
}
// Función para configurar el timer con el fotoperiodo establecido
void ProcessButton() {

  String hora_init, hora_fin, min_init, min_fin, hora_actual, min_actual;
  int i_hora_init, i_hora_fin, i_min_init, i_min_fin, i_hora_actual, i_min_actual;
  int Fotoperiodo_h, Fotoperiodo_m;
  int t_espera;

  String t_state = server.arg("VALUE");

  // Separa los datos del string recibido de la página web
  hora_init = t_state.substring(0,2);
  min_init = t_state.substring(3,5);

  hora_fin = t_state.substring(5,7);
  min_fin = t_state.substring(8,10);
  
  //Serial.println(t_state.substring(11,12));
  if(t_state.substring(11,12) == ":"){
    //Serial.println("Entró con :");
    hora_actual = t_state.substring(10);
    min_actual = t_state.substring(12,14);
  }
  else{
    //Serial.println("Entró sin :");
    hora_actual = t_state.substring(10,12);
    min_actual = t_state.substring(13,15);
  }

  // Convierte el String recibido de la página en enteros
  i_hora_init  = hora_init.toInt();
  i_min_init = min_init.toInt();

  i_hora_fin  = hora_fin.toInt();
  i_min_fin = min_fin.toInt();

  i_hora_actual = hora_actual.toInt();
  i_min_actual = min_actual.toInt();

  Serial.println(t_state);
  if(t_state.indexOf("P") != -1){
    //Serial.println("Con PM");
    i_hora_actual = i_hora_actual+12;
  } 
  else if(t_state.indexOf("A") & i_hora_actual == 12){
    //Serial.println("Am 12hr");
    i_hora_actual = 0;
  }

  // Convierte las horas a minutos
  i_min_init+= i_hora_init * 60;
  i_min_fin+= i_hora_fin * 60;
  i_min_actual+= i_hora_actual * 60;

  Serial.print("Min inicial: "); Serial.println(i_min_init);
  Serial.print("Min actual: "); Serial.println(i_min_actual);
  Serial.print("Min final: "); Serial.println(i_min_fin);

  // Cálculo del fotoperiodo
  Fotoperiodo_m = timeBetween(i_min_init,i_min_fin);
  Serial.print("Fotoperiodo: "); Serial.println(Fotoperiodo_m);

  // Cálculo tiempo de configuración del timer
  if(i_min_fin > i_min_init & i_min_fin > i_min_actual){
    if(i_min_init > i_min_actual){
      Serial.println("Caso 213");
      t_espera = timeBetween(i_min_actual,i_min_init);
      LED_status = 0;                                    //Se enciende el LED en t_espera(horas)
    }
    else{
      Serial.println("Caso 231");
      t_espera = timeBetween(i_min_actual,i_min_fin);
      LED_status = 1;                                    //Se apaga el LED en t_espera(horas)
    }
  }
  else if(i_min_init > i_min_fin & i_min_init > i_min_actual){
    if(i_min_fin > i_min_actual){
      Serial.println("Caso 123");
      t_espera = timeBetween(i_min_actual,i_min_fin);
      LED_status = 1;                                    //Se apaga el LED en t_espera(horas)
    }
    else{
      Serial.println("Caso 132");
      t_espera = timeBetween(i_min_actual,i_min_init);
      LED_status = 0;                                    //Se enciende el LED en t_espera(horas)
    }
  }
  else if(i_min_init > i_min_fin){
    Serial.println("Caso 312");
    t_espera = timeBetween(i_min_actual,i_min_fin);
    LED_status = 1;                                    //Se apaga el LED en t_espera(horas)
  }
  else{
    Serial.println("Caso 321");  
    t_espera = timeBetween(i_min_actual,i_min_init);
    LED_status = 0;                                    //Se enciende el LED en t_espera(horas)  
  }
  Serial.print("t_espera: "); Serial.println(t_espera);
  Serial.print("Led_Status: "); Serial.println(LED_status);

  // Cálculo tiempo que estaran encendidos y apagados los LEDs segun el fotoperiodo
  t_on = Fotoperiodo_m*120000;
  t_off = (60-Fotoperiodo_m)*120000;

  // Tiempos escalados para pruebas (cada hora dura un segundo)
  /*t_on = Fotoperiodo_m*33;
  t_off = (1440-Fotoperiodo_m)*33;*/

  Serial.print("t_on: "); Serial.println(t_on);
  Serial.print("t_off: "); Serial.println(t_off);

  // Configuración del temporizador de inicio
  timerStop(timer);
  timerRestart(timer);
  timerAlarmWrite(timer,t_espera*120000,true); // Se activa cada t_espera(minutos)
  //timerAlarmWrite(timer,t_espera*33,true);   // Escalado para pruebas
  timerAlarmEnable(timer);
  timerStart(timer);

  Serial.println("Inicia timer");
  if(LED_status == 1){
    ledcWrite(0,LEDV);
    ledcWrite(1,LEDA);
    ledcWrite(2,LEDIR);
    ledcWrite(3,LEDR);
    LED_status = 0;
  }
  else{
    ledcWrite(0,0);
    ledcWrite(1,0);
    ledcWrite(2,0);
    ledcWrite(3,0);
    LED_status = 1;
  }  
  Serial.println();

  // Envía de vuelta el fotoperiodo calculado a la página web
  strcpy(buf, "");
  sprintf(buf, "%d:%d", Fotoperiodo_m/60, Fotoperiodo_m%60);
  sprintf(buf, buf);
  
  server.send(200, "text/plain", buf); //Send web page
  
}

// Función para ajustar la intensidad del LED verde
void UpdateSliderV() {

  String t_state = server.arg("VALUE");

  // Convierte el String recibido de la página en entero
  LEDV_percent = t_state.toInt();
  Serial.print("LEDV: "); Serial.println(LEDV_percent);

  // Reescala el valor de 0 a 100 recibido de la página a la resolución 
  // de 12 bits establecida en la PWM (0 a 4096).
  LEDV = map(LEDV_percent, 0, 100, 0, 4096);
  ledcWrite(0, LEDV);
  
  strcpy(buf, "");
  sprintf(buf, "%d", LEDV_percent);
  sprintf(buf, buf);

  server.send(200, "text/plain", buf); // Se envía a la página web
}
// Función para ajustar la intensidad del LED azul
void UpdateSliderA() {

  String t_state = server.arg("VALUE");

  // Convierte el String recibido de la página en entero
  LEDA_percent = t_state.toInt();
  Serial.print("LEDA: "); Serial.println(LEDA_percent);

  // Reescala el valor de 0 a 100 recibido de la página a la resolución 
  // de 12 bits establecida en la PWM (0 a 4096).
  LEDA = map(LEDA_percent, 0, 100, 0, 4096);
  ledcWrite(1, LEDA);
  
  strcpy(buf, "");
  sprintf(buf, "%d", LEDA_percent);
  sprintf(buf, buf);

  server.send(200, "text/plain", buf); // Se envía a la página web
}
// Función para ajustar la intensidad del LED infrarojo
void UpdateSliderIR() {

  String t_state = server.arg("VALUE");

  // Convierte el String recibido de la página en entero
  LEDIR_percent = t_state.toInt();
  Serial.print("LEDIR: "); Serial.println(LEDIR_percent);

  // Reescala el valor de 0 a 100 recibido de la página a la resolución 
  // de 12 bits establecida en la PWM (0 a 4096).
  LEDIR = map(LEDIR_percent, 0, 100, 0, 4096);
  ledcWrite(2, LEDIR);
  
  strcpy(buf, "");
  sprintf(buf, "%d", LEDIR_percent);
  sprintf(buf, buf);

  server.send(200, "text/plain", buf); // Se envía a la página web
}
// Función para ajustar la intensidad del LED rojo
void UpdateSliderR() {

  String t_state = server.arg("VALUE");

  // Convierte el String recibido de la página en entero
  LEDR_percent = t_state.toInt();
  Serial.print("LEDR: "); Serial.println(LEDR_percent);

  // Reescala el valor de 0 a 100 recibido de la página a la resolución 
  // de 12 bits establecida en la PWM (0 a 4096).
  LEDR = map(LEDR_percent, 0, 100, 0, 4096);
  ledcWrite(3, LEDR);
  
  strcpy(buf, "");
  sprintf(buf, "%d", LEDR_percent);
  sprintf(buf, buf);

  server.send(200, "text/plain", buf); // Se envía a la página web
}
// Función que envía la página web al dispositivo que se conecte
// PAGE_MAIN es una variable char definida en el archivo WebPage.h
void SendWebsite() {

  Serial.println("sending web page");
  server.send(200, "text/html", PAGE_MAIN);
}

// Función que envía el archivo XLM al dispositivo que se conecte
void SendXML() {

  // Serial.println("sending xml");
  // En este caso se regresaron los valores necesarios en las funciones
  // individuales, sin embargo, se puede utilizar el formato XML para 
  // enviar variables de vuelta a la página.
  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  // send green Led value
  sprintf(buf, "<LEDV_percent>1</LEDV_percent>\n");
  strcat(XML, buf);

  strcat(XML, "</Data>\n");

  //Serial.println(XML);

  server.send(200, "text/xml", XML);
}

// Muestra datos de la red generada o a la que se conecta
void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("Open http://");
  Serial.println(ip);
}
// Función que calcula la diferencia de tiempos
int timeBetween(int HI,int HF){
  if (HI <= HF){
    return HF - HI;
  }
  else{
    return HF - HI + 1440;
  }
}