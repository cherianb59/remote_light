/*
  Example for receiving
  
  https://github.com/sui77/rc-switch/
  
  If you want to visualize a telegram copy the raw data and 
  paste it into http://test.sui.li/oszi/
*/

#include <RCSwitch.h>           // Include library for communicating with 433Mhz
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
int ledPin = 2; // 

RCSwitch mySwitch_rx = RCSwitch(); // create two my switch classes one for receiving and one for transmitting
RCSwitch mySwitch_tx = RCSwitch();

const char* ssid     = "The LAN down under";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "77788160";     // The password of the Wi-Fi network
String transmit = "000000000000"; // Initial binary "string" to transmit on 433Mhz
const char* style = "<style>\
table, td, th {\
  border: 1px solid black;\
}\
\
table {\
  border-collapse: collapse;\
  width: 100%;\
  height: 50%;\
}\
\
th {\
  height: 10%;\
  width: 10%;\
}\
td {\
  height: 10%;\
  width: 20%;\
  padding: 0px 0px;\
}\
.button {\
  background-color: #4CAF50;\
  border: none;\
  color: white;\
  padding: 15px 32px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 24px;\
  margin: 4px 2px;\
  cursor: pointer;\
  width: 100%;\
  height: 100%;\
}</style>"
;
const char* table = " <table>\
  <tr>\
    <th>Living</th>\
	<th>B1</th>\
    <th>B2</th>\
    <th>B3</th>\
	<th>B4</th>\
  </tr>\
  <tr>	\
	<td> <a href=\"/RADIO=101111110011\"\"><button class=\"button\">Light </button></a>  </td>\
	<td> <a href=\"/RADIO=101111111100\"\"><button class=\"button\">Light </button></a>  </td>\
	<td> <a href=\"/RADIO=101111111011\"\"><button class=\"button\">Light </button></a>  </td>\
	<td> <a href=\"/RADIO=101111111000\"\"><button class=\"button\">Light </button></a>  </td>\
	<td> <a href=\"/RADIO=101111111110\"\"><button class=\"button\">Light </button></a>  </td>\
  </tr>\
  <tr>	\
	<td> <a href=\"/RADIO=111110110011\"\"><button class=\"button\">Fan off </button></a>  </td>\
	<td> <a href=\"/RADIO=111110111100\"\"><button class=\"button\">Fan off </button></a>  </td>\
	<td> <a href=\"/RADIO=111110111011\"\"><button class=\"button\">Fan off </button></a>  </td>\
	<td> <a href=\"/RADIO=111110111000\"\"><button class=\"button\">Fan off </button></a>  </td>\
	<td> <a href=\"/RADIO=111110111110\"\"><button class=\"button\">Fan off </button></a>  </td>\
  </tr>\
  <tr>	\
	<td> <a href=\"/RADIO=011111110011\"\"><button class=\"button\">Fan low </button></a>  </td>\
	<td> <a href=\"/RADIO=011111111100\"\"><button class=\"button\">Fan low </button></a>  </td>\
	<td> <a href=\"/RADIO=011111111011\"\"><button class=\"button\">Fan low </button></a>  </td>\
	<td> <a href=\"/RADIO=011111111000\"\"><button class=\"button\">Fan low </button></a>  </td>\
	<td> <a href=\"/RADIO=011111111110\"\"><button class=\"button\">Fan low </button></a>  </td>\
  </tr>\
  <tr>	\
	<td> <a href=\"/RADIO=111011110011\"\"><button class=\"button\">Fan med </button></a>  </td>\
	<td> <a href=\"/RADIO=111011111100\"\"><button class=\"button\">Fan med </button></a>  </td>\
	<td> <a href=\"/RADIO=111011111011\"\"><button class=\"button\">Fan med </button></a>  </td>\
	<td> <a href=\"/RADIO=111011111000\"\"><button class=\"button\">Fan med </button></a>  </td>\
	<td> <a href=\"/RADIO=111011111110\"\"><button class=\"button\">Fan med </button></a>  </td>\
  </tr>\
  <tr>	\
	<td> <a href=\"/RADIO=111101110011\"\"><button class=\"button\">Fan high </button></a>  </td>\
	<td> <a href=\"/RADIO=111101111100\"\"><button class=\"button\">Fan high </button></a>  </td>\
	<td> <a href=\"/RADIO=111101111011\"\"><button class=\"button\">Fan high </button></a>  </td>\
	<td> <a href=\"/RADIO=111101111000\"\"><button class=\"button\">Fan high </button></a>  </td>\
	<td> <a href=\"/RADIO=111101111110\"\"><button class=\"button\">Fan high </button></a>  </td>\
  </tr>  \
</table> ";
int value = LOW; // store value of LED

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  mySwitch_rx.enableReceive(5);  // Receiver on pin D1 GPIO 5

  mySwitch_tx.enableTransmit(4); // Transmit on pin D2 GPIO 4
  // Optional set pulse length.
  mySwitch_tx.setPulseLength(241);
  // set protocol (default is 1, will work for most outlets)
  mySwitch_tx.setProtocol(11);

  // Connect to the network
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

 // Start the server
  server.begin();
  Serial.println("Server started");

// Set ledPin according to the request
  digitalWrite(ledPin, value); 
  
  pinMode(ledPin, OUTPUT);
}

void loop() {
  //print recieved 433 signals
  if (mySwitch_rx.available()) {
    output(mySwitch_rx.getReceivedValue(), mySwitch_rx.getReceivedBitlength(), mySwitch_rx.getReceivedDelay(), mySwitch_rx.getReceivedRawdata(),mySwitch_rx.getReceivedProtocol());
    mySwitch_rx.resetAvailable();
  }
 // Check if a client has connected, iof no client rest of code not executed
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
    Serial.println("client unavailable");
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request

  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, LOW);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = LOW;
  }

  int radio_idx = request.indexOf("/RADIO=") ; 
  if (radio_idx != -1 )  {
    //string to transmit
    //TODO add checks that string is valid 12 chars and each is 0 or 1
    transmit = request.substring(radio_idx + 7 ,radio_idx + 7 + 12) ;
    Serial.println(transmit);
    //mySwitch cant take String convert to char array using a buffer to store
    char charBuf[12 + 1];
    transmit.toCharArray(charBuf, 12 + 1);
    mySwitch_tx.send(charBuf);
  }
 

 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println(style);
  client.println("</head><body>");
 
  client.print("Led is now: ");
 
  if(value == HIGH) {
    client.print("Off");
  } else {
    client.print("On");
  }
  client.println("<br><br>");
  client.println("Radio is: ");
  client.println(transmit);

  client.println("<br>");
  client.println("<a href=\"/LED=ON\"\"><button class=\"button\">On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button class=\"button\">Off </button></a><br />");  
  client.println("<br>");
  client.println(table);
  client.println("</body>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");  
  
}
