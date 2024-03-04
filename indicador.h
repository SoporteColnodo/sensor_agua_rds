#ifndef INDICADOR_H
#define INDICADOR_H


#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
int tm = 2000;

void InicioLCD(){
lcd.init();
lcd.backlight();
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("WATER Data: ");
lcd.setCursor(0, 2);
lcd.print("iniciando... ");
delay(5000);
}

void IndicadorGeneral(double latitud, double longitud, float tds, float temperature, float conductivity, float pH) {
 // Mostrar datos en LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.print(tds, 2);
    lcd.print(" mS/cm");

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperature, 2);
    lcd.print(" C");

    lcd.setCursor(0, 2);
    lcd.print("Cond: ");
    lcd.print(conductivity, 4);
    lcd.print(" mS/cm");

    lcd.setCursor(0, 3);
    lcd.print("pH: ");
    lcd.print(pH, 2);
    lcd.print("   ");
  delay(tm);
}

void okguardarSD() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("informacion guardada ok  ");  
  delay(tm);
}

void okcreacionSD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Los archivos csv se ");
  lcd.setCursor(0, 1);
  lcd.print("crearon correctamente");

  delay(tm);
}

void errSD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error al inicializar");
  lcd.setCursor(0, 1);
  lcd.print("la tarjeta SD      ");

  delay(tm);
}

void okApi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enviando Información");
  lcd.setCursor(0, 1);
  lcd.print("al API..............");

  delay(tm);
}

void errApi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error en la conexión");
  lcd.setCursor(0, 1);
  lcd.print("a internet (4g)    ");

  delay(tm);
}

void okGPS() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Señal GPS correcta");

  delay(tm);
}

void errGPS() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Los dispositivos");
  lcd.setCursor(0, 1);
  lcd.print("arrojan valores NULL");

  delay(tm);
}

void reinicioESP() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Se superó el máximo ");
  lcd.setCursor(0, 1);
  lcd.print("de intentos.Reinicio");

  delay(tm);
}

void errRTC() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Error, RTC no proporciona valores validos");

  delay(tm);
}

#endif // INDICADOR_H