//const char* ssid = "Colnodo Invitados";     // Nombre de la red WiFi
//const char* password = "Cm*Go/cwN";  
const char* hostname = "ESP32AGUA_01"; // nombre del dispositivo

//extern String createcsv[] = {"generalaguacolnodo001.csv"};//archivos que se crean
extern String User = "esp32aguacolnodo_001@colnodo.apc.org";//nombre del dispositivo en AA
extern String Password = "V4X20PbA22xG26*";//contraseña del dispositivo en AA
extern String idDispositivo = "dd20844abe8020aec75fc1dcdf26c09a";//id del dispositivo

extern const long minutos = 2;  // tiempo en milisegundos
extern const long intervalSD = minutos * 60000;// 2 minutos tiempo en milisegundos - 60000 tiempo 2 minutos
extern const long hora = 3600000; // tiempo en milisegundos - 3600000 tiempo 1 hora

extern unsigned long previousMillisSD = 0; //tiempo inicial de envio a SD
extern int maxAttempts = 15; //intentos antes de finalizar proceso
extern int currentAttempts = 0;
extern uint32_t maxciclos = 0;
extern int tmg = 100; //valores delay inicio


//Si los dispositivos no se sincronizan o no responden este tiempo puede cambiar es decir el delay no es constante debido
// a que funciona a partir de la sincronización de dispositivos externos
/*extern unsigned long previousMillisAPI = 0; //tiempo inicial de envio del API
extern unsigned long previousMillisSD = 0; //tiempo inicial de envio a SD
extern const long intervalAPI = 60000; // tiempo de ciclo envio al API en milisegundos
extern const long intervalSD = 30000;  // tiempo de ciclo envio a SD en milisegundos
*/
IPAddress ip(192, 168, 7, 303); //IP del dispositivo
IPAddress gateway(192, 168, 7, 1); //IP-gateway del dispositivo
IPAddress subnet(255, 255, 255, 0); //IP-subnet del dispositivo