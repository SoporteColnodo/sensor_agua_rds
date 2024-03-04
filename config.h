const char* ssid = "Colnodo Invitados";     // Nombre de la red WiFi
const char* password = "Cm*Go/cwN";  
const char* hostname = "ESP32AIRE_01"; // nombre del dispositivo

extern String createcsv[] = {"generalaguacolnodo001.csv"};//archivos que se crean
extern String User = "esp32aguacolnodo_001@colnodo.apc.org";//nombre del dispositivo en AA
extern String Password = "V4X20PbA22xG26*";//contraseña del dispositivo en AA

//Si los dispositivos no se sincronizan o no responden este tiempo puede cambiar es decir el delay no es constante debido
// a que funciona a partir de la sincronización de dispositivos externos
extern unsigned long previousMillisAPI = 0; //tiempo inicial de envio del API
extern unsigned long previousMillisSD = 0; //tiempo inicial de envio a SD
extern const long intervalAPI = 60000; // tiempo de ciclo envio al API en milisegundos
extern const long intervalSD = 30000;  // tiempo de ciclo envio a SD en milisegundos

IPAddress ip(192, 168, 7, 303); //IP del dispositivo
IPAddress gateway(192, 168, 7, 1); //IP-gateway del dispositivo
IPAddress subnet(255, 255, 255, 0); //IP-subnet del dispositivo