#include "GPRS.h" // You have to add your APN here!
#include "MQTT.h" // You have to add your MQTT broker info here!
#include <ArduinoJson.h>
#define JSON_BUFFER_SIZE       400     // Calculate the right number using:
                                       // https://arduinojson.org/v5/assistant/
#define SERIAL_MON_BAUD_RATE   9600  // 115.2K baud serial connection to computer
#define DATA_PUB_FREQUENCY     30000L  // ms -> 30 seconds
int id = 0 ;

StaticJsonDocument<JSON_BUFFER_SIZE> DATA; // Json file that'll contain all data

unsigned long last_time_published = 0;
int packet_id = 0; // Variable used for demo purposes

float  lat ; 
float lon ; 
float accuracy;


void communicate_(){

  MQTT_setup();

  // Container for the data to be sent
  char mqtt_buffer[JSON_BUFFER_SIZE]; 
  
  // Prepare the Json file for sending
  serializeJson(DATA, mqtt_buffer);

  // I did have some issues with the broker timing-out if I don't connect to it just before sending
  MQTT_connect();
  
  // Consult MQTT.h for send_data()
  send_data(mqtt_buffer);

  Serial.print("Published: ");
  Serial.println(mqtt_buffer);
}

void setup() {
  Serial.begin(SERIAL_MON_BAUD_RATE);
  // This function call sets up the module but in a blocking matter!
  
  Serial.print(F("Connecting to "));
  Serial.print(APN);
  GPRS_wake_up();
  if (modem.isNetworkConnected())
  {
     Serial.println("network connect");
     Serial.println("IP Adress : ");
     Serial.print(modem.localIP());
  }
  else Serial.println("not connected to the network ");
  

  if (GPRS_connectivity_status()){ Serial.println("GPRS connected");}
  
  
  
}
void getLocation(){
      const char* Location=  modem.getGsmLocation().c_str();
    Serial.println(Location);
    sscanf(Location,"%f,%f,%f",&lat,&lon,&accuracy); 
    Serial.println(lon);
    Serial.println(lat);
    Serial.println(accuracy);
}

void loop() {
  // Send data each DATA_PUB_FREQUENCY (ms)
  if(millis() - last_time_published >= DATA_PUB_FREQUENCY){
    getLocation();
    DATA["location"]["lon"] = lat;
    DATA["location"]["lat"] = lon;
    DATA["location"]["accuracy"] = accuracy;
    communicate_();
    last_time_published = millis();
  }
}



