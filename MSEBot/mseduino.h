
// A convenience header which maps all the ESP32 pins
// to the MSEduino pins

#ifndef MSEDUINO_H
#define MSEDUINO_H

#define BRDLED LED_BUILTIN // ESP32 LED

// Analog Pins
// 3V pins
#define J2 5
#define J3 4
#define J4 7
#define J5 6
#define J6 3
#define J7 0

// Digital pins
// 3V pins
#define J8 15
#define J9 2
#define J10 4
#define J11 16
#define J12 17
#define J13 5
#define J14 18
#define J15 19
#define J16 13
#define J17 12
#define J18 14
#define J19 27
#define J20 26
#define J21 23
#define J22 25

// 5V pins
#define J23 23
#define J24 12
#define J25 19
#define J26 18
#define J27 5
#define J28 4
#define J29 2
#define J30 15

// Jumper Pins
#define JP1 5     // Current Sensor
#define JP2 4     // Pot 1
#define JP3 7     // Pot 2
#define JP4       // Reset
#define JP5 23    // Smart LED
#define JP6 26    // Switch 2a
#define JP7 28    // Switch 1a
#define JP8 6     // Switch 1b
#define JP9 7     // Switch 2b
#define JP11      // I2C Header
#define JP12      // I2C Header
#define JP13 J19  // PB1
#define JP14 24   // PB2
#define JP15      // UART Header
#define JP16      // UART Header

// Perepherals
#define IMon JP1  // Current Moniter
#define R1 JP2    // Pot 1
#define R2 JP3    // Pot 2
#define S1a JP7   // Swithc 1a
#define S1b JP8   // Switch 1b
#define S2a JP6   // Switch 2a
#define S2b JP9   // Switch 2b
#define PB1 JP13  // Push Button 1
#define PB2 JP14  // Push Button 2

#endif
