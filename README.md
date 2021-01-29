# Arduino Fireworks Controller

### Arduino MKR WiFi 1010 Web Based Fireworks Controller
My family and I love Fireworks and I love to build things with Arduino products so why not combine the two and build an Arduino based Fireworks Controller.
 I know there are lots of commercial products available and I am sure there are several micro controller products already as well, but hey one more can't hurt.

### Prerequisites
> - Skills and Tools to read a schematic and build circuits (not too hard, there are lots tutorials and information available on-line), BTW with all this COVID stuff your just sitting around anyway so don't let fear or time stop you
> - A few extra bucks, I spent around 70 bucks, but I'm sure you can reduce the cost
> - Test the range of the WiFi network you are using (you don't want to lose your connection at show time)
> - Ignitor's, Fuse and Lot's and Lot's of Fireworks
> 
### Old School Method
I would lay out different fireworks and effects one row at a time. At show time I would get up from my chair light the row, run back to my chair, then do that again and again, you get the picture.  Lighting fireworks is fun but sitting back with a cocktail and enjoying the show with the family is lot more fun (hey what's wrong with mixing booze and explosives, really what could go wrong)!

### Arduino Method
Build a Web Based controller that would fire off several rows, one at a time as I directed, while sitting with the family and enjoying the show.

> #### The Controller provides the operator with a very simple WEB based interface that allows the user to
> - Connect to the controller by simple entering its IP address in your browsers URL bar (I use my IPhone)
> - Click on and enter the row you want to fire
> - Arm the row (a little safety never hurts)
> - Click on Fire and enjoy

### Here's the Final Product
1. Power / Charge and Fan Switches
   
 ![Front View](/Images/Front.JPG)

2. Connections Charger & Igniter

![Connections](/Images/Connections.JPG)
![Connections Test](/Images/Connections_Test.JPG)
[Test Fire](https://www.youtube.com/watch?v=j5TMe5pu_uM)

3. Charging Up

 ![Charging](/Images/Charging.JPG)

4. Component Layout

![Component](/Images/Component_Layout.jpg)

5. Setting Up

![Component](/Images/Fireworks_Setup.png)

6. Application Screen

![Component](/Images/Fireworks_App.png)

### Electrical Schematic
1. Components and a Fantastic tool to create Schematics can be found at: [Digi-Key](https://www.digikey.com/schemeit/home)

2. Arduino Boards, Software and Help can be found at: [Arduino](https://www.arduino.cc/)

 ![Schematic](/Images/Schematic.png)

I'll assume your still with me, you have your Controller assembled and powered on (not smoking of course - that would be real bad when dealing with fireworks). Before you save the code to your Arduino we need to do a little network setup.

### Network Setup
 - You need to configure your network settings and IP address.  
*network_parameters.h* - defines the network settings your controller needs 
```sh
// your network SSID (Name)
#define MYHOME_SSID "your network name"

// your network password
#define MYHOME_PASSWORD "your network password"

// ipdaddress you want your arduino board to use
// keep in mind this address must be available and on the same network as your router
// for example my router's address is 167.80.0.1
// i'm telling my arduino to be address 167.80.0.99
#define MYHOME_IP_OCT1 167
#define MYHOME_IP_OCT2 80
#define MYHOME_IP_OCT3 0
#define MYHOME_IP_OCT4 99
```
### Arduino Setup
 - Setup the status LED's (LED_BUILTIN, ARMED and FIRE), they are internal to the controller and are they for development only
 - Setup the digital pins that activate the MOSFET transistors (1-10) and actual fire the connected row
```sh
  // initialize digital pins
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
```
- Setup the serial port to provide debugging information
```sh
  // turn on the serial port - we used this to trouble shoot the controller
  // throughout the code you will see a numerous Serial.print and Serial.println commands
  // they provide debug information for startup and normal operational information
  Serial.begin(9600);      // initialize serial communication
```
- Make sure the WiFi hardware is present, has the latest firmware, gets configured with our network parameters and connects to the network
```sh
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

    // wait 5 seconds for connection while flashing on board LED
    waitForWifi();
  }
```
- Once the WiFi gets connected we start the web server, send the connection information and turn the on board LED on solid to indicate we are ready
```sh
  // start the server and send the user the connection information
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  
  // LED on to indicate we have a wifi connection
  digitalWrite(LED_BUILTIN, HIGH);
```
### Arduino Loop - Normal Code Processing
- At this point the Arduino is connected to the network and waiting for a connection
```shell
void loop() {
  // listen for incoming clients
  // i.e. waiting for someone to connect to our static IP Address as define above
  WiFiClient client = server.available();
  if (client) {
```
- Once a connection is made display the IP of the device that made the connection, setup some vars we need to process this request
```shell
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
```
- While we have a connection and the client is available, start process the Initial GET request by sending out the web page *(writeResponse(client));*
- The Web Page has a form that sends the server two parameters (row=xx AND action=SETROW) xx is the row number and action is the action to take
- The Fire Button calls *plusePin* this function caused Arduino to make the selected row go active high for 1 second, this turns on the MOSFET which completes the connection between the ignitor to the +12v battery thereby lighting the fuse  
```shell
    while (client.connected()) 
    {
      if (client.available()) {
        char c = client.read();
        req_str += c;

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply = send the initial page with the default values
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("GET")) {  
          writeResponse(client);
          Serial.print("Response1:");
          Serial.println(rsp_str);
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

          Serial.print("Response3:");
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
          // resend the page but this time we will have an update state value
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
```
- After we process the request we close the current connection and wait for the next request
```shell
    Serial.print("Response4:");
    Serial.println(req_str);

    // give the web browser time to receive the data
    delay(50);
    
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected");
```

## Future Enhancements
 - Add a Connection strength status LED/Display to ensure we are with in range of our WiFi 
 - Bring the internal LEDS to the outside cover se we can see the lights for monitoring and troubleshooting, they will also look cool too
 - Add a circuit and code to monitor the battery voltage, so we know when the battery is low and needs charging
 - Add a Low Voltage LED to go along with the voltage monitor

## Next Up
This was my first published product.  I built this last year, since then I have learned a lot, A WHOLE LOT!  I'm currently working on:
> - A Remote Control Tank which includes: Upgraded WEB server interface with a continuous websocket connection for fast command processing, a 20W Burning Laser and Optional servo controlled Claw 
> - A bottle rocket launcher attachment for a Drone
> - A Paratrooper dropping device an RC airplane / Drone
> - A cool GEO cache device (my grandkids love to find them)
> - Several cool Arduino tutorials for Software Interrupts, Background Processing, Board to Board communication, Pulse Simulation, Websites using Base64 to save code space and some other Fun stuff TBD 

Please follow me on [YouTube](https://www.youtube.com/channel/UCgu_aWvmKPHFhXgSWBCj7EA) to catch the "Next Up Stuff"
