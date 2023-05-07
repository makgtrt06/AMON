/*************************************************************************
  AMON - Arnau, Nil, Martí, Oleguer

  Programa definitiu de la missió 1 i 2:
  Transmissió dades amb buzzer, programa definitiu amb detecció de muons
  v. 0.9.3

  Arduino MKR Zero
*************************************************************************/

// Última modificació: 27/04/2023 --> Martí Carrasco
//  v. 0.9.4

#include <TinyGPS.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>

File myFile;
TinyGPS gps;
SFE_BMP180 bmp180;

unsigned long fix_age;
char status;
double PresionNivellMar = 1013.25;
double Temperatura, Pressio, Altitud;
void gpsdump(TinyGPS &gps);
bool feedgps();
void getGPS();
long lat, lon;
float LAT, LON;
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long temps = 0;
const int Interval = 5000;
int EstatLed = 0;
int k = 0;
int altitudBase = 0;

//VARIABLES MISSIÓ 2
int INT1 = 6;
int RST1 = 0;
int INT1_STATUS = 0;

int INT2 = 7;
int RST2 = 1;
int INT2_STATUS = 0;
//FI

void setup(){
    // brunzidor:
    for (i = 0; i < 5; i++)
    {
        digitalWrite(10, HIGH);
        delay(500);
        digitalWrite(10, LOW);
        delay(500);
    }

    delay(500);
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println("Iniciant port sèrie");
    pinMode(LED_BUILTIN, OUTPUT);
    //MISSIÓ 2
    pinMode(INT1, OUTPUT);
    pinMode(RST1, INPUT);
    digitalWrite(RST1, LOW);

    pinMode(INT2, OUTPUT);
    pinMode(RST2, INPUT);
    digitalWrite(RST2, LOW);

    attachInterrupt (digitalPinToInterrupt(6), capturaMuons1, RISING);
    attachInterrupt (digitalPinToInterrupt(7), capturaMuons2, RISING);

    //FI

    if (bmp180.begin())
    {
        Serial.println("BMP180 iniciado");
        delay(500);
    }
    else
    {
        Serial.println("Error al iniciar BMP180");
    }

    // Inicialitzem la targeta SD
    Serial.println("Initializing SD card...");
    delay(1000);
    // Fi inicialització tarja SD
    if (!SD.begin(SS1)) // 4 per SparFun microSD Shield es el 8
    {
        Serial.println("Inicialitzacio fallada !!!");
        return;
    }
    Serial.println("Inicialitzacio correcte.");

    CapturaGPS();
    altitudBase = gps.f_altitude();
    altitudBase = altitudBase + 100;

    // Fi inicialització
    while (gps.f_altitude() < altitudBase)
    {
        CapturaGPS();
        Serial.println(gps.f_altitude())
        Serial1.println(gps.f_altitude())
        delay(3000);
    }
}
// Fi SETUP*******************************************************************
// INICI DEL PROGRAMA*********************************************************
void loop(){
        Serial.println("BMP180 iniciado");
        unsigned long TempsActual = millis();
        if (TempsActual - temps >= Interval)
        {

            temps = TempsActual;
            if (EstatLed == 0)
            {
                EstatLed = 1;
            }
            else
            {
                EstatLed = 0;
            }

            digitalWrite(LED_BUILTIN, HIGH);
        }

        CapturaGPS();
        CapturaBMP180();
        PrintDades();
        SDPrint();
        digitalWrite(LED_BUILTIN, LOW);
    }
    else {
        Serial1.println("ESPERANT ALTITUD");
    }

// Fi bucle del programa - inici funcions GPS*******************************************************

void CapturaGPS()
{
    gps.get_position(&lat, &lon, &fix_age);
    getGPS();
}

void getGPS()
{
    bool newdata = false;
    unsigned long start = millis();
    // Every 1 seconds we print an update
    while (millis() - start < 500)
    {
        if (feedgps())
        {
            newdata = true;
        }
    }
    if (newdata)
    {
        gpsdump(gps);
    }
}

bool feedgps()
{
    while (Serial1.available())
    {
        if (gps.encode(Serial1.read()))
            return true;
    }
    return 0;
}

void gpsdump(TinyGPS &gps)
{
    // byte month, day, hour, minute, second, hundredths;
    gps.get_position(&lat, &lon);
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);
    LAT = lat;
    LON = lon;
    {
        feedgps(); // If we don't feed the gps during this long routine, we may drop characters and get checksum errors
    }
}

// Fi funcios GPS *************************************************

void CapturaBMP180()
{

    status = bmp180.startTemperature(); // Inici de lectura de temperatura
    if (status != 0)
    {
        delay(status);                               // Pausa perque acavi la lectura
        status = bmp180.getTemperature(Temperatura); // Obtener la temperatura
        if (status != 0)

        {
            status = bmp180.startPressure(3); // Inici lectura de pressio
            if (status != 0)
            {
                delay(status);                                     // Pausa perque acavi la lectura
                status = bmp180.getPressure(Pressio, Temperatura); // Obtenim la presio
                if (status != 0)
                {
                }
            }
        }
    }
}
// Fi capturar dades‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Funció temps d'espera entre captura de dades, en minuts‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void Esperem(int minuts)
{
    Serial.print(" ‐> Esperant (minuts) :");
    Serial.println(minuts);
    Serial.println(" ");
    for (int a = 0; a < minuts; a++)
    {
        // delay(60000);  //60000 es un minut
        delay(5000); // 60000 es un minut
    }
}
// Fi funció temps d'espera‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void PrintDades()
{
    Serial.print("Latitud : ");
    Serial.print(LAT / 1000000, 7);
    Serial.print(" i Longitud : ");
    Serial.println(LON / 1000000, 7);
    Serial.print("Data            : ");
    Serial.print(day, DEC);
    Serial.print("/");
    Serial.print(month, DEC);
    Serial.print("/");
    Serial.println(year);
    Serial.print("Hora            : ");
    Serial.print(hour, DEC);
    Serial.print(":");
    Serial.print(minute, DEC);
    Serial.print(":");
    Serial.println(second, DEC);
    Serial.print("Altitud (metres): ");
    Serial.println(gps.f_altitude());
    Serial.print("Rumb (graus)    : ");
    Serial.println(gps.f_course());
    Serial.print("Velocitat(kmph) : ");
    Serial.println(gps.f_speed_kmph());
    Serial.print("Satelits        : ");
    Serial.println(gps.satellites());
    Serial.print("Temperatura: ");
    Serial.print(Temperatura, 2);
    Serial.print(" *C , ");
    Serial.print("Presion: ");
    Serial.print(Pressio, 2);
    Serial.print(" mb, ");
    Altitud = bmp180.altitude(Pressio, PresionNivellMar); // Calcular altura
    Serial.print("Altitud: ");
    Serial.print(Altitud);
    Serial.println(" m");
    Serial.println();

    Serial1.print(Temperatura, 2);
    Serial1.print(",");
    Serial1.print(Pressio, 2);
    Serial1.print(",");
    Serial1.print(gps.f_altitude());
    Serial1.println();
    Serial1.print("Satelits        : ");
    Serial1.println(gps.satellites());
    Serial1.print("Rumb (graus)    : ");
    Serial1.println(gps.f_course());
    Serial1.print("Latitud : ");
    Serial1.print(LAT / 1000000, 7);
    Serial1.print(" i Longitud : ");
    Serial1.println(LON / 1000000, 7);
    Serial1.print("Velocitat(kmph) : ");
    Serial1.println(gps.f_speed_kmph());
}

void SDPrint()
{
    myFile = SD.open("FITXER00.txt", FILE_WRITE);
    if (myFile) // Si el fitxer s'ha obert correctament, escrivim en el fitxer
    {

        // BMP(pressió i temp)
        myFile.print("Temp ");
        myFile.print(Temperatura, 2);
        myFile.print(";");
        myFile.print("Pres ");
        myFile.print(Pressio, 2);
        myFile.print(";");
        Altitud = bmp180.altitude(Pressio, PresionNivellMar); // Calcular altura
        myFile.print("Alt ");
        myFile.print(Altitud);
        myFile.print(";");
        // GPS
        myFile.print("Lat ");
        myFile.print(LAT / 1000000, 7);
        myFile.print(";");
        myFile.print("Lon ");
        myFile.print(LON / 1000000, 7);
        myFile.print(";");
        myFile.print("Data ");
        myFile.print(day, DEC);
        myFile.print("/");
        myFile.print(month, DEC);
        myFile.print("/");
        myFile.print(year);
        myFile.print(";");
        myFile.print("Hora ");
        myFile.print(hour, DEC);
        myFile.print(":");
        myFile.print(minute, DEC);
        myFile.print(":");
        myFile.print(second, DEC);
        myFile.print(";");
        myFile.print("AltGPS ");
        myFile.print(gps.f_altitude());
        myFile.print(";");
        myFile.print("Rumb ");
        myFile.print(gps.f_course());
        myFile.print(";");
        myFile.print("V ");
        myFile.print(gps.f_speed_kmph());
        myFile.print(";");
        myFile.print("Sats ");
        myFile.print(gps.satellites());
        myFile.print(";");
        myFile.println();

        myFile.close(); // tanquem el fitxer
        Serial.println("-");
    }
    else
    {
        // Si a l'obrir el fitxer tenim un error:
        Serial.println("Error obrint FITXER00.txt");
    }
}

void capturaMuons1()
{
        Serial.println("MUÓ DETECTAT al sensor 1");
        Serial.println("RESETTING...");
        delay(500);
        digitalWrite(RST1, HIGH);
        delay(1);
        digitalWrite(RST1, LOW);
        Serial.println("RESET COMPLETE");
}

void capturaMuons2()
{
        Serial.println("MUÓ DETECTAT al sensor 2");
        Serial.println("RESETTING...");
        delay(500);
        digitalWrite(RST2, HIGH);
        delay(1);
        digitalWrite(RST2, LOW);
        Serial.println("RESET COMPLETE");
}