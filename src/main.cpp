#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h> // Biblioteca do RTC

#define SD_CS 5 // Pino do SD
#define SENSOR_PIN 34 // Pino do ADS
#define n_leituras 10 // Nº de dados armazenados

float leituras[n_leituras]; 
int quantidadeLeituras = 0;

RTC_DS3231 rtc; // Objeto do RTC
String horarioAtual = ""; // Para armazenar data/hora formatada

// Função de reinício do ESP
void reiniciarESP() {
  Serial.println("Erro crítico. Reiniciando ESP32 em 5 segundos...");
  delay(5000);
  // Criar limite de reinícios e aviso de erro com log.
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("Erro ao iniciar o RTC!");
    reiniciarESP(); // Reinicia o ESP32
  }
  // Criar ajuste de data/hora iniciais
  if (rtc.lostPower()) {
    Serial.println("RTC perdeu o poder. Ajustando para a hora de compilação.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (!SD.begin(SD_CS)) {
    Serial.println("Falha na inicialização do SD!");
    reiniciarESP();
  }

  Serial.println("RTC inicializado com sucesso");
  Serial.println("Cartão SD inicializado com sucesso.");
}

// Função para simular várias leituras
void realizarLeituras() {
  quantidadeLeituras = 0; 

  // Leitura do RTC em formato BR
  DateTime now = rtc.now();
  horarioAtual = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + " " +
                 String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  // Leitura do ADC (Piranômetro exemplo)
  int leituraADC = analogRead(SENSOR_PIN);
  float tensao = (leituraADC / 4095.0) * 3.3; // Conversão Teórica
  leituras[quantidadeLeituras++] = tensao;

  // Leitura do sist. de Baterias
  float baterias = 50.0 + random(-10, 10); // Valores teste (A ser substituído)
  leituras[quantidadeLeituras++] = baterias;

  // Leitura de Temperatura
  float temperatura = 25.0 + random(-5, 5); // Valores teste (A ser substituído)
  leituras[quantidadeLeituras++] = temperatura;

  // Leitura de Umidade 
  float umidade = 50.0 + random(-10, 10); // Valores teste (A ser substituído)
  leituras[quantidadeLeituras++] = umidade;
}

// Função para salvar dados no SD
void salvarDadosNoSD() {
  // Criar checagem de armazenagem anterior

  while (true) {
    File dataFile = SD.open("/dados.csv", FILE_APPEND);
    if (dataFile) {
      dataFile.print(horarioAtual);
      dataFile.print(","); 

      for (int i = 0; i < quantidadeLeituras; i++) {
        dataFile.print(leituras[i], 2);
        if (i < quantidadeLeituras - 1) {
          dataFile.print(",");
        }
      }
      dataFile.println();
      dataFile.close();
      Serial.println("Dados salvos no cartão SD.");
      break; 
    } else {
      Serial.println("Erro ao abrir o arquivo para escrita. Tentando novamente em 30 segundos...");
      delay(30000); 
      // Adicionar número máximo de tentativas, log de erro e chamada.
    }
  }
}

void uploadNuvem() {
  // Lora
}

void loop() {
  realizarLeituras();      // Simula coleta de dados
  salvarDadosNoSD();       // Armazena no cartão SD
  uploadNuvem();
  delay(5000);             // Aguarda 5 segundos
}