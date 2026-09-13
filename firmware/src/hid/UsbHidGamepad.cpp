#include "UsbHidGamepad.h"
#include "USB.h"
#include "USBHIDGamepad.h"

static USBHIDGamepad _gamepad;

UsbHidGamepad::UsbHidGamepad() {}

void UsbHidGamepad::begin() {
    _gamepad.begin();
    USB.begin();
}

void UsbHidGamepad::sendReport(float steeringAngle, uint16_t buttons) {
    // Map steering angle (–540 to +540) → HID axis (–32767 to +32767)
    int16_t axisX = (int16_t)((steeringAngle / 540.0f) * 32767.0f);
    axisX = constrain(axisX, -32767, 32767);

    _gamepad.leftStick(axisX, 0);
    _gamepad.buttons(buttons);
    _gamepad.send();
}
