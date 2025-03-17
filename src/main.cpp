#include "Arduino.h"
#include "Vrekrer_scpi_parser.h"

SCPI_Parser scpi;

uint16_t vcc, aref, vref;

void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	interface.print(F("OpenSCPI,Wavgat UNO,0,0\n"));
}

void Measure(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	uint8_t pin;
	uint16_t value;

	switch(String(parameters[0]).toInt())
	{
	case 1:
		pin = A0;
		break;
	case 2:
		pin = A1;
		break;
	case 3:
		pin = A2;
		break;
	case 4:
		pin = A3;
		break;
	case 5:
		pin = A4;
		break;
	case 6:
		pin = A5;
		break;
	default:
		return;
	}

	value = analogRead(pin);
	interface.print(value * (0.001f * vref) / 4096, 4);
	interface.write('\n');
}

void Output(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	analogWrite(DAC0, constrain(256000 * String(parameters[0]).toFloat() / vref - 1, 0, 255));
}

void Reference(SCPI_C commands, SCPI_P parameters, Stream& interface)
{
	String ref = String(parameters[0]);
	ref.toUpperCase();

	if(ref == F("VCC"))
	{
		analogReference(DEFAULT);
		vref = vcc;
	}
	else if(ref == F("EXT"))
	{
		analogReference(EXTERNAL);
		vref = vcc; // FIXME: Measure external reference voltage.
	}
	else if(ref == F("INT1"))
	{
		analogReference(INTERNAL1V024);
		vref = 1024;
	}
	else if(ref == F("INT2"))
	{
		analogReference(INTERNAL2V048);
		vref = 2048;
	}
	else if(ref == F("INT4"))
	{
		analogReference(INTERNAL4V096);
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

	analogReadResolution(12);

	analogReference(INTERNAL1V024);
	vref = vcc = (5 * analogRead(V5D1)) / 4;
	analogReference(DEFAULT);
}

void loop()
{
	scpi.ProcessInput(Serial, "\n");
}
