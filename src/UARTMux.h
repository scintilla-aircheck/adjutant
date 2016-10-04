#include <Arduino.h>


class UARTMux
{
public:
    UARTMux(byte ext_select, byte int_select);
    ~UARTMux();

    /// Hard-coded addreses for the UART mux circuit
    enum class ETarget
    {
        FTDI,
        SDS021,
        SKM61
    };

    // Initializes pin states
    void Begin();

    // Selects a UARTMux target connection
    void Select(ETarget target);

private:
    byte ExtSelectPin_, IntSelectPin_;
};
