#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 4  // KY-022
#define IR_SEND_PIN 18    // IR
#define RELAY_PIN 5       
void setup() 
{
  Serial.begin(115200);
  
  // Inițializează receptorul IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Receptor IR activ!");

  // Inițializează transmisia IR
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK, 0);
  Serial.println("Transmițător IR activ!");

  // Inițializează releul
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Lustra stinsă 
}

void TV() 
{
  // Semnal Sony HEX: 0x95 
  uint32_t address = 0x1;      // Adresa (Sony Address)
  uint32_t command = 0x15;     // Comanda (Sony Command)
  IrSender.sendSony(address, command, 2, 12);//2 rep 12 lenght
  Serial.println("Semnal IR pentru TV transmis!");
}

void AC() 
{
   // Date capturate pentru AC
  uint64_t tRawData[] = {0x20402000E047C3, 0x6F05000000};
  IrSender.sendPulseDistanceWidthFromArray(
    38, 9000, 4500, 550, 1700, 550, 550, &tRawData[0], 
    104, PROTOCOL_IS_LSB_FIRST, 0, 1
  );//frecv,impulsului de start,impulsului LOW de start,intervalele de timp,date,lungime biti,LSB,Perioada,nr reps 
  Serial.println("Semnal IR pentru AC transmis!");
}

void LIGHT()
{
  static bool lustraState = false;
  lustraState = !lustraState;  
  digitalWrite(RELAY_PIN, lustraState ? LOW : HIGH); 
  Serial.println(lustraState ? "Lustra aprinsă!" : "Lustra stinsă!");
}

void loop() 
{
  if (IrReceiver.decode()) 
  {
    Serial.print("Cod IR primit: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    
    if (IrReceiver.decodedIRData.decodedRawData == 0xF30CFF00) 
    {
      Serial.println("Semnal de pornire TV detectat!");
      TV();
    }
    
    if (IrReceiver.decodedIRData.decodedRawData == 0xE718FF00) 
    {
      Serial.println("Semnal de pornire AC detectat!");
      AC();
    }



    if (IrReceiver.decodedIRData.decodedRawData == 0xA15EFF00) 
    {
      Serial.println("Semnal de comutare LIGHT detectat!");
      LIGHT();
    }
    IrReceiver.resume(); 
  }
}
