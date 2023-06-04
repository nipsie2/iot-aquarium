#include <DFRobot_PH.h>
#include <OneWire.h> //Library OneWire
#include <DallasTemperature.h> // Library untuk Dallas temperature
#include "CTBot.h" //Library CTBot untuk Telegram
  
#define ONE_WIRE_BUS 5 // pin D4 digunakan untuk data hasil baca sensor 
  
int LAMPU = 4; 
int POMPA = 2;
int sensorPH = 35;//pembacaan sensor ph menggunakan ADC
float tegangan,nilaiph,suhu;
String telemsg;

DFRobot_PH ph;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
CTBot myBot;

String ssid = "nipsie2";    //nama ssid wifi kalian
String pass = "HanIf411!";  //password wifi kalian
String token = "6027651641:AAFzllx-YoSG5LvHThoq1TD7ENFg1zBzJzA";    //token bot baru kalian
const int id = 1371583309;  //id telegram kalian

void setup(void)
{
    pinMode(POMPA, OUTPUT);
    pinMode(LAMPU, OUTPUT);
    pinMode(sensorPH, INPUT);
    digitalWrite(LAMPU, HIGH);
    digitalWrite(POMPA, LOW);
    Serial.begin(9600);
    ph.begin(); //mulai untuk pembacaan sensor ph meter
    sensors.begin(); //mulai untuk pembacaan sensor ds18b20
    myBot.wifiConnect(ssid, pass); //koneksi dengan hotspot
    myBot.setTelegramToken(token); //memastikan token
    //tes koneksi dengan hotspot
    if (myBot.testConnection()) {
      myBot.sendMessage(id, "Koneksi Bagus");
    } else {
      Serial.println("Koneksi Jelek");
    }
}
  
void loop(void)
{
  sensors.requestTemperatures(); 
  static unsigned long timepoint = millis();
  if(millis()-timepoint>10000U){  //time interval: 10s
    timepoint = millis();
    tegangan = analogRead(sensorPH)/1024.0*5000; //mengubah tegangan analog menjadi digital dan menjadi tegangan
    suhu = sensors.getTempCByIndex(00);
    nilaiph = ph.readPH(tegangan,suhu);  //konversi tegangan menjadi ph meter dengan kompensasi suhu
    Serial.print("Suhu Air: ");
    Serial.print(suhu,1); //nilai pembacaan suhu
    Serial.print("^C  pH:");
    Serial.println(nilaiph,2); //nilai pembacaan ph meter
    if (suhu < 26){
      myBot.sendMessage(id, "Suhu Air Rendah!");
    }
    else if (suhu > 30){
      myBot.sendMessage(id, "Suhu Air Tinggi!");
    }
    else {
      myBot.sendMessage(id, "Suhu Air Normal");
    }
    if (nilaiph < 6) {
      myBot.sendMessage(id, "pH Air Rendah!");
    }
    else if (nilaiph > 8) {
      myBot.sendMessage(id, "pH Air Tinggi!");
    }
    else {
      myBot.sendMessage(id, "pH Air Normal");
    }
  }
  ph.calibration(tegangan,suhu); 
  
  TBMessage msg; 
  if (myBot.getNewMessage(msg)) {
    // there is a valid message in msg
    Serial.print("Received message from: ");
    Serial.println(msg.sender.username);
    if (msg.text.equalsIgnoreCase("POMPA ON")) {
      digitalWrite(POMPA,HIGH);
      myBot.sendMessage(id, "Pompa dinyalakan");
    }
    else if (msg.text.equalsIgnoreCase("POMPA OFF")) {
      digitalWrite(POMPA,LOW);
      myBot.sendMessage(id, "Pompa dimatikan");
      }
    else if (msg.text.equalsIgnoreCase("LAMPU ON")) {
      digitalWrite(LAMPU,HIGH);
      myBot.sendMessage(id, "Lampu dinyalakan");
      }
    else if (msg.text.equalsIgnoreCase("LAMPU OFF")) {
      digitalWrite(LAMPU,LOW);
      myBot.sendMessage(id, "Lampu dimatikan");
      }
  }
}
