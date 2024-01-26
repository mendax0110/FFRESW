/**
 * @file main.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>

#ifndef MAIN_H
#define MAIN_H

/// @brief This is the main namespace for the project \namespace main
namespace main
{
    class mainInternals
    {
    public:
        int main(int argc, char const *argv[]);

    private:
        void healthCheck();

    protected:

    };
}

#endif // MAIN_H