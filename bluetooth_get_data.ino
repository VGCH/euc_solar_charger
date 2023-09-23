String parse_string(String string_in,  String STS, String LST){       // Функция парсинга принятых данных Bluetooth
            String data1;
            char buf[string_in.length()];
            string_in.toCharArray(buf, string_in.length());
            bool start = false; 
            int i = 0;
            while(i< string_in.length()){
             if (string_in.startsWith(LST, i)) {
                start = false; 
               }
             if(start){
                data1 += buf[i];
               }      
            if (string_in.startsWith(STS, i)) {
              start = true; 
               }
              i++;
            }
          return data1;
        
 }
