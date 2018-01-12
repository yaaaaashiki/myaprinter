#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x3F, 0xF4 };
byte ip[] = {133, 2, 208, 174};
byte gateway[] = { 133, 2, 208, 254};
byte subnetmask[] = {255, 255, 255, 128};

byte HOST[] = { 133, 2, 208, 182 };
int PORT = 5000;
String hoststr = "133.2.208.182";
String END_POINT = "/ReceivePost.php";

EthernetClient client;

int val = 0; // value from light sensor
int vals[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // values for calculating average.
int ave = 0; // average 
int flag = 0; // Put on: 1, Put off: 0. initialized as 0
String objname = "\"object\""; // object name.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(500);
  while(!Serial){
    ;
  }
  delay(1000);

  Ethernet.begin(mac, ip, gateway, gateway, subnetmask);
  delay(3000);
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Ready...");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(0);
  ave = update_average(val);

  Serial.print("val= ");
  Serial.println(val);
  Serial.print("ave= ");
  Serial.println(ave);

  if(vals[9] != 0){
    flag = Calc(val);
    
    if(!client.available()){
      post(flag);
    }
    
    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      client.stop();
    }
  }
  delay(5000);

}

bool post(int val) {
    Serial.println("connecting...");
    if (client.connect(HOST, PORT)) {
        Serial.println("connected");
        String json_data = "{\"value\":" + String(val) + ", \"name\":" + objname + "}";
 
        client.println("POST " + END_POINT + " HTTP/1.1");
        client.print("Host: ");
        client.println(hoststr);
        client.println("Content-Type: application/json");
        client.println("User-Agent: Arduino Post Client");
                
        client.print("Content-Length: ");
        client.println(json_data.length());
        
        //client.println("Connection: close");
        client.println();
        client.print(json_data);
        
        Serial.println(json_data);
        Serial.println("sent");
        delay(500);
        client.stop();
        
        return true;
    }
    return false;
}

int Calc(int val){
  if(abs(val - ave) >= 50){
    if(val > ave){
      return 0; //Put off object.
    }else{
      return 1; //Put on object.
    }
  }

  return val;
}

int update_average(int val){
  int total = 0;
  for (int i=9;i>0;i--){
    vals[i] = vals[i-1];
    total += vals[i];
  }
  
  total += val;
  vals[0] = val;

  return total / 10;
}
