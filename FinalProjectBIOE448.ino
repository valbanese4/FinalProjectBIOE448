#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h> // Necessary for LCD communication 
#include <ArduinoBLE.h>

// Initialization of LCD Screen
LiquidCrystal lcd(10, 8, 5, 4, 3, 2); 

// Initializaion of Bluetooth
BLEService newService("180A");
BLEByteCharacteristic read_Steps("2A57", BLERead); // Steps
BLEByteCharacteristic read_Distance("2A58", BLERead); // Distance
BLEByteCharacteristic read_IdleTime("2A59", BLERead); // Idle Time


// Initialization of Step Counter
float total_acceleration; 
int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;

// Initialization of Additional Features
float distance;
float last_step_time;
const int Idle_Indicator = 6; // Red LED
const int BluetoothStatus = 7; // White LED

// Defining average stride length for women 
float stridelength = 2.2; //feet

// Initialization of Flags and Checkpoints
int steps = 0;
bool any_peak_detected = false;
int threshold = 450;

void setup() {
  Serial.begin(9600);

  // Bluetooth Signal Setup
  while(!Serial);
  if (!BLE.begin()){
    Serial.println("Waiting for ArduinoBLE");
    while(1);
  }
  
  BLE.setLocalName("SuperPedometer");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(read_Steps);
  newService.addCharacteristic(read_Distance);
  newService.addCharacteristic(read_IdleTime);

  BLE.addService(newService);
  
  read_Steps.writeValue(0);
  read_Distance.writeValue(0);
  read_IdleTime.writeValue(0);

  BLE.advertise();
  Serial.println("Bluetooth device active");

  // Pedometer + Extra Features Setup
  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();

  lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration
  lcd.print("Steps: ");
  lcd.setCursor(0,1);
  lcd.print("Dist: ");
  pinMode(Idle_Indicator, OUTPUT); // red idle LED
  pinMode(BluetoothStatus, OUTPUT); // bluetooth connection LED
}

void loop() {
  BLEDevice central = BLE.central(); // wait for a BLE central

   if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");
    Serial.println(central.address()); // print the central's BT address
    digitalWrite(BluetoothStatus, HIGH); // turn on the LED to indicate the connection
  } else {
    digitalWrite(BluetoothStatus, LOW); // turn off the LED to connection is lost
  }
  
  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)

  x = (int16_t)(Wire.read() | (Wire.read() << 8));
  y = (int16_t)(Wire.read() | (Wire.read() << 8));
  z = (int16_t)(Wire.read() | (Wire.read() << 8));

  // Total Acceleration 
  total_acceleration = sqrt(2*x*x + 2*y*y + z*z);

  // Peak Detection to Calculate Steps Taken
  if (total_acceleration > threshold && any_peak_detected == false) {
    steps = steps + 1;
    distance = steps*stridelength;
    any_peak_detected = true;
    last_step_time = millis();
    digitalWrite(Idle_Indicator, LOW);
  }

  if (total_acceleration < threshold && any_peak_detected == true) {
    any_peak_detected = false;
  }

  if (millis() - last_step_time > 5*60*1000) {
      //Turns the LED on
      digitalWrite(Idle_Indicator, HIGH);
  }

  // Display metrics on the LCD Screen
  delay(50);
  lcd.setCursor(7,0);
  lcd.print(steps);
  lcd.setCursor(6,1);
  lcd.print(distance);
  lcd.setCursor(10,1);
  lcd.print(" ft");

  // Transmit metrics to connected device through Bluetooth
  read_Steps.writeValue(steps);
  read_Distance.writeValue(distance);
  read_IdleTime.writeValue((millis() - last_step_time)/(60*1000));  
  
}