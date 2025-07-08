//ARQUIVO TESTE PARA as FUNÇÕES DO RTC.

// comunicação I2C e para o RTC.

#include <Wire.h>
#include <RTClib.h>

// objeto "rtc" para módulo DS3231.
RTC_DS3231 rtc;

// Array para dia da semana em texto.
char diasDaSemana[7][15] = {"Domingo", "Segunda-feira", "Terca-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sabado"};

void setup() {
  // Inicia a comunicação Serial para podermos ver as informações no Monitor Serial.
  Serial.begin(115200);
  delay(1000); // Uma pequena pausa para garantir que o Monitor Serial esteja pronto.

  // Tenta iniciar a comunicação com o módulo RTC.
  if (!rtc.begin()) {
    Serial.println("Nao foi possivel encontrar o RTC! Verifique as conexoes.");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC perdeu energia! Ajustando a hora com a do computador...");
    // ajusta o RTC para a data e hora em que este código foi compilado.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC ja esta em funcionamento com a hora correta.");
  }
}

void loop() {
  // ---- FUNÇÃO PRINCIPAL: Ler a hora atual ----
  // Pega todas as informações de data e hora atuais do RTC e armazena no objeto "agora".
  DateTime agora = rtc.now();

  // Imprime a data no formato DD/MM/AAAA
  Serial.print("Data: ");
  if (agora.day() < 10) Serial.print('0');
  Serial.print(agora.day());
  Serial.print('/');
  if (agora.month() < 10) Serial.print('0');
  Serial.print(agora.month());
  Serial.print('/');
  Serial.print(agora.year());
  
  // Imprime o dia da semana em texto
  Serial.print(" (");
  Serial.print(diasDaSemana[agora.dayOfTheWeek()]);
  Serial.print(")");

  // Imprime a hora no formato hh:mm:ss
  Serial.print(" - Hora: ");
  if (agora.hour() < 10) Serial.print('0');
  Serial.print(agora.hour());
  Serial.print(':');
  if (agora.minute() < 10) Serial.print('0');
  Serial.print(agora.minute());
  Serial.print(':');
  if (agora.second() < 10) Serial.print('0');
  Serial.print(agora.second());
  Serial.println();
  
  // ---- FUNÇÃO PRINCIPAL: Ler a temperatura ----
  // O DS3231 possui um sensor de temperatura integrado.
  Serial.print("Temperatura: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");
  Serial.println("---------------------------------------");

  // Espera 3 segundos antes de ler e imprimir novamente.
  delay(3000);
}