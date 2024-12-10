#define O_OPEN_TIME 1000
#define U_OPEN_TIME 1000


const int buttonPin = 2;   // GPIO Taster
const int relayPin1 = 8;    // GPIO Relais 1 für oberes Ventil
const int relayPin2 = 9;    // GPIO Relais 2 für unteres Ventil
void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
  pinMode(relayPin1, OUTPUT);        
  pinMode(relayPin2, OUTPUT);        
  digitalWrite(relayPin, LOW);      
}

void loop() {
  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {

// in methode auslagern
    digitalWrite(relayPin1, HIGH); // Relais einschalten
    delay(O_OPEN_TIME); // wie lange das obere Ventil geöffnet sein soll 
    digitalWrite(relayPin1, LOW); // Relais ausschalten
// bis hier hin


  } else {
    digitalWrite(relayPin, LOW);  
  }
}

