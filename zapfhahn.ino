const int buttonPin = 2;   // GPIO Taster
const int relayPin1 = 8;    // GPIO Relais 1 für oberes Ventil
const int relayPin2 = 9;    // GPIO Relais 2 für unteres Ventil

const int open_time1 = 1000;
const int open_time2 = 1000;

bool isFilling = false;       // Zustand der Zapfanlage
unsigned long startTime = 0;  // Zeitstempel für Ventilsteuerung
int currentStep = 0;          // Schritt des Prozesses

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

State currentState = STATE_IDLE;
unsigned long startTime = 0;



/////////////////////////////////////////////////////////////////////////////

void closeAllValves() {
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
  pinMode(relayPin1, OUTPUT);        
  pinMode(relayPin2, OUTPUT);        

  closeAllValves(); // Zu Beginn alles schließen
  Serial.begin(9600);
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

