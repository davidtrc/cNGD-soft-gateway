#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/********** VARS FOR THE INCOMING PACKET *********/
WiFiUDP port;
IPAddress remoteip;
int remoteport = 0;
int packetSend = 0;
int packetlength = 0;
int packetSize = 0;
/********** END OF VARS FOR THE INCOMING PACKET *********/

/********* GPIOS DEFINITION ******/
#define RTS 13
#define NODEIN 12
#define WIFISTATUS 2
#define LEDDEBUG 16
/********* END OF GPIOS DEFINITION ******/

/**** VARS FOR WAIT THE NODE ACK *****/
unsigned long start_timer_ack = 0;
unsigned long stop_timer_ack = 0;
unsigned long time_elapsed_ack = 0;
int wait_for_ack_exit = 0; //If 1, the node has response. If 0, hasn't.
/**** END OF VARS FOR WAIT THE NODE ACK *****/

#define MAXSIZE 2048
char packetBuffer[MAXSIZE]; //Tamaño máximo de la cadena recibida
char receivedbyserial[MAXSIZE];
char actualcharserial;
int receivedindex = 0;

int wait_for_ack(void){
  while(digitalRead(NODEIN) == LOW){
    stop_timer_ack = millis();
    if((time_elapsed_ack = stop_timer_ack - start_timer_ack) >= 1000){
      return 0;
    }
  }
  return 1;
}

void wait_for_response(){
  receivedindex = 0;
  while(digitalRead(NODEIN) == HIGH){
    while(Serial.available()>0) {
      actualcharserial = Serial.read();
      receivedbyserial[receivedindex] = actualcharserial; 
      receivedindex++;
    }
  }
}

void send_unreachable_node (void){
    port.beginPacket(remoteip, remoteport);
    port.write("El modulo Wi-Fi informa: no ha sido posible contactar con el nodo");
    port.endPacket();
}

void send_response(void){
  int i = 0;
  if(receivedindex == 0){
   send_unreachable_node();
  } else {
    port.beginPacket(remoteip, remoteport);
    port.write("El nodo informa: ");
    port.write(receivedbyserial);
    port.endPacket();
    receivedindex = 0;
    for(i=0; i<sizeof(receivedbyserial); i++){
      receivedbyserial[i] = NULL;
    }
  }
}

void setup() {
  pinMode(WIFISTATUS, OUTPUT);
  pinMode(LEDDEBUG, OUTPUT);
  digitalWrite(WIFISTATUS, 1);
  pinMode(RTS, OUTPUT);
  digitalWrite(RTS, 0);
  pinMode(NODEIN, INPUT);
  
  Serial.begin(115200);
  //WiFi.begin("B105_net", "FiNaO?16");
  WiFi.begin("DEMOTFGDAVIDTRC", "DEMOTFGDAVIDTRC");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
  }
  port.begin(9999);
  digitalWrite(WIFISTATUS, 0);
  
  digitalWrite(RTS, 1);
  while(digitalRead(NODEIN) == LOW){
    delay(10);
  }
  Serial.print(WiFi.localIP());
  delay(10);
  digitalWrite(RTS, 0);
}

void loop() {
  digitalWrite(RTS, 0);
  
  packetSize = port.parsePacket();
  if (packetSize) {
    packetlength = port.read(packetBuffer, MAXSIZE);
    remoteip = port.remoteIP();
    remoteport = port.remotePort();
    if (packetlength > 0) packetBuffer[packetlength] = 0;
    digitalWrite(RTS, 1);
    start_timer_ack = millis();
    wait_for_ack_exit = wait_for_ack();
    if(wait_for_ack_exit == 0){
      send_unreachable_node();
    }
    if(wait_for_ack_exit == 1){
        Serial.print(packetBuffer);
        delay(10);
        digitalWrite(RTS, 0);
    }
    delay(100);
  }
  
  if(digitalRead(NODEIN) == HIGH){
      digitalWrite(RTS, 1);
      wait_for_response();
      digitalWrite(RTS, 0);
      if(receivedindex!=0){
        send_response();
      }
      delay(20); 
  }
}

