//add arduino libraries  
#include <ArduinoJson.h>
#include <Servo.h>
#include <SPI.h>
#include <Ethernet.h>

//Servo Int
int servoPin = 7; 
Servo servo;  
int servoAngle = 0;   // servo position in degrees 



//global varaibles 
char server[] = "api.coindesk.com"; //site to ping JSON string
float value = 0;

byte mac[] = { 0x, 0x, 0x, 0x, 0x, 0x };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin); // attach the servo to the pin 
  
// start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /v1/bpi/currentprice/USD.json HTTP/1.1");
    client.println("Host: api.coindesk.com");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}


void loop() {
 //acesses the client and stores the JSON string 
  if (client.available()) { //if the webpage can be reached
    char data_str = client.read(); //stores the JSON string
    Serial.print(data_str);
    value = JSONParse(data_str); //returns the current value of bitcoin 
  }
  else{
    Serial.println("Webpage Not Avaliable");
  }

//map value into servo range 
int pos = map(value, 0, 40000, 0, 180);  // as of setting this value, the middle of the range (20,000) is roughly the current value of bitcoin 

//move servo
servo.write(pos);  

delay(60000); //wait a minute
}

float JSONParse(char json_str) { //this parses the json file and returns the bitcoin value 
  
const size_t bufferSize = JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 410;
DynamicJsonBuffer jsonBuffer(bufferSize);

const char* json = json_str; //sets the json varaible to the newest version of the json string grabbed from the website 

JsonObject& root = jsonBuffer.parseObject(json);

JsonObject& time = root["time"];
const char* time_updated = time["updated"]; // "Dec 1, 2017 00:51:00 UTC"
const char* time_updatedISO = time["updatedISO"]; // "2017-12-01T00:51:00+00:00"
const char* time_updateduk = time["updateduk"]; // "Dec 1, 2017 at 00:51 GMT"

const char* disclaimer = root["disclaimer"]; // "This data was produced from the CoinDesk Bitcoin Price Index (USD)...."

JsonObject& bpi_USD = root["bpi"]["USD"];
const char* bpi_USD_code = bpi_USD["code"]; // "USD"
const char* bpi_USD_rate = bpi_USD["rate"]; // "9,878.1313"
const char* bpi_USD_description = bpi_USD["description"]; // "United States Dollar"
float bpi_USD_rate_float = bpi_USD["rate_float"]; // 9878.1313 <--- THIS IS THE VALUE WE WANT TO RETURN

return bpi_USD_rate_float; //returns desired value (current value of bitcoin)
}

void servoMoveF(int old_angle, int new_angle) { //moves the servo slowly to its new spot
    for(servoAngle = old_angle; servoAngle < new_angle; servoAngle++)  //move the micro servo from 0 degrees to 180 degrees
  {                                  
    servo.write(servoAngle);              
    delay(10);                  
  }

}

void servoMoveB(int old_angle, int new_angle) { //moves the servo slowly to its new spot
    for(servoAngle = old_angle; servoAngle < new_angle; servoAngle--)  //move the micro servo from 0 degrees to 180 degrees
  {                                  
    servo.write(servoAngle);              
    delay(10);                  
  }

}
