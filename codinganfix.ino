
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_BIRU 5 //akses diterima LED BIRU pin
#define LED_HIJAU 3 //akses ditolak LED BIRU pin
#define BUZZER 4 //buzzer pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myServo; //define servo name

String AP = "patt I-3B";       // WIFI NAME
String PASS = "21212121"; // WIFI PASSWORD
String IP = "192.168.43.62";
String Port = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

SoftwareSerial mySerial(6, 7); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(LED_BIRU, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  myServo.attach(2); //servo pin
  myServo.write(0); //servo start position
  Serial.println("Put your card to the reader...");
  Serial.println();
}

void loop() { // run over and over
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  String data = content.substring(0);
  String getdata = "GET /F1/prosesdata.php?uid="+data;
 
  if (content.substring(0) == "DA038A19" || content.substring(0) == "046E1CAA705B80") //change here the UID of the card/cards that you want to give access
  {
    sendCommand("AT",5,"OK");
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ IP +"\","+ Port,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getdata.length()+4),4,">");
    mySerial.println("GET /F1/prosesdata.php?uid="+data);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
    Serial.println("akses kartu diterima");
    Serial.println();
    delay(500);
    digitalWrite(LED_BIRU, HIGH);
    tone(BUZZER, 500);
    delay(300);
    noTone(BUZZER);
    myServo.write(60);
    delay(5000);
    myServo.write(0);
    digitalWrite(LED_BIRU, LOW);
  }
  else   {
    Serial.println("akses kartu ditolak");
    digitalWrite(LED_HIJAU, HIGH);
    tone(BUZZER, 300);
    delay(1000);
    digitalWrite(LED_HIJAU, LOW);
    noTone(BUZZER);
  }

  delay(1000);

}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    mySerial.println(command);//at+cipsend
    if(mySerial.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
