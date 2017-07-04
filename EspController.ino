/*
 *  Este sketch conecta-se a um wifi server e envia os dados para o robô
 *
 *
 * autor: João Campos
 */

#include <ESP8266WiFi.h>
// Biblioteca Ticker da biblioteca Arduino Esp8266
#include <Ticker.h>

// Dados do robô que está em modo station
const char* ssid     = "robotFC";
const char* password = "robotFC1234";

// botões usados para controlar
// usar os mappings encontrados no seguinte seguintes
// https://github.com/nodemcu/nodemcu-devkit-v1.0
// D8
const int pinButtonFront = 15;
// D1
const int pinButtonLeft = 5;
// D2
const int pinButtonRight = 4;
//D3
const int pinButtonBack = 0;
//D6
const int pinGun1 = 12;
//D10
const int pinGun2 = 1;

// Biblioteca que envia o comando
// para o robô a cada x milisegundos
Ticker command;
/*
 Usar variáveis volatile porque elas são usadas dentro dos interrupts
 */
volatile int motorLeftSpeed = 0;
volatile int motorRightSpeed = 0;
volatile int direction = 0;
volatile int gun1 = 0;
volatile int gun2 = 0;

// Usar porta 23 do telnet, se usar http usar porta 80
WiFiClient client;
const int wifiPort = 23;
// Ip do robô
IPAddress host(192, 168, 4,1);

const int maxVelocity = 1023;


bool flagSendCommand = false;
/**
 * [sendCommand rotina que coloca a flag positiva ]
 */
void sendCommand()
{
 flagSendCommand = true;
}


void pressButtonFront()
{
   direction = 1;
   motorLeftSpeed = maxVelocity;
   motorRightSpeed = maxVelocity;
}
void depressButtonFront()
{
   motorLeftSpeed = 0;
   motorRightSpeed = 0;
}

void pressButtonLeft()
{
   motorLeftSpeed = maxVelocity;
   motorRightSpeed = 0.5*maxVelocity;
}

void pressButtonRight()
{
   motorLeftSpeed = 0.5*maxVelocity;
   motorRightSpeed = maxVelocity;
}

void pressButtonBack()
{
   direction = 0;
   motorLeftSpeed = maxVelocity;
   motorRightSpeed = maxVelocity;
}

void depressButtonBack()
{
   direction = 0;
   motorLeftSpeed = 0;
   motorRightSpeed = 0;
}


void pressGun1()
{
  gun1 = 1023;
}

void depressGun1()
{
  gun1 = 0;
}

void pressGun2()
{
  gun2 = 1023;
}

void depressGun2()
{
  gun2 = 0;
}


void setup() {

// Serial para debug
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Colocar o Esp como cliente para não atuar como station */
  WiFi.mode(WIFI_STA);
  // conectar ao robô
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //  Com pull up a lógica é inversa
  // significando que o pino do microcontrolador está sempre em alto
  //
  pinMode(pinButtonFront, INPUT_PULLUP);
  pinMode(pinButtonBack, INPUT_PULLUP);
  pinMode(pinButtonLeft, INPUT_PULLUP);
  pinMode(pinButtonRight, INPUT_PULLUP);
  pinMode(pinButtonRight, INPUT_PULLUP);

  // manda comando a cada 0.3 segundos
  // usamos aqui o calback sendCommand para ser chamado a cada 0.2 segundos
  command.attach(0.3, sendCommand);

  // Usando interrupções para iniciar commandos
  // Como estamos em pull up quando clicamos no botão temos um falling edge
  attachInterrupt(pinButtonFront, pressButtonFront, FALLING);
  attachInterrupt(pinButtonFront, depressButtonFront, RISING);
  attachInterrupt(pinButtonBack, pressButtonBack, FALLING);
  attachInterrupt(pinButtonBack, depressButtonBack, RISING);
  attachInterrupt(pinButtonRight, pressButtonRight, FALLING);
  attachInterrupt(pinButtonLeft, pressButtonLeft, FALLING);
  attachInterrupt(pinGun1, pressGun1, FALLING);
  attachInterrupt(pinGun1, depressGun1, RISING);
  attachInterrupt(pinGun2, pressGun2, FALLING);
  attachInterrupt(pinGun2, depressGun2, RISING);

}

int value = 0;

void loop() {
// rotina só é executada a cada 300 milisegundos
  if(flagSendCommand){

    if (!client.connect(host, wifiPort)) {
      Serial.println("connection failed");
      return;
    }
    // protocolo de comunicação adotado para o robô
    String command = motorLeftSpeed + ";"
        + motorRightSpeed + ";" + direction +";"+ gun1 + ";" + gun2 +"\n";

    client.print(command);
    flagSendCommand = false;
  }



  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String command = client.readStringUntil('\n');
  //  Serial.print(line);
  }


}
