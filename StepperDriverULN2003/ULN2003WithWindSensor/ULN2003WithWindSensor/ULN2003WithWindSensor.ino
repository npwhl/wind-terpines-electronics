#define analogPinForRV A1
#define analogPinForTMP A0
#include <Stepper.h>

// to calibrate your sensor, put a glass over it, but the sensor should not be
// touching the desktop surface however.
// adjust the zeroWindAdjustment until your sensor reads about zero with the glass over it. 

const float zeroWindAdjustment = 0.2; // negative numbers yield smaller wind speeds and vice versa.

// wind speed module variables 
int TMP_Therm_ADunits;
float RV_Wind_ADunits;
float RV_Wind_Volts;
unsigned long lastMillis;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 6, 7, 8, 9); // IN1-IN3-IN2-IN4

const float stepAngle = 360.0 / stepsPerRevolution;  
const int stepsFor30deg = (int)(30.0 / stepAngle);   // 171ish steps for 30°
bool turnedIn = false;  // track whether stepper is turned in

void setup() {
  Serial.begin(57600);
  Serial.println("Start: Wind sensor + stepper control");

  // optional: reset analog pins used by sensor
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A3, LOW);

  myStepper.setSpeed(10); // RPM
}

void loop() {
  // read sensors every 0.5 seconds
  if (millis() - lastMillis > 500) {
    TMP_Therm_ADunits = analogRead(analogPinForTMP);
    RV_Wind_ADunits = analogRead(analogPinForRV);
    RV_Wind_Volts = RV_Wind_ADunits * 0.0048828125;

    // wind sensor calculations 
    TempCtimes100 = (0.005 * (float)TMP_Therm_ADunits * TMP_Therm_ADunits) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;
    zeroWind_ADunits = -0.0006 * ((float)TMP_Therm_ADunits * TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;
    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;
    WindSpeed_MPH = pow(((RV_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265);

    Serial.print("WindSpeed (MPH): ");
    Serial.println(WindSpeed_MPH);
    float WindSpeed_MS = WindSpeed_MPH * 0.44704; // m/s

    // if wind >= 1 m/s, rotate stepper 30° inward. need to change angle depending on mech team needs?
    // what would the angle need to be to completely feather/stop the blade from turning?
    if (WindSpeed_MS >= 1.0 && !turnedIn) {
      Serial.println("Wind >= 1 m/s, turning");
      myStepper.setSpeed(10);
      myStepper.step(stepsFor30deg);
      turnedIn = true;
    } 
    // if wind drops below 1 m/s, return to original position
    else if (WindSpeed_MS < 1.0 && turnedIn) {
      Serial.println("Wind < 1 m/s, returning to original position");
      myStepper.setSpeed(10);
      myStepper.step(-stepsFor30deg);
      turnedIn = false;
    }

    lastMillis = millis();
  }
}
