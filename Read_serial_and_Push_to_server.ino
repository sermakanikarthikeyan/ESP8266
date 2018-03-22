/*
This code used to receive serial data,then push into cloud using mqtt
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

String readString;
String vrms;
const char* ssid = "Testing";
const char* password = "12345678";
const char* mqtt_server = "iot.eclipse.org";
char msg1[100];
int i=0;
double sumWH = 0.00000;
float WH = 0;//energy consumption in watt hour 
double Voltage = 0;//AC supply peak voltage
//double vrms = 0;//AC supply rms voltage
int A_pos=0,H_pos=0,T_pos=0;
double current = 0;//load peak current
double irms = 0;//load rms current
double power = 0;//
String stringOne,stringTwo,stringThree,stringFour,stringFive,stringSix,temp;
int heart,temperature,axisx,axisy,axisz;
String  sheart,stemp,saxis;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Simple serial echo test"); // so I can keep track of what is loaded
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client_socket")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  i=0;

  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the String readString
    msg1[i++]=c;
    delay(2);  //slow looping to allow buffer to fill with next character
    if(c==']')
    {
      break;
    }
  }
  if((msg1[0]=='['))
  {
      if (readString.length() >0) {
       
          Serial.println(msg1);
          Serial.println(i);
          T_pos=readString.indexOf('T');
          H_pos=readString.indexOf('H');
          A_pos=readString.indexOf('A'); 
          stringOne=String(readString.substring(T_pos+2, H_pos)); //temperature
          stringTwo=String(readString.substring(H_pos+2, A_pos)); //heart
          stringThree=String(readString.substring(A_pos+2,i+1));   //axis 
          stringFour = String(stringThree.substring(0,stringThree.indexOf(',')));
          temp = String(stringThree.substring(stringThree.indexOf(',')+1));
          stringFive = String(temp.substring(0,temp.indexOf(',')));
          stringSix =  String(temp.substring(temp.indexOf(',')+1));   
          temperature =   stringOne.toFloat();
          heart = stringTwo.toInt();
          axisx = stringFour.toInt();
          axisy = stringFive.toInt();
          axisz = stringSix.toInt(); 
          Serial.print("temperature");
          Serial.println(temperature);
          Serial.print("Heart");
          Serial.println(heart);
          Serial.print("Axis");
          Serial.println(axisx);
          Serial.println(axisy);       
          Serial.println(axisz); 

          if(temperature > 32)
          {
            stemp = "Abnormal";
          }
          else
          {
            stemp = "Normal";
          }


          if(heart > 90)
          {
            sheart = "Abnormal";
          }
          else
          {
            sheart = "Normal";
          }


          if(axisx > 95 or axisy > 95 or axisz > 95)
          {
            saxis = "Fall down";
          }
          else
          {
            saxis = "Normal";
          }
          
          client.publish("health/T", String(stemp).c_str(), true);
          client.publish("health/H", String(sheart).c_str() , true);
          client.publish("health/A",String(saxis).c_str() , true);
          readString="";

        }
        for(i=0;i<100;i++)
        {
        msg1[i]=' ';
        } 
  }

}
