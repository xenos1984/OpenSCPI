#include "Analog.h"

namespace Analog
{
	namespace Reference
	{
		void SetSource(Source ref)
		{
			ADMUX = (ADMUX & 0x3f) | (static_cast<uint8_t>(ref) << 6);
			ADCSRD = (ADCSRD & 0xbf) | (static_cast<uint8_t>(ref) << 4 & 0x40);

			if(ref == Source::Vcc)
				DACON &= 0x0c;
			else if(ref == Source::Vref)
				DACON = (DACON & 0x0c) | 0x01;
			else
			{
				DACON = (DACON & 0x0c) | 0x02;
				SetInternal(ref);
			}
		}

		void SetInternal(Source ref)
		{
			ADCSRD &= 0xcf;

			if(ref == Source::Int1024)
			{
				VCAL = VCAL1;
			}
			else if(ref == Source::Int2048)
			{
				ADCSRD |= 0x10;
				VCAL = VCAL2;
			}
			else
			{
				ADCSRD |= 0x20;
				VCAL = VCAL3;
			}
		}
	}
}
