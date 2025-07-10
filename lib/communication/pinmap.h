#ifndef PINMAP_H
#define PINMAP_H

// Motor control for pill dispenser
#define DISPENSER_MOTOR_PIN     5

// Buzzer to alert user
#define BUZZER_PIN              6

// User button to acknowledge or confirm
#define USER_BUTTON_PIN         12

void initPins();  // Initializes all defined GPIOs
void toggleLEDNonBlocking(int pin, unsigned long interval);
void commandConfirmationBlink(int pin, volatile bool* flag);
// Non-blocking LED toggle function
#endif
