 String millis2time(){                                // преобразование миллисекунд в вид ч/м/с
         int times =(millis()/1000);
         int days =  (times/3600)/24;
         int timehour =(((times)  % 86400L) / 3600);  // часы
        if ( ((times % 3600) / 60) < 10 ) {
         int timehour = 0;
               }
         int timeminuts=((times  % 3600) / 60);       // минуты
         if ( (times % 60) < 10 ) {
         int timeminuts = 0;
             }
         int timeseconds=(times % 60);                // секунды
       String Time = String(twoDigits(timehour))+":"+String(twoDigits(timeminuts))+":"+String(twoDigits(timeseconds));
       return Time;
     }

 String twoDigits(int digits){
             if(digits < 10) {
          String i = '0'+String(digits);
             return i;
         }else {
             return String(digits);
         }
      }
