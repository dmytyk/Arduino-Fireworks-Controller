/*
  WiFi Web Server Based Fireworks Controller

 A simple web server that lets fire off fireworks.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 This code will set off 10 rows of fireworks, one at a time as directed by the user.

 Circuit:
 * Board with NINA module (Arduino MKR WiFi 1010)

 created 1/21/2021
 by David Mytyk
*/

#include <SPI.h>
#include <WiFiNINA.h>

#include "network_parameters.h"
// enter your sensitive data in the Secret tab/arduino_secrets.h
char myhomessid[] = MYHOME_SSID;        // your network SSID (network name)
char myhomepassword[] = MYHOME_PASSWORD;    // your network password (network password)

int currentrow = 0;               // current row selected to fire
int selectrow = 0;                // row selected to fire
String state = "INIT";            // user state

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  // initialize digital pins.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // LED off to indicate no wifi connection yet
  pinMode(15, OUTPUT);    // set the pin mode, Row 1
  pinMode(16, OUTPUT);    // set the pin mode, Row 2
  pinMode(17, OUTPUT);    // set the pin mode, Row 3
  pinMode(18, OUTPUT);    // set the pin mode, Row 4
  pinMode(19, OUTPUT);    // set the pin mode, Row 5
  pinMode(20, OUTPUT);    // set the pin mode, Row 6
  pinMode(21, OUTPUT);    // set the pin mode, Row 7
  pinMode(0, OUTPUT);     // set the pin mode, Row 8
  pinMode(1, OUTPUT);     // set the pin mode, Row 9
  pinMode(2, OUTPUT);     // set the pin mode, Row 10
  pinMode(5, OUTPUT);     // set the pin mode, ARMED
  pinMode(7, OUTPUT);     // set the pin mode, FIRE
  allOff();  
   
  Serial.begin(9600);      // initialize serial communication
 
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to Network named: ");
    Serial.println(myhomessid);                   // print the network name (SSID);

    // force a static IP address
    WiFi.config(IPAddress(MYHOME_IP_OCT1, MYHOME_IP_OCT2, MYHOME_IP_OCT3, MYHOME_IP_OCT4));

    // Connect to WPA/WP17 network. Change this line if using open or WEP network:
    status = WiFi.begin(myhomessid, myhomepassword);

    // wait 10 seconds for connection flashing on board LED
    tryingWifiStatus();
  }
  
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status

  // LED on to indicate we have a wifi connection
  digitalWrite(LED_BUILTIN, HIGH);
}

void tryingWifiStatus() {
    //delay 5 seconds
    for (int i = 0; i <= 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
  }  
}

void allOff() {
  digitalWrite(15, LOW);  // All off
  digitalWrite(16, LOW);  
  digitalWrite(17, LOW);  
  digitalWrite(18, LOW);  
  digitalWrite(19, LOW);  
  digitalWrite(20, LOW);  
  digitalWrite(21, LOW);  
  digitalWrite(0, LOW);  
  digitalWrite(1, LOW);  
  digitalWrite(2, LOW);   
  digitalWrite(5, LOW);  
  digitalWrite(7, LOW);
}

void plusePin(int pin) {
  digitalWrite(7, HIGH);
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(7, LOW); 
  digitalWrite(pin, LOW); 
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("MYHOMESSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  printMacAddress(mac);
}

void writeResponse(WiFiClient client) {
  // send a standard http response header
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection:close");
  client.println();

  // send the web page (user interface)
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<style>");
  client.println("table {border: 1px solid black;}");  
  client.println("</style>");

  client.println("<table style=\"width:50%\"");
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
    
  client.println("<tr>");
  client.println("<td style=\"text-align:center; font-size:125%;\">System Ready!</td>");
  client.println("</tr>");
    
  client.println("<tr>");      
  client.println("<td style=\"text-align:center; font-size:125%;\">Select a Row to Fire (01-10),  Arm the Row,  Fire the Row:</td>");
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td colspan=\"2\" style=\"text-align:center; font-size:125%;\">Current State:&nbsp;");     
  client.print(state);
  client.println("</td>");  
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td colspan=\"2\" style=\"text-align:center; font-size:125%;\">Current Row Selected:&nbsp;");     
  client.print(currentrow);
  client.println("</td>");  
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\"><form method=\"POST\"></td>");   
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td colspan=\"2\" style=\"text-align:center; font-size:125%;\">Select Row:&nbsp;<input type=\"text\" name=\"row\" size=\"2\" maxlength=\"2\" />");        
  client.println("</tr>");
    
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");  
      
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");  
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\"><input type=\"submit\" name=\"action\" value=\"SETROW\" /></td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\"><input type=\"submit\" name=\"action\" value=\"ARM\" /></td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%; color:\"><input type=\"submit\" name=\"action\" value=\"FIRE\" /></td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\"><input type=\"submit\" name=\"action\" value=\"RESET\" /></td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\"></form></td>");   
  client.println("</tr>");
  
  client.println("<tr>");   
  client.println("<td style=\"text-align:center; font-size:125%;\">&nbsp;</td>");   
  client.println("</tr>");
  client.println("</table>");
  
  client.println("</html>");
}

void loop() {
  // listen for incoming clients
  // i.e. waiting for someone to connect to our static IP Address as define above
  WiFiClient client = server.available();
  if (client) {
    Serial.print("New client Started at IP [");
    Serial.print(client.remoteIP());
    Serial.println("]");
    Serial.println("");
    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean skip = true;
    String req_str = "";
    String rsp_str = "";
    String rsp_var = "";
    int data_length = -1;
    int data_loop = 0;
    int postVarsCount = 0;
    String postVars[2] = {};  //0 = Row, 1 = Action

    //int empty_line_count = 0;
    while (client.connected()) 
    {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        req_str += c;

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply       
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("GET")) {
          writeResponse(client);
          break;
        }
        
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST") && !skip) {
          writeResponse(client);
          break;
        }  
         
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST") && skip) {
          // indicate we have processed this post
          skip = false;

          // get the content length (post data length)
          String temp = req_str.substring(req_str.indexOf("Content-Length:") + 15);
          temp.trim();
          data_length = temp.toInt();
          
          Serial.print("Content-Length=");
          Serial.println(data_length);
          Serial.println("");

          // build the response string and count post vars
          data_loop = data_length;
          while(data_loop-- > 0)
          {
            // build the response string
            // get the data
            c = client.read();
            
            // process the post vars and then count them
            if(c == '&') {
              postVars[postVarsCount] = rsp_var;
              rsp_var = "";
              postVarsCount += 1;
            } else {
              rsp_str += c;
              rsp_var += c;
            }
          }
          postVars[postVarsCount] = rsp_var;  

          Serial.print("Response:");
          Serial.println(rsp_str);
          Serial.print("Post Vars Count:");
          Serial.println(postVarsCount + 1);
          Serial.println("Post Vars:");
          for (byte i = 0; i <= postVarsCount; i = i + 1) {
            Serial.print(i);
            Serial.print(":");
            Serial.println(postVars[i]);
          }
          Serial.println("");
     
          // process the post vars
          // process the action
          String userInput = postVars[1];

          // SET ROW
          if(userInput == "action=SETROW") {                
            // set the row number
            String userInput = postVars[0];
            if(userInput.length() == 4) {
            selectrow = 0;  
            } else {
              String temp2 = userInput.substring(4,6);
              selectrow = temp2.toInt();               
            }
            
            Serial.print("Row Value:");
            Serial.println(selectrow);

            // see if the row has changed
            if(selectrow != currentrow || selectrow != 0) {
              if(selectrow >= 1 && selectrow <= 12) { 
                // turn off the arm and fire pins before switching rows
                digitalWrite(5, LOW);
                digitalWrite(7, LOW);
                
                // change the row
                currentrow = selectrow;
                state = "ROWSET";
              } else {
                  state = "BADROW";
                  currentrow = 0;
                  allOff();                
              }
            }   
          }

          // ARM ROW
          if(userInput == "action=ARM") {

            Serial.print("ARM:");
            Serial.println(state);

            if(state == "ROWSET") {               
              state = "ARMED";
              digitalWrite(5, HIGH);
            } else {
              state = "NOROWSET";  
            }
          }

          // FIRE ROW
          if (userInput == "action=FIRE") {
            Serial.print("FIRE:");
            Serial.println(state);
            
            if(state == "ARMED" || state == "FIRE") {
              state = "FIRE";
              // set the output poins
              switch(currentrow) {
                case 1:
                  plusePin(15);
                  break;
                case 2:
                  plusePin(16);
                  break;
                case 3:
                  plusePin(17);
                  break;
                case 4:
                  plusePin(18);
                  break;
                case 5:
                  plusePin(19);
                  break;
                case 6:
                  plusePin(20);
                  break;
                case 7:
                  plusePin(21);
                  break;
                case 8:
                  plusePin(0);
                  break;
                case 9:
                  plusePin(1);
                  break;
                case 10:
                  plusePin(2);
                  break;
                default:
                  state = "BADROW";
                  currentrow = 0;
                  allOff();
                 break;
              }              
            } else {
              state = "NOTARMED";  
            }
          }
          
          // RESET ALL
          if(userInput == "action=RESET") {                 
            state = "RESET";
            currentrow = 0;
            allOff();
         } 

          // end while loop we have processed the post data
          writeResponse(client);
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    Serial.println(req_str);

    // give the web browser time to receive the data
    delay(50);
    
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected");
  }
}
