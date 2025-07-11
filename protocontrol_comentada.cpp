//////////////////////////////////////////////////////////////////////////////////////////
//
//    Arduino library for the ADS1220 24-bit ADC breakout board
//
//    Author: Ashwin Whitchurch
//    Copyright (c) 2018 ProtoCentral
//
//    Based on original code from Texas Instruments
//
//    This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//  For information on how to use, visit https://github.com/Protocentral/Protocentral_ADS1220/
//
/////////////////////////////////////////////////////////////////////////////////////////

// Garante que este arquivo seja incluído apenas uma vez durante a compilação, evitando erros.
#pragma once

// Inclui as definições básicas da plataforma Arduino (ex: HIGH, LOW, pinMode).
#include "Arduino.h"
// Inclui a biblioteca para comunicação SPI, essencial para conversar com o ADS1220.
#include <SPI.h>

// --- SEÇÃO DE COMANDOS SPI ---
// Estes são os bytes de comando que o ESP32 envia para dar ordens ao ADS1220.
#define SPI_MASTER_DUMMY   0xFF  // Byte "vazio" enviado quando o ESP32 só quer receber dados do ADC.
#define RESET              0x06  // Comando para resetar o chip para suas configurações de fábrica.
#define START              0x08  // Comando para iniciar uma conversão de dados.
#define WREG               0x40  // Comando base para "Escrever em um Registrador" (Write Register).
#define RREG               0x20  // Comando base para "Ler um Registrador" (Read Register).

// --- SEÇÃO DE ENDEREÇOS DOS REGISTRADORES ---
// O ADS1220 tem 4 registradores internos para guardar as configurações. Estes são seus endereços.
#define CONFIG_REG0_ADDRESS 0x00
#define CONFIG_REG1_ADDRESS 0x01
#define CONFIG_REG2_ADDRESS 0x02
#define CONFIG_REG3_ADDRESS 0x03

// --- SEÇÃO DE MÁSCARAS DE BITS (AVANÇADO) ---
// Usado internamente pela biblioteca para isolar ou modificar bits específicos em um registrador.
#define REG_CONFIG3_IDAC1routing_MASK    0xE0
#define REG_CONFIG3_IDAC2routing_MASK    0x1C
#define REG_CONFIG2_VREF_MASK            0xC0
#define REG_CONFIG2_FIR_MASK             0x30
#define REG_CONFIG2_IDACcurrent_MASK     0x07
#define REG_CONFIG1_MODE_MASK            0x18
#define REG_CONFIG1_DR_MASK              0xE0
#define REG_CONFIG0_PGA_GAIN_MASK        0x0E
#define REG_CONFIG0_MUX_MASK             0xF0

// --- SEÇÃO DE CONFIGURAÇÕES POSSÍVEIS (CONSTANTES) ---
// Estes são os "valores" que você usa nas funções de configuração.

// IDAC: Fontes de Corrente Internas (usadas para sensores como PT100/RTD)
#define IDAC1_disable     0x00
#define IDAC1_AIN0        0x20
// ... (outras opções de roteamento do IDAC1)
#define IDAC2_disable     0x00
// ... (outras opções de roteamento do IDAC2)

// Corrente do IDAC
#define IDAC_OFF      0x00
#define IDAC_10       0x01
// ... (outras opções de corrente)

// Filtro Digital FIR (para rejeitar ruído de 50/60Hz da rede elétrica)
#define FIR_OFF       0x00
#define FIR_5060      0x10
#define FIR_50Hz      0x20
#define FIR_60Hz      0x30

// Fonte de Tensão de Referência (VREF) - Define a "régua" da medição
#define VREF_2048       0x00  // Usa a referência interna de 2.048V.
#define VREF_REFP0      0x40  // Usa os pinos externos REFP0 e REFN0 como referência.
#define VREF_AIN0       0x80  // Usa os pinos AIN0/AIN1 como referência.
#define VREF_ANALOG     0xC0  // Usa a própria alimentação analógica (AVDD) como referência.

// Modo de Operação (trade-off entre velocidade e consumo de energia)
#define MODE_NORMAL     0x00
#define MODE_DUTYCYCLE  0x08
#define MODE_TURBO      0x10
#define MODE_RESERVED   0x18

// Taxa de Amostragem (Data Rate - Amostras Por Segundo, SPS)
#define DR_20SPS    0x00
#define DR_45SPS    0x20
#define DR_90SPS    0x40
#define DR_175SPS   0x60
#define DR_330SPS   0x80
#define DR_600SPS   0xA0
#define DR_1000SPS  0xC0

// Ganho do Amplificador (PGA - Programmable Gain Amplifier) - Amplifica sinais pequenos
#define PGA_GAIN_1   0x00  // Sem ganho
#define PGA_GAIN_2   0x02  // 2x
#define PGA_GAIN_4   0x04  // 4x
#define PGA_GAIN_8   0x06  // 8x
#define PGA_GAIN_16  0x08  // 16x
#define PGA_GAIN_32  0x0A  // 32x
#define PGA_GAIN_64  0x0C  // 64x
#define PGA_GAIN_128 0x0E  // 128x

// MUX: Multiplexador - Seleciona quais pinos de entrada (AIN) serão medidos
#define MUX_AIN0_AIN1   0x00  // Medição diferencial entre AIN0 (positivo) e AIN1 (negativo)
#define MUX_AIN0_AIN2   0x10
// ... (outras combinações diferenciais)
#define MUX_AIN0_AVSS   0x80  // Medição single-ended do AIN0 em relação ao terra (AGND)
#define MUX_AIN1_AVSS   0x90
// ... (outras combinações single-ended)

// --- FIM DA SEÇÃO DE CONSTANTES ---


// --- DECLARAÇÃO DA CLASSE ---
// Define a estrutura da biblioteca, com suas variáveis e funções.
class Protocentral_ADS1220
{
      // --- SEÇÃO PRIVADA ---
      // Variáveis e funções internas, não devem ser usadas diretamente no seu código.
      private:
            uint8_t m_config_reg0;
            uint8_t m_config_reg1;
            uint8_t m_config_reg2;
            uint8_t m_config_reg3;

            uint8_t m_drdy_pin;  // Armazena o pino DRDY
            uint8_t m_cs_pin;    // Armazena o pino CS

      // --- SEÇÃO PÚBLICA ---
      // Funções que você pode chamar a partir do seu código principal (sketch).
      public:
            // Construtor da classe (chamado automaticamente ao criar o objeto)
            Protocentral_ADS1220();
            // Função de inicialização
            void begin(uint8_t cs_pin, uint8_t drdy_pin);

            // Imprime os valores atuais dos 4 registradores de configuração (útil para debug)
            void PrintRegisterValues();

            // Envia o comando para iniciar uma nova conversão
            void Start_Conv(void);
            // Envia o comando para resetar o chip
            void ads1220_Reset(void);

            // Funções de baixo nível para comunicação SPI
            void SPI_Command(unsigned char data_in);
            void writeRegister(uint8_t address, uint8_t value);
            uint8_t readRegister(uint8_t address);

            // Lê os 3 bytes de dados brutos da conversão
            uint8_t * Read_Data(void);
            // Espera o sinal DRDY e retorna o resultado da conversão como um inteiro de 32 bits
            int32_t Read_WaitForData();

            // --- FUNÇÕES DE CONFIGURAÇÃO (AS MAIS USADAS) ---
            
            // Registrador 0
            void select_mux_channels(int channels_conf); // Seleciona os canais de entrada (ex: MUX_AIN0_AIN1)
            void set_pga_gain(int pgagain);              // Configura o ganho (ex: PGA_GAIN_1)
            void PGA_OFF(void);                          // Desliga o PGA (amplificador)
            void PGA_ON(void);                           // Liga o PGA
            
            // Registrador 1
            void set_data_rate(int datarate);           // Configura a taxa de amostragem (ex: DR_20SPS)
            void set_OperationMode(int OPmode);         // Configura o modo de operação (ex: MODE_NORMAL)
            void set_conv_mode_single_shot(void);       // Configura para o modo de conversão única
            void set_conv_mode_continuous(void);        // Configura para o modo de conversão contínua
            void TemperatureSensorMode_enable(void);    // Habilita o sensor de temperatura interno
            void TemperatureSensorMode_disable(void);   // Desabilita o sensor de temperatura
            
            // Registrador 2
            void set_VREF(int vref);                    // Configura a fonte de tensão de referência (ex: VREF_REFP0)
            void set_FIR_Filter(int filter);            // Configura o filtro de 50/60Hz (ex: FIR_5060)
            void set_IDAC_Current(int IDACcurrent);     // Configura a corrente do IDAC (ex: IDAC_1000)
            
            // Registrador 3
            void set_IDAC1_Route(int IDAC1routing);     // Configura para onde a corrente do IDAC1 será enviada
            void set_IDAC2_Route(int IDAC2routing);     // Configura para onde a corrente do IDAC2 será enviada
            
            // --- FUNÇÕES DE ALTO NÍVEL (MUITO ÚTEIS) ---

            // Combina "Start_Conv", a espera pelo DRDY e a leitura dos dados em uma única chamada.
            int32_t Read_SingleShot_WaitForData(void);
            // Faz o mesmo, mas configura o MUX para um canal single-ended antes da leitura.
            int32_t Read_SingleShot_SingleEnded_WaitForData(uint8_t channel_no);
};