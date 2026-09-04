#include <Arduino.h>

// This is for DRV8871

// Define GPIO pin mappings 
const int pinIN1 = 12; 
const int pinIN2 = 13; 

// Setting PWM properties
const int freq = 4000;
const int pwmChannel = 0;
const int resolution = 8;

const int buttonPin = 4; // GPIO pin connected to the switch

#define ADC_PIN34 34 // pressure
#define ADC_PIN35 35 // pos

String inputString = "";         // A string to hold incoming data
bool stringComplete = false;     // Whether the string is complete

int spd = 200;

bool bCW = true;

void setspd(int spd);

void setup() {
  // Set motor pins as outputs
  pinMode(pinIN2, OUTPUT);
  // pinMode(pinIN2, OUTPUT);

  // Configure LEDC PWM
  ledcSetup(pwmChannel, freq, resolution);
  // Attach the channel to the GPIO
  ledcAttachPin(pinIN1, pwmChannel);

  analogSetAttenuation(ADC_11db); // Set to max range

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(115200);

  inputString.reserve(200); // Reserve memory for efficiency
  Serial.println("ESP32 Serial Terminal Ready.");
  Serial.print("\n> "); 

   /*
  digitalWrite(pinIN2, LOW);
  ledcWrite(pwmChannel, spd); // 0-255 duty cycle

  delay(2000);

  digitalWrite(pinIN2, LOW);
  ledcWrite(pwmChannel, 0); // 0-255 duty cycle

  delay(1000);

  digitalWrite(pinIN2, HIGH);
  ledcWrite(pwmChannel, 256-spd); // 0-255 duty cycle

  delay(2000);

  digitalWrite(pinIN2, LOW);
  ledcWrite(pwmChannel, 0); // 0-255 duty cycle

  delay(1000);
 */

  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  digitalWrite(pinIN2, LOW);
  digitalWrite(pinIN1, LOW);

}

void processCommand(String command) {
    command.trim(); // Remove extra spaces or newlines

    if (command.length() == 0) return;

  // Find the space separating the opcode and the integer
    int spaceIndex = command.indexOf(' ');

    String opcode = "";
    int value = 0;

    if (spaceIndex == -1) {
        // No space found: the entire string is the opcode (e.g., "help")
        opcode = command;
    } else {
        // Space found: split into opcode and value string
        opcode = command.substring(0, spaceIndex);
        String valueStr = command.substring(spaceIndex + 1);
        valueStr.trim();
        
        // Convert the value string to an integer
        value = valueStr.toInt(); 
    }

/*
    if (command == "led on") {
        Serial.println("Executing: Turning LED ON");
        // Add your turn-on code here
        digitalWrite(pinIN2, LOW);
        ledcWrite(pwmChannel, spd); // 0-255 duty cycle
    } else if (command == "led off") {
        Serial.println("Executing: Turning LED OFF");
        // Add your turn-off code here
        digitalWrite(pinIN2, HIGH);
        ledcWrite(pwmChannel, 256-spd); // 0-255 duty cycle
    }     
    else if (command == "stop") {
        Serial.println("Executing: Stopping");
        // Add your turn-off code here
        digitalWrite(pinIN2, LOW);
        ledcWrite(pwmChannel, 0); // 0-255 duty cycle
    }         
    else {
        Serial.println("Unknown command: " + command);
    }

    */

    if (opcode == "cw") {
        Serial.print("Setting motor speed to: ");
        Serial.println(value);
        // analogWrite(MOTOR_PIN, value);

        if(value > 255) value = 255;
        else if(value < 0) value = 0;

        bCW = true;

        pinMode(pinIN2, OUTPUT);
        ledcAttachPin(pinIN1, pwmChannel);

        digitalWrite(pinIN2, HIGH);
        ledcWrite(pwmChannel, 256 - value); // 0-255 duty cycle
    } 
    else if (opcode == "ccw") {
        Serial.print("Setting motor speed to: ");
        Serial.println(value);
        // analogWrite(MOTOR_PIN, value);

        if(value > 255) value = 255;
        else if(value < 0) value = 0;

        bCW = false;

        pinMode(pinIN1, OUTPUT);
        ledcAttachPin(pinIN2, pwmChannel);

        digitalWrite(pinIN1, HIGH);
        ledcWrite(pwmChannel, 256 - value); // 0-255 duty cycle
    } 
    else if (opcode == "coast") {
        Serial.println("Executing: Stopping");
        // Add your turn-off code here
        pinMode(pinIN1, OUTPUT);
        pinMode(pinIN2, OUTPUT);
        digitalWrite(pinIN2, LOW);
        digitalWrite(pinIN1, LOW);
    }         
    else if (opcode == "lock") {
        Serial.println("Executing: lock");
        // Add your turn-off code here
        pinMode(pinIN1, OUTPUT);
        pinMode(pinIN2, OUTPUT);
        digitalWrite(pinIN2, HIGH);
        digitalWrite(pinIN1, HIGH);
    }         
    else {
        Serial.print("Unknown command: ");
        Serial.println(opcode);
    }

}

void loop() {


  while (Serial.available()) {
        char inChar = (char)Serial.read();

        // Echo the character back to the terminal immediately
        Serial.print(inChar); 
        
        // If the incoming character is a newline, set a flag
        if (inChar == '\n' || inChar == '\r') {
            if (inputString.length() > 0) {
                stringComplete = true;
            }
        } else {
            // Otherwise, add it to the inputString
            inputString += inChar;
        }
    }

    // Process the command when complete
    if (stringComplete) {
        processCommand(inputString);
        inputString = "";      // Clear the string for the next command
        stringComplete = false;
        Serial.print("\n> "); 
    }


  

  int adc = analogRead(ADC_PIN34); // Returns 0-4095
  int pos = analogRead(ADC_PIN35); // Returns 0-4095

  // Serial.println(spd);

  int buttonState = digitalRead(buttonPin);

  if(buttonState > 0) ledcWrite(pwmChannel, 255); 
  else ledcWrite(pwmChannel, 30); // button pressed

  //Serial.println(buttonState);

  //delay(500);

}

