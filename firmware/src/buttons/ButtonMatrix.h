#pragma once
#include <Arduino.h>

// ============================================================
//  ButtonMatrix.h
//  Handles multiple buttons with software debounce
// ============================================================

#define MAX_BUTTONS 12
#define DEBOUNCE_MS 20

struct Button {
    uint8_t  pin;
    bool     state;
    bool     lastState;
    unsigned long lastDebounce;
};

class ButtonMatrix {
public:
    ButtonMatrix();
    void addButton(uint8_t pin);         // Register a button GPIO pin
    void begin();                        // Set pinMode for all buttons
    void update();                       // Read all buttons (call in loop)
    bool isPressed(uint8_t index) const; // Check if button[index] is pressed
    uint16_t getBitmask() const;         // Get all button states as bitmask (for HID)

private:
    Button   _buttons[MAX_BUTTONS];
    uint8_t  _count;
};
