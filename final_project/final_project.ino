#include <AsyncDelay.h>
#include <Adafruit_CircuitPlayground.h>

// Pin settings for buttons and switch
#define slidder 7
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
bool click = false;

uint8_t wheel_center = 0;
uint8_t catch_pos    = 0;


// Interrupt handlers
void handle_pause()  {paused = IsPaused; reset = true;}
void handle_buttons() {reset = DoubleClick; if (!reset) click = SingleClick;}


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
    if (reset) {
      click      = false;  // make sure there was no extra bounce
      wheel_center = random(0,9);
      catch_pos = random(0,9);
      while (catch_pos == wheel_center) catch_pos = random(0,9);

      reset = false;
    }
    if (timer.isExpired()) {
      wheel_center = (wheel_center + 1) % 10;  // Keep incrememnting between 0 and 9
      
      uint8_t lightup[10] = {0};

      // Build array of pixel settings
      lightup[(wheel_center + 1) % 10] = 1;
      lightup[wheel_center]            = 1;
      lightup[(wheel_center - 1) % 10] = 1;
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



    }

  } else {
    CircuitPlayground.clearPixels();
    timer.expire();
  }

}
