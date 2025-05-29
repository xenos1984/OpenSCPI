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
	Analog::ADConv::Muxer::Input pin;
	uint16_t value;

	switch(String(parameters[0]).toInt())
	{
	case 1:
		pin = Analog::ADConv::Muxer::Input::AIn0;
		break;
	case 2:
		pin = Analog::ADConv::Muxer::Input::AIn1;
		break;
	case 3:
		pin = Analog::ADConv::Muxer::Input::AIn2;
		break;
	case 4:
		pin = Analog::ADConv::Muxer::Input::AIn3;
		break;
	case 5:
		pin = Analog::ADConv::Muxer::Input::AIn4;
		break;
	case 6:
		pin = Analog::ADConv::Muxer::Input::AIn5;
		break;
	default:
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

	analogWrite(DAC0, constrain(val, 0, 255));
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

	pinMode(DAC0, ANALOG);
	analogWrite(DAC0, 0);

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
