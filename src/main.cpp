#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h> // Biblioteca do RTC
#include <Protocentral_ADS1220.h>

#define SD_CS 32 // Pino CS do SD

#define ADS_CS 5 // Pino CS do ADS
#define ADS_DRDY 4
#define n_leituras 10 // Nº de dados armazenados

float leituras[n_leituras]; 
int quantidadeLeituras = 0;

RTC_DS3231 rtc; // Objeto do RTC
Protocentral_ADS1220 ads;
String dataAtual = ""; // Para armazenar data formatada
String horaAtual = ""; // Armazenar a hora formatada

// Função de reinício do ESP
void reiniciarESP() {
  Serial.println("Erro crítico. Reiniciando ESP32 em 5 segundos...");
  delay(5000);
  // Criar limite de reinícios e aviso de erro com log.
  ESP.restart();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Wire.begin();

  // configuração do ADS com a biblioteca Protocentral
  ads.begin(ADS_CS, ADS_DRDY);
  ads.select_mux_channels(MUX_AIN0_AIN1); // Seta a medição
  ads.set_pga_gain(PGA_GAIN_1); // Seta o ganho (do amp interno)
  ads.set_data_rate(DR_20SPS); // Velocidade de medição
  ads.set_OperationMode(MODE_NORMAL);
  ads.set_conv_mode_single_shot();
  ads.set_VREF(VREF_REFP0);
  Serial.println("ADS1220 Configurado!");

  if (!rtc.begin()) {
    Serial.println("Erro ao iniciar o RTC!");
    reiniciarESP(); 
  }

  // ajusta a hora do RTC
  if (rtc.lostPower()) {
    Serial.println("RTC perdeu o poder. Ajustando para a hora de compilação.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Criar outro arquivo para re-calibrar RTC em caso de perda
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
  float vref = 3.3;
  float full_scale = 8388607.0;

  // Leitura do RTC em formato BR
  DateTime now = rtc.now();
  dataAtual = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  horaAtual = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  int32_t raw_data = ads.Read_SingleShot_WaitForData();

  float medicaoConvertida = (raw_data/full_scale)*vref;
  leituras[quantidadeLeituras++] = medicaoConvertida;

  // // Leitura do ADC (Piranômetro exemplo)
  // int leituraADC = analogRead(ADS_CS);
  // float tensao = (leituraADC / 4095.0) * 3.3; // Conversão Teórica
  // leituras[quantidadeLeituras++] = tensao;

   // Leitura do sist. de Baterias
  float baterias = 50.0 + random(-10, 10); // Valores teste (A ser substituído)
  leituras[quantidadeLeituras++] = baterias;

  // Leitura de Temperatura
  float temperatura = 25.0 + random(-5, 5); // Valores teste (A ser substituído)
  leituras[quantidadeLeituras++] = temperatura;

  // // Leitura de Umidade 
  // float umidade = 50.0 + random(-10, 10); // Valores teste (A ser substituído)
  // leituras[quantidadeLeituras++] = umidade;
}

// Função para salvar dados no SD
void salvarDadosNoSD() {
  // Criar checagem de armazenagem anterior

  while (true) {
    File dataFile = SD.open("/dados.csv", FILE_APPEND);
    if (dataFile) {
      dataFile.print(dataAtual);
      dataFile.print(","); 
      dataFile.print(horaAtual);
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
  realizarLeituras();      // Coleta de dados
  salvarDadosNoSD();       // Armazena no cartão SD
  uploadNuvem();
  delay(5000);             // Aguarda 5 segundos
}