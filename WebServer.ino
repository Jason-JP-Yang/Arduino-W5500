#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 3, 81);
IPAddress myDns(192, 168, 3, 1);
const int floatPrecision = 6;
const int doublePrecision = 6;

EthernetServer server(80);

// Testing Variables
float floatVar = 3.1415926535;
float floatArray[3] = {3.1415926, 0.142857, 0.285714};
double doubleVar = 3.1415926535;
double doubleArray[3] = {2.1415926, 1.142857, 3.285714};

bool boolVar = true;
bool boolArray[4] = {true, false, true, false};

char charVar = 65;
unsigned char uncharVar = 244;
byte byteVar = 0xDE;

int intVar = 89;
unsigned int unintVar = 55535;
long longVar = 21474836;
unsigned long unlongVar = 4294967275;
short shortVar = 2767;

String strVar = "Hello_World!";

void setup() {
  Serial.begin(38400);
  while (!Serial) { delay(100); }

  Ethernet.begin(mac, ip, myDns);
  delay(1000);

  server.begin();
  Serial.print("Ethernet Server Started: ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.print("==========\nnew client: ");
    Serial.println(client.remoteIP());
    
    bool currentLineIsBlank = true;
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (c == '\n' && currentLineIsBlank) {
          Serial.print(request);
          processRequest(request, client);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    Serial.println("Responsed Finished (Wait Client Stop Connection)\n==========");
  }
}

String convertVarToString(float value) {
  return String(value, floatPrecision);
}
String convertVarToString(double value) {
  return String(value, doublePrecision);
}
String convertVarToString(bool value) {
  return value ? "true" : "false";
}
template <typename T>
String convertVarToString(T value) {
  return String(value);
}
template <typename T>
void ConvertArrayToString(T* array, int array_length, EthernetClient& client) {
  for (int i = 0; i < array_length; i++) {
    client.print("Array-Element_");
    client.print(i);
    client.print(": ");
    client.println(convertVarToString(boolArray[i]));
  }
}
void processRequest(String request, EthernetClient& client) {
  if (request.indexOf("GET /floatVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=float");
    client.println("Variable-ID: floatVar");
    client.print("Value: ");
    client.println(convertVarToString(floatVar));

  } else if (request.indexOf("GET /doubleVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=double");
    client.println("Variable-ID: doubleVar");
    client.print("Value: ");
    client.println(convertVarToString(doubleVar));

  } else if (request.indexOf("GET /boolVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=bool");
    client.println("Variable-ID: boolVar");
    client.print("Value: ");
    client.println(convertVarToString(boolVar));
    
  } else if (request.indexOf("GET /charVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=char");
    client.println("Variable-ID: charVar");
    client.print("Value: ");
    client.println(convertVarToString(charVar));
  
  } else if (request.indexOf("GET /uncharVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=unsigned_char");
    client.println("Variable-ID: uncharVar");
    client.print("Value: ");
    client.println(convertVarToString(uncharVar));
  
  } else if (request.indexOf("GET /byteVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=byte");
    client.println("Variable-ID: byteVar");
    client.print("Value: ");
    client.println(convertVarToString(byteVar));
  
  } else if (request.indexOf("GET /intVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=int");
    client.println("Variable-ID: intVar");
    client.print("Value: ");
    client.println(convertVarToString(intVar));
  
  } else if (request.indexOf("GET /unintVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=unsigned_int");
    client.println("Variable-ID: unintVar");
    client.print("Value: ");
    client.println(convertVarToString(unintVar));
  
  } else if (request.indexOf("GET /longVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=long");
    client.println("Variable-ID: longVar");
    client.print("Value: ");
    client.println(convertVarToString(longVar));
  
  } else if (request.indexOf("GET /unlongVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=unsigned_long");
    client.println("Variable-ID: unlongVar");
    client.print("Value: ");
    client.println(convertVarToString(unlongVar));
  
  } else if (request.indexOf("GET /shortVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=short");
    client.println("Variable-ID: shortVar");
    client.print("Value: ");
    client.println(convertVarToString(shortVar));
  
  } else if (request.indexOf("GET /strVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=string");
    client.println("Variable-ID: strVar");
    client.print("Value: ");
    client.println(convertVarToString(strVar));
  
  } else if (request.indexOf("GET /boolArray\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=array_bool");
    client.println("Variable-ID: boolArray");
    ConvertArrayToString(boolArray, sizeof(boolArray) / sizeof(boolArray[0]), client);

  } else {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain");
    client.println("Path not found");
  }
  client.println("Connection: close\n");
}
