#include <Adafruit_NeoPixel.h>

// PIR motion sensor
#define PIR_P        3

// LED strips
#define NUM_STRIPS     6
#define STRIP_1_P      12
#define NUM_PIXELS_1   28
#define STRIP_2_P      11
#define NUM_PIXELS_2   30
#define STRIP_3_P      9
#define NUM_PIXELS_3   29
#define STRIP_4_P      8
#define NUM_PIXELS_4   34
#define STRIP_5_P      6
#define NUM_PIXELS_5   43
#define STRIP_6_P      5
#define NUM_PIXELS_6   33

// LED animation
#define LOOP_DELAY_MS             30
#define DANGER_DELAY_MS           15
#define FADE_LOW                  25
#define FADE_HIGH                 255
#define DEFAULT_SATURATION        255
#define DANGER_SATURATION         255

Adafruit_NeoPixel strip1(NUM_PIXELS_1, STRIP_1_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(NUM_PIXELS_2, STRIP_2_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3(NUM_PIXELS_3, STRIP_3_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4(NUM_PIXELS_4, STRIP_4_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5(NUM_PIXELS_5, STRIP_5_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip6(NUM_PIXELS_6, STRIP_6_P, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel *strips[NUM_STRIPS] = {&strip1, &strip2, &strip3, &strip4, &strip5, &strip6};

uint8_t stripLength[NUM_STRIPS] = {NUM_PIXELS_1, NUM_PIXELS_2, NUM_PIXELS_3, NUM_PIXELS_4, NUM_PIXELS_5, NUM_PIXELS_6};

uint16_t hues1[NUM_PIXELS_1];
uint16_t hues2[NUM_PIXELS_2];
uint16_t hues3[NUM_PIXELS_3];
uint16_t hues4[NUM_PIXELS_4];
uint16_t hues5[NUM_PIXELS_5];
uint16_t hues6[NUM_PIXELS_6];
uint16_t *hueArray[NUM_STRIPS] = {hues1, hues2, hues3, hues4, hues5, hues6};

uint8_t saturation = DEFAULT_SATURATION;

uint8_t wormPos[NUM_STRIPS] = { 0 };

#define SINE_WAVE_LENGTH    20
const uint8_t sineWave[SINE_WAVE_LENGTH] = {.5, 1, 1, .5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t sineWavePos = 0;

uint8_t fade = FADE_LOW;
bool directionUp = true;

bool dangerMode = false;

void setup()
{
  // initialize LED strips
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip4.begin();
  strip5.begin();
  strip6.begin();

  // set up PIR/motion sensor digital signal pin as input
  pinMode(PIR_P, INPUT);

  // Do the wormmm (translation: Calculate hues along each LED strip)
  for(int i = 0; i < NUM_STRIPS; i++)
  {
    for(int j = 0; j < stripLength[i]; j++)
    {
      if(!dangerMode)
      {
        uint32_t hueAdd = j * 2000;
        while(hueAdd > 30000)
        {
          hueAdd -= 30000;
        }
        hueArray[i][j] = 30000 + hueAdd;
      }
      else
        hueArray[i][j] = 0;
    }
  }
}

void loop()
{
  if(sineWavePos > 0)
    sineWavePos--;
  else
    sineWavePos = SINE_WAVE_LENGTH - 1;
     
  // Set pixels to calculated hues
  for(int i = 0; i < NUM_STRIPS; i++)
  {
    for(int j = 0; j < stripLength[i]; j++)
    {
      if(dangerMode)
      {
        strips[i]->setPixelColor(j, strips[i]->gamma32(strips[i]->ColorHSV(0, saturation, (int)(255 * (sineWave[(sineWavePos + j) % SINE_WAVE_LENGTH])))));
      }
      else 
      {
        strips[i]->setPixelColor(j, strips[i]->gamma32(strips[i]->ColorHSV(hueArray[i][j], saturation, (int)((50 + 9*(stripLength[i]/2 - (abs(j - stripLength[i]/2)))) * (sineWave[(sineWavePos + j + i*5) % SINE_WAVE_LENGTH])))));
      }
    }
  }

  // Bit-bang the LEDs to show the colors
  for(int i = 0; i < NUM_STRIPS; i++)
  {
    strips[i]->show();
  }

  // Change the hue for the next worm on all strips
  if(!digitalRead(PIR_P))
  {
    saturation = DEFAULT_SATURATION;
    dangerMode = false;

    delay(LOOP_DELAY_MS);
  }
  else
  {
    // motion detected -> danger -> red LEDs
    saturation = DANGER_SATURATION;
    dangerMode = true;

    delay(DANGER_DELAY_MS);
  }
}
