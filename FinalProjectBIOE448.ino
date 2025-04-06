#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h> // Necessary for LCD communication 
LiquidCrystal lcd(10, 8, 5, 4, 3, 2); 
float total_acceleration;
int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z;
float xmod, ymod, zmod;

// Flags and Checkpoints Initializing
int steps = 0;

bool any_peak_detected = false;
bool first_peak_detected = false;
//unsigned long first_peak_time = 0;
//unsigned long second_peak_time = 0;

int upper_threshold = 450;
int lower_threshold = 400;

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

  if (total_acceleration > upper_threshold && any_peak_detected == false) {
    steps = steps + 1;
    any_peak_detected = true;
  }

  if (total_acceleration < lower_threshold && any_peak_detected == true) {
    any_peak_detected = false;
  }

  Serial.println(steps);
  delay(50);
  lcd.setCursor(7,0);
  lcd.print(steps);
}