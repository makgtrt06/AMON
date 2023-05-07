/*************************************************************************
  AMON - Arnau, Nil, Martí, Oleguer

  Programa de la missió 1:
  Tansols lectura de dades, preparat pel llançament, brunzidor.
  v. 1.0.0

  Arduino MKR Zero
*************************************************************************/

//Última modificació: 28/02/2023 --> Martí Carrasco
// v. 0.9.2

#include <TinyGPS.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include "pitches.h"

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
int  EstatLed = 0;
int k = 0;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup()
{
  delay(500);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Iniciant port sèrie");
  pinMode(LED_BUILTIN, OUTPUT);
  void soDeControl();

  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }

  if (bmp180.begin()) {
    Serial.println("BMP180 iniciado");
    delay(500);
  } else
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
  // Fi inicialització
}
// Fi SETUP*******************************************************************
// INICI DEL PROGRAMA*********************************************************
void loop()
{
  Serial.println("BMP180 iniciado");
  unsigned long TempsActual = millis();
  if (TempsActual - temps >= Interval) {

    temps = TempsActual;
    if (EstatLed == 0) {
      EstatLed = 1;
    } else {
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

void PrintDades() {
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
  Serial1.print(",");
  Serial1.print(hour, DEC);
  Serial1.print(".");
  Serial1.print(minute, DEC);
  Serial1.print(".");
  Serial1.println(second, DEC);
  Serial1.println();
}

void SDPrint() {
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
void soDeControl(){
 loop();{
    delay(60000);
  
    // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
 }
}