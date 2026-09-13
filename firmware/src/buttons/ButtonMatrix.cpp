#include "ButtonMatrix.h"

ButtonMatrix::ButtonMatrix() : _count(0) {}

void ButtonMatrix::addButton(uint8_t pin) {
    if (_count >= MAX_BUTTONS) return;
    _buttons[_count++] = { pin, false, false, 0 };
}

void ButtonMatrix::begin() {
    for (uint8_t i = 0; i < _count; i++) {
        pinMode(_buttons[i].pin, INPUT_PULLUP);
    }
}

void ButtonMatrix::update() {
    unsigned long now = millis();
    for (uint8_t i = 0; i < _count; i++) {
        bool reading = !digitalRead(_buttons[i].pin); // Active LOW (pull-up)
        if (reading != _buttons[i].lastState) {
            _buttons[i].lastDebounce = now;
        }
        if ((now - _buttons[i].lastDebounce) > DEBOUNCE_MS) {
            _buttons[i].state = reading;
        }
        _buttons[i].lastState = reading;
    }
}

bool ButtonMatrix::isPressed(uint8_t index) const {
    if (index >= _count) return false;
    return _buttons[index].state;
}

uint16_t ButtonMatrix::getBitmask() const {
    uint16_t mask = 0;
    for (uint8_t i = 0; i < _count; i++) {
        if (_buttons[i].state) mask |= (1 << i);
    }
    return mask;
}
