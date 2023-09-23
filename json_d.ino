String JSON_DATA(){
     float power_p   = v_panel*c_panel;
     float power_i   = v_charger*c_charger;
     String outjson  = "{\"v_panel\":\""+String(v_panel)+"\",\"c_panel\":\""+String(c_panel)+"\",\"v_invertor\":\""+String(v_charger)+"\",\"c_invertor\":\""+String(c_charger)+"\",\"power_p\":\""+String(power_p)+"\",\"power_i\":\""+String(power_i)+"\",";
            outjson += "\"set_v\":\""+String(set_volt)+"\",\"set_c\":\""+String(set_cur)+"\",\"power_inc\":\""+String(Power_inc)+"\",\"backlight\":\""+String(ledbackligth)+"\",\"time_work\":\""+String(millis2time())+"\",\"auto_k\":\""+String(auto_k)+"\",\"analog1\":\"";
            outjson += String(analogRead(current_panel))+"\",\"analog2\":\""+String(analogRead(current_charger))+"\",\"analogVin\":\""+String(analogRead(voltage_panel))+"\",\"analogVOUT\":\""+String(analogRead(voltage_charger))+"\",\"BTSTAT\":\""+String(stat)+"\",\"flyback\":\""+String(digitalRead(flback_i))+"\"}";
     return outjson;
}
