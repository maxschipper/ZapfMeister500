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

const int button_pin = 12;   // GPIO Taster

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

const int upper_open_time = 4000;
const int lower_open_time = 4000;

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};


State current_state = STATE_IDLE;
unsigned long start_time = 0;



/////////////////////////////////////////////////////



void setup() {
  pinMode(button_pin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  Serial.begin(9600);
  DEBUG("setup done");
}

/////////////////////////////////////////////////////


void setStep(int step[4]) {
  DEBUG("entered setStep()");
    DEBUG("Setze Pins: " + String(step[0]) + "," + String(step[1]) + "," + String(step[2]) + "," + String(step[3]));
  digitalWrite(IN1, step[0]);
  digitalWrite(IN2, step[1]);
  digitalWrite(IN3, step[2]);
  digitalWrite(IN4, step[3]);
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


  DEBUG("Starte Vorwärtsbewegung");
  for (int i = 0; i < 8; i++) {
    DEBUG("Setze Schrittindex: " + String(i));
    setStep(steps[i]);
    delay(2); // Schrittgeschwindigkeit
  }

  delay(1000); // Pause

  // Rückwärtsbewegung
  DEBUG("Starte Rückwärtsbewegung");
  for (int i = 7; i >= 0; i--) {
    DEBUG("Setze Schrittindex (rückwärts): " + String(i));
    setStep(steps[i]);
    delay(2);
  }

  delay(1000); // Pause


  last_button_state = button_state;
}

