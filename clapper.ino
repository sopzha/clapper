#include <SPI.h>
  #include <TFT_eSPI.h>
  TFT_eSPI tft = TFT_eSPI();
  
  const int UPDATE_PERIOD = 20;
  uint32_t primary_timer;
  float sample_rate = 2000; //Hz
  float sample_period = (int)(1e6 / sample_rate);
  
  int render_counter;
  uint16_t raw_reading;
  float scaled_reading_for_serial;
  float without_offset;
  
  int state;
  float values[50];
  
  float clap_threshold = 100;
  
  int counter_threshold = 50;
  int counter;
  
  int timer;
  
  void setup() {
    Serial.begin(115200);
    primary_timer = micros();
    render_counter = 0;
    tft.init();  //init screen
    tft.setRotation(2); //adjust rotation
    tft.setTextSize(1); //default font size
    tft.fillScreen(TFT_BLACK); //fill background
  
    state = 0;
    timer = millis();
  }
  
  // Averaging filter function
  float averaging_filter(float input, float* stored_values, int order) {
    float average = 0.0;
  
    for (int i = order; i > 0; i--) {
      stored_values[i] = stored_values[i - 1];
    }
  
    stored_values[0] = input;
  
    for (int p = 0; p <= order; p++) {
      average = average + (1.0 / (1 + order)) * stored_values[p];
    }
  
    return average;
  }
  
  
  void loop() {
    raw_reading = analogRead(A0);
    without_offset = raw_reading - ((1.1 / 3.3) * 4096); // Remove offset
  
    float output = averaging_filter(without_offset, values, 10);
  
    // Black screen when state = 0
    if (state == 0) {
      tft.fillScreen(TFT_BLACK);
  
      if (output > clap_threshold) {
        state = 1;  // Switch to state = 1 when first clap detected
        counter = 0;
        timer = millis();
      }
    }
  
    else if (state == 1) {
      counter ++;
  
      if (counter > counter_threshold) {
        if (output > clap_threshold) {
          if (millis() - timer >= 250) {  // Switch to state = 2 if second clap detected after 250 ms
            state = 2;
            counter = 0;
            timer = millis();
          }
          else {
            state = 0;  // Switch to state = 0 if claps are too quick
          }
        }
        else if (millis() - timer >= 1000) {
          state = 0;  // Switch to state = 0 if wait time between claps is too long
        }
      }
  
    }
    else if (state == 2) {
      counter ++;
  
      if (counter > counter_threshold and output < clap_threshold) {
        if (millis() - timer >= 500) {
          state = 3;  // Switch to state = 3 after 500 ms passed
          counter = 0;
          timer = millis();
        }
  
      }
  
    }
  
    // Green screen when state = 3
    else if (state == 3) {
      tft.fillScreen(TFT_GREEN);
      if (output > clap_threshold) {  // Switch to state = 4 when first clap detected
        state = 4;
        counter = 0;
        timer = millis();
      }
  
    }
    else if (state == 4) {
      counter ++;
  
      if (counter > counter_threshold) {
        if (output > clap_threshold) {
          if (millis() - timer >= 250) {
  
            state = 5;
            counter = 0;
            timer = millis();
          }
          else {
            state = 3;
          }
        }
        else if (millis() - timer >= 1000) {
          state = 3;
        }
  
      }
    }
    else if (state == 5) {
      counter ++;
  
      if (counter > counter_threshold and output < clap_threshold) {
  
        if (millis() - timer >= 500) {
          state = 0;
        }
  
      }
  
    }
  }
