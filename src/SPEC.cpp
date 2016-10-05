#include "SPEC.h"

namespace SPEC
{
	Configuration::Configuration(
		LMP91000::ETIAGain tia_gain, LMP91000::ERLoad r_load, bool ext_ref,
		LMP91000::EIntZero int_zero, bool bias_sign, LMP91000::EBias bias,
		bool fet_short, LMP91000::EOpMode op_mode,
		String target, int range, double v_min, double v_max) :
		LMP91000::Configuration(
			tia_gain, r_load, ext_ref,
			int_zero, bias_sign, bias,
			fet_short, op_mode)
	{
		Target = target;
		Range = range;
		Vmin = v_min;
		Vmax = v_max;
	}
}
