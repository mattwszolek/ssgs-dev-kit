#SSGS Dev Kit

A cheap kit that allows you to build and test GameSense applications for the SteelSeries Engine 3.

##Setup
	Download and make sure you can build this project: `https://github.com/abcminiuser/lufa`
	Clone this repo
	`cd ssgs-dev-kit`
	Put your board into Boot mode
	`make avrdude`

##Hardware

I'm using this OLED screen, but any SSD1306 32x128 will work: `http://www.aliexpress.com/item/Free-shipping-0-91-inch-12832-white-and-blue-color-128X32-OLED-LCD-LED-Display-Module/32661533323.html?spm=2114.13010608.0.94.kwyFUR`
For the microcontroller I'm using an Arduino Pro Micro, but any AVR with I2C and USB support should be sufficient.
