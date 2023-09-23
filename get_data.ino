void get_sensor_data(){
  unsigned long currentMillis = millis();
         if((currentMillis - t200ms) > 10 || currentMillis < t200ms){ // Событие срабатывающее каждые 10 мсек 
          v_panel   = analogRead(voltage_panel)*0.0061976;
          float i1  = 0;
          if(stat){
             i1 = ACS_input.mA_DC(1, 10)*0.00182; 
          }else{
             i1 = ACS_input.mA_DC(1, 0)*0.00182; 
          }
          if(i1 > 0.0){
             c_panel   = i1;
          }else{
             c_panel   = 0.00;
          }
          v_charger = analogRead(voltage_charger)*0.0426024;
          float i2  = 0;
          if(stat){
               i2  = ACS_out.mA_DC(1, 10)*0.00182; 
          }else{
               i2  = ACS_out.mA_DC(1, 0)*0.00182; 
          }
          if(i2 > 0.0){
             c_charger = i2;
          }else{
             c_charger = 0.00;
          }
          float power_p = v_panel*c_panel;
              Power_inc = Power_inc + (power_p*0.000002778);
                 t200ms = currentMillis;
          point_eff_sp();
        }
}

void send_display(){
  unsigned long currentMillis = millis();
         if((currentMillis - dt200ms) > 1000 || currentMillis < dt200ms){ // Событие срабатывающее каждые 1000 мсек 
          display_info();
          dt200ms = currentMillis;
      }
}


void get_data_bt(){                                                       // получение параметров от мобильного приложения
 if (ESP_BT.available()){
      String text = ESP_BT.readString(); 
          if(text.length() > 1){
             String volt      =  parse_string(text, "V", "v");
             String current   =  parse_string(text, "I", "i");
             String ledbc     =  parse_string(text, "L", "l");
             String autok     =  parse_string(text, "C", "c");
             if(volt.length() >= 1){
                 save_EEPROM(volt, 1);
                 set_volt = volt.toFloat();
              }
             if(current.length() >= 1){
                 save_EEPROM(current, 100);
                 set_cur = current.toFloat();      
              }
             if(ledbc.length() >= 1){
                 ledbackligth = ledbc.toInt();
                 EEPROM.write(1000, ledbackligth);
                 EEPROM.commit();     
              }
             if(autok.length() >= 1){
                 save_EEPROM(autok, 300);
                 auto_k = autok.toInt();      
              } 
          }       
     }
}
