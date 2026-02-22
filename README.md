# loxone-arduino-relays
This is a tiny Arduino project to attach up to 64 relays to a Loxone Smart Home system at low cost.

My challenge was to control the state LEDs of a number of switches ("Gira Tastsensor 2") by my Loxone Smart Home system.

Using the native way e.g. by Loxone's Relay extension would cost about 25-30€ per LED.
IMHO that's too much to see if a light outside is turned on or a configuration setting is enabled.

So I started to look around for a less expensive way and ended up in the Arduino world.

Here is my setup:
- Arduino Nano R3
- W5500 Ethernet module (SPI)
- PCF8574 8 port GPIO exensions (I2C, up to 8 daisy chained modules)
- 8 channel relay modules (5V, one per PCF8574 module, directly attached)

Finally this will cost something about 100€ to control 64 state LEDs (or whatever is attached to the relays).

The Loxone Miniserver is configured to send a UDP telegram to the Arduino to turn the state LED on or off.
Sending the UDP telegram is done by configuring a "Virtual Output" /dev/udp/\<ip>/\<port> and one "Virtual Output Command" per state LED with hard coded contents when turning it on or off.
The telegram contents is very simple: "\<bank>-\<port>:\<state>" with:
- \<bank>: the index of the PCF8574 module (0..7)
- \<port>: the index of the GPIO pin of this PCF8574 module (0..7)
- \<state>: the LED state to set (0 or 1)

By now (February 2026) I just tested it with one PCF8574 module and two LEDs, more to come up soon. 
