//---------------------------------------------------
// Librerias Camara
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
//---------------------------------------------------
// Librerias EEPROM
#include "EEPROM.h"
#define EEPROM_SIZE 2
//---------------------------------------------------
//Librerias SD
#include "FS.h"
#include "SD_MMC.h"
#include <SPI.h>
//---------------------------------------------------
//Librerias y definición DHT11
#include "DHT.h"          //Libreria DHT11
#define DHTPIN 3          //Pin donde estará conectado el sensor
#define DHTYPE DHT11      //Tipo de sensor
DHT dht(DHTPIN, DHTYPE);  //Declara el pin y el sensor dentro de la libreria
float h = 0;
float t = 0;
//---------------------------------------------------
//Variable timer interrupciones
uint64_t uS_2_S = 1000000;
uint64_t Sleep_S = 3595;
//uint64_t Sleep_S = 5;
//---------------------------------------------------
// Variables para guardar datos
int pictureCount = 0;
String dataMessage;
//---------------------------------------------------
// Pin definitions for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
//---------------------------------------------------
void configESPCamera() {
  // Configure Camera parameters

  // Object to store the camera configuration parameters
  camera_config_t config;
//-----------------------------------------------------------------------------
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.pixel_format = PIXFORMAT_JPEG; // Choices are YUV422, GRAYSCALE, RGB565, JPEG
  
  //PRUEBA
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
//-----------------------------------------------------------------------------
  // Select lower framesize if the camera doesn't support PSRAM
  if (psramFound()) {
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }
//-----------------------------------------------------------------------------
  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
//-----------------------------------------------------------------------------
  // Camera quality adjustments
  sensor_t * s = esp_camera_sensor_get();
//-----------------------------------------------------------------------------
  // BRIGHTNESS (-2 to 2)
  s->set_brightness(s, -2);
  // CONTRAST (-2 to 2)
  s->set_contrast(s, 0);
  // SATURATION (-2 to 2)
  s->set_saturation(s, 2);
  // SPECIAL EFFECTS (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_special_effect(s, 0);
  // WHITE BALANCE (0 = Disable , 1 = Enable)
  s->set_whitebal(s, 1);
  // AWB GAIN (0 = Disable , 1 = Enable)
  s->set_awb_gain(s, 1);
  // WB MODES (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_wb_mode(s, 0);
  // EXPOSURE CONTROLS (0 = Disable , 1 = Enable)
  s->set_exposure_ctrl(s, 1);
  // AEC2 (0 = Disable , 1 = Enable)
  s->set_aec2(s, 0);
  // AE LEVELS (-2 to 2)
  s->set_ae_level(s, 0);
  // AEC VALUES (0 to 1200)
  s->set_aec_value(s, 300);
  // GAIN CONTROLS (0 = Disable , 1 = Enable)
  s->set_gain_ctrl(s, 1);
  // AGC GAIN (0 to 30)
  s->set_agc_gain(s, 0);
  // GAIN CEILING (0 to 6)
  s->set_gainceiling(s, (gainceiling_t)0);
  // BPC (0 = Disable , 1 = Enable)
  s->set_bpc(s, 0);
  // WPC (0 = Disable , 1 = Enable)
  s->set_wpc(s, 1);
  // RAW GMA (0 = Disable , 1 = Enable)
  s->set_raw_gma(s, 1);
  // LENC (0 = Disable , 1 = Enable)
  s->set_lenc(s, 1);
  // HORIZ MIRROR (0 = Disable , 1 = Enable)
  s->set_hmirror(s, 0);
  // VERT FLIP (0 = Disable , 1 = Enable)
  s->set_vflip(s, 1);
  // DCW (0 = Disable , 1 = Enable)
  s->set_dcw(s, 1);
  // COLOR BAR PATTERN (0 = Disable , 1 = Enable)
  s->set_colorbar(s, 0);
}
//---------------------------------------------------
void initMicroSDCard() {
  // Start the MicroSD card
  Serial.println("Mounting MicroSD Card");
  if (!SD_MMC.begin()) {
    Serial.println("MicroSD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No MicroSD Card found");
    return;
  }
}
//---------------------------------------------------
void writeFile(fs::FS &fs, const char * path, const char * message) {
  // Crea el archivo de datos
  Serial.printf("Writing file: %s\n", path);

  File create_file = fs.open(path, FILE_WRITE);
  if(!create_file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(create_file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  create_file.close();
}
//---------------------------------------------------
void colectData(String path, const char * Archivo_txt, const char * lecturas) {
  fs::FS &fs = SD_MMC;
//-----------------------------------------------------------------------------
  // Toma la foto

  // Setup frame buffer
  camera_fb_t  * fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // Guarda la foto en la SD
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in write mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  // Close the file
  file.close();

  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
//----------------------------------------------------------------------------- 
  //Escribir datos del sensor
  Serial.print("Save data: ");
  Serial.println(lecturas);
  
  Serial.printf("Appending to file: %s\n", Archivo_txt);

  File filedata = fs.open(Archivo_txt, FILE_APPEND);
  if(!filedata) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(filedata.print(lecturas)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  filedata.close();

}
//---------------------------------------------------
void setup() {
  Serial.begin(115200);
//---------------------------------------------------
  EEPROM.begin(EEPROM_SIZE); 
  if(EEPROM.read(1) == 255){
    EEPROM.write(1,0);
    EEPROM.commit();
  }
  if(EEPROM.read(0) <= 254)
  {
    EEPROM.write(0, EEPROM.read(0)+1);
    EEPROM.commit();   
  }
  else{
    EEPROM.write(1, EEPROM.read(1)+1);
    EEPROM.commit();
    EEPROM.write(0, 0);
    EEPROM.commit();
  }
  //--------------------------------------------------- 
  dht.begin();
//---------------------------------------------------  
  // Inicia la MicroSD
  initMicroSDCard();
//--------------------------------------------------- 
  // Inicializa the camera
  Serial.print("Initializing the camera module...");
  configESPCamera();
  Serial.println("Camera OK!");
//---------------------------------------------------
  // Crea el archivo txt para guardar los datos si no existe
  File file = SD_MMC.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD_MMC, "/data.txt", "Temperatura, Humedad, Foto \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
//---------------------------------------------------  
  esp_sleep_enable_timer_wakeup(uS_2_S*Sleep_S);
  delay(5000);
  t = dht.readTemperature();
  h = dht.readHumidity();
  Serial.println("");
  Serial.print("T:");
  Serial.print(t);
 
  pictureCount = (EEPROM.read(1)<<8) | (EEPROM.read(0));
    
  // Define el nombre con el que se va a guardar la foto
  String path = "/imagen" + String(pictureCount) + ".jpg";
  Serial.printf("Picture file name: %s\n", path.c_str());

  //Guarda los datos en una cadena de caracteres
  dataMessage = String(t) + "," + String(h) + "," + path + "\r\n";
    
  // Toma la foto y guarda los datos
  colectData(path, "/data.txt", dataMessage.c_str());
  
  esp_deep_sleep_start();
}

void loop() {
}
