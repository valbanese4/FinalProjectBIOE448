#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h> // Necessary for LCD communication 
LiquidCrystal lcd(10, 8, 5, 4, 3, 2); 
float total_acceleration;
int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;

// Flags and Checkpoints Initializing
int steps = 0;
bool any_peak_detected = false;
int threshold = 450;

void setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();
  lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration
  lcd.print("Steps: ");
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
    any_peak_detected = true;
  }

  if (total_acceleration < threshold && any_peak_detected == true) {
    any_peak_detected = false;
  }

  Serial.println(steps);
  delay(50);
  lcd.setCursor(7,0);
  lcd.print(steps);
}