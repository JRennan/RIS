#include <SPI.h>
#include <SD.h>

// ARQUIVO TESTE PARA MÓDULO MICROSD
const int csPin = 5;

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Teste Simples do Cartao SD ---");

  if (!SD.begin(csPin)) {
    Serial.println("Falha na inicialização");
    return; 
  }
  Serial.println("Cartao SD inicializado com sucesso.");

  File meuArquivo = SD.open("/log.txt", FILE_WRITE);

  if (meuArquivo) {
    Serial.print("Escrevendo no arquivo /log.txt...");
    meuArquivo.println("Teste de escrita funcionou.");

    meuArquivo.close();
    Serial.println(" OK.");
  } else {
    Serial.println("Erro ao abrir o arquivo para escrita.");
    return;
  }

  meuArquivo = SD.open("/log.txt");
  if (meuArquivo) {
    Serial.println("\nConteúdo lido do arquivo /log.txt:");

    while (meuArquivo.available()) {
      Serial.write(meuArquivo.read());
    }

    meuArquivo.close();
  } else {
    Serial.println("Erro ao abrir o arquivo /log.txt para leitura.");
    return;
  }
  
  Serial.println("\nfim do teste");
}

void loop() {
  //
}