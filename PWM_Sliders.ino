// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

// Replace with your network credentials
const char* ssid = "Sebastian";
const char* password = "12345678";

const int output = 2;

String sliderValue = "0";

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
int DC1 = 0;       //Verde
int DC2 = 0;       //Azul
int DC3 = 0;       //Infrarojo
int DC4 = 0;       //Rojo

//Ciclo de trabajo anterior
int DC1L;
int DC2L;
int DC3L;
int DC4L;

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
    DC1L = DC1;
    DC2L = DC2;
    DC3L = DC3;
    DC4L = DC4;
    DC1 = 0;
    DC2 = 0;
    DC3 = 0;
    DC4 = 0;
    /*ledcWrite(C1, DC1);
    ledcWrite(C2, DC2);
    ledcWrite(C3, DC3);
    ledcWrite(C4, DC4);*/
    a = 1;
  }
}

  // Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP Web Server</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" name="rangeInput" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  
  <p><span id="textSliderValue2">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" name="rangeInput" id="pwmSlider2" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  
  <p><span id="textSliderValue3">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" name="rangeInput" id="pwmSlider3" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
  
  <p><span id="textSliderValue4">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" name="rangeInput" id="pwmSlider4" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>

<script>
function updateSliderPWM(element) {
  let sliderInputs = document.getElementsByName("rangeInput");
  let sliderValues = [];
  sliderInputs.forEach((a) => {
    sliderValues.push(a.value);
  });
  let sliderSpans = document.getElementsByTagName("span");
  for(let i = 0; i < sliderSpans.length; i++){
    sliderSpans[i].innerHTML = sliderValues[i];
  }
  console.table(sliderValues, sliderSpans);
  var xhr = new XMLHttpRequest();
  let URL_params = "/slider?values="+sliderValues[0]+"&values2="+sliderValues[1]+"&values3="+sliderValues[2]+"&values4="+sliderValues[3];
  xhr.open("GET", URL_params, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();
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

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage, inputMessage2, inputMessage3, inputMessage4;
    
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam("values")) {
      inputMessage = request->getParam("values")->value();
      inputMessage2 = request->getParam("values2")->value();
      inputMessage3 = request->getParam("values3")->value();
      inputMessage4 = request->getParam("values4")->value();

      Serial.print("Actualizado");
      
      if (!a){
        DC1 = inputMessage.toInt();       //Verde
        DC2 = inputMessage2.toInt();       //Azul
        DC3 = inputMessage3.toInt();       //Infrarojo
        DC4 = inputMessage4.toInt();       //Rojo
      }
      else{
        DC1L = inputMessage.toInt();       //Verde
        DC2L = inputMessage2.toInt();       //Azul
        DC3L = inputMessage3.toInt();       //Infrarojo
        DC4L = inputMessage4.toInt();       //RojO        
      }

      
      
    }else {
      inputMessage = "No message sent";
    }
    request->send_P(200, "text/html", index_html, processor);
  });
  
  // Start server
  server.begin();
  
  //Inicializa el timer
  timer = timerBegin(1,40000,true); //Cuenta cada 500uS --> 1S = 2000 ticks
  timerAttachInterrupt(timer,&onTimer,true);
  timerAlarmWrite(timer,4000,true); //Se activa cada 5s
  timerAlarmEnable(timer);
  }

void loop() {
  //if (!a){
        //Genera la PWM
        ledcWrite(C1, DC1);
        ledcWrite(C2, DC2);
        ledcWrite(C3, DC3);
        ledcWrite(C4, DC4);
      //}
  Serial.print(DC1);
  Serial.print(" ");
  Serial.print(DC2);
  Serial.print(" ");
  Serial.print(DC3);
  Serial.print(" ");
  Serial.print(DC4);
  Serial.print(" a = ");
  Serial.println(a);
  delay(500);

}
