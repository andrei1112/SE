#include <dht.h>

dht DHT;
int VehicleJoinedServer = 0; // 0 -> not joined , 1 joined server
int VehicleLifetimeTerminatedCheck = 0;
int Command=0;
int ConnectedToVehicle = 0;
int InterogatedDataReceived =0;
int RequestPending = 0;   //request of interogated data 
int TemperatureSensor =0;
int LightSensor =0;

#define DHT11_PIN 7
#define ConnectToVehicle                49   //character 1 in ascii code
#define InterogateData                  50   //character 2 in ascii code   
#define ConfirmReceptionOfData          51   //character 3 in ascii code
#define TerminateConnection             52   //character 4 in ascii code
#define VehicleLifetimeTerminated       53   //character 5 in ascii code
 
void setup(){
  Serial.begin(9600);
  pinMode(2,INPUT);
  pinMode(3,OUTPUT);
  pinMode(8,OUTPUT);
}

void CheckForSensors()
{
  int chk = DHT.read11(DHT11_PIN);
  int ButtonState = digitalRead(2);
  int LightValue = analogRead(0);
  if (DHT.temperature == 0)
    TemperatureSensor = 0;
  else 
    TemperatureSensor = 1;
  if(LightValue<20)   // light sensor check with value of 20 because of ADC conversion errors 
    LightSensor = 0 ;
  else 
    LightSensor = 1 ;
}

void GetDataFromSensors()
{
  CheckForSensors();
  int chk = DHT.read11(DHT11_PIN);
  int ButtonState = digitalRead(2);
  int LightValue = analogRead(0);
  if (TemperatureSensor==1)
  {
    Serial.print("Temperature = ");
    Serial.println(DHT.temperature);
    Serial.print("Humidity = ");
    Serial.println(DHT.humidity);
    Serial.print("ButtonStatus = ");
  }
  else
    Serial.println("Temperature and humidity sensor malfunctioning");
  if (ButtonState==1)
  {
    Serial.println(0);
    digitalWrite(3,LOW);
  }
  else
  {
    Serial.println(1);
    digitalWrite(3,HIGH);
  }
  if (LightSensor==1)
  {
    Serial.print("Light = ");
    Serial.println(LightValue);
  }
  else
        Serial.println("Light sensor malfunctioning");
  digitalWrite(8,HIGH);
  delay(500);
  digitalWrite(8,LOW);
  delay(500);
}

void RequestToJoin()
{
  int byte=0;
  Serial.println("Request to join");
  delay(1000);
  byte=Serial.read();
  if(byte == 49)  // 49 for character 1 in ASCII code
  {
     Serial.println("Message acknowledge");
     VehicleJoinedServer =1;
  }
  else 
     Serial.println("Vehicle not registred");
}

void VehicleLifetimeTerminated_F()
{
     Serial.println("Vehicle lifetime terminated");
     VehicleLifetimeTerminatedCheck = 1 ;
}

void ConnectToVehicle_F()
{

     Serial.println("Connected to Vechicle");
     ConnectedToVehicle = 1 ;
}

void ConfirmReception()
{
  InterogatedDataReceived =1;
  RequestPending =0;
  Serial.println("Data received");
}

void TerminateConnection_F()
{
  ConnectedToVehicle = 1;
     Serial.println("Connection Terminated");
}

void loop(){
  if(VehicleJoinedServer==0)
       RequestToJoin();
  if(VehicleJoinedServer==1)
  {
    Command=Serial.read();
    if(Command == ConnectToVehicle)  
      ConnectToVehicle_F();
    if(ConnectedToVehicle==1)  
    {
      if(Command == InterogateData)
      {
        GetDataFromSensors();
        RequestPending =1;
      }
      if(Command == ConfirmReceptionOfData)
        ConfirmReception();
      if (Command == TerminateConnection)
        TerminateConnection_F();
      if(VehicleLifetimeTerminatedCheck==0 && Command == VehicleLifetimeTerminated)
       VehicleLifetimeTerminated_F();
    }
  }
  delay (1000);
  if (RequestPending == 1 && InterogatedDataReceived == 0)
     GetDataFromSensors();

}
