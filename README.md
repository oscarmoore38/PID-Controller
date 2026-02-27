## PID Controller 

### Why build a PID Controller?
If you’ve glanced around my GitHub, you’ve probably noticed that my interests tend to orbit two areas: **C++** and **robotics**. I’m still early in this space and feeling out where I’d like to go, but one thing is consistent: I’m curious, and I like understanding how things work all the way down to the hardware.

My last project was entirely software-driven. I designed the kinematics and motion logic, but didn’t actually build the physical arm. This time I wanted to get my hands on sensors, encoders, and a microcontroller, to bridge the gap between simulation and something I can actually see and touch.

The idea for this project came while watching my Roomba navigate around my living room. As it moved from hardwood to a rug, I caught myself thinking:

> *How does it keep its speed up when hitting the rug?*

A few Google searches later I ended up deep in the world of **PID controllers**.

## Hardware Design
### Overview 
The goal here is to write my own PID logic in C++ and run it on real hardware. The system is simple: a 12V DC motor, an encoder for feedback, and an Arduino running the control loop and driving the motor through a motor driver. I added a small 0.96" OLED so I can display position in real time while tuning. It’s a small setup, but it has just enough moving parts to experiment, break things, figure out why they broke, and tweak the control logic. I could pretend every hardware choice was deeply researched, but this is a personal project. Cost was the main factor. If it was inexpensive and had decent reviews, I used it. The one decision that *was* intentional was the controller. I chose a microcontroller instead of a Raspberry Pi because this is a real-time control problem. I don’t need an OS, networking, or a UI. I just need predictable timing and direct control of the hardware. An Arduino fits that better than a Pi. I considered both the **Arduino Uno Rev3** and the **Arduino Nano**. Since they use the same **ATmega328P**, the capabilities are almost identical. I chose the Nano purely for its smaller footprint.

### Hardware List

**Motor / Encoder**  
*12V DC motor with built-in quadrature encoder*  
25GA370 DC Encoder Metal Gearmotor – 12V, 150 RPM, Two-Channel Hall-Effect Encoder

**Microcontroller**  
Arduino Nano [A000005] – ATmega328P, 22 Digital I/O Pins, 8 Analog Inputs, USB Interface

**Breadboard + Jumpers**  
BOJACK Solderless Breadboard Kit – 830pt + 400pt boards and jumper wires

**Power Supply (AC → DC)**  
12V 2A regulated wall adapter

**Buck Converter**  
Maxmoral LM2596 Step-Down (DC-DC) Module – for generating 5V logic power

**Motor Driver**  
DROK L298 Dual H-Bridge Motor Speed Controller – optocoupler isolation, 6.5V–27V input

**Display**  
ELEGOO 0.96" OLED I2C Display (SSD1306-compatible)

### Wiring Diagram

I wanted to keep the wiring for this project as simple as possible, so here’s the whole layout in one place. The idea is straightforward: 12V comes in through the barrel jack, the buck converter steps that down to 5V for all the logic, and the motor driver handles the power for the motor. Everything else (encoder, OLED, Arduino) taps into the shared 5V/GND rails and feeds its signals back into the Arduino for control. I used Fritzing to build the wiring diagram below: 

![Wiring Diagram](diagrams/wiring.png)

**Quick reference:**
- **12V** → motor driver VIN + buck converter IN  
- **Buck converter 5V** → Arduino + OLED + encoder + motor driver logic  
- **Encoder A/B** → D2 / D3 on Arduino
- **OLED SDA/SCL** → A4 / A5 on Arduino
- **Motor driver DIR/PWM** → D4 / D6 on Arduino
- **All grounds** tied into one common rail

## Software Design

### Tooling and Development Environment

There are two parts to this project, the code I designed and the tools I used to write it. For the tooling, I did some research on writing firmware for an Arduino microcontroller. Since I am using an Arduino board I could have used the Arduino IDE, but I chose PlatformIO with VS Code instead. I already use VS Code for everything else, so being able to write embedded software in the same environment felt natural.

Installing PlatformIO was straightforward, and it handles building and uploading to the microcontroller with just a few clicks. It has been great so far, and it is probably what I will continue to use.

---

### Design Approach, or How I Stumbled My Way Into a PI Controller

The difficulty here is that I have no industry experience to guide best practices, so I am navigating this space mostly on my own. To make matters more interesting, I had never written firmware before this project, so I also did not fully understand how the board actually worked.

To fill in some of the gaps I started reading *Making Embedded Systems* by Elecia White. It has been extremely helpful. Two quotes that stuck with me from the software design chapter were:

> “Implement features, make them work, test them out, and then make them smaller or faster as needed,” — Elecia White
 
> “Premature optimization is the root of all evil,” — Donald Knuth

I know that does not mean ignore design entirely and jump right in, but since I had zero embedded experience I decided to lean into tinkering first, just to understand the system. My goal was to get something working, then refine it.

So I started small:

* First I blinked an LED.
* Then I drove the motor in one direction.
* Then I initialized the OLED and displayed a simple message, without worrying about RPM yet.

Each step taught me something, and the structure of the program started to become clearer. At that point I realized I was not that far from a working control loop. All I really needed was the math.

---

### Version 1, Big File, Global Variables, It Works

Version one of the program was essentially everything in a single file. I already had an ISR, pin modes, and global variables defined, so the remaining task was implementing the control math, which was the hardest part conceptually.

I started with proportional control only. I figured to start all I needed was for the motor to respond to a setpoint and increase RPM. I sketched the control flow on paper, translated the proportional equation into code, and got a working response faster than I expected.

For the constants Kp and Ki I literally started with values from the PID Wikipedia article. Kp at 0.5 worked surprisingly well and I left it alone. A Ki of 0.5 was far too high, so through a very scientific process of tinkering I kept lowering it until it worked.

With only proportional control I immediately saw steady state error, so I added the integral term. That required handling integral windup. I implemented a simple gating approach that prevents the integral from accumulating when the motor is at its PWM limits and the error would push it further in the same direction. The PWM output is always clamped to the valid range of 0 to 255.

I did not find overshoot to be severe with just P and I, so I chose not to implement D for now. That may be a future experiment, possibly along with delving into RTOS, which I know nothing about.

---

### Version 2, Refactor Time

The controller worked and the code was functional but messy, with a lot of global state. However, the design became a lot more obvious. I had clear groups of functionality for the motor, the display, and the PI controller.

So, for version two, I refactored the code by creating `Display` and `PID` classes to encapsulate their behavior.

For the display I wanted the user to be able to pass values of different types, within the limits of the Adafruit library, so I built a templated display function that can print different data types without duplicating code.

For the motor I did not create a class and instead used a `struct` to group its core state, such as RPM, setpoint, and PWM. This keeps it lightweight and reflects the fact that the motor module does not own control logic, it simply holds values used by other components.

All shared types are grouped into a `Types.h` file for organization.

---

### Final Thoughts

I get it, this is not an industry level architecture, and an embedded engineer might tsk-tsk and shake their head at the above, but it reflects my current level of experience. As I continue reading and working on embedded systems I expect my design process to become more formal.

For now the focus was on having some old fashioned embedded fun, learning the hardware, implementing a working control loop, and then improving the structure afterward.

**Tinkering for the win!**

## Tests 

### Test 1 (Ki = 0.07)

This test used a 150 RPM setpoint. I logged both the measured RPM and the PWM output over time.

On startup the motor reaches the target quickly with a small overshoot (peaks around ~160 RPM) and then settles close to the setpoint. After about 15–20 seconds the response is stable and stays within a very tight band around 150 RPM.

I also introduced two manual disturbances by briefly loading the shaft with a wheel and pressing my finger on it. Each time, the RPM dropped sharply and then recovered. The controller pushed the PWM higher to compensate, overshot slightly, and then returned to steady state. The recovery was fast but a bit aggressive, which shows up as the larger spikes in both RPM and PWM.

Overall behavior:
- Fast rise to the setpoint  
- Small steady-state error  
- Noticeable overshoot after disturbances  
- PWM increases significantly during recovery  

This indicated that the integral term was contributing strongly during error, which helped eliminate offset but made the recovery more aggressive.

#### Step Response - No Load 

![PID Step Response](Firmware/data/Test1/PID%20Step%20Response.png)

#### PWM Output - No Load

![PID Step Response](Firmware/data/Test1/PWM%20Output%20.png)

#### Step Response - Load 

![PID Step Response](Firmware/data/Test1/PID%20Step%20Response%20Disturbance%20Test.png)

#### PWM Output - Load

![PID Step Response](Firmware/data/Test1/PWM%20Output%20Disturbance%20Test.png)

### Test 2 (Ki = 0.05)

This test used the same 150 RPM setpoint with a lower integral gain.

Startup behavior is similar to Test 1: the motor reaches the setpoint quickly with a smaller overshoot and then settles close to 150 RPM. The steady-state error remains effectively zero.

Two manual disturbances were applied again. The controller still recovers quickly, but the response is noticeably smoother. The RPM spikes are smaller and the PWM output does not jump as aggressively as in Test 1.

Overall behavior compared to Test 1:
- Similar rise time  
- Same steady-state accuracy  
- Reduced overshoot after disturbances  
- Smoother PWM response  

Lowering Ki reduced how strongly the controller reacted to accumulated error, which made recovery less aggressive while maintaining accuracy.

#### Step Response - No Load 

![PID Step Response](Firmware/data/Test2KiLowered/PID%20Step%20Response%20Ki%20Lowered.png)

#### PWM Output - No Load

![PWM Output](Firmware/data/Test2KiLowered/PWM%20output%20Ki%20Lowered.png)

#### Step Response - Load 

![PID Step Response](Firmware/data/Test2KiLowered/PID%20Step%20Response%20Disturbance%20Test%20Ki%20Lowered.png)

#### PWM Output - Load

![PWM Output](Firmware/data/Test2KiLowered/PWM%20Output%20Disturbance%20Test%20Ki%20Lowered%20.png)

### Test 3 - Sampling window lowered. 

This week I went down a bit of a timing rabbit hole. Up to now my control loop was running at a 500 ms sampling window, which I picked arbitrarily. I didn’t question it. That’s 2 Hz, which I’ve since learned is very slow for a motor speed loop. I also learned about drift. My understanding is if the work inside the loop takes longer than the interval, or the controller is late checking, your timing can drift. As you can see in my latest commit, I switched to the increment method on my lastTime variables so the timing stays anchored. To check for drift, the first thing I did was log dt every cycle. I didn’t see any drift, so I assumed from that I must be executing faster than 500 ms, but how much more? 

After that, more out of curiosity, I turned logging off and timed how long the control block actually takes to run. First pass was about 61 ms and the average was around 43 ms, so way faster than a 500 ms window. That led to the next question, which was what the loop rate should actually be. I went digging through a bunch of control engineer forums and kept seeing Nyquist sampling rate mentioned. I definitely don’t understand most of the theory, but the practical takeaway I used was to sample 2 to 3 times faster than the signal’s highest frequency. That made intuitive sense. I thought about a wave that goes up and down every 1 second. If I sampled at 1 second, as opposed to at least 500 ms, I might only capture it at its peak and think it’s a straight line. 

To estimate that response, I followed what others on the forums were suggesting and measured when the motor hit 90% of the setpoint after a sudden change in PWM. So I disabled the I term, ran the same 0 → 150 RPM step, and measured the time to 90% of the setpoint. Result was 306 ms, which is about 3.3 Hz. Since I already knew roughly how fast my loop could run, I went ×3 and got about 10 Hz, so a 100 ms window. I pushed it slightly faster and chose 80 ms, which is about 12.5 Hz. That’s a huge step up from 2 Hz and still well within the Nano’s budget I measured. At that speed, though, the OLED was updating way too fast to be useful, so I split the timing so the motor control runs at 80 ms, the OLED updates at 400 ms, and the serial logging stays at 500 ms. I kept logging at 500 ms so I can compare new tests directly with older tests.

While doing this I had a realization. My integral is calculated as `Integral += error * dt`. Since dt is basically the loop period, a bigger sampling window must mean the integral adds more each step. So by moving from 500 ms to 80 ms, I must effectively be weakening the integral action without touching Ki, right? 

**Test time!**

---

This test kept the same PI gains as the previous test but changed the control loop timing from 500 ms to 80 ms.

Startup behavior is similar in that the motor reaches the setpoint quickly and PWM spikes due to the large initial error. While the initial overshoot is about the same as previous tests, the motor settles much faster than in the 500 ms test. I did notice a bit more ripple around the setpoint (±2–3 RPM) than in the 500 ms test. I'm not sure if that counts as oscillation, but it was noticeable. Two manual disturbances were applied again. The RPM drops and PWM saturates as expected, but when the load is removed the overshoot is much lower than before and the motor returns to the setpoint more quickly. The response is smoother even though the controller is updating more often.

Overall behavior compared to the 500 ms loop:
- Similar rise time  
- Much faster settling time after startup  
- Reduced overshoot after disturbances  
- Faster recovery  
- Slightly more active PWM at steady state  

So I think these tests show that reducing the loop interval reduced the effective integral action because dt is smaller in the integral calculation. This made the controller less aggressive while keeping the same Ki value. While I do see more movement around the setpoint, I don't think it counts as oscillation, and with the reduced overshoot after disturbances I'm still not inclined to introduce D right now.

## Learnings from Project

Below are a couple of things that tripped me up during this project. 

**Breadboard power rails are passive**  
My barrel jack was plugged into normal columns, not the power rails, so only those columns had 12V and GND. The rails stayed unpowered until I jumpered them over. Obvious in hindsight, but a good reminder that breadboards don’t distribute power unless you wire them to.

**Motor Control**  
The motor just spins when you apply power. Speed comes from PWM duty cycle, where the signal is rapidly switched on and off. The percentage of time it stays on determines the average power the motor sees. Direction doesn’t come from “left” or “right” encoder wires. It comes from order: if channel A triggers before B the shaft is moving one way; if B triggers before A it’s the other. Orientation doesn’t matter because the software defines the meaning.

**Keep ISRs enabled as much as possible**  
In my main loop, I originally disabled interrupts and then did real work before re-enabling them (RPM math). I learnt this isn't best practice, and you should keep the interrupts-off window minimal. 

**Atomic snapshot for encoder counts**  
I created a race condition by reading `pulseCount` and resetting it outside the same interrupts-off section. An ISR could fire between those operations, corrupting the measurement window. The fix was to copy and reset the counter inside a single interrupts-off block, then compute RPM afterward.

**Nano bootloader and baud rate mismatch**  
Initial uploads failed with `avrdude: stk500_recv(): programmer is not responding`. The fix was setting `board = nanoatmega328new`, which uses the 115200 baud bootloader. Baud, as I learnt, is simply the data rate on the serial line, and both sides must match.



