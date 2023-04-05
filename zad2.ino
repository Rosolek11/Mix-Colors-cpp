#include <cstdint>
#include <cmath>
#include "I2C.h" 
#include "PCA9633.h" 

constexpr uint8_t PCA9633PW_ADDRESS = 0x62; // The I2C address of the PCA9633PW IC
constexpr float PI = 3.14159265f;

void setup() {
    I2C.begin();
    PCA9633.begin(PCA9633PW_ADDRESS);
    PCA9633.enableAutoIncrement(true);
    PCA9633.setMode1(0x00); // No sleep
    PCA9633.setMode2(0x04); // Output logic state, not inverted
    PCA9633.setLedOutput(0xFF); // Set all LEDs to individual brightness and group dimming/blinking
}

void loop() {
    for (uint16_t hue = 0; hue < 360; hue++) {
        float rad = hue * PI / 180.0f;
        uint8_t red = static_cast<uint8_t>((std::sin(rad) * 127.5f) + 127.5f);
        uint8_t green = static_cast<uint8_t>((std::sin(rad + 2 * PI / 3) * 127.5f) + 127.5f);
        uint8_t blue = static_cast<uint8_t>((std::sin(rad + 4 * PI / 3) * 127.5f) + 127.5f);
        uint8_t white = 0;

        PCA9633.setChannelPWM(0, red);
        PCA9633.setChannelPWM(1, green);
        PCA9633.setChannelPWM(2, blue);
        PCA9633.setChannelPWM(3, white);

        delay(50); // Adjust the delay to change the speed of the color transition
    }
}

int main() {
    setup();
    while (true) {
        loop();
    }
    return 0;
}
