// Import required libraries
#include <RCSwitch.h>           // Include library for communicating with 433Mhz
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

int ledPin = 2; // 

RCSwitch mySwitch_rx = RCSwitch(); // create two my switch classes one for receiving and one for transmitting
RCSwitch mySwitch_tx = RCSwitch();


// Replace with your network credentials
const char* ssid     = "The LAN down under";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "77788160";     // The password of the Wi-Fi network


const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    table, td, th {  border: 1px solid black;}
    table {  border-collapse: collapse;  width: 100%;  height: 50%;}
    th {  height: 10%;  width: 10%;}
    td {  height: 10%;  width: 20%;  padding: 0px 0px;}
    .button {  background-color: #4CAF50;  border: none;  color: white;
    padding: 15px 32px;  text-align: center;  text-decoration: none;  display: inline-block;
    font-size: 24px;  margin: 4px 2px;  cursor: pointer;  width: 100%;  height: 100%;
    }
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  <button  onclick="clickButton(this)" class="button" data-medium="LED" data-value="=0">LED On </button>
  <button  onclick="clickButton(this)" class="button" data-medium="LED" data-value="=1">LED Off </button>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
function clickButton(element) {
  var xhr = new XMLHttpRequest();
  var medium = element.getAttribute("data-medium") ;
  var value =   element.getAttribute("data-value") ;
  xhr.open("GET", "/update?output="+medium+"&state"+value, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";


// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = R"rawliteral(
      <table>  
        <tr>    <th>Living</th>	<th>B1</th>    <th>B2</th>    <th>B3</th>	<th>B4</th>  </tr>  
        <tr>		<td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=101111110011">Light </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=101111111100">Light </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=101111111011">Light </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=101111111000">Light </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=101111111110">Light </button></a>  </td>  </tr> 
        <tr>		<td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111110110011">Fan off </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111110111100">Fan off </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111110111011">Fan off </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111110111000">Fan off </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111110111110">Fan off </button></a>  </td>  </tr> 
        <tr>		<td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=011111110011">Fan low </button></a>  </td>
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=011111111100">Fan low </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=011111111011">Fan low </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=011111111000">Fan low </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=011111111110">Fan low </button></a>  </td>  </tr>  
        <tr>		<td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111011110011">Fan med </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111011111100">Fan med </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111011111011">Fan med </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111011111000">Fan med </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111011111110">Fan med </button></a>  </td>  </tr>  
        <tr>		<td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111101110011">Fan high </button></a>  </td>
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111101111100">Fan high </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111101111011">Fan high </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111101111000">Fan high </button></a>  </td>	
          <td> <button  onclick="clickButton(this)" class="button" data-medium="RADIO" data-value="=111101111110">Fan high </button></a>  </td>  </tr>  
        </table> 
    )rawliteral";
    return buttons;
  }
  return String();
}


String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  mySwitch_rx.enableReceive(5);  // Receiver on pin D1 GPIO 5

  mySwitch_tx.enableTransmit(4); // Transmit on pin D2 GPIO 4
  // Optional set pulse length.
  mySwitch_tx.setPulseLength(241);
  // set protocol (default is 1, will work for most outlets)
  mySwitch_tx.setProtocol(11);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
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

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      
      if (inputMessage1 == "RADIO"){
        char charBuf[12 + 1];
        inputMessage2.toCharArray(charBuf, 12 + 1);
        mySwitch_tx.send(charBuf);
      }
      else if (inputMessage1 == "LED"){
        digitalWrite(ledPin, inputMessage2.toInt());
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("Output: ");
    Serial.print(inputMessage1);
    Serial.print("Value: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {
  //TODO print recieved 433 signals
}
