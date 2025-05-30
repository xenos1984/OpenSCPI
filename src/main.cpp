#include "Arduino.h"
#include "Vrekrer_scpi_parser.h"
#include "Analog.h"

SCPI_Parser scpi;

uint16_t vref;

void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	interface.print(F("OpenSCPI,Wavgat UNO,0,0\n"));
}

void Measure(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	String p0 = String(parameters[0]);
	Analog::ADConv::Muxer::Input pin = Analog::ADConv::Muxer::Input::AGnd;
	uint16_t value;

	p0.toUpperCase();

	if(p0[0] == 'A' && p0.length() >= 2)
	{
		int n = p0.substring(1).toInt();

		if(n >= 8)
			n++;

		if(n < 0 || n >= 12)
			return;

		pin = static_cast<Analog::ADConv::Muxer::Input>(n);
	}
	else if(p0[0] == 'P' && p0.length() == 3)
	{
		int n = p0.substring(2).toInt();

		if(p0[1] == 'C')
		{
			if(n == 7)
				pin = Analog::ADConv::Muxer::Input::PinC7;
			else if(n >= 0 && n <= 5)
				pin = static_cast<Analog::ADConv::Muxer::Input>(n);
			else
				return;
		}
		else if(p0[1] == 'F' && n == 0)
		{
			pin = Analog::ADConv::Muxer::Input::PinF0;
		}
		else if(p0[1] == 'E')
		{
			switch(n)
			{
			case 1:
				pin = Analog::ADConv::Muxer::Input::PinE1;
				break;
			case 3:
				pin = Analog::ADConv::Muxer::Input::PinE3;
				break;
			case 6:
				pin = Analog::ADConv::Muxer::Input::PinE6;
				break;
			case 7:
				pin = Analog::ADConv::Muxer::Input::PinE7;
				break;
			default:
				return;
			}
		}
		else
		{
			return;
		}
	}
	else if(p0 == F("DAC"))
	{
		pin = Analog::ADConv::Muxer::Input::DAOut;
	}
	else if(p0 == F("IVREF"))
	{
		pin = Analog::ADConv::Muxer::Input::IVref;
	}
	else if(p0 == F("VCC15"))
	{
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Vcc);
		pin = Analog::ADConv::Muxer::Input::Vdo15;
	}
	else if(p0 == F("VCC45"))
	{
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Vcc);
		pin = Analog::ADConv::Muxer::Input::Vdo45;
	}
	else if(p0 == F("EXT15"))
	{
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Ext);
		pin = Analog::ADConv::Muxer::Input::Vdo15;
	}
	else if(p0 == F("EXT45"))
	{
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Ext);
		pin = Analog::ADConv::Muxer::Input::Vdo45;
	}
	else
	{
		return;
	}

	Analog::ADConv::Muxer::SetInput(pin);
	value = Analog::ADConv::InputComp();
	interface.print(value * (0.001f * vref) / 4096, 4);
	interface.write('\n');
}

void Output(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	String p0 = String(parameters[0]);
	float val = p0.toFloat();

	if(p0.endsWith(F("mV")))
		val = 256 * val / vref - 1;
	else if(p0.endsWith(F("V")))
		val = 256000 * val / vref - 1;
	else if(p0.endsWith(F("%")))
		val = 255 * val / 100;

	Analog::DAConv::SetValue(constrain(val, 0, 255));
}

void Reference(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	String ref = String(parameters[0]);
	ref.toUpperCase();

	if(ref == F("VCC"))
	{
		Analog::Reference::SetSource(Analog::Reference::Source::Int1024);
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Vcc);
		Analog::ADConv::Muxer::SetInput(Analog::ADConv::Muxer::Input::Vdo15);
		vref = (5 * Analog::ADConv::InputComp()) / 4;
		Analog::Reference::SetSource(Analog::Reference::Source::Vcc);
	}
	else if(ref == F("EXT"))
	{
		Analog::Reference::SetSource(Analog::Reference::Source::Int1024);
		Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Ext);
		Analog::ADConv::Muxer::SetInput(Analog::ADConv::Muxer::Input::Vdo15);
		vref = (5 * Analog::ADConv::InputComp()) / 4;
		Analog::Reference::SetSource(Analog::Reference::Source::Vref);
	}
	else if(ref == F("INT1"))
	{
		Analog::Reference::SetSource(Analog::Reference::Source::Int1024);
		vref = 1024;
	}
	else if(ref == F("INT2"))
	{
		Analog::Reference::SetSource(Analog::Reference::Source::Int2048);
		vref = 2048;
	}
	else if(ref == F("INT4"))
	{
		Analog::Reference::SetSource(Analog::Reference::Source::Int4096);
		vref = 4096;
	}
}

void setup()
{
	Serial.begin(115200);

	scpi.timeout = 30000;
	scpi.RegisterCommand(F("*IDN?"), &Identify);
	scpi.RegisterCommand(F(":MEASure:VOLTage?"), &Measure);
	scpi.RegisterCommand(F(":SOURce:VOLTage"), &Output);
	scpi.RegisterCommand(F(":CONFigure:ANALog:VREFerence"), &Reference);

	Analog::DAConv::EnableConv();
	Analog::DAConv::EnableOutput();
	Analog::DAConv::SetValue(0);

	Analog::Reference::SetSource(Analog::Reference::Source::Int1024);
	Analog::ADConv::Divider::SetInput(Analog::ADConv::Divider::Input::Vcc);
	Analog::ADConv::Muxer::SetInput(Analog::ADConv::Muxer::Input::Vdo15);
	vref = (5 * Analog::ADConv::InputComp()) / 4;
	Analog::Reference::SetSource(Analog::Reference::Source::Vref);
}

void loop()
{
	scpi.ProcessInput(Serial, "\n");
}
