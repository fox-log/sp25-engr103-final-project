#include <AsyncDelay.h>
#include <Adafruit_CircuitPlayground.h>

// Constants
#define slidder 7  // Pin numbers for switch and buttons
#define lbutton 4
#define rbutton 5
#define catch_color 0xFFFFFF
#define wheel_color 0xFF0077

// Reader functions for switch and buttons
#define IsPaused digitalRead(slidder)  // Switch reads 1 when to the left position
#define DoubleClick digitalRead(rbutton) && digitalRead(lbutton)  // Take the AND of both buttons
#define SingleClick digitalRead(rbutton) || digitalRead(lbutton)  // Take the OR of both buttons

// Needed Variables
AsyncDelay timer;
bool paused  = true;
bool reset   = true;
bool clicked = false;

uint8_t wheel_pos = 0;
uint8_t wheel_len = 0;
uint8_t catch_pos = 0;
ushort score      = 0;
bool mode         = true;  // True for game, False for score


// Interrupt handlers
void handle_pause()  {paused = IsPaused; reset = true;}
void handle_buttons() {clicked = true;}


void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  while (!Serial);  // Await serial connection before moving on
  
  // Initialize the switch and buttons to digital reads
  pinMode(slidder, INPUT_PULLUP);
  pinMode(lbutton, INPUT_PULLDOWN);
  pinMode(rbutton, INPUT_PULLDOWN);

  paused = IsPaused;
  timer.start(150, AsyncDelay::MILLIS);  // Initilize 1 second time
  timer.expire();  // Force stop timer until its needed

  // Attach interrupts to the buttons and switch
  attachInterrupt(digitalPinToInterrupt(slidder), handle_pause, CHANGE);
  attachInterrupt(digitalPinToInterrupt(lbutton), handle_buttons, RISING);
  attachInterrupt(digitalPinToInterrupt(rbutton), handle_buttons, RISING);
}

void loop() {
  if (!paused) {
    
    if (mode) {  // True is game screen
      if (reset) {
        clicked   = false;  // make sure there was no extra bounce
        wheel_pos = random(0,9);  // Choose a new random position to start the wheel at
        wheel_len = 3 - map(score > 20? 20:score, 0, 20, 0, 2);
        catch_pos = (wheel_pos + 5) % 10;  // Place catch pixel on opposite side of field
        reset = false;

        delay(10);  // Try and debounce
        clicked = false;

        timer.start(150 - map(score > 50? 50:score, 0, 50, 0, 75), AsyncDelay::MILLIS);  // Redeclare timer with increased speed
        timer.expire();  // Force timer off to display game right away
      }

      if (timer.isExpired()) {
        wheel_pos = (wheel_pos + 1) % 10;  // Keep incrememnting between 0 and 9
        
        uint8_t lightup[10] = {0};

        // Build array of pixel settings
        for (uint8_t i=0; i<wheel_len; i++) {lightup[(wheel_pos - i) % 10] = 1;}
        lightup[catch_pos] = 2;

        for (uint8_t i=0; i<10; i++) {
          if (lightup[i] == 1)      CircuitPlayground.setPixelColor(i, wheel_color);
          else if (lightup[i] == 2) CircuitPlayground.setPixelColor(i, catch_color);
          else if (lightup[i] == 0) CircuitPlayground.setPixelColor(i, 0);
        }
        timer.repeat();
      }

      if (clicked) {
        clicked = false;
        timer.expire();

        // Check if the wheel is overlapping the catch pixel
        bool caught = false;
        for (uint8_t i=0; i<wheel_len; i++) {caught = caught || catch_pos == (wheel_pos-i)%10;}
        
        if (caught) {
          CircuitPlayground.playTone(524.0, 200);
          CircuitPlayground.playTone(587.0, 100);
          CircuitPlayground.playTone(659.0, 50);
          CircuitPlayground.playTone(587.0, 150);
          CircuitPlayground.playTone(524.0, 200);
          
          score++;
        }
        else {
          CircuitPlayground.playTone(220.0, 100);
          CircuitPlayground.playTone(247.0, 100);
          CircuitPlayground.playTone(220.0, 100);
        }
        mode = false;
      }

    } else {  // False is score screen

      ushort color = 255 << map(score > 50? score%51:score, 0, 50, 0, 16);
      uint8_t num_active = score % 11;

      // Turn on a number of pixels equal to the score's ones place and make sure everythin else is off
      // Simply using "clear_pixels" causing a lot of clickering, so want to manually turn off unwanted lights
      for (ushort i=0; i<10; i++) {CircuitPlayground.setPixelColor(i, i < num_active? color:0x00);}
      Serial.print(score);
      Serial.print(" ");
      Serial.println(num_active);

      if (clicked) {  // Check for double click to start new game
        if (DoubleClick) {reset = true; mode = true;}
        
        delay(10);  // Try and debounce
        clicked = false;
      }
    }

  } else {
    CircuitPlayground.clearPixels();
    timer.expire();
  }

}
