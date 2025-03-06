#include "serialMenu.h"

SerialMenu::SerialMenu() : currentMenu(nullptr), menuSize(0)
{

}

/// @brief Function to load the menu items
/// @param items -> The items in the menu
/// @param size -> The size of the items
void SerialMenu::load(MenuItem* items, size_t size)
{
    currentMenu = items;
    menuSize = size;
}

/// @brief Function to show the serial menu
void SerialMenu::show()
{
    if (currentMenu == nullptr) return;

    for (size_t i = 0; i < menuSize; i++)
    {
        Serial.print(currentMenu[i].key);
        Serial.print(": ");
        Serial.println(currentMenu[i].label);
    }
    Serial.print("Select an option: ");
}

/// @brief Function to run the chosen choices from the menu
void SerialMenu::run()
{
    if (currentMenu == nullptr || !Serial.available()) return;

    char choice = Serial.read();

    for (size_t i = 0; i < menuSize; i++)
    {
        if (currentMenu[i].key == choice)
        {
            currentMenu[i].callback();
            return;
        }
    }
}
