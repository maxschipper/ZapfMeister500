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

const int button_pin = 13;   // GPIO Taster
const int relay_pin_1 = 7;    // GPIO Relais 1 für oberes Ventil
const int relay_pin_2 = 6;    // GPIO Relais 2 für unteres Ventil

const int upper_open_time = 1000;
const int lower_open_time = 1000;

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

State current_state = STATE_IDLE;
unsigned long start_time = 0;



/////////////////////////////////////////////////////

void close_all_valves() {
  digitalWrite(relay_pin_1, LOW);
  digitalWrite(relay_pin_2, LOW);
  INFO("alle Ventile geschlossen");
}


void setup() {
  pinMode(button_pin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
  pinMode(relay_pin_1, OUTPUT);        
  pinMode(relay_pin_2, OUTPUT);        

  close_all_valves(); // Zu Beginn alles schließen
  Serial.begin(9600);
  DEBUG("setup done");
}

/////////////////////////////////////////////////////


void open_upper_valve() {
  digitalWrite(relay_pin_1, HIGH);
  DEBUG("oberes Ventil geöffnet");
}

void open_lower_valve() {
  digitalWrite(relay_pin_2, HIGH);
  DEBUG("unteres Ventil geöffnet");
}

void close_upper_valve() {
  digitalWrite(relay_pin_1, LOW);
  DEBUG("oberes Ventil geschlossen");
}

void close_lower_valve() {
  digitalWrite(relay_pin_2, LOW);
  DEBUG("unters Ventil geschlossen");
}


/////////////////////////////////////////////////////

void loop() {
  static int last_button_state = HIGH;
  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  int button_state = digitalRead(button_pin);

  // Flankenerkennung: Event nur, wenn HIGH -> LOW switcht
  bool button_pressed_event = (button_state == LOW && last_button_state == HIGH);
  if (button_pressed_event) {
    Serial.println("");
    DEBUG("BUTTON PRESS EVENT REGISTERD");
  }


  switch (current_state) {
    case STATE_IDLE:
      // Im Leerlauf warten wir auf einen Knopfdruck
      if (button_pressed_event) {
        INFO("Starte Füllvorgang..."); //INFO
        open_upper_valve();
        start_time = millis();
        current_state = STATE_FILLING;
      }
      break;

    case STATE_FILLING:
      // Während des Füllens prüfen wir, ob die Füllzeit abgelaufen ist
      // oder ob der Knopf erneut gedrückt wurde für Abbruch
      if (button_pressed_event) {
        WARN("Abbruch während des Füllens!");
        close_all_valves();
        current_state = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - start_time;

        if (elapsed >= upper_open_time) {
          close_upper_valve();
          INFO("Wechsle zum Entleeren...(in 1s)");
          delay(1000);
          open_lower_valve();
          start_time = millis();
          current_state = STATE_DISPENSING;
        }
      }
      break;

    case STATE_DISPENSING:
      // Während des Entleerens prüfen wir ebenfalls auf Abbruch oder Ablauf der Zeit
      if (button_pressed_event) {
        WARN("Abbruch während des Entleerens!");
        close_all_valves();
        current_state = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - start_time;

        if (elapsed >= lower_open_time) {
          close_lower_valve();
          INFO("Vorgang abgeschlossen, zurück zu IDLE");
          Serial.println("");
          current_state = STATE_IDLE;
        }
      }
      break;
  }
  last_button_state = button_state;
}

