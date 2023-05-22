// VARIABLES MISSIÓ 2
int INT1 = 6;
int RST1 = 0;
int INT1_STATUS = 0;

int INT2 = 7;
int RST2 = 1;
int INT2_STATUS = 0;
// FI

void setup()
{
    Serial.begin(9600);
    pinMode(INT1, OUTPUT);
    pinMode(RST1, INPUT);
    digitalWrite(RST1, LOW);

    pinMode(INT2, OUTPUT);
    pinMode(RST2, INPUT);
    digitalWrite(RST2, LOW);

    attachInterrupt(digitalPinToInterrupt(6), capturaMuons1, RISING);
    attachInterrupt(digitalPinToInterrupt(7), capturaMuons2, RISING);
}

void loop()
{
    Serial.println("hola");
    delay(1000);
}

void capturaMuons1()
{
    INT1_STATUS = digitalRead(INT1);
    if (INT1_STATUS > 0)
    {
        Serial.println("MUÓ DETECTAT, STATUS:");
        Serial.println(INT1_STATUS);
        Serial.println("RESETTING...");
        delay(10000);
        digitalWrite(RST1, HIGH);
        delay(200);
        digitalWrite(RST1, LOW);
        INT1_STATUS = digitalRead(INT1);
        Serial.println("RESET COMPLETE, STATUS:");
        Serial.println(INT1_STATUS);
    }
    else
    {
        Serial.println(INT1_STATUS);
        delay(1000);
    }
}

void capturaMuons2()
{
    INT2_STATUS = digitalRead(INT2);
    if (INT2_STATUS > 0)
    {
        Serial.println("MUÓ DETECTAT, STATUS:");
        Serial.println(INT2_STATUS);
        Serial.println("RESETTING...");
        delay(10000);
        digitalWrite(RST2, HIGH);
        delay(500);
        digitalWrite(RST2, LOW);
        INT2_STATUS = digitalRead(INT2);
        Serial.println("RESET COMPLETE, STATUS:");
        Serial.println(INT2_STATUS);
    }
    else
    {
        Serial.println(INT2_STATUS);
        delay(1000);
    }
}