#include <ESP8266WiFi.h>
#define DEBUG 0
// Replace with your network credentials
const char* ssid     = "Your-ssid";
const char* password = "your-password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String ESP_LED_State = "off";
String STM_LED_State = "off";

// Assign output variables to GPIO pins
const int ESP_LED = 0;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void UART_Transmit(const char* msg, int _delay)
{
  for (int i = 0; i < strlen(msg); i++)
  {
    Serial.write(msg[i]);
    delay(_delay);
    Serial.flush();
  }
}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(ESP_LED, OUTPUT);
  //  pinMode(STM_LED, OUTPUT);
  // Set outputs to LOW
  digitalWrite(ESP_LED, HIGH);
  //  digitalWrite(STM_LED, LOW);

  // Connect to Wi-Fi network with SSID and password
  if (DEBUG)
    Serial.print("Connecting to ");
  if (DEBUG)
    Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (DEBUG)
      Serial.print(".");
  }
  // Print local IP address and start web server
  if (DEBUG)
  {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  server.begin();
}
//int pi,ei;
//bool tempread = false,timeread=false;
bool tempready = false,timeready=false;
String temp = " 000.00", timer = "00:00:00  1\t00/00/00";
void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  char c;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (Serial.available())
  {
    c = Serial.read();
    if (c == '!')
    {
      UART_Transmit(WiFi.localIP().toString().c_str(), 10);
      UART_Transmit("\r\n", 10);
      c = ' ';
    }
    else if (c == 'P')
    {
      temp = Serial.readStringUntil('!');
      tempready = true;
    }
    else if (c == 'E')
    {
      timer = Serial.readStringUntil('!');
      timeready = true;
    }
  }
  if (client) {                             // If a new client connects,
    if (DEBUG)
      Serial.println("New Client.");          // print a message out in the serial port
    if (Serial.available())
    {
      c = Serial.read();
      if (c == '!')
      {
        UART_Transmit(WiFi.localIP().toString().c_str(), 10);
        UART_Transmit("\r\n", 10);
        c = ' ';
      }
    }
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (DEBUG)
          Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // turns the LEDs on and off
            if (header.indexOf("GET /ESP/on") >= 0) {
              if (DEBUG)
                Serial.println("ESP LED on");
              ESP_LED_State = "on";
              digitalWrite(ESP_LED, LOW);
            } else if (header.indexOf("GET /ESP/off") >= 0) {
              if (DEBUG)
                Serial.println("ESP LED off");
              ESP_LED_State = "off";
              digitalWrite(ESP_LED, HIGH);
            } else if (header.indexOf("GET /STM/on") >= 0) {
              if (DEBUG)
                Serial.println("STM LED on");
              STM_LED_State = "on";
              UART_Transmit("T_ON", 10);
            } else if (header.indexOf("GET /STM/off") >= 0) {
              if (DEBUG)
                Serial.println("STM LED off");
              STM_LED_State = "off";
              UART_Transmit("T_OF", 10);
            } else if (header.indexOf("GET /RTC") >= 0) {
              if (DEBUG)
                Serial.println("Display Time");
              UART_Transmit("TIME", 10);
            } else if (header.indexOf("GET /TEMP") >= 0) {
              if (DEBUG)
                Serial.println("Display Temp");
              UART_Transmit("TEMP", 10);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");

            // Display current state, and ON/OFF buttons for ESP LED
            client.println("<p>ESP LED - State " + ESP_LED_State + "</p>");
            // If the ESP_LED_State is off, it displays the ON button
            if (ESP_LED_State == "off") {
              client.println("<p><a href=\"/ESP/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ESP/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for STM LED
            client.println("<p>STM LED - State " + STM_LED_State + "</p>");
            // If the STM_LED_State is off, it displays the ON button
            if (STM_LED_State == "off") {
              client.println("<p><a href=\"/STM/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/STM/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            // Display current state, and ON/OFF buttons for STM LED
            if(!timeready)
              client.println("<p>RTC Timer </p>");
            else
              client.println("<p>RTC Timer = " + timer + "</p>");
            client.println("<p><a href=\"/RTC\"><button class=\"button\">DISPLAY</button></a></p>");
            // Display current state, and ON/OFF buttons for STM LED
            if(!tempready)
            client.println("<p>Temprature </p>");
            else
            client.println("<p>Temprature = " + temp + "</p>");
            client.println("<p><a href=\"/TEMP\"><button class=\"button\">GET</button></a></p>");

            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    if (DEBUG)
    {
      Serial.println("Client disconnected.");
      Serial.println("");
    }
  }
}
