#include <Adafruit_NeoPixel.h>
#include <Stepper.h>


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

// PINS - Step1: 8,9,10,11 - Step2: 4,5,6,7
const int button_pin = 13;   // GPIO Taster
const int trig_pin = 3;
const int echo_pin = 2;
const int led_pin = 12; //Pin, an dem der NeoPixel angeschlossen ist

//LED RING
const int leds = 8; //Anzahl der LEDs
int rgb_red = 255; //Rot
int rgb_green = 255; //Grün
int rgb_blue = 255;  //Blau

Adafruit_NeoPixel ring = Adafruit_NeoPixel(leds, led_pin, NEO_GRB + NEO_KHZ800);


//ULTRASCHALL
float duration, distance;
const int min_distance = 9;

// STEPPER
const int upper_open_time = 4000;
const int lower_open_time = 4000;
// Voll-Schritt:4096 Halb:2038 Viertel:1024
const int steps_per_revolution = 512;

// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper upper_stepper = Stepper(steps_per_revolution, 8, 10, 9, 11);
bool upper_stepper_open = false;

// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper lower_stepper = Stepper(steps_per_revolution, 4, 6, 5, 7);
bool lower_stepper_open = false;

enum State {
  STATE_IDLE,
  STATE_FILLING,
  STATE_DISPENSING
};

State current_state = STATE_IDLE;
unsigned long start_time = 0;



/////////////////////////////////////////////////////

void close_all_valves() {
  if (upper_stepper_open) {
    close_upper_valve();
  } 
  if (lower_stepper_open) {
    close_lower_valve();
  }
  INFO("alle Ventile geschlossen");
}


void setup() {
  Serial.begin(9600);
  pinMode(button_pin, INPUT_PULLUP); // Pullup um Störsignale (float) zu verhindern
 	pinMode(trig_pin, OUTPUT);  
	pinMode(echo_pin, INPUT);  

  pinMode (led_pin, OUTPUT);
  ring.begin();
  ring.setBrightness(10); //Helligkeit: 0 (aus) - 255

  close_all_valves(); // Zu Beginn alles schließen
  DEBUG("setup done");
}

/////////////////////////////////////////////////////
// METHODEN

void loading_animation(int red, int green, int blue) {
    static unsigned long previousMillis = 0; // Speichert die letzte Zeitaktualisierung
    static int currentLED = 0;              // Aktuelle LED, die leuchtet
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 200) { // Intervall von 200ms
        previousMillis = currentMillis;
        ring.clear();
        ring.setPixelColor(currentLED, ring.Color(red, green, blue));
        ring.show();
        currentLED++;
        if (currentLED >= ring.numPixels()) {
            currentLED = 0;
        }
    }
}

void set_all_leds(int red, int green, int blue) {
    for (int i = 0; i < ring.numPixels(); i++) {
        ring.setPixelColor(i, ring.Color(red, green, blue));
    }
    ring.show();
}

void blink_leds(int red, int green, int blue, int blinkCount, int blinkDuration) {
    for (int i = 0; i < blinkCount; i++) {
        set_all_leds(red, green, blue);
        delay(blinkDuration);

        set_all_leds(0, 0, 0);
        delay(blinkDuration);
    }
}

float ultraschall_dist() {
  static float lastDistance = -1; // Speichert den letzten gültigen Wert
  // Triggersignal senden
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  // Messung des Echos
  long duration = pulseIn(echo_pin, HIGH);
  // Berechnung der Distanz
  float distance = (duration * 0.0343) / 2;
  // Fehlerbehandlung
  if (distance == 0 || distance > 250) {
    if (lastDistance == -1) {
      // Wenn auch die letzte Messung ungültig war, gib -1 zurück
      return -1;
    } else {
      // Wenn die vorherige Messung gültig war, nutze sie weiter
      return lastDistance;
    }
  }
  // Gültigen Wert speichern und zurückgeben
  lastDistance = distance;
  return distance;
}

void open_upper_valve() {
  if (upper_stepper_open) {
    WARN("oberes Ventil ist bereits offen!!");
  } else {
    upper_stepper.setSpeed(50);
    upper_stepper.step(4*steps_per_revolution);
    upper_stepper_open = true;
    DEBUG("oberes Ventil geöffnet");
  }
}

void open_lower_valve() {
  if (lower_stepper_open) {
    WARN("unteres Ventil ist bereits offen!!");
  } else {
    lower_stepper.setSpeed(50);
    lower_stepper.step(4*steps_per_revolution);
    lower_stepper_open = true;
    DEBUG("unteres Ventil geöffnet");
  }
}

void close_upper_valve() {
  if (upper_stepper_open) {
    upper_stepper.setSpeed(50);
    upper_stepper.step(-4*steps_per_revolution);
    upper_stepper_open = false;
    DEBUG("oberes Ventil geschlossen");
  } else {
    WARN("oberes Ventil ist bereits geschlossen!!");
  }
}

void close_lower_valve() {
  if (lower_stepper_open) {
    lower_stepper.setSpeed(50);
    lower_stepper.step(-4*steps_per_revolution);
    lower_stepper_open = false;
    DEBUG("unteres Ventil geschlossen");
  } else {
    WARN("unteres Ventil ist bereits geschlossen!!");
  }
}
/////////////////////////////////////////////////////

void loop() {
  static bool is_glass = false;
  static int last_button_state = HIGH;
  int button_state = digitalRead(button_pin);  // Knopfstatus lesen (LOW = gedrückt, HIGH = nicht gedrückt)
  bool button_pressed_event = (button_state == LOW && last_button_state == HIGH);  // Flankenerkennung: Event nur, wenn HIGH -> LOW switcht
  if (button_pressed_event) {
    DEBUG("BUTTON PRESS EVENT REGISTERD");
  }

  distance = ultraschall_dist();
  DEBUG(distance);

  if(distance <= min_distance && !is_glass) {
    is_glass = true;
    INFO("Glas erkannt");
  } else if(distance > min_distance && is_glass){
    is_glass = false;
  }


  switch (current_state) {
    case STATE_IDLE:
      if(is_glass) {
        set_all_leds(0, 255, 0);
      } else {
        set_all_leds(0, 0, 255);
      }
      // Im Leerlauf warten wir auf einen Knopfdruck
      if (button_pressed_event) {
        INFO("Starte Füllvorgang..."); //INFO
        open_upper_valve();
        start_time = millis();
        current_state = STATE_FILLING;
      }
      break;

    case STATE_FILLING:
      loading_animation(255, 100, 100); //orange
      // Check if the glass is still there
      if (!is_glass) {
        WARN("Abbruch: Glas entfernt während des Füllens!");
        close_all_valves();
        blink_leds(255, 0, 0, 3, 200); // blink red LEDs as abort signal
        current_state = STATE_IDLE;
        break; // exit this state immediately
      }
      // oder ob der Knopf erneut gedrückt wurde für Abbruch
      if (button_pressed_event) {
        WARN("Abbruch während des Füllens!");
        close_all_valves();
        blink_leds(255, 0, 0, 3, 200); // blinkt 3x rot für 200ms
        current_state = STATE_IDLE;
      } else {
        unsigned long elapsed = millis() - start_time;

        if (elapsed >= upper_open_time) {
          close_upper_valve();
          INFO("Wechsle zum Entleeren...(in 5s)");
          delay(5000);
          open_lower_valve();
          start_time = millis();
          current_state = STATE_DISPENSING;
        }
      }
      break;

    case STATE_DISPENSING:
      loading_animation(0, 255, 0); //green
      // Check if the glass is still there
      if (!is_glass) {
        WARN("Abbruch: Glas entfernt während des Entleerens!");
        close_all_valves();
        blink_leds(255, 0, 0, 3, 200); // blink red LEDs as abort signal
        current_state = STATE_IDLE;
        break; // exit this state immediately
      }
      // Während des Entleerens prüfen wir ebenfalls auf Abbruch oder Ablauf der Zeit
      if (button_pressed_event) {
        WARN("Abbruch während des Entleerens!");
        close_all_valves();
        blink_leds(255, 0, 0, 3, 200); // blinkt 3x rot für 200ms
        current_state = STATE_IDLE;

      } else {
        unsigned long elapsed = millis() - start_time;

        if (elapsed >= lower_open_time) {
          close_lower_valve();
          INFO("Vorgang abgeschlossen, zurück zu IDLE");
          blink_leds(0, 255, 0, 3, 200); // blinkt 3x grün für 200ms
          current_state = STATE_IDLE;
        }
      }
      break;
  }
  last_button_state = button_state;
}

