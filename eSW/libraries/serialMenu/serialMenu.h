#ifndef SERIAL_MENU_H
#define SERIAL_MENU_H

#include <Arduino.h>


/// @brief Serial menu structure.
struct MenuItem
{
    const char* label;
    char key;
    void (*callback)();
};

class SerialMenu
{
public:
	SerialMenu();
    void load(MenuItem* items, size_t size);
    void show();
    void run();

private:
    MenuItem* currentMenu;
    size_t menuSize;
};

#endif // SERIAL_MENU_H
