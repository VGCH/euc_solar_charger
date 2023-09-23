/* Микро ПО модуля управления солнечной зарядной станцией для моноколеса
 * 
 * CYBEREX TECH
 * 2023
 * 
 */
#include "BluetoothSerial.h"                       
#include <EEPROM.h>
#include "SPI.h"
#include <Adafruit_ILI9341.h>
#include "ACS712.h"
#include "logo.h"
#include "FontsRus/TimesNRCyr10.h"               // Русские шрифты
#include "FontsRus/CrystalNormal16.h"

#define voltage_panel    33                      // вход датчика напряжения солнечной панели
#define current_panel    35                      // вход датчика тока солнечной панели
#define voltage_charger  32                      // вход датчика напряжения на выходе преобразователя
#define current_charger  34                      // вход датчика тока на выходе преобразователя
#define flback_i         25                      // выход для отопары обратной связи преобразователя

/*  ACS712 5A  uses 185 mV per A
    ACS712 20A uses 100 mV per A
    ACS712 30A uses  66 mV per A
 
   схема делителя напряжения
             ______      ______
   input----|__1k__|----|__2k__|---GND
                     |
    ADC---------------              
*/       
                                                  // Токовые датчики
ACS712  ACS_input(current_panel, 3.3, 4095, 100); // ток панели 
ACS712  ACS_out(current_charger, 3.3, 4095, 100); // ток зарядного преобразователя
BluetoothSerial ESP_BT;                           // Bluetooth
                                                  // Подключение дисплея по SPI
#define _cs   22                                  // 3 goes to TFT CS
#define _dc   21                                  // 4 goes to TFT DC
#define _mosi 23                                  // 5 goes to TFT MOSI
#define _sclk 18                                  // 6 goes to TFT SCK/CLK
#define _rst  17                                  // ESP RST to TFT RESET
#define _miso                                     // Not connected
#define pwmPin 26                                 // Пин подсветки дисплея

boolean stat, stat2, flyback, calcul;
uint32_t t200ms   = 0;
uint32_t dt200ms  = 0;
uint32_t dt300ms  = 0;
uint32_t mst200ms = 0;
uint32_t mstTms   = 0;
int   counts_mess = 0;


//Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst);
Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _rst);                 // HARDWARE SPI

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){      // колбек подключения к bluetooth
  if(event == ESP_SPP_SRV_OPEN_EVT){
          stat = true;
       }else if(event == ESP_SPP_CLOSE_EVT){
          stat = false;
     }  
  }

int pwmChannel     = 0;             // Selects channel 0
int frequence      = 1000;          // PWM frequency of 1 KHz
int resolution     = 8;             // 8-bit resolution, 256 possible values
float c_panel      = 0;
float v_panel      = 0;
float v_charger    = 0;
float c_charger    = 0;

float c_panel2     = 0;
float v_panel2     = 0;
float v_charger2   = 0;
float c_charger2   = 0;

float c_panel3     = 0;
float v_panel3     = 0;
float v_charger3   = 0;
float c_charger3   = 0;

float set_volt, set_cur, set_volt_r;
float Power_inc    = 0;
int ledbackligth   = 0;
int ledbackligth_r = 0;
int auto_k         = 0;

void setup() { 
  EEPROM.begin(2048);
  set_volt = read_EEPROM(1).toFloat();
  set_cur  = read_EEPROM(100).toFloat();
  int ledb  = EEPROM.read(1000);
  if(ledb == 0){
    ledbackligth = 158;
  }else{
    ledbackligth = ledb;
  }
  auto_k = read_EEPROM(300).toInt();
  ledcSetup(pwmChannel, frequence, resolution);
  ledcAttachPin(pwmPin, pwmChannel);
  ESP_BT.register_callback(callback);
  ESP_BT.begin("SOLAR CHARGER [CYBEREX TECH]");         // Задаем имя устройства Bluetooth
  pinMode(flback_i, OUTPUT);
  digitalWrite(flback_i, HIGH);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRGBBitmap(5, 25,(uint16_t *)logo, 110, 110);   // Логотип
  tft.setCursor(115, 70);
  tft.setFont(&CrystalNormal16pt8b);                     // выбор шрифта
  tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(1);
  tft.println("CYBEREX TECH");
  tft.setFont();                                         // выбор шрифта по умолчанию
  tft.setCursor(50, 185);
  tft.setFont(&TimesNRCyr10pt8b);                        // выбор шрифта
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.print(utf8rus2("Твоя портативная энергия"));
  tft.setFont();                                         // выбор шрифта по умолчанию
  for(int light=0; light < 158; light++) {
      ledcWrite(pwmChannel, light); // 1.65 V
      delay(20);
  }
  delay(2000);
  for(int light = 158; light > -1; light--) {
      ledcWrite(pwmChannel, light);                     // 1.65 V
      delay(20);
  }
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRGBBitmap(270, 20,(uint16_t *)BTS_NC, 48, 48);
  ledcWrite(pwmChannel, ledbackligth);                  // 1.65 V
  ACS_input.autoMidPoint();
  ACS_out.autoMidPoint();
  delay(1000);
  digitalWrite(flback_i, LOW);
  }
#define maxString 100                                   // ограничиваем строку шириной экрана
char target[maxString + 1] = "";

char *utf8rus2(char *source){
  int i,j,k;
  unsigned char n;
  char m[2] = { '0', '\0' };
  strcpy(target, ""); k = strlen(source); i = j = 0;
  while (i < k) {
    n = source[i]; 
    i++;
    if (n >= 127) {
      switch (n) {
        case 208: {
          n = source[i]; 
          i++;
          if (n == 129) { n = 192; break; }            // перекодируем букву Ё
          break;
        }
        case 209: {
          n = source[i]; 
          i++;
          if (n == 145) { n = 193; break; }            // перекодируем букву ё
          break;
        }
      }
    }

    m[0] = n; strcat(target, m);
    j++; 
    if (j >= maxString) break;
  }
  return target;
}



void loop(void) {
      if(auto_k == 1){
          calc_mess();
        }else{
          if(calcul){
            calcul = false;
            set_volt = set_volt_r;
            counts_mess = 0;
          }
        }
      if(calcul){calc_mess_cur();}
      send_display();
      get_data_bt();
      get_sensor_data();
      if(ledbackligth != ledbackligth_r){
        ledcSetup(pwmChannel, frequence, resolution);
        ledcAttachPin(pwmPin, pwmChannel);
        ledcWrite(pwmChannel, ledbackligth);        // 1.65 V 
        ledbackligth_r = ledbackligth;
      }

}


void display_info(){
    tft.setCursor(10, 10);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_YELLOW);  tft.setTextSize(1);
    tft.println(utf8rus2("Генерация солнечной панели"));
    tft.setFont(); // выбор шрифта по умолчанию
    
    tft.setCursor(10, 35);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print(utf8rus2("Напряжение, В"));
    tft.setFont(); // выбор шрифта по умолчанию

    tft.setCursor(10, 95);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println(String(v_panel2));
    tft.setFont();
    
    tft.setCursor(10, 95);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
    v_panel2 = v_panel;
    tft.println(String(v_panel2));
    tft.setFont(); 

    tft.setCursor(190, 35);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print(utf8rus2("Ток, А"));
    tft.setFont(); // выбор шрифта по умолчанию
    
    tft.setCursor(160, 95);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println(String(c_panel2));
    tft.setFont();
    
    tft.setCursor(160, 95);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
    c_panel2 = c_panel;
    tft.println(String(c_panel2));
    tft.setFont(); // выбор шрифта по умолчанию
 

    tft.setCursor(10, 120);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_RED);  tft.setTextSize(1);
    tft.println(utf8rus2("Преобразователь"));
    tft.setFont(); // выбор шрифта по умолчанию
    
    tft.setCursor(10, 145);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print(utf8rus2("Напряжение, В"));
    tft.setFont(); // выбор шрифта по умолчанию

    tft.setCursor(10, 205);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println(String(v_charger2));
    tft.setFont();
    
    tft.setCursor(10, 205);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
    v_charger2 = v_charger;
    tft.println(String(v_charger2));
    tft.setFont(); 

    tft.setCursor(190, 145);
    tft.setFont(&TimesNRCyr10pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.print(utf8rus2("Ток, А"));
    tft.setFont(); // выбор шрифта по умолчанию
    
    tft.setCursor(160, 205);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(2);
    tft.println(String(c_charger2));
    tft.setFont();
    
    tft.setCursor(160, 205);
    tft.setFont(&CrystalNormal16pt8b); // выбор шрифта
    tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(2);
    c_charger2 = c_charger;
    tft.println(String(c_charger2));
    tft.setFont(); // выбор шрифта по умолчанию
    if(stat){
              ESP_BT.print(JSON_DATA());
              ESP_BT.print(" \n ");
            }
    if(stat && stat != stat2){
       tft.drawRGBBitmap(270, 20,(uint16_t *)BTS, 48, 48);
       stat2 = stat;
     }else if(!stat && stat != stat2 ){
       tft.drawRGBBitmap(270, 20,(uint16_t *)BTS_NC, 48, 48);
       stat2 = stat;
  }
  
}




   
