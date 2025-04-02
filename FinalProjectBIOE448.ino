#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h>
LiquidCrystal lcd(10, 8, 5, 4, 3, 2);
float total_acceleration;
int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;

// Flags and Checkpoints Initializing
int steps = 0;

bool any_peak_detected = false;
bool first_peak_detected = false;
//unsigned long first_peak_time = 0;
//unsigned long second_peak_time = 0;

int upper_threshold = 90000;
int lower_threshold = 70000;

int peak_period = 0;

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
  x = (Wire.read() | Wire.read() << 8); // Parse x values
  y = (Wire.read() | Wire.read() << 8); // Parse y values
  y = (Wire.read() | Wire.read() << 8); // Parse z values

  total_acceleration = sqrt(x*x + y*y + z*z);
  Serial.println(total_acceleration);

  if (total_acceleration > upper_threshold && any_peak_detected == false) {
    any_peak_detected = true;

    if (first_peak_detected == false) {
      //first_peak_time = millis();
      first_peak_detected = true;    

    } else {
      //second_pulse_time = millis();
      //pulse_period = second_pulse_time - first_pulse_time;        
      steps = steps + 1;
      first_peak_detected = false;
    }
  }

  if (total_acceleration < lower_threshold) {
    any_peak_detected = false;
  }
  Serial.println(steps);
  delay(60);
}