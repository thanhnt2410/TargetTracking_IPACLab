#define BT1 25
#define BT2 26
#define BT3 27
#define BT4 17
#define bientro1 32
#define bientro2 33

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BT1, INPUT);
  pinMode(BT1, INPUT);
  pinMode(BT1, INPUT);
  pinMode(BT1, INPUT);
  pinMode(bientro1, INPUT);
  pinMode(bientro2, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("BT1: ");
  Serial.println(digitalRead(BT1));
  Serial.print("BT2: ");
  Serial.println(digitalRead(BT2));
  Serial.print("BT3: ");
  Serial.println(digitalRead(BT3));
  Serial.print("BT4: ");
  Serial.println(digitalRead(BT4));
  Serial.print("Bientro1: ");
  Serial.println(analogRead(bientro1));
  Serial.print("Bientro2: ");
  Serial.println(analogRead(bientro2));
}
