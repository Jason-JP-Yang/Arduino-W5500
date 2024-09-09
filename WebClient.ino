#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

IPAddress server(192, 168, 3, 81);
IPAddress ip(192, 168, 3, 83);
IPAddress myDns(192, 168, 3, 1);
EthernetClient client;
int floatPrecision = 6;
int doublePrecision = 6;

bool currentLineIsBlank = true;
String response = "";

void setup() {
  Serial.begin(38400);
  while (!Serial) { delay(100); }

  Ethernet.begin(mac, ip, myDns);
  delay(1000);
  Serial.print("Ethernet Server Initialized: ");
  Serial.println(Ethernet.localIP());

  sendRequest();
}

void sendRequest() {
  if (client.connect(server, 80)) {
    Serial.print("==========\nconnected to ");
    Serial.println(client.remoteIP());
    Serial.println("Sending Request .....");
    // Make a HTTP request:
    client.println("GET /boolArray");
    client.print("Host: ");
    client.print(Ethernet.localIP());
    client.println(" (Arduino UNO R3)");
    client.println("Connection: close\n");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("=== connection failed ===");
  }
}

void loop() {
  if (client.available()) {
    char c = client.read();
    response += c;

    if (c == '\n' && currentLineIsBlank) {
      client.stop();
      Serial.print(response);
      
      String contentType = parseString(response, "Content-Type: ");
      String VarID = parseString(response, "Variable-ID: ");
      String value = parseString(response, "Value: ");
      if (contentType == "text/plain=float") {
        if (VarID == "floatVar") {
          float output = convertStrFloat(value);
          Serial.println("Prased Result: ");
          Serial.println(ConvertToString(output));
        }

      } else if (contentType == "text/plain=double") {
        if (VarID == "doubleVar") {
          double output = convertStrDouble(value);
          Serial.println("Prased Result: ");
          Serial.println(ConvertToString(output));
        }

      } else if (contentType == "text/plain=array_double") {
        if (VarID == "doubleArray") {
          double output[3];
          parseArray(response, output, 3, contentType);
          Serial.println("Prased Result: ");
          Serial.println(ConvertToString(output[0]));
        }
      }
      
      Serial.println("End Connection with Server\n==========");
      response = "";
      delay(1000);
      sendRequest();
    }

    if (c == '\n') {
      currentLineIsBlank = true;
    } else if (c != '\r') {
      currentLineIsBlank = false;
    }
  }
}

String parseString(String response, String keyword) {
  int index = response.indexOf(keyword);
  if (index >= 0) {
    index += keyword.length();
    int endIndex = response.indexOf('\n', index);
    if (endIndex < 0) endIndex = response.length();
    String contentType = response.substring(index, endIndex);
    contentType.trim();
    return contentType;
  }
  return "";
}

template <typename T>
void parseArray(String str, T* array, int array_length, String array_type) {
  for (int i = 0; i < array_length; i++) {
    String value = parseString(response, "Array-Element_" + String(i) + ": ");
    if (array_type == "text/plain=array_float") {
      array[i] = convertStrFloat(value);
    } else if (array_type == "text/plain=array_double") {
      array[i] = convertStrDouble(value);
    }

    
  }
}

String ConvertToString(float value) {
  return String(value, floatPrecision);
}
String ConvertToString(double value) {
  return String(value, doublePrecision);
}
String ConvertToString(bool value) {
  return value ? "true" : "false";
}
template <typename T>
String ConvertToString(T value) {
  return String(value);
}

float convertStrFloat(String value) {
  return value.toFloat();
}
double convertStrDouble(String value) {
  return value.toDouble();
}
bool convertStrBool(String value) {
  return (value == "true");
}
char convertStrChar(String value) {
  return value.charAt(0);
}
unsigned char convertStrUnsignedChar(String value) {
  return (unsigned char)value.toInt();
}
byte convertStrByte(String value) {
  return (byte)value.toInt();
}
int convertStrInt(String value) {
  return value.toInt();
}
unsigned int convertStrUnsignedInt(String value) {
  return (unsigned int)value.toInt();
}
long convertStrLong(String value) {
  return value.toInt();
}
unsigned long convertStrUnsignedLong(String value) {
  return value.toInt();
}
short convertStrShort(String value) {
  return (short)value.toInt();
}