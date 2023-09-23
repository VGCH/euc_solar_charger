   String read_EEPROM (int star_t){                            // Чтение данных их ячейки
            String data;
            int end_t = star_t + 32;
            for(int i = star_t; i < end_t; ++i){
                int bu = EEPROM.read(i);
              if (bu > 31 && bu < 241){
                   data += char(bu);
                 }
              }
            return data;
         }
            
    String save_EEPROM (String datas, int cell){               // запись данных в ячейки 
          int end_cell = cell + 32;
          for (int i = cell; i < end_cell; ++i)                // стираем данные перед записью
          { 
            EEPROM.write(i, 0); 
            }
          for (int i = 0; i < end_cell; ++i)                   // записываем данные в ячейки
            {
            EEPROM.write(cell+i, datas[i]);
            }  
            EEPROM.commit();     
           return datas;        
        }
