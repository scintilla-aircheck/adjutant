#pragma once

#include <Arduino.h>
#include "LMP91000.h"

namespace SPEC
{
    struct Configuration : public LMP91000::Configuration
    {
        // Target gas
        String Target;

        // Sensor range (in ppm)
        int Range;

        // Nominal output voltage at 0ppm
        double Vmin;
        
        // Nominal output voltage at maximum saturation
        double Vmax;

        Configuration(
            LMP91000::ETIAGain tia_gain, LMP91000::ERLoad r_load, bool ext_ref,
            LMP91000::EIntZero int_zero, bool bias_sign, LMP91000::EBias bias,
            bool fet_short, LMP91000::EOpMode op_mode,
            String target, int range, double v_min, double v_max);
    };

    static const Configuration CO = Configuration(
        LMP91000::ETIAGain::R350k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d20pct,
        true,
        LMP91000::EBias::d1pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "CO",
        1000,
        0.5,
        2.16
    );

    static const Configuration H2S = Configuration(
        LMP91000::ETIAGain::R120k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d20pct,
        true,
        LMP91000::EBias::d1pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "H2S",
        50,
        0.5,
        1.70
    );

    static const Configuration NO2 = Configuration(
        LMP91000::ETIAGain::R350k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d20pct,
        false,
        LMP91000::EBias::d8pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "NO2",
        20,
        1.25,
        1.04
    );

    static const Configuration SO2 = Configuration(
        LMP91000::ETIAGain::R350k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d20pct,
        true,
        LMP91000::EBias::d8pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "S02",
        20,
        1.25,
        1.46
    );

    static const Configuration O3 = Configuration(
        LMP91000::ETIAGain::R350k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d50pct,
        false,
        LMP91000::EBias::d1pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "O3",
        20,
        1.25,
        1.11
    );

    static const Configuration ETOH = Configuration(
        LMP91000::ETIAGain::R120k,
        LMP91000::ERLoad::R10,
        true,
        LMP91000::EIntZero::d50pct,
        true,
        LMP91000::EBias::d4pct,
        false,
        LMP91000::EOpMode::ThreeCell,
        "ETOH",
        1000,
        0.5,
        2.18
    );
}
