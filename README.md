![](/READEME_FOLDER/Arduino%20W5500-zerobg.png)
# Usage of BASE CODE of Server-Client Mode (TCP/IP Connection) between Two Arduino UNO R3 with W5500 shield
## Physical Connection Graph
*Remarks: Because W5500 doesn't have ability of **AUTO-MDIX** so We should use crossover wire to connect or connect to a switch or router.* 
*Reference links: (MDI vs MDIX And Auto MDI/MDIX Basics)[https://community.fs.com/article/mdi-vs-mdix-and-auto-mdimdix-basis.html]*
> MDI/MDIX are types of Ethernet interface (both physical and electrical/optical) in a computer network used to carry transmission. **For connecting two MDI device we need to use a crossover wire**

``` 
Power Supply → Arduino UNO R3 ⇆ W5500 shield 
                                    ⇵
                               Switch / Router 
                                    ⇵
Power Supply → Arduino UNO R3 ⇆ W5500 shield
```
## Usage of Server Code
One Arduino UNO R3 is act as a server wait for client request of information. </br>
For sending data through Ethernet, only can send string, thus, we need to convert different type of data or variables to string before sending. </br>
***Flow Chart of the Server***
``` flowchart
- Initialize Itself and act as a server: 
Serial Begin → Ethernet Begin → Wait for client request

- Loop for waiting client request:
Detecting Client State → Client available → 
Client send the request message (include GET message and Host) → 
Wait until a double blank line (\r\n\n) sended by client mark as request finished →
Process the request message → Convert the data in string → 
Send to the client with double blank line at the end
→ Stop connection
```

### Initialize Itself and act as a server
``` C++
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 3, 81);
IPAddress myDns(192, 168, 3, 1);

EthernetServer server(80);
```
Set the mac adress randomly is ok. **Should NOT be the same with the client or any other device connected to the switch**

Set the ip adress with the correspond DNS adress of switch. **IP Adress should NOT be the same with the devices connected to the switch or switch itself**

Finally Set the server port at 80 (HTTP PORT in defeault)
``` C++
void setup() {
  Serial.begin(38400);
  while (!Serial) { delay(100); }

  Ethernet.begin(mac, ip, myDns);
  delay(1000);
  server.begin();

  Serial.print("Ethernet Server Started: ");
  Serial.println(Ethernet.localIP());
}
```
Initialize the Serial Monitor: ***Disable `while (!Serial) { delay(100); }` after DEBUG***

Initialize the Ethernet with mac, ip, myDNS: ***1 second wait for itself initialization** </br>
- YOU CAN ALSO USE FOLLOWING CODE FOR "DHCP" Connection. that the Switch will automatically attribute a ip adress to the server.
    ``` C++
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // Check for Ethernet hardware present
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry,   can't run without hardware. :(");
        while (true) {
          delay(1); // do nothing, no point running without   Ethernet hardware
        }
      }
      if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
      }
      // try to congifure using IP address instead of DHCP:
      Ethernet.begin(mac, ip, myDns);
    } else {
      Serial.print("  DHCP assigned IP ");
      Serial.println(Ethernet.localIP());
    }
    ```

### Getting Client Request Message
``` C++
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
```
When `(EthernetClient client  = server.available()) == True` means there is a new client that available for communication. 

After connect to the client, code run in the whileLoop.
It will continuely reading the request message send from the client using `char c = client.read();` and write into variable. 

``` C++
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
```
After receive double blank line represent request finished, code jump into function `processRequest(request, client);`

### Processing the Request
- ***There is some base function need to defined before processing the request about convert different type of data into string***
    - Following is the code use to convert data to string
    - Accept these types: FLOAT, DOUBLE, BOOL, CHAR, UNSIGNED CHAR, LONG, UNSIGNED LONG, SHORT, STRING
    ``` C++
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
    ```
    - Following is the code use to convert array to string
    ``` C++
    template <typename T>
    void ConvertArrayToString(T* array, int array_length,   EthernetClient& client) {
      for (int i = 0; i < array_length; i++) {
        client.print("Array-Element_");
        client.print(i);
        client.print(": ");
        client.println(convertVarToString(boolArray[i]));
      }
    }
    ```

``` C++
void processRequest(String request, EthernetClient& client) {
  if (request.indexOf("GET /floatVar\r\n") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain=float");
    client.println("Variable-ID: floatVar");
    client.print("Value: ");
    client.println(convertVarToString(floatVar));
  
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
``` 
Here is the Example Code for responsing the request in corresponding GET MESSAGE. </br>
A complete response should include HTTP header, Content Type, information, finally connection close. </br>
Following is the response will send to the client.
``` plain
HTTP/1.1 200 OK
Content-Type: text/plain=float
Value: 3.141593
Connection: close

```
``` plain
HTTP/1.1 200 OK
Content-Type: text/plain=array_float
Array-Element_1: 0.142857
Array-Element_2: 0.285714
Array-Element_3: 0.428571
Connection: close

```