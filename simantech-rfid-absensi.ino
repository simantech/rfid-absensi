/*
 * 
 * All the resources for this project: https://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
char buff[100];

#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const String UID_KARTU_RFID = "15 90 03 46"; // Ini diganti sesuai dengan UID kartu RFID Anda
const String NAMA_USER = "Joshua"; // Ini adalah nama user yang memiliki akses kartu RFID
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  // Setup SPI and RFID Reader
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Dekatkan Kartu Anda ke Reader...");
  Serial.println();

  // Setup RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

}

String getCurrentTimeString()
{
    DateTime now = rtc.now();
    sprintf(buff, "%s %d/%d/%d %02d:%02d:%02d", daysOfTheWeek[now.dayOfTheWeek()],now.day(),now.month(),now.year(), now.hour(), now.minute(), now.second());
    return buff;
}

void loop() 
{
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
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == UID_KARTU_RFID) //main logic untuk menentukan akses
  {
    Serial.println("Authorized access");
    Serial.println();
    String finalMessage = "User dengan \nnama: " + NAMA_USER + "\nID kartu: " + UID_KARTU_RFID + "\nmelakukan absensi pada: " + getCurrentTimeString();
    Serial.println(finalMessage);
    delay(1000); // kasih delay 1 detik sebelum membaca data baru
  }
 
 else   {
    Serial.println(" Access denied");
    delay(1000); // kasih delay 1 detik sebelum membaca data baru
  }
  
  
} 
