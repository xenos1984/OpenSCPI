#pragma once

#include <cstdint>
#include <avr/io.h>
#include <pins_arduino.h>

namespace Analog
{
	/** Voltage reference sources. */
	namespace Reference
	{
		enum class Source : uint8_t
		{
			Vref = 0x00,
			Vcc = 0x01,
			Int2048 = 0x02,
			Int1024 = 0x03,
			Int4096 = 0x04
		};

		/** Sets the reference voltage source for DAC and ADC. */
		void SetSource(Source ref);

		/** Sets the internal voltage reference input in case the reference voltage is Vcc or Vext. */
		void SetInternal(Source ref);

		/** Enables output of the reference voltage on AVREF. Note that this only works if the multiplexer output is set to IVref! */
		inline void EnableOutput(void)
		{
			ADCSRC |= 0x01;
			DIDR1 |= 0x40;
		}

		/** Disables output of the reference voltage on AVREF. */
		inline void DisableOutput(void)
		{
			ADCSRC &= 0xfe;
		}
	}

	/** Digital-to-analog converter. */
	namespace DAConv
	{
		/** Enable DAC circuit. */
		inline void EnableConv(void)
		{
			DACON |= 0x8;
		}

		/** Enable output of DAC voltage. */
		inline void EnableOutput(void)
		{
			DACON |= 0xc;
		}

		/** Disable DAC circuit. */
		inline void DisableConv(void)
		{
			DACON = 0x3;
		}

		/** Disable output of DAC voltage. */
		inline void DisableOutput(void)
		{
			DACON &= 0xfb;
		}

		/**
		 * @brief Sets the DAC voltage value.
		 *
		 * @param val Output value in the range 0-255.
		 *
		 * Sets the DAC output voltage to U = ref * (val + 1) / 256. Hence, val = 256 * U / ref - 1.
		 */
		inline void SetValue(uint8_t val)
		{
			DAL0 = val;
		}
	}

	/** Analog-to-digital converter. */
	namespace ADConv
	{
		/** Enable ADC circuit. */
		inline void Enable(void)
		{
			ADCSRA |= 0x80;
		}

		/** Disable ADC circuit. */
		inline void Disable(void)
		{
			ADCSRA &= 0x7f;
		}

		/** ADC multiplexer. */
		namespace Muxer
		{
			/** Input pins. */
			enum class Input : uint8_t
			{
				AIn0  = 0x00,
				AIn1  = 0x01,
				AIn2  = 0x02,
				AIn3  = 0x03,
				AIn4  = 0x04,
				AIn5  = 0x05,
				AIn6  = 0x06,
				AIn7  = 0x07,
				AIn8  = 0x09,
				AIn9  = 0x0a,
				AIn10 = 0x0b,
				AIn11 = 0x0c,
				PinC0 = 0x00,
				PinC1 = 0x01,
				PinC2 = 0x02,
				PinC3 = 0x03,
				PinC4 = 0x04,
				PinC5 = 0x05,
				PinE1 = 0x06,
				PinE3 = 0x07,
				PinC7 = 0x09,
				PinF0 = 0x0a,
				PinE6 = 0x0b,
				PinE7 = 0x0c,
				Vdo15 = 0x08,
				Vdo45 = 0x0e,
				IVref = 0x0d,
				AGnd  = 0x0f,
				DAOut = 0x10
			};

			/** Set input channel. */
			inline void SetInput(Input val)
			{
				ADMUX = (ADMUX & 0xe0) | static_cast<uint8_t>(val);
			}
		}

		/** Differential amplifier. */
		namespace DiffAmp
		{
			/** Positive input pins. */
			enum class InputPos : uint8_t
			{
				Mux   = 0x00,
				AIn0  = 0x01,
				AIn1  = 0x02,
				AGnd  = 0x03,
				PinC0 = 0x01,
				PinC1 = 0x02
			};

			/** Negative input pins. */
			enum class InputNeg : uint8_t
			{
				AIn2  = 0x00,
				AIn3  = 0x04,
				AIn8  = 0x08,
				AIn9  = 0x0c,
				Mux   = 0x14,
				AGnd  = 0x18,
				Off   = 0x1c,
				PinC2 = 0x00,
				PinC3 = 0x04,
				PinC7 = 0x08,
				PinF0 = 0x0c,
				PinE0 = 0x10
			};

			/** Enable differential amplifier circuit. */
			inline void Enable(void)
			{
				DAPCR |= 0x80;
			}

			/** Disable differential amplifier circuit. */
			inline void Disable(void)
			{
				DAPCR &= 0x7f;
			}

			/** Set positive input. */
			inline void SetInputPos(InputPos val)
			{
				DAPCR = (DAPCR & 0xfc) | static_cast<uint8_t>(val);
			}

			/** Set negative input. */
			inline void SetInputNeg(InputNeg val)
			{
				DAPCR = (DAPCR & 0xe3) | static_cast<uint8_t>(val);
			}

			/** Set gain factor. */
			inline void SetGain(uint8_t gain)
			{
				uint8_t bits = 0x00;

				switch(gain)
				{
				case 8:
					bits = 0x20;
					break;
				case 16:
					bits = 0x40;
					break;
				case 32:
					bits = 0x60;
					break;
				default:
					bits = 0x00;
				}

				DAPCR = (DAPCR & 0x9f) | bits;
			}
		}

		/** Voltage divider. */
		namespace Divider
		{
			/** Input sources. */
			enum class Input : uint8_t
			{
				AIn0 = 0x01,
				AIn1 = 0x02,
				AIn4 = 0x03,
				AIn5 = 0x04,
				Ext  = 0x05,
				Vcc  = 0x06,
				Off  = 0x00
			};

			/** Set input. */
			inline void SetInput(Input val)
			{
				ADCSRD = (ADCSRD & 0xfc) | static_cast<uint8_t>(val);
			}

			/** Disable. */
			inline void Disable(void)
			{
				SetInput(Input::Off);
			}
		}

		/** Set ADC input to multiplexer output. */
		inline void SetInputMux(void)
		{
			ADCSRC &= 0xfd;
		}

		/** Set ADC input to differential amplifier output. */
		inline void SetInputDiff(void)
		{
			ADCSRC |= 0x02;
		}

		/** Measure ADC value synchronously. */
		inline uint16_t InputSync(void)
		{
			ADCSRA |= 0x40;

			while(ADCSRA & 0x40)
				;

			return ADC;
		}

		/** Measure ADC value with automatic offset compensation. */
		inline uint16_t InputComp(void)
		{
			uint16_t pv, nv;

			ADCSRC |= 0x20;
			pv = InputSync();
			ADCSRC &= 0xdf;
			nv = InputSync();

			return (pv + nv) >> 1;
		}
	}
}
