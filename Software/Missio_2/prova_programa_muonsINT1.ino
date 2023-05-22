
int INT1 = 2;
int RST1 = 0;

int INT1_STATUS = 0;

void setup() {
  Serial.begin(9600);
  pinMode(INT1, OUTPUT);
  pinMode(RST1, INPUT);
  digitalWrite(RST1, LOW);
}

void loop() {
  INT1_STATUS = digitalRead(INT1);
  if (INT1_STATUS > 0){
    Serial.println("MUÓ DETECTAT, STATUS:");
    Serial.println(INT1_STATUS);
    Serial.println("RESETTING...");
    delay(10000);
    digitalWrite(RST1, HIGH);
    delay(500);
    digitalWrite(RST1, LOW);
    INT1_STATUS = digitalRead(INT1);
    Serial.println("RESET COMPLETE, STATUS:");
    Serial.println(INT1_STATUS);
  }
  else{
    Serial.println(INT1_STATUS);
    delay(1000);
  }
}