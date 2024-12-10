const int buttonPin = 2;   // GPIO Taster
const int relayPin1 = 8;    // GPIO Relais 1 für oberes Ventil
const int relayPin2 = 9;    // GPIO Relais 2 für unteres Ventil

const int obere_open_time = 1000;
const int untere_open_time = 1000;
// #define O_OPEN_TIME 1000
// #define U_OPEN_TIME 1000

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
  pinMode(relayPin1, OUTPUT);        
  pinMode(relayPin2, OUTPUT);        
  digitalWrite(relayPin, LOW);      
}

/////////////////////////////////////////////////////////////////////////////



void oberes_ventil_open(int open_time) {

    digitalWrite(relayPin1, HIGH); // Relais einschalten
    delay(open_time); // wie lange das obere Ventil geöffnet sein soll 
    digitalWrite(relayPin1, LOW); // Relais ausschalten
}




/////////////////////////////////////////////////////////////////////////////

void loop() {
  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    oberes_ventil_open(obere_open_time);


  } else {
    digitalWrite(relayPin, LOW);  
  }
}

