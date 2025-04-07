#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h> // Necessary for LCD communication 
LiquidCrystal lcd(10, 8, 5, 4, 3, 2); 
float total_acceleration;
int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;
float distance;
float last_step_time;
const int LEDPin = 6;

// Flags and Checkpoints Initializing
int steps = 0;
bool any_peak_detected = false;
int threshold = 450;

// Defining average stride length for women 
float stridelength = 2.2; //feet

void setup() {
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); // red LED
  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();
  lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration
  lcd.print("Steps: ");
  lcd.setCursor(0,1);
  lcd.print("Distance: ");
  
}

void loop() {
  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)

  x = (int16_t)(Wire.read() | (Wire.read() << 8));
  y = (int16_t)(Wire.read() | (Wire.read() << 8));
  z = (int16_t)(Wire.read() | (Wire.read() << 8));
  
  Serial.print("x = "); // Print values
  Serial.print(x);
  Serial.print(", y = ");
  Serial.print(y);
  Serial.print(", z = ");
  Serial.println(z);

  total_acceleration = sqrt(2*x*x + 2*y*y + z*z);
  Serial.println(total_acceleration);

  if (total_acceleration > threshold && any_peak_detected == false) {
    steps = steps + 1;
    distance = steps*stridelength;
    any_peak_detected = true;
    last_step_time = millis();
    digitalWrite(LEDPin, LOW);
  }

  if (total_acceleration < threshold && any_peak_detected == true) {
    any_peak_detected = false;
    
  }

  if (millis() - last_step_time > 5*60*1000) {
      //Turns the LED on
      digitalWrite(LEDPin, HIGH);
  }

  Serial.println(steps);
  Serial.println(distance);
  delay(50);
  lcd.setCursor(7,0);
  lcd.print(steps);
  lcd.setCursor(10,1);
  lcd.print(distance);

}