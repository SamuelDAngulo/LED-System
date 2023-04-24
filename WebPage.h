const char PAGE_MAIN[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

  <style>
    /* -------Cabecera-------*/
    header{
      max-width: 600px;
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      margin: 0;
      padding: 10px 0px;
      /*background-color: #b38383;*/
      border-bottom: 5px solid #293578;
    }

    .navtitle {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 50px;
      font-weight: bold;
      line-height: 50px;
      height: 50px;
    }

    .navdate{
      display: flex;
      flex-direction: column;
      height: 50px;
      padding-left: 20px;
      /*background-color: #636fb3;*/
    }

    .navdata {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 20px;
      line-height: 20px;
      height: 50px;
      padding-right: 20px;
    }

    /* -------Cuerpo-------*/
    .container_body {
      max-width: 600px;
      /*background-color: #293578;*/
    }

    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display:inline;
    }

    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      margin-bottom: 5px;
      color: #000000;
    }

    .container {
      display: flex;
      flex-direction: column;
      flex-wrap: wrap;
      margin: 0 auto;
      /*background-color: #d6c5c5;*/
      border-bottom: 5px solid #293578;
    }

    .container_center {
      display: flex;
      justify-content: center;
      flex-direction: column;
      align-items: center;
      /*background-color: #293578;*/
    }

    /* -------Control LEDs-------*/
    .LEDslider {
        width: 50%;
        height: 55px;
        outline: none;
        height: 25px;
        margin-bottom: 10px;
      }

      /* -------Fotoperiodo-------*/
    .fotoperiod {
      display: flex;
      flex-direction: row;
      align-items: center;
      justify-content: space-evenly;
      margin-bottom: 8px;
    }
    /*
    <!-- HTML !-->
    <button class="button-22" role="button">Button 22</button>*/

    /* -------Boton enviar------- */
    .button_send {
      align-items: center;
      appearance: button;
      background-color: #0276FF;
      border-radius: 8px;
      border-style: none;
      box-shadow: rgba(255, 255, 255, 0.26) 0 1px 2px inset;
      box-sizing: border-box;
      color: #fff;
      cursor: pointer;
      display: flex;
      flex-direction: row;
      flex-shrink: 0;
      font-family: "RM Neue",sans-serif;
      font-size: 100%;
      line-height: 1.15;
      margin: 0;
      padding: 10px 21px;
      text-align: center;
      text-transform: none;
      transition: color .13s ease-in-out,background .13s ease-in-out,opacity .13s ease-in-out,box-shadow .13s ease-in-out;
      user-select: none;
      -webkit-user-select: none;
      touch-action: manipulation;
    }

    .button_send:active {
      background-color: #006AE8;
    }

    .button_send:hover {
      background-color: #1C84FF;
    }

      .foot {
        font-family: "Verdana", "Arial", sans-serif;
        font-size: 20px;
        position: relative;
        height:   30px;
        text-align: center;   
        color: #AAAAAA;
        line-height: 20px;
      }
  </style>

  <head>
    <meta charset = "UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>ESP LED System</title>
    <!--<link rel="stylesheet" href="style.css">-->
  </head>

  <body style="background-color: #eeeeee" onload="process()">

    <!-- Cabecera -->
    <header> 
      <div class="navtitle">LED Control</div>
      <div class="navdate">
        <div class="navdata">Fecha: <span id="date">mm/dd/yyyy</span></div>
        <div class="navdata">Hora: <span id="time">00:00:00</span></div>            
      </div>
    </header>

    <!-- Cuerpo -->
    <main class="container_body">   

      <!-- Control LEDs -->
      <div class="container">
        <div class="category">Control intensidad LEDs</div>

        <!-- LEDs Verde-->
        <div class="container_center">
          <div class="bodytext">LEDs Verdes: <span id="LEDV_percent">0</span>%</div>
          <input type="range" class="LEDslider" min="0" max="100" value = "0" width = "0%" id="slider_v" oninput="UpdateSliderV(this.value)"/>
        </div>

        <!-- LEDs Azul-->
        <div class="container_center">
          <div class="bodytext">LEDs Azules: <span id="LEDA_percent">0</span>%</div>
          <input type="range" class="LEDslider" min="0" max="100" value = "0" width = "0%" id="slider_a" oninput="UpdateSliderA(this.value)"/>
        </div>

        <!-- LEDs Infrarojo-->
        <div class="container_center">
          <div class="bodytext">LEDs Infrarojos: <span id="LEDIR_percent">0</span>%</div>
          <input type="range" class="LEDslider" min="0" max="100" value = "0" width = "0%" id="slider_ir" oninput="UpdateSliderIR(this.value)"/>
        </div>

        <!-- LEDs Rojo-->
        <div class="container_center">
          <div class="bodytext">LEDs Rojos: <span id="LEDR_percent">0</span>%</div>
          <input type="range" class="LEDslider" min="0" max="100" value = "0" width = "0%" id="slider_r" oninput="UpdateSliderR(this.value)"/>
        </div>
      </div>
      
      <!-- Fotoperiodo -->
      <div class="container">
        <div class="category">Fotoperiodo</div>

        <div class="fotoperiod">
            <label for="hora_inicio" class="bodytext">Hora inicial</label>
            <label for="hora_fin" class="bodytext" style="padding-right: 9px;">Hora final</label>
        </div>

        <div class="fotoperiod">
          <input id="hora_inicio" value="06:00" type="time">
          <input id="hora_fin" value="13:00" type="time">
        </div>

        <div style="padding-top: 10px;" class="fotoperiod">
          <div class="bodytext">Fotoperiodo: <span id="fotoperiodo_h">0</span> horas y <span id="fotoperiodo_m">0</span> minutos</div>
        </div> 

        <div style="margin-bottom: 10px; margin-top: 10px;" class="container_center">
          <button type="button" class = "button_send" id = "enviar" onclick="ButtonPress()">Enviar</button>
        </div>
      </div>
      
    </main>
    <!--
    <footer div class="foot" id = "temp" >ESP32 Web Page</div></footer>
    -->
  </body>

  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    function ButtonPress() {
      var xhttp = new XMLHttpRequest(); 
      var message;

      var hora_init;
      var hora_fin;
      var hora_actual;

      hora_init = document.getElementById("hora_inicio").value;
      hora_fin = document.getElementById("hora_fin").value;
      //hora_actual = document.getElementById("hora_act").value;
      hora_actual = document.getElementById("time").textContent;

      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from ESP
          var buf = this.responseText;
          document.getElementById("fotoperiodo_h").innerHTML= buf.substring(0,buf.indexOf(':'));
          document.getElementById("fotoperiodo_m").innerHTML= buf.substring(buf.indexOf(':')+1);
        }
      }

      xhttp.open("PUT", "HORA?VALUE="+hora_init+hora_fin+hora_actual, true);
      xhttp.send();
    }

    function UpdateSliderV(value) {
      var xhttp = new XMLHttpRequest();

      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          // update the web based on reply from ESP
          document.getElementById("LEDV_percent").innerHTML=this.responseText;
        }
      }

      xhttp.open("PUT", "SLIDER_V?VALUE="+value, true);
      xhttp.send();
    }

    function UpdateSliderA(value) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("LEDA_percent").innerHTML=this.responseText;
        }
      }

      xhttp.open("PUT", "SLIDER_A?VALUE="+value, true);
      xhttp.send();
    }

    function UpdateSliderIR(value) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("LEDIR_percent").innerHTML=this.responseText;
        }
      }

      xhttp.open("PUT", "SLIDER_IR?VALUE="+value, true);
      xhttp.send();
    }

    function UpdateSliderR(value) {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          document.getElementById("LEDR_percent").innerHTML=this.responseText;
        }
      }

      xhttp.open("PUT", "SLIDER_R?VALUE="+value, true);
      xhttp.send();
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
     
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
  
      // LEDV
      //xmldoc = xmlResponse.getElementsByTagName("LEDV_percent"); //LEDV percent
      //message = xmldoc[0].firstChild.nodeValue;
      
      //document.getElementById("LEDV").innerHTML=message;
     }
  
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        setTimeout("process()",100);
    }
  
  </script>

</html>
)=====";