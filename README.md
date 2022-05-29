# clapper
Created for 6.08: Embedded Systems at MIT.

# Video

Demo: <a href="https://youtu.be/NO2lPzKiamA" target="_blank">https://youtu.be/NO2lPzKiamA</a>

# Specifications

Below is how I addressed each specification:

## Signal Magnitude

**Requirement:**

* Use the running average of the audio signal amplitude (which can also be thought of as the magnitude of the signal). Because the actual signal from the microphone module is not just the audio signal, but a constant offset term as well, you will need to make sure to remove that first before doing the running average.

**Implementation:**

To remove the constant offset term, I subtract `(1.1 / 3.3) * 4096` from the microphone module signal. The `1.1` offset was determined by calibrating the original `1.25` offset until the fixed signal lies approximately at 0 when no sound is generated. This can be visualized using the Serial Plotter on the Arduino.
The signal with the removed offset is then subjected to an averaging filter, which provides a more stable reading for the signal magnitude.

## System Functionality

**Requirement:**

* When the LCD is OFF (filled color of your choosing), the appropriately spaced double-clap-clap sound should turn it on.
* When the LCD is ON (distinctive other filled color of your choosing), the appropriately spaced double-clap-clap should turn it off.
* Your system should NOT respond if distinct loud sounds arrive >= 1.0 sec apart.
* Your system should NOT respond if distinct loud sounds arrive less than 0.25 sec apart.
* Your system should NOT respond to loud continous sounds.
* Your system shoudl NOT respond to continuous clap sounds.

**Implementation:**

I used a finite state machine to fulfill most of the requirements. The system consists of 6 states. At state 0, the LCD displays a black background. When the first of two claps is detected, 
the state switches to 1. If the second clap occurs at the reasonable time after (>= 500 ms), the state switches to 2. Otherwise, the system switches to state 0, negating the effect of the first clap and making 
the system immune to really quick claps. If no clap is detected one second after the first, the system similarly switches to state 0. In state 2, if no clap is detected and 500 ms passes, the system switches to state 3. The intent is to make the system
immune to continuous claps following the second clap; however, I made a mistake because I did not address the event in which a clap is detected prior to the end of the 500 ms, in which the system would have reset to state = 0.

In state 3, the LCD displays a green background. Otherwise, the behavior of and transitions between states 3, 4, 5 are analogous to that of state 0, 1, 2, respectively. As previously noted, the code is not entirely successful; the system may fail to reject continuous clap
sounds. Other requirements are generally met, though not 100% reliable. If the clap sounds are not loud enough for the signal to surpass the threshold, we may not achieve the desired transitions between states.
