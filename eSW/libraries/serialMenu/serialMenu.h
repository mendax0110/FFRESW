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

/// @brief Class for the serial menu. \class SerialMenu
class SerialMenu
{
public:
	SerialMenu();

    /**
     * @brief Function to load the menu items.
     *
     * @param items -> The menu items.
     * @param size ->  The size of the menu items.
     */
    void load(MenuItem* items, size_t size);

    /**
     * @brief Function to show the menu.
     *
     */
    void show();

    /**
     * @brief Function to run the menu.
     *
     */
    void run();

private:
    MenuItem* currentMenu;
    size_t menuSize;
};

#endif // SERIAL_MENU_H
