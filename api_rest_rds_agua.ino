/*
El proceso de toma de datos en la siguiente aplicación consta de las siguiente etapas:
1.Cargue de dispositivos en serial2 de ESP32 (respuesta correcta de GPS,  temperatura, ads Y SD)
2.Cada ciclo de toma de información de los dispositivos se evalua para que sean diferentes a O (cero)
3.Se genera un ciclo especial de 5 segundos 5000 ms para enviar información a la SD
4.Si la información enviada a SD es correcta se guarda en un archivo distinto cada hora (ver nota)
5. Si hay un error en la conexión a wifi, se crea un archivo de respaldo para subir la información manualmente

La hora se estima a partir de los ciclos que se programaron de 2 minutos restandolos de la hora actual generada
por el RTC
*/

#include "RTC_Utils.hpp"
#include <TinyGPSPlus.h>
#include "config.h"
#include "indicador.h"
#include "Wifi_Utils.hpp"
#include "SD_Utils.hpp"
#include "API.hpp"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Configuración del módulo GPS
HardwareSerial gpsSerial(2);  // Usa Serial2 (TX2 y RX2)
TinyGPSPlus gps;


Adafruit_ADS1115 ads;

// variables muestra en pantalla -PMS -SD nombre
bool mostrarIndicador = true;

const int CONDUCTIVITY_PIN = 0;     // pin A0 sensor conductividad
const int PH_PIN = 1;               // pin A1 sensor ph
const int DS18B20_PIN = 33;         // pin sensor temperatura
double latitud = 0.0;   // latitud
double longitud = 0.0;  // longitud
String ssid = "";
String password = "";

//pulsador
const int pinPulsador = 2; // Pin donde está conectado el pulsador
const int pinLED = 13; // Pin donde está conectado el LED
unsigned long tiempoInicio = 0; // Variable para almacenar el tiempo de inicio del temporizador
bool ledEncendido = false; // Variable para controlar si el LED está encendido

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

DateTime fecha;

unsigned long previousMillis = 0;
const long interval = 5000;



void setup() {
RTCSet();
SDSet();
// Inicializar la pantalla LCD
InicioLCD();
lcd.init();
lcd.backlight();
Serial.begin(9600);
gpsSerial.begin(9600);
Wire.begin();
delay(2000);
pinMode(pinPulsador, INPUT); // Configurar el pin del pulsador como entrada
pinMode(pinLED, OUTPUT); // Configurar el pin del LED como salida

// inicializar ads
if (!ads.begin()) {
    Serial.println("¡Error al inicializar el ADS1115!");
    while (1);
  }
  Serial.println("¡ADS1115 inicializado correctamente!");
  sensors.begin();

fecha = DS1307_RTC.now();
uint32_t milisegundosEnHora = fecha.minute() * 60000 + fecha.second() * 1000;
maxciclos = (hora - milisegundosEnHora) / intervalSD;
}//fin setup


void loop() {

// Leer el estado del pulsador
  int estadoPulsador = digitalRead(pinPulsador);
 // Comprobar si el pulsador está presionado
  if (estadoPulsador == HIGH) {
    // El pulsador está presionado
    if (!ledEncendido) {
      // Si el LED no está encendido, encenderlo y comenzar el temporizador
      digitalWrite(pinLED, HIGH); // Encender el LED
      tiempoInicio = millis(); // Guardar el tiempo de inicio del temporizador
      ledEncendido = true; // Actualizar el estado del LED a encendido
    }
  } else {
    // El pulsador no está presionado
    if (ledEncendido) {
      // Si el LED está encendido, verificar si ha pasado el tiempo y apagarlo
      unsigned long tiempoActual = millis(); // Obtener el tiempo actual
      if (tiempoActual - tiempoInicio >= 5000) {
        // Si han pasado 5 segundos, apagar el LED
        digitalWrite(pinLED, LOW); // Apagar el LED
        ledEncendido = false; // Actualizar el estado del LED a apagado
      }
    }
  }


ssid = LeerSSID();
password = LeerPassword();
unsigned long currentMillis = millis();

float temperature = getTemperature(); //toma temperatura
float conductivity = getConductivity(temperature);//toma conductividad
float pH = getPH(); //capturar PH 
float tds = calculateTDS(temperature, conductivity, pH); //captura tds
generarLatLng(); // muestra latitud longitud

if (mostrarIndicador) {
IndicadorGeneral(latitud, longitud, tds, temperature, conductivity, pH); 
}
          //impreson monitor serial
          Serial.println();
          Serial.print("TDS: ");
          Serial.print(tds, 2);
          Serial.print(", Temperatura: ");
          Serial.print(temperature, 2);
          Serial.print(" °C, Conductividad: ");
          Serial.print(conductivity, 4);
          Serial.print(" mS/cm, pH: ");
          Serial.print(pH, 2);
          Serial.print(", Latitud: ");
          Serial.print(latitud, 6);
          Serial.print(", Longitud: ");
          Serial.print(longitud, 6);

mostrarIndicador = true;
Serial.print("Maxciclos: ");
Serial.println(maxciclos);

if (currentMillis - previousMillisSD >= intervalSD) {
    static int contador = 0;
    static double promediotds = 0.0;
    static double promediotemperature = 0.0;
    static double promedioconductivity = 0.0;
    static double promediopH = 0.0;
    static String nombreArchivo;
    // Actualiza el tiempo del último envío
    previousMillisSD = currentMillis;
    DateTime fecha = DS1307_RTC.now();
   
  Serial.print("Maxciclos: ");
  Serial.println(maxciclos);
  if (latitud != 0.0 && longitud != 0.0 && tds != 0.0 && temperature != 0.0 && conductivity != 0.0 && pH != 0.0) { 
  nombreArchivo = String(fecha.day()) + "-" + String(fecha.month()) + "-" + String(fecha.year()) +  "-" + String(fecha.hour()) + ".csv";
  crearArchivo(nombreArchivo);
  okcreacionSD();
  guardaInformacionCSV(latitud, longitud, tds, temperature, conductivity, pH, fecha, nombreArchivo, idDispositivo);
  mostrarIndicador = false;
  okguardarSD();

  //temporal mayo
  ConnectWiFi_STA(false, ssid, password);
  CrearRegistro(latitud, longitud, tds, temperature, conductivity, pH, LoginUser(User, Password));
  WiFi.disconnect(true);
  delay(tmg);
  mostrarIndicador = false;
  okApi();//indicador
//fin temporal mayo

contador++;
if (contador >= maxciclos) {
promediotds = calcularPromediotds(nombreArchivo,maxciclos,"pm25");// se lee archivo y calculan promedios
promediotemperature = calcularPromeditemperature(nombreArchivo,maxciclos,"temperature");// se lee archivo y calculan promedios
promedioconductivity = calcularPromedioconductivity(nombreArchivo,maxciclos,"conductivity");// se lee archivo y calculan promedios
promediopH = calcularPromediopH(nombreArchivo,maxciclos,"ph");// se lee archivo y calculan promedios

if(ConnectWiFi_STA(false,ssid,password)&& promediotds > 0.0 && promediotemperature > 0.0 && promedioconductivity > 0.0 && promediopH > 0.0){
CrearRegistro(latitud, longitud, promediotds, promediotemperature, promedioconductivity, promediopH,  LoginUser(User, Password));
WiFi.disconnect(true);
delay(tmg);
mostrarIndicador = false;
okApi();//indicador
}else{
crearArchivo("informacion_manual_aa.csv");
guardaInformacionCSV(latitud, longitud, promediotds, promediotemperature, promedioconductivity, promediopH, fecha, "informacion_manual_aa.csv", idDispositivo);
delay(tmg);
mostrarIndicador = false;
errApi();
}
contador = 0;
}
} else {
Serial.println("Error: GPS no proporciona valores válidos para la API.");
mostrarIndicador = false;
errGPS();
currentAttempts++;
if (currentAttempts >= maxAttempts) {
Serial.println("Se superó el número máximo de intentos. Reiniciando.");
reinicioESP();
while (true) {
ESP.restart();
}
}
}
}
}//end loop


void generarLatLng() {
while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      latitud = gps.location.lat();
      longitud = gps.location.lng();
      //Serial.println(latitud);
      //Serial.println(longitud);
    }
  }
}



//capturar temperatura 
float getTemperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
//capturar conductividad
float getConductivity(float temperature) {
  int16_t adcValue = ads.readADC_SingleEnded(CONDUCTIVITY_PIN);
  float conductivity_K = 0.996;  // Ajusta según las características de tu sensor.

  // Corrección de la conductividad según la temperatura
  float correctedConductivity = adcValue * (conductivity_K / 1000.0);
  correctedConductivity *= temperature / 25.0;  // Ajuste según la temperatura (ejemplo simplificado)

  return correctedConductivity;
}

//capturar PH 
float getPH() {
  // Realizar múltiples lecturas del sensor de pH para suavizar los valores
  const int numReadings = 10;
  int adcSum = 0;
  for (int i = 0; i < numReadings; i++) {
    adcSum += ads.readADC_SingleEnded(PH_PIN);
    delay(10); // Pequeño retraso entre lecturas
  }
  int adcAverage = adcSum / numReadings;

  // Convertir el valor de voltaje medido a un valor de pH utilizando una ecuación lineal
  // Ajusta los valores de mapeo según sea necesario para tu sensor de pH específico
  // Puedes necesitar calibrar el sensor y ajustar los valores de mapeo según las mediciones reales
  float voltage = adcAverage * (5.0 / 32767.0); // Convertir el valor ADC a voltaje (0-5V)
  float pHValue = mapFloat(voltage, 0.0, 5.0, 0.0, 14.0); // Convertir el voltaje a pH (0-14)

  return pHValue;
}

// Función auxiliar para mapear un valor float
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
//calcula tds
float calculateTDS(float temperature, float conductivity, float pH) {
  // Constante de conversión
  float Ke = 0.7;

  // Calcular TDS usando la ecuación TDS = Ke * CE
  float TDS = Ke * conductivity;

  return TDS;
}


