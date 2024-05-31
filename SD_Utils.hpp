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

bool crearArchivo(String archivo)
{
  if (!SD.begin(CS, SPI_SPEED_SD))
  {
    Serial.println("Error al inicializar la tarjeta SD.");
    errSD();
    while (1);
    return false;
  }else {
      delay(100);
      if (!SD.exists(archivo))
      {
        dataFile = SD.open(archivo, FILE_WRITE);
        if (dataFile)
        {
          dataFile.println("TDS, Temperatura, Conductividad, pH, Lat, Lng , idDispositivo, FechaEpoch, Fecha , Hora ");
          dataFile.close(); // Cierra el archivo después de escribir.
          Serial.print("Archivo ");
          Serial.print(archivo);
          Serial.println(" creado.");
          okcreacionSD();
          return true; // Sale de la función indicando éxito.
        }
        else
        {
          Serial.println("Error al abrir el archivo para creación.");
          errSD();
          return false; // Sale de la función indicando fallo.
        }
      }
    }
  return true; // Si el archivo ya existe, también indica fallo.
}//end bool


//guardad datos en csv 
void guardaInformacionCSV(double lat, double lng, float tds, float temperature, float conductivity, float pH, DateTime fecha, String archivo, String idDispositivo)
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

      // Calculamos el epoch time a partir del objeto DateTime
      uint32_t epochTime = fecha.unixtime();

      dataFile.print("Agua-");
      dataFile.print(epochTime);
      dataFile.print(",");
      dataFile.print("agua");
      dataFile.print(",");
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
      dataFile.print(idDispositivo);
      dataFile.print(",");
      dataFile.print(epochTime); // Guardamos el epoch time
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


// promedio tds
double calcularPromediotds(String archivo,int ciclos, String tds) {
  File dataFile = SD.open(archivo);

  if (dataFile) {
    Serial.println("Calculando...");
    String encabezados;
    String valorestds;
    float sum = 0.0;
    int count = 0;
    int cuentaFila = 0;

    while (dataFile.available()) {
      encabezados = dataFile.readStringUntil('\n');
      cuentaFila++;

      if (cuentaFila >= 2 && cuentaFila <= ciclos+1) {
        size_t commaIndex = encabezados.indexOf(',');

        if (commaIndex != -1) {
          if (tds == "tds") {
            valorestds = encabezados.substring(0, commaIndex);
          } else {
            size_t secondCommaIndex = encabezados.indexOf(',', commaIndex + 1);
            
            if (secondCommaIndex != -1) {
              valorestds = encabezados.substring(commaIndex + 1, secondCommaIndex);
            } else {
              valorestds = encabezados.substring(commaIndex + 1);
            }
          }

          Serial.println("valorestds: ");
          Serial.print(valorestds);

          float dB = valorestds.toFloat();
          sum += dB;
          Serial.println(" suma: ");
          Serial.print(sum);
          count++;
        } else {
          Serial.println("La línea no contiene una coma.");
        }
      }

      if (cuentaFila == ciclos+1) {
        break;
      }
    }
    dataFile.close();

    if (count > 0) {
      double average = sum / count;
      Serial.print("Promedio de 'tds' del archivo CSV: ");
     char buffer[10];
      snprintf(buffer, sizeof(buffer), "%.2f", average);
      Serial.println(buffer);
      return atof(buffer);
    } else {
      Serial.println("No se encontraron valores en el rango de filas en la columna 'tds' ");
    }
  } else {
    Serial.println("Error al abrir el archivo CSV.");
    errSD();
  }
  return 0.0;
}//promedio tds


//promedio temperatura 
double calcularPromeditemperature(String archivo,int ciclos, String temperature ) {
  File dataFile = SD.open(archivo);

  if (dataFile) {
    Serial.println("Calculando...");
    String encabezados;
    String valorestemperature;
    float sum = 0.0;
    int count = 0;
    int cuentaFila = 0;

    while (dataFile.available()) {
      encabezados = dataFile.readStringUntil('\n');
      cuentaFila++;

      if (cuentaFila >= 2 && cuentaFila <= ciclos+1) {
        size_t commaIndex = encabezados.indexOf(',');

        if (commaIndex != -1) {
          if (temperature == "temperature") {
            valorestemperature = encabezados.substring(0, commaIndex);
          } else {
            size_t secondCommaIndex = encabezados.indexOf(',', commaIndex + 1);
            
            if (secondCommaIndex != -1) {
              valorestemperature = encabezados.substring(commaIndex + 1, secondCommaIndex);
            } else {
              valorestemperature = encabezados.substring(commaIndex + 1);
            }
          }

          Serial.println("valorestemperature: ");
          Serial.print(valorestemperature);

          float dB = valorestemperature.toFloat();
          sum += dB;
          Serial.println(" suma: ");
          Serial.print(sum);
          count++;
        } else {
          Serial.println("La línea no contiene una coma.");
        }
      }

      if (cuentaFila == ciclos+1) {
        break;
      }
    }
    dataFile.close();

    if (count > 0) {
      double average = sum / count;
      Serial.print("Promedio de 'temperature' del archivo CSV: ");
     char buffer[10];
      snprintf(buffer, sizeof(buffer), "%.2f", average);
      Serial.println(buffer);
      return atof(buffer);
    } else {
      Serial.println("No se encontraron valores en el rango de filas en la columna 'temperature' ");
    }
  } else {
    Serial.println("Error al abrir el archivo CSV.");
    errSD();
  }
  return 0.0;
}//promedio temperature

//promedio conductividad
double calcularPromedioconductivity(String archivo,int ciclos, String conductivity) {
  File dataFile = SD.open(archivo);

  if (dataFile) {
    Serial.println("Calculando...");
    String encabezados;
    String valoresconductivity;
    float sum = 0.0;
    int count = 0;
    int cuentaFila = 0;

    while (dataFile.available()) {
      encabezados = dataFile.readStringUntil('\n');
      cuentaFila++;

      if (cuentaFila >= 2 && cuentaFila <= ciclos+1) {
        size_t commaIndex = encabezados.indexOf(',');

        if (commaIndex != -1) {
          if (conductivity == "conductivity") {
            valoresconductivity = encabezados.substring(0, commaIndex);
          } else {
            size_t secondCommaIndex = encabezados.indexOf(',', commaIndex + 1);
            
            if (secondCommaIndex != -1) {
              valoresconductivity = encabezados.substring(commaIndex + 1, secondCommaIndex);
            } else {
              valoresconductivity = encabezados.substring(commaIndex + 1);
            }
          }

          Serial.println("valoresconductivity: ");
          Serial.print(valoresconductivity);

          float dB = valoresconductivity.toFloat();
          sum += dB;
          Serial.println(" suma: ");
          Serial.print(sum);
          count++;
        } else {
          Serial.println("La línea no contiene una coma.");
        }
      }

      if (cuentaFila == ciclos+1) {
        break;
      }
    }
    dataFile.close();

    if (count > 0) {
      double average = sum / count;
      Serial.print("Promedio de 'conductivity' del archivo CSV: ");
     char buffer[10];
      snprintf(buffer, sizeof(buffer), "%.2f", average);
      Serial.println(buffer);
      return atof(buffer);
    } else {
      Serial.println("No se encontraron valores en el rango de filas en la columna 'conductivity' ");
    }
  } else {
    Serial.println("Error al abrir el archivo CSV.");
    errSD();
  }
  return 0.0;
}//promedio conductivity


//promedio pH
double calcularPromediopH(String archivo,int ciclos, String pH) {
  File dataFile = SD.open(archivo);

  if (dataFile) {
    Serial.println("Calculando...");
    String encabezados;
    String valorespH;
    float sum = 0.0;
    int count = 0;
    int cuentaFila = 0;

    while (dataFile.available()) {
      encabezados = dataFile.readStringUntil('\n');
      cuentaFila++;

      if (cuentaFila >= 2 && cuentaFila <= ciclos+1) {
        size_t commaIndex = encabezados.indexOf(',');

        if (commaIndex != -1) {
          if (pH == "pH") {
            valorespH = encabezados.substring(0, commaIndex);
          } else {
            size_t secondCommaIndex = encabezados.indexOf(',', commaIndex + 1);
            
            if (secondCommaIndex != -1) {
              valorespH = encabezados.substring(commaIndex + 1, secondCommaIndex);
            } else {
              valorespH = encabezados.substring(commaIndex + 1);
            }
          }

          Serial.println("valorespH: ");
          Serial.print(valorespH);

          float dB = valorespH.toFloat();
          sum += dB;
          Serial.println(" suma: ");
          Serial.print(sum);
          count++;
        } else {
          Serial.println("La línea no contiene una coma.");
        }
      }

      if (cuentaFila == ciclos+1) {
        break;
      }
    }
    dataFile.close();

    if (count > 0) {
      double average = sum / count;
      Serial.print("Promedio de 'pH' del archivo CSV: ");
     char buffer[10];
      snprintf(buffer, sizeof(buffer), "%.2f", average);
      Serial.println(buffer);
      return atof(buffer);
    } else {
      Serial.println("No se encontraron valores en el rango de filas en la columna 'pH' ");
    }
  } else {
    Serial.println("Error al abrir el archivo CSV.");
    errSD();
  }
  return 0.0;
}//promedio pH



// Validar Wifi
void logErrorWifi(DateTime fecha, String archivo)
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
      dataFile.print("Hubo un error al conectarse a la red WIFI");
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

//leer archivo txt
String LeerSSID()
{
    File dataFile = SD.open("config.txt");
    if(dataFile)
    {
        // Leer el SSID
        String ssidLinea = dataFile.readStringUntil('\n');
        int primerComillaSSID = ssidLinea.indexOf('"') + 1;
        int segundaComillaSSID = ssidLinea.indexOf('"', primerComillaSSID);
        String ssid = ssidLinea.substring(primerComillaSSID, segundaComillaSSID);
        // Cerrar el archivo
        dataFile.close();
        return ssid;
    }
    else
    {
        Serial.println("Error al abrir el archivo para lectura.");
        errSD();
        return ""; // Retornar una cadena vacía en caso de error
    }
}

String LeerPassword()
{
    File dataFile = SD.open("config.txt");
    if(dataFile)
    {
        // Saltar la línea del SSID
        dataFile.readStringUntil('\n');
        // Leer la contraseña
        String passwordLinea = dataFile.readStringUntil('\n');
        int primerComillaPass = passwordLinea.indexOf('"') + 1;
        int segundaComillaPass = passwordLinea.indexOf('"', primerComillaPass);
        String password = passwordLinea.substring(primerComillaPass, segundaComillaPass);
        // Cerrar el archivo
        dataFile.close();
        // Retornar la contraseña
        return password;
    }
    else
    {
        Serial.println("Error al abrir el archivo para lectura.");
        errSD();
        return ""; // Retornar una cadena vacía en caso de error
    }
}