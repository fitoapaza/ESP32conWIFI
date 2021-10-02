#include <WiFi.h>

//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char *ssid = "✖‿✖";
const char *password = "0291nino@";

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

String estadoSalida = "off";

const int salida = 2;

//------------------------CODIGO HTML------------------------------
String pagina = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<meta charset='utf-8' />"
                "<title>Servidor Web ESP32</title>"
                "</head>"
                "<body>"
                "<center>"
                "<h1>Luz de la habitacion de Fito</h1>"
                "<p><a href='/on'><button style='height:50px;width:100px'>ON</button></a></p>"
                "<p><a href='/off'><button style='height:50px;width:100px'>OFF</button></a></p>"
                "</center>"
                "</body>"
                "</html>";

//---------------------------SETUP--------------------------------
void setup()
{
  Serial.begin(115200);
  Serial.println("Se inicializo con una velocidad de 115200 baudios \n");

  pinMode(salida, OUTPUT);
  digitalWrite(salida, LOW);

  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion < 50)
  {
    ++contconexion;
    delay(500);
    Serial.print("No se logró conectar\n");
  }
  Serial.print("Se logró conectar a WIFI con exito. \n");

  if (contconexion < 50)
  {
    //para usar con ip fija
    IPAddress ip(192, 168, 0, 180);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println(WiFi.localIP());
    server.begin(); // iniciamos el servidor
  }
  else
  {
    Serial.println("");
    Serial.println("Error de conexion");
  }
}

//----------------------------LOOP----------------------------------

void loop()
{
  WiFiClient client = server.available(); // Escucha a los clientes entrantes

  if (client)
  {                                // Si se conecta un nuevo cliente
    Serial.println("New Client."); //
    String currentLine = "";       //
    while (client.connected())
    { // loop mientras el cliente está conectado
      if (client.available())
      {                         // si hay bytes para leer desde el cliente
        char c = client.read(); // lee un byte
        Serial.write(c);        // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n')
        { // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // enciende y apaga el GPIO
            if (header.indexOf("GET /on") >= 0)
            {
              Serial.println("GPIO on");
              estadoSalida = "on";
              digitalWrite(salida, HIGH);
            }
            else if (header.indexOf("GET /off") >= 0)
            {
              Serial.println("GPIO off");
              estadoSalida = "off";
              digitalWrite(salida, LOW);
            }

            // Muestra la página web
            client.println(pagina);

            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          }
          else
          { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // si C es distinto al caracter de retorno de carro
          currentLine += c; // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}