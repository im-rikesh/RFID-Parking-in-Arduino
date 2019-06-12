
#include <SPI.h>
#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include<MFRC522.h>
#include<SoftwareSerial.h>

#define SS_PIN 4 //FOR RFID SS PIN BECASUSE WE ARE USING BOTH ETHERNET SHIELD AND RS-522
#define RST_PIN 9
#define No_Of_Card 3
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };

IPAddress server_addr(169,254,194,36);  // IP of your laptop
char user[] = "techminer";              // MySQL user login username
char password[] = "techm";        // MySQL user login password
IPAddress ip(169,254,194,48);
// Sample query

MFRC522 rfid(SS_PIN,RST_PIN);
MFRC522::MIFARE_Key key; 

byte id_byte[3][3];
byte i;


EthernetClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(9600);
    
  SPI.begin();
  
  rfid.PCD_Init();
  
  while (!Serial); // wait for serial port to connect
  Ethernet.begin(mac_addr,ip);
  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
     Serial.println("Connection Successful.");
    delay(10);
  }
  else
    Serial.println("Connection failed.");
}


void loop() {
  delay(20);
char id_char2[20];
String id_char="";
  int m=0;
  if(!rfid.PICC_IsNewCardPresent())
  return;
  if(!rfid.PICC_ReadCardSerial())
  return;


  for(i=0;i<4;i++)
  {
   id_byte[0][i]=rfid.uid.uidByte[i]; 
       id_char = id_char + id_byte[0][i];
             delay(50);  
  }
id_char=id_char+'\0';
for(int i=0; i<id_char.length();i++){
  id_char2[i]=id_char[i];
}

    
 for(i=0;i<4;i++)
  {
   Serial.println(id_byte[0][i]); 

             delay(50);
  }
  


Serial.println(id_char);
  
const char INSERT_SQL[] = "INSERT INTO test.test_arduino (message) VALUES ";
char query[] = "SELECT * FROM test.test_arduino LIMIT 50";
char query1[80];
snprintf(query1,80, "%s(\"%s\")",INSERT_SQL,id_char2);
  Serial.println(query1);
  Serial.println("Recording data.");


  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query1);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delay(20);
 Serial.println("\nRunning SELECT and printing results\n");

  // Initiate the query class instance
  // Execute the query
  cur_mem->execute(query);
  // Fetch the columns and print them
  column_names *cols = cur_mem->get_columns();
  for (int f = 0; f < cols->num_fields; f++) {
    Serial.print(cols->fields[f]->name);
    if (f < cols->num_fields-1) {
      Serial.print(", ");
    }
  }
  Serial.println();
  // Read the rows and print them
  row_values *row = NULL;
  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      for (int f = 0; f < cols->num_fields; f++) {
        String str=row->values[f];
                Serial.print(str);
        if(str.indexOf("")>0){
        Serial.println("Glow LED");
        }
        str="";
        if (f < cols->num_fields-1) {
          Serial.print(", ");
        }
      }
      Serial.println();
    }
  } while (row != NULL);
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}
