const int buttonPin = 2;   // GPIO Taster
const int relayPin1 = 8;    // GPIO Relais 1 für oberes Ventil
const int relayPin2 = 9;    // GPIO Relais 2 für unteres Ventil

const int upper_open_time = 1000;
const int lower_open_time = 1000;

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

State currentState = STATE_IDLE;
unsigned long startTime = 0;



/////////////////////////////////////////////////////////////////////////////

void close_all_valves() {
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);
}


void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
  pinMode(relayPin1, OUTPUT);        
  pinMode(relayPin2, OUTPUT);        

  close_all_valves(); // Zu Beginn alles schließen
  Serial.begin(9600);
}

/////////////////////////////////////////////////////////////////////////////


void open_upper_valve() {
  digitalWrite(relayPin1, HIGH);
}

void open_lower_valve() {
  digitalWrite(relayPin2, HIGH);
}

void close_upper_valve() {
  digitalWrite(relayPin1, LOW);
}

void close_lower_valve() {
  digitalWrite(relayPin2, LOW);
}


/////////////////////////////////////////////////////////////////////////////

void loop() {
  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  int buttonState = digitalRead(buttonPin);
  bool buttonPressed = (buttonState == LOW);

  switch (currentState) {
    case STATE_IDLE:
      // Im Leerlauf warten wir auf einen Knopfdruck
      if (buttonPressed) {
        Serial.println("Starte Füllvorgang...");
        open_upper_valve();
        startTime = millis();
        currentState = STATE_FILLING;
      }
      break;

    case STATE_FILLING:
      // Während des Füllens prüfen wir, ob die Füllzeit abgelaufen ist
      // oder ob der Knopf erneut gedrückt wurde für Abbruch
      if (buttonPressed) {
        Serial.println("Abbruch während des Füllens!");
        close_all_valves();
        currentState = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - startTime;

        if (elapsed >= upper_open_time) {
          close_upper_valve();
          Serial.println("Wechsle zum Entleeren...(in 1s)");
          delay(1000);
          open_lower_valve();
          startTime = millis();
          currentState = STATE_DISPENSING;
        }
      }
      break;

    case STATE_DISPENSING:
      // Während des Entleerens prüfen wir ebenfalls auf Abbruch oder Ablauf der Zeit
      if (buttonPressed) {
        Serial.println("Abbruch während des Entleerens!");
        close_all_valves();
        currentState = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - startTime;

        if (elapsed >= lower_open_time) {
          close_lower_valve();
          Serial.println("Vorgang abgeschlossen, zurück zu IDLE");
          currentState = STATE_IDLE;
        }
      }
      break;
  }
}

