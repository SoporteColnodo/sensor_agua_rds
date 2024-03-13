#include <SPI.h>
#include <SdFat.h>
#include "indicador.h"

// Conexiones físicas SD:
#define SPI_SPEED_SD 4000000 // 4MHz
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
SdFat SD;


File dataFile;

void SDSet(){
  if (!SD.begin(CS, SPI_SPEED_SD))
  {
    Serial.println("Error al inicializar la tarjeta SD.");
    errSD();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error al inicializar la tarjeta microSD");
    while (1);
  }
}

bool CreateCSV(String createcsv[])
{
  if (!SD.begin(CS, SPI_SPEED_SD))
  {
    Serial.println("Error al inicializar la tarjeta SD.");
    //errLed();
    while (1);
    return false;
  }
  else
  {
    for (int i = 0; i < 1; i++)
    {
      delay(100);
      if (!SD.exists(createcsv[i]))
      {
        dataFile = SD.open(createcsv[i], FILE_WRITE);
        if (dataFile)
        {
          dataFile.println("TDS, Temperatura, Conductividad, pH, Lat, Lng , Fecha , Hora ");
          Serial.print("Archivo ");
          Serial.print(createcsv[i]);
          Serial.println(" creado.");
          dataFile.close(); // Cierra el archivo después de escribir.
         // okLed();
          return true; // Sale de la función indicando éxito.
        }
        else
        {
          Serial.println("Error al abrir el archivo para creación.");
         // errLed();
          return false; // Sale de la función indicando fallo.
        }
      }
    }
  }
  return true; // Si el archivo ya existe, también indica fallo.
}//end bool

//guardad datos en csv 
void calculoGeneralCSV(double lat, double lng, float tds, float temperature, float conductivity, float pH, DateTime fecha, String archivo)
{
  if (!SD.begin(CS, SPI_SPEED_SD))
  {
    Serial.println("Error al inicializar la tarjeta SD.");
    errSD();
  }
  else
  {
    dataFile = SD.open(archivo, FILE_WRITE);
    if (dataFile)
    {
      
      dataFile.print(tds, 2);
      dataFile.print(",");
      dataFile.print(temperature, 2);
      dataFile.print(",");
      dataFile.print(conductivity, 4);
      dataFile.print(",");
      dataFile.print(pH, 2);
      dataFile.print(",");
      dataFile.print(lat, 6);
      dataFile.print(",");
      dataFile.print(lng, 6);
      dataFile.print(",");
      dataFile.print(fecha.year(), DEC);
      dataFile.print('/');
      dataFile.print(fecha.month(), DEC);
      dataFile.print('/');
      dataFile.print(fecha.day(), DEC);
      dataFile.print(",");
      dataFile.print(fecha.hour(), DEC);
      dataFile.print(':');
      dataFile.print(fecha.minute(), DEC);
      dataFile.print(':');
      dataFile.print(fecha.second(), DEC);
      dataFile.println();
      dataFile.close();
      Serial.println();
      Serial.println("Información guardada en la tarjeta SD.");
      okcreacionSD();
    }
    else
    {
      Serial.println("Error al abrir el archivo para escritura.");
      errSD();
    }
    dataFile.close();
  }
}
