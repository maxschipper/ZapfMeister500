#define DEBUG_MODE
#define INFO_MODE

#ifdef DEBUG_MODE
  #define DEBUG(x) Serial.print("DEBUG    : "); Serial.println(x)
#else
  #define DEBUG(x)    // Tut nichts
#endif

#ifdef INFO_MODE
  #define INFO(x) Serial.print("INFO     : "); Serial.println(x)
#else
  #define INFO(x)    // Tut nichts
#endif

#define WARN(x) Serial.print("WARNING  : "); Serial.println(x)
/////////////////////////////////////////////////////

const int buttonPin = 13;   // GPIO Taster
const int relayPin1 = 7;    // GPIO Relais 1 für oberes Ventil
const int relayPin2 = 6;    // GPIO Relais 2 für unteres Ventil

const int upper_open_time = 1000;
const int lower_open_time = 1000;

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

State currentState = STATE_IDLE;
unsigned long startTime = 0;



/////////////////////////////////////////////////////

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
  DEBUG("setup done");
}

/////////////////////////////////////////////////////


void open_upper_valve() {
  digitalWrite(relayPin1, HIGH);
  DEBUG("oberes Ventil geöffnet");
}

void open_lower_valve() {
  digitalWrite(relayPin2, HIGH);
  DEBUG("unteres Ventil geöffnet");
}

void close_upper_valve() {
  digitalWrite(relayPin1, LOW);
  DEBUG("oberes Ventil geschlossen");
}

void close_lower_valve() {
  digitalWrite(relayPin2, LOW);
  DEBUG("unters Ventil geschlossen");
}


/////////////////////////////////////////////////////

void loop() {
  static int lastButtonState = HIGH;
  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  int buttonState = digitalRead(buttonPin);
  // bool buttonPressed = (buttonState == LOW);

  // Flankenerkennung: Event nur, wenn HIGH -> LOW switcht
  bool buttonPressedEvent = (buttonState == LOW && lastButtonState == HIGH);
  if (buttonPressedEvent) {
    DEBUG("BUTTON PRESS EVENT REGISTERD");
  }


  switch (currentState) {
    case STATE_IDLE:
      // Im Leerlauf warten wir auf einen Knopfdruck
      if (buttonPressedEvent) {
        INFO("Starte Füllvorgang..."); //INFO
        open_upper_valve();
        startTime = millis();
        currentState = STATE_FILLING;
      }
      break;

    case STATE_FILLING:
      // Während des Füllens prüfen wir, ob die Füllzeit abgelaufen ist
      // oder ob der Knopf erneut gedrückt wurde für Abbruch
      if (buttonPressedEvent) {
        WARN("Abbruch während des Füllens!");
        close_all_valves();
        currentState = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - startTime;

        if (elapsed >= upper_open_time) {
          close_upper_valve();
          INFO("Wechsle zum Entleeren...(in 1s)");
          delay(1000);
          open_lower_valve();
          startTime = millis();
          currentState = STATE_DISPENSING;
        }
      }
      break;

    case STATE_DISPENSING:
      // Während des Entleerens prüfen wir ebenfalls auf Abbruch oder Ablauf der Zeit
      if (buttonPressedEvent) {
        WARN("Abbruch während des Entleerens!");
        close_all_valves();
        currentState = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - startTime;

        if (elapsed >= lower_open_time) {
          close_lower_valve();
          INFO("Vorgang abgeschlossen, zurück zu IDLE");
          Serial.println("");
          currentState = STATE_IDLE;
        }
      }
      break;
  }
  lastButtonState = buttonState;
}

