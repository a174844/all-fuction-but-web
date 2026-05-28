#include "screen_display.h"
#include "screen_driver.h"

void screen_display_init() {
}

void screen_display_normal() {
    screen.fillScreen(TFT_BLACK);
    screen.setCursor(0, 0);
    screen.println("System Running...");
}

void screen_display_warning(const char *event) {
    screen.fillScreen(TFT_RED);
    screen.setCursor(10, 50);
    screen.setTextSize(3);
    screen.print(event);
    screen.setTextSize(2);
}

void screen_display_update() {
    screen.setCursor(0, 180);
    screen.print(millis() / 1000);
}