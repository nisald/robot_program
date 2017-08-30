const int led = 13;

void setup() 
{
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    Serial.begin(9600);
}

void loop() 
{
    if (Serial.available()) {
        light(Serial.read() - '0');
        delay(500);
    }
}

void light(int n)
{
    for (int i = 0; i < n; i++) {
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led, LOW);
        delay(500);
    }
}

