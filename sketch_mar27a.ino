#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
        
        irrecv.resume();
  }
  if(results.value == 4294967295){
    Serial.println("on");
    
  }
  else{
    Serial.println(results.value);
      Serial.println("off");

  }
}
