#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "Dados.h"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

static const char htmlHead[] PROGMEM = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta charset=\"UTF-8\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>ClickRun</title><style type=\"text/css\">body, html {margin: 0;padding: 0;background: #333333;height: 100vh;min-height;}body {background-repeat: no-repeat;background-position: center center;background-size: contain;background-image: url(https://gabi1415.github.io/arduino/";
static const char htmlMeio[] PROGMEM = ".png);}a {display: block;width: 100%;height: 100%;text-decoration: none;}</style></head><body>";
static const char htmlFinalLink[] PROGMEM = "<a href=\"/iniciar\">&nbsp;</a></body></html>\r\n";
static const char htmlFinalJogo[] PROGMEM = "<script>setTimeout(function () { window.location.href='/jogo'; }, 5000);</script></body></html>\r\n";

#define bot1 D0
#define bot2 10 // SD3
#define led11 D1
#define led12 D2
#define led13 D3
#define led14 D4
#define led21 D8
#define led22 D7
#define led23 D6
#define led24 D5

int vencedor;
int pontos1;
int estadoAnterior1;
int pontos2;
int estadoAnterior2;
long horaUltimoClick1;
long horaUltimoClick2;

void enviarPaginaInicial() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(htmlHead);
  server.sendContent("home");
  server.sendContent(htmlMeio);
  server.sendContent(htmlFinalLink);
}

void enviarAguarde() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(htmlHead);
  server.sendContent("fight");
  server.sendContent(htmlMeio);
  server.sendContent(htmlFinalJogo);
}

void atualizarBarrinha() {
  // Para ligar os leds do jogador 1
  if (pontos1 < 8) {
    // Apaga todos os leds
    digitalWrite(led11, LOW);
    digitalWrite(led12, LOW);
    digitalWrite(led13, LOW);
    digitalWrite(led14, LOW);
  } else if (pontos1 >= 8 && pontos1 < 16) {
    // Apaga tudo, e acende 1 led
    digitalWrite(led11, HIGH);
    digitalWrite(led12, LOW);
    digitalWrite(led13, LOW);
    digitalWrite(led14, LOW);
  } else if (pontos1 >= 16 && pontos1 < 24) {
    // Apaga tudo, e acende 2 leds
    digitalWrite(led11, HIGH);
    digitalWrite(led12, HIGH);
    digitalWrite(led13, LOW);
    digitalWrite(led14, LOW);
  } else if (pontos1 >= 24 && pontos1 < 32) {
    // Apaga tudo, e acende 3 leds
    digitalWrite(led11, HIGH);
    digitalWrite(led12, HIGH);
    digitalWrite(led13, HIGH);
    digitalWrite(led14, LOW);
  } else {
    vencedor = 1;
    // Acende todos os leds
    digitalWrite(led21, LOW);
    digitalWrite(led22, LOW);
    digitalWrite(led23, LOW);
    digitalWrite(led24, LOW);
    digitalWrite(led11, HIGH);
    digitalWrite(led12, HIGH);
    digitalWrite(led13, HIGH);
    digitalWrite(led14, HIGH);
    return;
  }

  // Para ligar os leds do jogador 2
  if (pontos2 < 8) {
    digitalWrite(led21, LOW);
    digitalWrite(led22, LOW);
    digitalWrite(led23, LOW);
    digitalWrite(led24, LOW);
  } else if (pontos2 >= 8 && pontos2 < 16) {
    digitalWrite(led21, HIGH);
    digitalWrite(led22, LOW);
    digitalWrite(led23, LOW);
    digitalWrite(led24, LOW);
  } else if (pontos2 >= 16 && pontos2 < 24) {
    digitalWrite(led21, HIGH);
    digitalWrite(led22, HIGH);
    digitalWrite(led23, LOW);
    digitalWrite(led24, LOW);
  } else if (pontos2 >= 24 && pontos2 < 32) {
    digitalWrite(led21, HIGH);
    digitalWrite(led22, HIGH);
    digitalWrite(led23, HIGH);
    digitalWrite(led24, LOW);
  } else {
    vencedor = 2;
    digitalWrite(led11, LOW);
    digitalWrite(led12, LOW);
    digitalWrite(led13, LOW);
    digitalWrite(led14, LOW);
    digitalWrite(led21, HIGH);
    digitalWrite(led22, HIGH);
    digitalWrite(led23, HIGH);
    digitalWrite(led24, HIGH);
  }
}

void loopJogo() {
  if (vencedor > 0) {
    return;
  }

  long tempoDecorrido;
  int pressionado;
  long agora = millis();
  
  // Código player 1
  pressionado = digitalRead(bot1);
  if (pressionado == 1) {
    if (estadoAnterior1 == 0) {
      estadoAnterior1 = 1;
      horaUltimoClick1 = agora;
      pontos1 = pontos1 + 1;
    }
  } else {
    estadoAnterior1 = 0;
    tempoDecorrido = agora - horaUltimoClick1;
    if (tempoDecorrido > 150 && pontos1 > 0) {
      pontos1 = pontos1 - 1;
      horaUltimoClick1 = agora;
    }
  }

  // Código para o player 2
  pressionado = digitalRead(bot2);
  if (pressionado == 1) {
    if (estadoAnterior2 == 0) {
      estadoAnterior2 = 1;
      horaUltimoClick2 = agora;
      pontos2 = pontos2 + 1;
    }
  } else {
    estadoAnterior2 = 0;
    tempoDecorrido = agora - horaUltimoClick2;
    if (tempoDecorrido > 150 && pontos2 > 0) {
      pontos2 = pontos2 - 1;
      horaUltimoClick2 = agora;
    }
  }
  
  atualizarBarrinha();
}

void executarJogo() {
  vencedor = 0;
  pontos1 = 0;
  estadoAnterior1 = 0;
  pontos2 = 0;
  estadoAnterior2 = 0;
  horaUltimoClick1 = 0;
  horaUltimoClick2 = 0;

  while (vencedor <= 0) {
    loopJogo();
    delay(10);
  }

  digitalWrite(led11, LOW);
  digitalWrite(led12, LOW);
  digitalWrite(led13, LOW);
  digitalWrite(led14, LOW);
  digitalWrite(led21, LOW);
  digitalWrite(led22, LOW);
  digitalWrite(led23, LOW);
  digitalWrite(led24, LOW);
}

void enviarJogo() {
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  executarJogo();

  server.sendContent(htmlHead);

  if (vencedor == 1) {
    server.sendContent("vermelho-vencedor");
  } else {
    server.sendContent("verde-vencedor");
  }

  server.sendContent(htmlMeio);
  server.sendContent(htmlFinalLink);
}

void setup() {
  pinMode(bot1, INPUT);
  pinMode(bot2, INPUT);
  pinMode(led11, OUTPUT);
  pinMode(led12, OUTPUT);
  pinMode(led13, OUTPUT);
  pinMode(led14, OUTPUT);
  pinMode(led21, OUTPUT);
  pinMode(led22, OUTPUT);
  pinMode(led23, OUTPUT);
  pinMode(led24, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", enviarPaginaInicial);
  server.on("/iniciar", enviarAguarde);
  server.on("/jogo", enviarJogo);
  server.onNotFound(enviarPaginaInicial);

  randomSeed(analogRead(0) + millis());

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}
