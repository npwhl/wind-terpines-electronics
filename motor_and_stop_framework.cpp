#include "Arduino.h"
#include "Servo.h"
#include "Button.h"

#define BRUSHLESSMOTOR_PIN_DATA 5
#define PUSHBUTTONMOMENTARY_PIN_2 2

const int brushlessMotorLowSpeed = 1000;
const int brushlessMotorFastSpeed = 2000;

Servo brushlessMotor;
Button pushButtonMomentary(PUSHBUTTONMOMENTARY_PIN_2);

const int timeout = 10000;
char menuOption = 0;
long time0;

void setup() 
{
    Serial.begin(9600);
    while (!Serial);
    Serial.println("start");
    
    brushlessMotor.attach(BRUSHLESSMOTOR_PIN_DATA);
    brushlessMotor.writeMicroseconds(brushlessMotorLowSpeed);
    pushButtonMomentary.init();
    menuOption = menu();
}

void loop() 
{
    if(menuOption == '1') {
        for(int brushlessMotorRotationSpeed = brushlessMotorLowSpeed; brushlessMotorRotationSpeed <= brushlessMotorFastSpeed; brushlessMotorRotationSpeed += 10)
        {
            brushlessMotor.writeMicroseconds(brushlessMotorRotationSpeed);
            Serial.println(brushlessMotorRotationSpeed);
            delay(100);
        }
        for(int brushlessMotorRotationSpeed = brushlessMotorFastSpeed; brushlessMotorRotationSpeed >= brushlessMotorLowSpeed; brushlessMotorRotationSpeed -= 10)
        {
            brushlessMotor.writeMicroseconds(brushlessMotorRotationSpeed);
            Serial.println(brushlessMotorRotationSpeed);
            delay(100);
        }
        brushlessMotor.detach();
    }
    else if(menuOption == '2') {
        bool pushButtonMomentaryVal = pushButtonMomentary.read();
        Serial.print(F("Val: ")); 
        Serial.println(pushButtonMomentaryVal);
    }
    
    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
}

char menu()
{
    Serial.println(F("\nWhich component would you like to test?"));
    Serial.println(F("(1) A2212 Brushless Motor 1000KV (with 30A ESC)"));
    Serial.println(F("(2) Momentary Push Button - Panel Mount (Red)"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available());

    while (Serial.available()) 
    {
        char c = Serial.read();
        if (isAlphaNumeric(c)) 
        {   
            if(c == '1') 
                Serial.println(F("Now Testing A2212 Brushless Motor 1000KV (with 30A ESC)"));
            else if(c == '2') 
                Serial.println(F("Now Testing Momentary Push Button - Panel Mount (Red)"));
            else
            {
                Serial.println(F("illegal input!"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}
