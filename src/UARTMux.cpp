#include "UARTMux.h"


UARTMux::UARTMux(byte ext_select, byte int_select)
{
    ExtSelectPin_ = ext_select;
    IntSelectPin_ = int_select;
}


UARTMux::~UARTMux() {}


void UARTMux::Begin()
{
    // Set pin modes
    pinMode(ExtSelectPin_, OUTPUT);
    pinMode(IntSelectPin_, OUTPUT);

    // Set default pin states
    digitalWrite(ExtSelectPin_, LOW);
    digitalWrite(IntSelectPin_, LOW);
}


void UARTMux::Select(ETarget target)
{  
    // Wait for output buffer to clear
    Serial.flush();

    // Slect appropreate mux circuit from input target
    if (target == ETarget::FTDI)
    {
        digitalWrite(ExtSelectPin_, LOW);
        digitalWrite(IntSelectPin_, LOW);
    }
    else if (target == ETarget::SDS021)
    {
        digitalWrite(ExtSelectPin_, HIGH);
        digitalWrite(IntSelectPin_, LOW);
    }
    else if (target == ETarget::SKM61)
    {
        digitalWrite(ExtSelectPin_, HIGH);
        digitalWrite(IntSelectPin_, HIGH);
    }
} 