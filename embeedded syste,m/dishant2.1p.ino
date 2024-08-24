#include "DHT.h"
#include "WiFiNINA.h"
#include "secrets.h"
#include "ThingSpeak.h" // Include ThingSpeak after other headers

// DHT sensor setup
#define DHT_PIN 2     // Pin connected to the DHT sensor
#define DHT_TYPE DHT22   // DHT 22 (AM2302), AM2321
DHT dht(DHT_PIN, DHT_TYPE);

// WiFi and ThingSpeak setup
char ssid[] = SECRET_SSID;   // Network SSID
char pass[] = SECRET_PASS;   // Network password
WiFiClient client;
unsigned long channelNumber = SECRET_CH_ID;
const char *writeAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(115200);      // Start serial communication
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  dht.begin();  // Start the DHT sensor
  ThingSpeak.begin(client);  // Start ThingSpeak
}

void loop() 
{
  // Ensure WiFi connection
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
     {
      WiFi.begin(ssid, pass);  
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected to WiFi.");
  }

  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // Celsius

  // Check for read errors
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Error reading from DHT sensor!");
    return;
  }

  // Display sensor data
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Update ThingSpeak fields
  ThingSpeak.setField(1, temperature);  // Temperature in Celsius
  ThingSpeak.setField(2, humidity);  // Humidity

  // Determine status message
  String statusMessage;
  if (temperature > 30) 
  {
    statusMessage = "It's hot!";
  } 
  else if (temperature < 15) 
  {
    statusMessage = "It's cold!";
  } 
  else 
  {
    statusMessage = "Temperature is moderate.";
  }

  
  ThingSpeak.setStatus(statusMessage);

  
  int response = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  if (response == 200) 
  {
    Serial.println("Channel update successful.");
  } 
  else 
  {
    Serial.println("Error updating channel. HTTP code: " + String(response));
  }

  delay(60000); // Wait 60 seconds before next update
}