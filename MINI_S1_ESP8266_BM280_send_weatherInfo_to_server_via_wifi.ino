#include <Wire.h>
#include <SPI.h>
#include <string.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


//#define SEALEVELPRESSURE_HPA (1013.25)
#define SEALEVELPRESSURE_HPA (1026)

//BME code
Adafruit_BME280 bme; // I2C
char temperature[7]={0};
char humidity[7]={0};
char pressure[9]={0};
char altitude[9]={0};

//WIFI code
int status1 = WL_IDLE_STATUS;
char ssid[] = "ssid_name"; //  your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)


int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 2390; 

IPAddress ipTarget(192, 168, 20, 100);
const uint16_t port = 6788;

char  sendBuffer[] = "xxxxxxxx,xxxxxxxx,xxxxxxxxxx,xxxxxxxxxx0";       // a string to send back
//int waitCount =0;

WiFiUDP Udp;

volatile boolean flagOneSec;
unsigned long countTick = 0;

// the setup function runs once when you press reset or power the board
void setup() {
    ESP.wdtDisable();
    ESP.wdtEnable(8000);
    
    Serial.begin(115200);    
    Serial.println(F("BME280 test"));
    ESP.wdtFeed();
    bool status;  
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        delay(100);
    }    
    Serial.println("-- Default Test --");    
    Serial.println();
    yield();
    delaySeconds(1); // let sensor boot up
    ESP.wdtFeed();

    // We start by connecting to a WiFi network
    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD) {           
      ESP.wdtFeed();
      Serial.println("WiFi shield not present");
    // don't continue:
      while (true);
    }else{
      Serial.println("WiFi shield OK");
    }

    WiFi.begin(ssid,pass);
    Serial.print("Attempting to connect to SSID: ");   
    //attempt to connect to Wifi network:
    while (status1 != WL_CONNECTED) {
      Serial.print(".");
      WiFi.begin(ssid,pass);
      delaySeconds(5);
      status1 = WiFi.status();
      ESP.wdtFeed();  
      Serial.print("Status: ");
      Serial.print(status);
    }
//    Serial.println("Connected to wifi");
//    printWifiStatus();
//    delaySeconds(1); 

    //timer1_enable(160,0,200);
//    timer1_enable(160,0,200);
//    timer1_attachInterrupt(writeTick);
//    timer1_isr_init();
}

// the loop function runs over and over again forever
void loop() {
    
    //yield();
    //Serial.println("loop");
    //printWifiStatus();
    
    //printValues();
    //delay(5000);
    formatDataDisplay();
    
    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(ipTarget, port);
    Udp.write(sendBuffer);
    Udp.endPacket();
    //delaySeconds(3);
    delaySeconds(300);
}

void writeTick(){
  countTick++;
  if(countTick == 100){
    Serial.println("4 SECONDS");
    countTick=0;
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    Serial.println();
}


void formatDataDisplay(){
float offset = 2.3;
float temp_offset;
  
  // example for dtostrf
  // |(+/-)|x|x|x|.|x|x| = width 7, precision 2
  //   1    2 3 4 5 6 7

  temp_offset =  bme.readTemperature() - offset; 
  dtostrf(temp_offset, 6, 2, temperature);
  
  temperature[strlen(temperature)]='\0';
//  Serial.print("temp converted char: ");
//  Serial.println(temperature);
//  countChar(temperature);
  
  
  dtostrf(bme.readHumidity(), 6, 2, humidity);
  humidity[strlen(humidity)]='\0';
//  Serial.print("humidity converted char: ");
//  Serial.println(humidity);
//  countChar(humidity);
  
  dtostrf(bme.readPressure()/ 100.0F,8, 2, pressure);
  pressure[strlen(pressure)]='\0';  
//  Serial.print("pressure converted char: ");
//  Serial.println(pressure);
//  countChar(pressure);
  

  dtostrf(bme.readAltitude(SEALEVELPRESSURE_HPA), 8, 2, altitude);
  altitude[strlen(altitude)]='\0';
//  Serial.print("altitude converted char: ");
//  Serial.println(altitude);
//  countChar(altitude);


  
  memset(sendBuffer,'\0',strlen(sendBuffer));
  strncpy(sendBuffer,temperature,9);
  strcat(sendBuffer,",");
  strcat(sendBuffer,humidity);
  strcat(sendBuffer,",");
  strcat(sendBuffer,pressure);
  strcat(sendBuffer,",");
  strcat(sendBuffer,altitude);  
  Serial.print("sendBuffer: ");
  Serial.println(sendBuffer);
 
}

void delaySeconds(int seconds) { 
  for (int i = 0; i <= seconds*10; i++){
    delay(100);
    ESP.wdtDisable();
    ESP.wdtEnable(0);
    //ESP.wdtFeed();
    //ESP.wdtRestart();
  }
}


void countChar(char * palavra){
  for (int i =0;i<16;i++){
    if(palavra[i]=='\0'){
      Serial.print("Last char: ");
      Serial.println(i);
      break;
    }else{
      Serial.print("dig");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(palavra[i]);
    }
  }
}

