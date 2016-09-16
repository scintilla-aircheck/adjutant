#pragma once

#include <Arduino.h>

#include "LMP91000.h"
#include "MCP3425.h"

class PSTAT : public LMP91000
{
public:
	PSTAT();
	PSTAT(byte addr_0_pin, byte addr_1_pin, byte addr_2_pin, byte menb_pin);
	~PSTAT();

	// Sets PSTAT circuit pin states
	void Begin();

	// Configures an LMP91000 at a given address
	void Configure(byte addr, LMP91000::Configuration target);

	// ADC interface
	double ADC();
	void ADC(bool continuous, MCP3425::EResolution resolution, MCP3425::EGain gain);

	// Select PSTAT circuit
	void Select(byte target);

private:
	byte Address_[3];
	MCP3425 ADC_;
	//LMP91000 PSTAT_;
};
