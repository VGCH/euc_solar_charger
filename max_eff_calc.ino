struct Messuring {
  float voltage;
  float current;
  float power;
};

Messuring coef_calc_array[23];                                                //массив измерений

void calc_mess(){
  unsigned long currentMillis = millis();
         if((currentMillis - mst200ms) > 300000 || currentMillis < mst200ms){ // Событие срабатывающее каждые 5 мин 
           calcul = true;
           mst200ms = currentMillis;
        }
  }

void calc_mess_cur(){
  unsigned long currentMillis = millis();
         if((currentMillis - mstTms) > 5000 || currentMillis < mstTms){       // Событие срабатывающее каждые 5 сек 
           if(counts_mess == 0){ 
             set_volt_r = set_volt;
             set_volt   = 8.5;  
            }
           if(counts_mess > 0 && counts_mess < 24){ 
             coef_calc_array[counts_mess -1].voltage = set_volt;
             coef_calc_array[counts_mess -1].current = c_panel;
             coef_calc_array[counts_mess -1].power   = v_panel*c_panel;
             set_volt = set_volt + 0.5;  
            }
             if(counts_mess > 23){ 
             coef_calc_array[counts_mess -1].voltage = set_volt;
             coef_calc_array[counts_mess -1].current = c_panel;
             coef_calc_array[counts_mess -1].power   = v_panel*c_panel;
             set_volt = max_coeff(); 
             calcul = false; 
             counts_mess = 0;
            }
            if(calcul){
                counts_mess++;
            }
           mstTms = currentMillis;
        }
  } 

  float max_coeff(){
     float maxim = 0;
     int index_max = 0;
     float maximcf = 0;
    for (int i = 0; i < 24; i++){
      if(coef_calc_array[i].power > maxim) {
          maxim = coef_calc_array[i].power;
          index_max = i;
        }
     }
     if(maxim > 10){
        maximcf = coef_calc_array[index_max].voltage;
      }else{
        maximcf = set_volt_r;
      }
     return maximcf;
  }
