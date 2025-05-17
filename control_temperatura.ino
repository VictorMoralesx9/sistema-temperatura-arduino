#include <DHT.h>
#include <ESP8266WiFi.h>

// Configuración
#define DHTPIN 2          // Pin del sensor DHT11
#define RELAY_PIN 3       // Pin del relay (ventilador)
#define DHTTYPE DHT11     // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

// Variables
float tempThreshold = 28.0; // Temperatura para activar ventilador
const char* ssid = "TuRedWiFi";
const char* password = "TuContraseña";

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  
  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
}

void loop() {
  float temp = dht.readTemperature();
  
  // Control automático
  if (temp > tempThreshold) {
    digitalWrite(RELAY_PIN, HIGH); // Ventilador ON
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Ventilador OFF
  }

  // Servidor web
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    
    // Interfaz web básica
    client.println("<html><body>");
    client.println("<h1>Sistema de Temperatura</h1>");
    client.print("<p>Temperatura actual: ");
    client.print(temp);
    client.println("°C</p>");
    client.println("<a href='/on'><button>Encender Ventilador</button></a>");
    client.println("<a href='/off'><button>Apagar Ventilador</button></a>");
    client.println("</body></html>");

    // Manejo de botones
    if (request.indexOf("/on") != -1) {
      digitalWrite(RELAY_PIN, HIGH);
    } else if (request.indexOf("/off") != -1) {
      digitalWrite(RELAY_PIN, LOW);
    }
  }
  delay(2000);
}