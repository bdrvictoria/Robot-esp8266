#define DEBUG true
// Includem biblioteca necesara pentru I2C
#include <Wire.h>
#include <Servo.h>

// Pinii motor 1
#define mpin00 5
#define mpin01 6
// Pinii motor 2
#define mpin10 3
#define mpin11 11

int buton = 0;
int LED = 13;
Servo srv; 

void setup() {
 Serial.begin(115200);

 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, LOW);
 sendData("AT+RST\r\n", 2000, false); // resetare modul
 sendData("AT+CWMODE=2\r\n", 1000, false); // configurare ca access point
 sendData("AT+CIFSR\r\n", 1000, DEBUG); // citeste adresa IP 
 sendData("AT+CWSAP?\r\n", 2000, DEBUG); // citeste informatia SSID (nume retea)
 sendData("AT+CIPMUX=1\r\n", 1000, false); // configurare conexiuni multiple
 sendData("AT+CIPSERVER=1,80\r\n", 1000, false); // pornire server pe port 80

 // configurarea pinilor motor ca iesire, initial valoare 0
 digitalWrite(mpin00, 0);
 digitalWrite(mpin01, 0);
 digitalWrite(mpin10, 0);
 digitalWrite(mpin11, 0);
 pinMode (mpin00, OUTPUT);
 pinMode (mpin01, OUTPUT);
 pinMode (mpin10, OUTPUT);
 pinMode (mpin11, OUTPUT);
 // pin LED
 pinMode(13, OUTPUT); 
 
}

 void playWithServo(int pin)
{
 srv.attach(pin);
 srv.write(0);
 delay(1000);
 srv.write(180);
 delay(1000);
 srv.write(90);
 delay(1000);
 srv.detach();
} 

void delayStopped(int ms)
{
  digitalWrite(mpin00, 0);
  digitalWrite(mpin01, 0);
  digitalWrite(mpin10, 0);
  digitalWrite(mpin11, 0);
  delay(ms);
}
void StartMotor (int m1, int m2, int forward, int speed)
{

 if (speed==0) // oprire
 {
 digitalWrite(m1, 0); 
 digitalWrite(m2, 0);
 }
 else
 {
 if (forward)
 {
 digitalWrite(m2, 0);
 analogWrite (m1, speed); // folosire PWM
 }
 else
 {
 digitalWrite(m1, 0);
 analogWrite(m2, speed);
 }
 }
}

void inainte(int m1, int m2, int m3, int m4)
{
  ////
   StartMotor (m1, m2, 1, 100);
   StartMotor (m3, m4, 1, 100);
  
  delay(200);
  delayStopped(100);
}

void stanga(int m1, int m2, int m3, int m4)
{
    ////

  StartMotor (m1, m2, 1, 0); 
  StartMotor (m3, m4, 1, 100);
  
  delay(500);
  delayStopped(100);

}

void dreapta(int m1, int m2, int m3, int m4)
{
    ////
StartMotor (m1, m2, 1, 100);
StartMotor (m3, m4, 1, 0);
  delay(500);
  delayStopped(100);
  }

void oprire(int m1, int m2, int m3, int m4)
{
   StartMotor (m1, m2, 0, 0);
   StartMotor (m3, m4, 0, 0);
  delay(300);
  delayStopped(100);
}
void inapoi (int m1, int m2, int m3, int m4)
{
  ////
   StartMotor (m1, m2, 0, 100);
   StartMotor (m3, m4, 0, 100);
  
  delay(500);
  delayStopped(100);
}

void loop() {
 if (Serial.available()) {
 if (Serial.find("+IPD,")) {
 delay(500);
 int connectionId = Serial.read() - 48; // functia read() returneaza valori zecimale ASCII
 // si caracterul ‘0’ are codul ASCII 48
 String webpage = "<h1>ROBOT</h1><a href=\"/l0\"><button>INAINTE</button></a>";
 String cipSend = "AT+CIPSEND=";
 cipSend += connectionId;
 cipSend += ",";
 webpage += "<a href=\"/l1\"><button>STANGA</button></a>";
 webpage += "<a href=\"/l2\"><button>STOP</button></a>";
 webpage += "<a href=\"/l3\"><button>DREAPTA</button></a>";
 webpage += "<a href=\"/l4\"><button>INAPOI</button></a>";


 cipSend += webpage.length();
 cipSend += "\r\n";
 sendData(cipSend, 100, DEBUG);
 sendData(webpage, 150, DEBUG);

 String closeCommand = "AT+CIPCLOSE=";
 closeCommand += connectionId; //se adauga identificatorul conexiunii
 closeCommand += "\r\n";
 sendData(closeCommand, 300, DEBUG);
 }
  switch(buton)
 {
  case 1:
  inainte(mpin00, mpin01, mpin10, mpin11);
  buton = 0;
  break;
  case 2:
  stanga(mpin00, mpin01, mpin10, mpin11);
  buton = 0;
  break;
  case 3:
  oprire(mpin00, mpin01, mpin10, mpin11);
  buton = 0;
  break;
  case 4:
  dreapta(mpin00, mpin01, mpin10, mpin11);
  buton = 0;
  break;
  case 5:
  inapoi(mpin00, mpin01, mpin10, mpin11);
  buton = 0;
  break;
 }
 }
}
String sendData(String command, const int timeout, boolean debug) {
 String response = "";
 Serial.print(command); // trimite comanda la esp8266
 long int time = millis();
 while ((time + timeout) > millis()) {
 while (Serial.available()) {
 char c = Serial.read(); // citeste caracter urmator
 response += c;
 }
 }
 if (response.indexOf("/l0") != -1) {
  buton = 1;
    //functie merge in fata
 }
 if (response.indexOf("/l1") != -1) { 
    buton = 2;

    //functie merge in stanga
 }
  if (response.indexOf("/l2") != -1) {

  buton = 3;
    //functie stop
 }
  if (response.indexOf("/l3") != -1) {
  buton = 4;
    
    //functie merge in dreapta

 }
  if (response.indexOf("/l4") != -1) {
  buton = 5;

    //functie merge in spate
 }
 if (debug) {

 }
 return response;
}

