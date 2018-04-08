# Arduino Extras
Here's a small list of things that you might want to know about Arduinos during AUhack.

To borrow hardware from us go to [the event page at mlh](https://hardware.mlh.io/events/auhack-2018).

# WiFi (and IoT) capabilities

The ESP8266 chip can be programmed from the Arduino IDE. We have some Wemos D1 minis available that have this chip on board.

[Webbutton.ino](webButton.ino) is an example sketch that connects to a wifi, and pokes a webserver when it senses a button press.

[mailCheck.ino](mailCheck.ino) is the counterpart that connects to a wifi and checks whether the webserver has been poked. It shines an LED if that's the case.

The examples use [the excellent wifimanager library](https://github.com/tzapu/WiFiManager). and the free [dweet.io](https://dweet.io/) service. The wizards should be able to help you change the url to one of your choice.

# Keyboard / Mouse emulation

Can be done with the Arduinos that have the AtMega32U4 chip on board, like the Leonardo or the ProMicro. Ask your local wizard to provide you.