#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

/****************************************************************************
               BSM313 Nesnelerin İnterneti ve  Uygulamaları  Dersi
               Akıllı Çöp  Uygulaması - Özge Çinko
*****************************************************************************/

/*  Kablosuz  Bağlantı  Bilgileri */
const char* WLAN_SSID = "";
const char* WLAN_PASSWORD = "";

/*  ThingSpeak  Kurulumu */
const char* thingSpeakHost = "api.thingspeak.com";
unsigned long Channel_ID = 0 ; // Channel ID
const char * WriteAPIKey = ""; // Write API Key
const int field_no = 1; // Veriyi yazacağımız field numarası

const int totalHeight = 21; // Çöp kutusunun CM cinsinden uzunluğu
const int holdHeight = 18; // CM cinsinden çöpün dolabileceği uzunluk
int capacity = 0;

/* Değişken ve Pin Tanımlamaları */
const int trigPin = 16; // D0
const int echoPin = 5; // D1

long duration;
int distance;
int result;
int garbageLevel = 0;


WiFiClient client;

/* WiFi Bağlantısı */
void WiFi_Setup()
{
  if (WiFi.status() != WL_CONNECTED)
  {
      Serial.print("Kablosuz  Agina Baglaniyor");
      WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
      //  WiFi  durum kontrolü
      while (WiFi.status()  !=  WL_CONNECTED) {
          delay(1000);
          Serial.print(".");
      }
      Serial.println();
      Serial.print(WLAN_SSID);  
      Serial.println(" Kablosuz  Aga Baglandi");
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  capacity = totalHeight - holdHeight;
  delay(1000);
}

void loop()
{
  WiFi_Setup();
  delay(100);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034;
  result = distance / 2;

  garbageLevel = map(result, capacity, totalHeight, 100, 0);

  const unsigned long oneDay = 24UL*3600*1000;
  if (garbageLevel > 100) 
  {
    garbageLevel = 100;
    Serial.println("Uyarı: Çöp doldu, 24 saat içerisinde boşaltılmalı. ");
    for(int i=0;i<1;i++)
    {
    Serial.println("-------------------------");
    Serial.println("Çöpü boşaltınız....");
    delay(oneDay);
    Serial.print(i);
    Serial.println(" gün geçti.");
    }
  }
  
  else if (garbageLevel < 0)
  {
    garbageLevel = 0;
  }
  
  Serial.print("Çöp Seviyesi: ");
  Serial.print(garbageLevel);
  Serial.println("%");
  
  uploadData();

  for (int i = 1; i < 2 ; i++)
  {
    Serial.println("-------------------------");
    Serial.println("Sistem beklemede....");
    delay(60000);
    Serial.print(i);
    Serial.println(" dakika geçti.");

  }
}

void uploadData()
{
  WiFi_Setup();

  int data;
  data = ThingSpeak.writeField(Channel_ID, field_no, garbageLevel, WriteAPIKey);
  if (data == 200)Serial.println("Veri güncellendi.");
  else
  {
    Serial.println("Veri güncelleme başarısız, tekrar deneniyor..");
    delay(20000);
    uploadData();
  }
}
