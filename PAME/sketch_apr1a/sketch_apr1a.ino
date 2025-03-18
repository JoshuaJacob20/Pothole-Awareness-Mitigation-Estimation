#include <TinyGPSPlus.h>
#include <WiFi.h>
WiFiClient client;   //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "ASIANET ABHI";     // replace with your wifi ssid and wpa2 key
const char *pass =  "abhijith003";
const char* host = "api.thingspeak.com";
const char* streamId   = "WKS3QZRPCRHT9Q9H";

static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
String loc="";
// The TinyGPSPlus object
TinyGPSPlus gps;
const int trigPin = 2;
const int echoPin = 4;
int pushButton = 5;
// defines variables
long duration;
int distance;
void setup()
{
  Serial.begin(115200);
   WiFi.mode(WIFI_STA);
  Serial2.begin(9600,SERIAL_8N1,RXPin,TXPin);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
  pinMode(pushButton, INPUT_PULLUP);
  Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
            
     }
      Serial.println("");
      Serial.println("WiFi connected");
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();
      

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  ultra();
  

  int buttonState = digitalRead(pushButton);
  if(buttonState==0)
  {
    Serial.println(loc);
    Serial.println(distance);
 sendvaluetoserver();

delay(10000);    
  }

}

void displayInfo()
{ 
  if (gps.location.isValid())
  {
    loc="lat:"+ String(gps.location.lat(), 6)+","+"long:"+String(gps.location.lng(), 6);
    

  }
  else
  {
    Serial.print(F("INVALID"));
  }
  
}
void ultra()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
}
void sendvaluetoserver()
{
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  // We now create a URI for the request
  String url = "/update?api_key=";
  url += streamId;
  url += "&field1=";
  url += loc;
  url += "&field2=";
  url += distance;


  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  delay(5000);

}