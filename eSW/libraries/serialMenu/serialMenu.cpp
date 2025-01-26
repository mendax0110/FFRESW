#include "serialMenu.h"

SerialMenu::SerialMenu() : currentMenu(nullptr), menuSize(0)
{

}

void SerialMenu::load(MenuItem* items, size_t size)
{
    currentMenu = items;
    menuSize = size;
}

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