# Arduino Fireworks Controller

### Arduino MKR WiFi 1010 Web Based Fireworks Controller
My family and I love Fireworks and I love building things with Arduino products.  What better way to combine both loves then to build a Fireworks Controller.
 There are lots of commercial products available and I am sure there are several micro controller products already out there, but hey one more can't hurt.

### Old School Method:
I would lay out different fireworks and effects one row at a time. At show time I would get up from my chair light the row, run back to my chair, then do that again and again, you get the picture.  Lighting fireworks is fun but sitting back with a cocktail and enjoying the show with the family is lot more fun (hey what's wrong with mixing booze and explosives, really what could go wrong)!

### Fancy Pants Arduino Method:
Build a Web Based controller that would fire off several rows, one at a time as I directed, while sitting with the family and enjoy the show together.

### End Product
1. Power / Charge and Fan Switches
   
 ![Front View](/Images/Front.JPG)

2. Connections Ignitor / Charger

 ![Connections](/Images/Connections.JPG)

3. Charging Up

 ![Charging](/Images/Charging.JPG)

4. Component Layout

 ![Component](/Images/Component_Layout.jpg)

### Electrical Schematic
1. Components and a Fantastic tool to create Schematics can be found at: [Digi-Key](https://www.digikey.com/schemeit/home)

2. Arduino Boards, Software and Help can be found at: [Arduino](https://www.arduino.cc/)

 ![Schematic](/Images/Schematic.png)

### Prerequisites
> - Skills and Tools to read a schematic and build circuits (not too hard, there are lots tutorials and information available on-line), BTW with all this COVID crap your just sitting around anyway so don't let fear or time stop you)
> - A few extra bucks, I spent around 70 dollars, but I'm sure you can reduce the cost
> - Know the range of the WiFi network you are using (you don't want to lose your connection at show time)
> - Computer, WiFi connection, some programming skills and lots & lots of Ignitor's, Fuse and Fireworks

## Getting Started
I'll assume if your still with me you have your Controller assembled, powered on (and not smoking or on fire - that would be real bad when dealing with fireworks).  Let's start coding!

### Network Setup / Configuration
 - You need to configure your network settings and IP address.  
*network_parameters.h* - defines the network setting your controller needs 
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

