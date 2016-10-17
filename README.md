# Adjutant

**Adjuant** is the development firmware repository for the Scintilla air quality monitoring IoT device.

## Hardware Overview

The **Adjutant** uses an `ESP-12F` microcontroller.

### Power, Ground, and Reset circuits

Prototypical versions of the **Adjutant** utilize a physical button flashing system connected to `RST` and `GPIO 0`.
So far there is no reason to put the MCU to sleep and no external method to control such actions, so the `EN` pin is
permenantly tied to `3V3`.

**Pin Assignments:**

* **VCC:** `BU33SD5WG` 3V3 voltage regulator
* **GND:** Ground
* **RST:** Reset circuit (RST switch, pulled HIGH)
* **EN:** (pulled HIGH)

### I2C & Flash Circuit

`GPIO 0` and `GPIO 2` are both pulled HIGH in order to enable the `ESP-12F`. Both of these pins double as the `SCL` and
`SDA` pins, respectively.

I2C communications should work out of the box, provided you assign the correct pins.

**Pin Assignments:**

* **GPIO 0:** SCL (FLSH switch, pulled HIGH)
* **GPIO 2:** SDA (pulled HIGH)

**Example**

```cpp
#include <Wire.h>

#define SDA 2
#define SCL 0

setup()
{
    # Inititialize I2C bus with explicit pin declarations
    Wire.pins(SDA, SCL);
    Wire.begin();
}
```

### UART Mux Circuit

The **Adjutant** uses a `74LV4052a` IC to mux between three UART channels, allowing it to communicate with the
`SDS021` dust sensor and `SKM61` GPS module.

**Pin Assignments:**

* **TX:** FTDI / SDS021 / SKM61
* **RX:** FTDI / SDS021 / SKM61
* **GPIO 10:** UART internal select
* **GPIO 15:** UART external select (pulled LOW)

If the external select pin (`GPIO 15`) is pulled LOW (default), the UART mux will only communicate with the FTDI chip
and the internel select pin is ignored. If the external select pin is driven HIGH, the internel select pin (`GPIO 10`)
can be driven LOW to communicate with the `SDS021` dust sensor or HIGH to communicate with the `SKM61` GPS module.

Control of the UART mux circuit is provided by `UARTMux` class which must be instantiated using the appropreate pin
assignments and initialized using the `Begin()` method. Once the mux circuit is initialized, you can use the `Select()`
method to switch between circuits identified in the `UARTMux::ETarget` enum.

**Example:**

```cpp
#include <UARTMux.h>

#define USEL_INT 10
#define USEL_EXT 15

UARTMux uart = UARTMux(USEL_EXT, USEL_INT);

setup()
{
    uart.Begin();

    # Select dust sensor
    uart.Select(UARTMux::ETarget::SDS021);
    # ...

    # Select GPS module
    uart.Select(UARTMux::ETarget::SKM61);
    # ...

    # Select FTDI chip
    uart.Select(UARTMux::ETarget::FTDI);
    # ...
}
```

***Warning:*** The UART mux is highly sensitive to timing issues and race conditions. The `UARTMux` library is designed
to wait until the serial output buffer is cleared, but it is up to the developer to calculate an appropreate delay when
waiting for a message to return from the dust sensor or GPS module.

### PSTAT Circuit

The **Adjutant** is also armed with a full suite of SPEC gas sensors. Each of these sensors is controlled by an
`LMP91000` potentiostat IC, which can be programmed using the `PSTAT` class with the associated configurations
found in the `SPEC` library.

The corresponding output voltage for the potentiostat circuit is measured using an `MCP3425` IC, which can be configured
using the `ADC()` method of the `PSTAT` class.

**Pin Assignments:**

* **GPIO 14:** PSTAT select 0
* **GPIO 12:** PSTAT select 1
* **GPIO 13:** PSTAT select 2
* **GPIO 16:** MENB (read/write to LMP91000)

**Example:**

```cpp
#include <PSTAT.h>
#include <SPEC.h>

#define MENB 16
#define PSTAT0 14
#define PSTAT1 12
#define PSTAT2 13

# Instantiate PSTAT circuit
PSTAT pstat = PSTAT(PSTAT0, PSTAT1, PSTAT2, MENB);

setup()
{
    # Initialize PSTAT pins
    pstat.Begin();

    # Configure PSTAT ADC
    pstat.ADC(true, MCP3425::EResolution::d16Bit, MCP3425::EGain::x8);

    # Configure SPEC gas sensors
    pstat.Configure(0, SPEC::CO);
    pstat.Configure(1, SPEC::O3);
    pstat.Configure(2, SPEC::SO2);
    pstat.Configure(3, SPEC::NO2);
    pstat.Configure(4, SPEC::H2S);
}

loop()
{
    double v_out;
    for (int i = 0; i < 5; i++)
    {
        # Select the gas sensor and measure the output voltage
        pstat.Select(i);
        v_out = pstat.ADC();

        # Do something with the data
        # ...
    }
}
```

### Unused Pins

* **GPIO 5:** *(NC)*
* **GPIO 4:** *(NC)*
* **GPIO 9:** *(NC)*

* **ADC:** *(NC)*
* **SCLK:** *(NC)*
* **MOSI:** *(NC)*
* **MISO:** *(NC)*
* **CSO:** *(NC)*

----

## Cloning and Flashing

To clone all submodules:
```
$ git submodule init
$ git submodule update
```

To update all submodules:
```
$ git submodule foreach git pull origin master
```

To build on a vanilla Ubuntu installation:
```
$ sudo cpan YAML
```
