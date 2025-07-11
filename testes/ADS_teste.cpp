#include <SPI.h>
#include <Protocentral_ADS1220.h>

#define CS_PIN    5
#define DRDY_PIN  4

Protocentral_ADS1220 ads;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("-={Teste do Conversor ADS1220}=-");

  ads.begin(CS_PIN, DRDY_PIN);
  ads.select_mux_channels(MUX_AIN0_AIN1);
  ads.set_pga_gain(PGA_GAIN_1);
  ads.set_data_rate(DR_20SPS);
  ads.set_OperationMode(MODE_NORMAL);
  ads.set_conv_mode_single_shot();
  ads.set_VREF(VREF_REFP0);
  Serial.println("ADS1220 configurado.");
}

void loop() {
  int32_t raw_data = ads.Read_SingleShot_WaitForData();

  float Vref = 3.3;
  float full_scale = 8388607.0; // (2^23 - 1)
  
  float voltage = (raw_data / full_scale) * Vref;

  Serial.print("Valor Bruto (24 bits): ");
  Serial.print(raw_data);
  Serial.print("\t Tensao Medida: ");
  Serial.print(voltage, 6); // 6 casas decimais
  Serial.println(" V");

  delay(5000);
}