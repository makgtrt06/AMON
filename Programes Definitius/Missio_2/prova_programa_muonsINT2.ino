
int INT2 = 3;
int RST2 = 1;

int INT2_STATUS = 0;

void setup() {
  Serial.begin(9600);
  pinMode(INT2, OUTPUT);
  pinMode(RST2, INPUT);
  digitalWrite(RST2, LOW);
}

void loop() {
  INT2_STATUS = digitalRead(INT2);
  if (INT2_STATUS > 0){
    Serial.println("MUÓ DETECTAT, STATUS:");
    Serial.println(INT2_STATUS);
    Serial.println("RESETTING...");
    delay(10000);
    digitalWrite(RST2, HIGH);
    delay(500);
    digitalWrite(RST2, LOW);
    INT2_STATUS = digitalRead(INT2);
    Serial.println("RESET COMPLETE, STATUS:");
    Serial.println(INT2_STATUS);
  }
  else{
    Serial.println(INT2_STATUS);
    delay(1000);
  }
}