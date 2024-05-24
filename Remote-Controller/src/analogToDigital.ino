int xPin = A0;
int yPin = A1;
int butonPin = 2;
int xPozisyon;
int yPozisyon;
int butonDurum;

void setup() 
{
	Serial.begin(9600);
	pinMode(xPin, INPUT);
	pinMode(yPin, INPUT);
	pinMode(butonPin, INPUT_PULLUP);
}

void loop() 
{
	xPozisyon = analogRead(xPin);
	yPozisyon = analogRead(yPin);
	butonDurum = digitalRead(butonPin);

	if(xPozisyon >= 250 && xPozisyon <= 600)
	{
		xPozisyon = 0;
	}
	
	else if(xPozisyon > 600)
	{
		xPozisyon = 1;
	}
	
	else if(xPozisyon < 250)
	{
		xPozisyon = -1;
	}

	if(yPozisyon >= 250 && yPozisyon <= 600)
	{
		yPozisyon = 0;
	}
	
	else if(yPozisyon > 600)
	{
		yPozisyon = 1;
	}
	
	else if(yPozisyon < 250)
	{
		yPozisyon = -1;
	}

	Serial.print(xPozisyon);
	Serial.print(" ");
	Serial.print(yPozisyon);
	Serial.print(" ");
	Serial.println(butonDurum);
	
	delay(100);
}