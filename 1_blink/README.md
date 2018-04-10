# Blink

Here we will read the state of a button with the Arduino and mirror this state by lighting an LED.

# Hardware setup
https://cdn-learn.adafruit.com/assets/assets/000/002/209/medium800/learn_arduino_fritzing.jpg?1396780471

# Code

```c++


int ledPin = 5; //pin for the "input"/anode leg of the LED (the long leg)
int buttonApin = 9; //input pin for button A
int buttonBpin = 8; //input pin for button B
 
void setup() 
{
  pinMode(ledPin, OUTPUT); //set LED pin to be an output
  pinMode(buttonApin, INPUT_PULLUP); //set button A pin to be an input using the internal pull-up resistor to make sure there are no random switchreadings happening
  pinMode(buttonBpin, INPUT_PULLUP); //set button B pin to be an input using the internal pull-up resistor to make sure there are no random switchreadings happening
}
 
void loop() 
{
  if (digitalRead(buttonApin) == LOW) //if button a is pressed then the LED is turned on.
  {
    digitalWrite(ledPin, HIGH); //led turns on - high means yehaw!
  }
  if (digitalRead(buttonBpin) == LOW) //if button b is pressed then the LED is turned off.
  {
    digitalWrite(ledPin, LOW); //led turns on - low means awwwwwww...
  }
}
```

# Now What?
Build the curcuit, upload the code, push the button.

Does the LED light up? - Is it the proper way around?
