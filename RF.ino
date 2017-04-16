//Libraries
#include <SPI.h>
#include <Ethernet.h>

//RF data
#define BITS 38
int rfPin = 9;
int repeat = 12;
float period = 350;
float pausePeriod = 12000;
int lightCode [BITS]  = {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0};
int fanMinCode [BITS] = {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
int fanMaxCode [BITS] = {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
int fanOffCode [BITS] = {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0};

//WebServer data
byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x9C, 0x79};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);
boolean reading = false;
String readString = "";

void setup() {
  pinMode(rfPin, OUTPUT);
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100) {
          readString += c;
        }

        if (c == '\n') {
          Serial.println(readString); //print to serial monitor for debuging

          if (readString.indexOf('?') >= 0) { //don't send new page
            client.println("HTTP/1.1 204 Zoomkat\r\n\r\n");
          }
          else {
            client.println("HTTP/1.1 200 OK"); //send new page
            client.println("Content-Type: text/html");
            client.println();

            client.println("<HTML>");
            client.println("<HEAD>");
            client.println("<TITLE>Arduino GET test page</TITLE>");
            client.println("</HEAD>");
            client.println("<BODY style='font-family:verdana;'>");

            client.print("<center><h3>Awesome Fan Controller</h3>");
            client.print("<p><a href='/?1'>Lights</a><p>");
            client.print("<p><a href='/?2'>Fans Min</a><p>");
            client.print("<p><a href='/?3'>Fans Max</a><p>");
            client.print("<p><a href='/?4'>Fans Off</a><p>");
            client.print("</center>");

            client.println("</BODY>");
            client.println("</HTML>");
          }

          delay(1);
          client.stop();

          if (readString.indexOf("/?1") > 0) //checks for on
          {
            sendRF(lightCode);
            Serial.println("Lights");
          }
          if (readString.indexOf("/?2") > 0) //checks for off
          {
            sendRF(fanMinCode);
            Serial.println("Fans min");
          }
          if (readString.indexOf("/?3") > 0) //checks for off
          {
            sendRF(fanMaxCode);
            Serial.println("Fans max");
          }
          if (readString.indexOf("/?4") > 0) //checks for off
          {
            sendRF(fanOffCode);
            Serial.println("Fans off");
          }

          readString = "";

        }
      }
    }
  }
}

void sendRF(int code []) {
  for (int i = 0; i < repeat; i++) {
    for (int j = 0; j < BITS; j++) {
      if (code[j] == 0) {
        digitalWrite(rfPin, LOW);
      }
      else {
        digitalWrite(rfPin, HIGH);
      }
      delayMicroseconds(period);
    }
    delayMicroseconds(pausePeriod);
  }
}
