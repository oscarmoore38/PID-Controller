# PID Motor Control Project

## Introduction

This project is the next step in my exploration of robotics. Unlike my last project—which focused on higher-level motion control and kinematics in a simulated 2-DOF robotic arm—this one leans much more into the hardware side.

I wanted to use this README as more than just installation instructions. Consider it a small project showcase: a place where I walk through my design decisions, what I learned, the challenges I hit, and the fun (and sometimes frustrating) bits of actually wiring, tuning, and debugging real hardware.

If you *do* want to clone this and build your own version, the setup instructions are at the bottom. But if you’re reading this because I’ve applied for a role and you want a look into what I’ve been building in my own time—welcome. This is for you.

Could I put all of this on a personal website? Sure. But nobody really visits personal websites anymore, and I’d rather keep everything in one place, close to the code.

---

## Why build a PID Controller?

If you’ve glanced around my GitHub, you’ve probably noticed that my interests tend to orbit two areas: **C++** and **robotics**. I’m still early in this space and feeling out where I’d like to go, but one thing is consistent: I’m curious, and I like understanding how things work all the way down to the hardware.

My last project was entirely software-driven. I designed the kinematics and motion logic, but didn’t actually build the physical arm. This time I wanted to get my hands on sensors, encoders, and a microcontroller—to bridge the gap between simulation and something I can actually see and touch.

The idea for this project came while watching my Roomba navigate around my living room. As it moved from hardwood to a rug, I caught myself thinking:

> *How does it keep its speed up when hitting the rug?*

A few Google searches later I ended up deep in the world of **PID controllers**.

A PID controller lets you maintain stability in almost any measurable variable: **speed**, **position**, **orientation**, **pressure**, **temperature**—you name it. They’re fundamental to many robotic systems, and building one felt like the right way to start working with real hardware and learning how software interacts with the physical world.

---

## What exaclty is a PID Controller?

PID stands for *Proportional, Integral, Derivative*. It’s a type of closed-loop controller that reduces error — basically the difference between where you want the system to be and where it currently is.

The way it clicked for me was by thinking about my car’s adaptive cruise control. If I set it to 70 mph, that’s my target (or “setpoint,” SP). If I’m currently at 62 mph, that’s the “process variable” (PV). The difference between the two — 8 mph — is the error. A PID controller takes that error and uses it in three different ways: P, I, and D.

---

### Proportional (P)

Formally, P means the output is proportional to the size of the error. Or in simple terms: the bigger the error, the harder the system pushes.

But P-only control has some quirks. As you get closer to the setpoint, P naturally weakens, and the system can end up consistently stopping just short for a while — a steady-state error. On the other hand, when the error is large at the start, P can push pretty hard, and because real systems have momentum, you can still overshoot even though P is “softening” near the target.

---

### Integral (I)

Integral looks at error *over time*. If you’ve been wrong for a while, I ramps up to eliminate whatever offset P can’t fully fix. It’s great for cleaning up that last little bit of error.

But it comes with a downside: **integral windup**.

If the system can’t physically move — maybe the motor is saturated, or something is blocking it — the I term doesn’t know that. It just keeps accumulating like a stretched rubber band. When the system finally can move again, all that stored correction comes blasting out at once and you get a huge overshoot.

---

### Derivative (D)

D is basically the brakes. More formally, it reacts to the rate of change of the error. If the system is rushing toward the target too quickly, D says, “alright, slow down,” and prevents overshoot.

It also helps with jitter — which are tiny rapid back-and-forth movements around the setpoint — because jitter produces fast changes in error, and D pushes back against that.

---

One thing I thought was pretty neat when learning about PID controllers is that not every system uses the full PID. For example, I read that some robotic arms use just P and D. Since I is slow and brings the risk of windup (especially if something blocks the arm), a lot of engineers prefer to skip it entirely and rely on P and D. I originally assumed you’d always want I to eliminate steady-state error and get perfect positioning, but apparently a well-tuned PD loop, modern encoders, and good mechanical design can hit target angles with sub-degree accuracy. There’s obviously a lot more behind how real robotic systems use just PD - with lots of terms I don't understand yet - and my explanation here is really surface-level, but I’m not there yet. Perhaps I’ll dig into it one day, but, for now, I’m sticking to what I actually need to get this project moving.

## Project Details

Ok, now into the details of this project. The ultimate goal here is to play around with writing my own PID logic in C++. To do that, I’m building a simple closed-loop system made up of a 12V DC motor, an encoder, and an Arduino microcontroller that will run the PID loop and send commands to the motor driver. I’ve also added a 0.96" OLED display module so I can visually show the current position while everything is running. It’s a small setup, but it gives me just enough moving parts to experiment, break things, learn why they broke, and tweak the control logic as I go.

I could list out all the hardware I bought and pretend there were deep reasons for each choice, but honestly, this is a personal DIY project. Nothing here is supporting anything critical, so cost was the main factor behind every purchase. If it was cheap and had reasonable reviews, I bought it.

The one thing that probably is worth talking about is the choice of “brain” for the system — specifically, why I went with a microcontroller board instead of a Raspberry Pi. A Raspberry Pi would be the better choice if this project needed higher-level computing — camera inputs, running multiple processes, logging data, or any kind of networking or UI. Basically, if the project needed “computer stuff.” But for a simple real-time control loop where timing needs to be predictable and I just need to run my own C++ logic without an OS getting in the way, an Arduino felt like the better fit.

There were a few different Arduinos I could have chosen. The two I considered were the **Arduino Uno Rev3** and the **Arduino Nano**. Both use an **ATmega328P** microcontroller, so there wasn’t much daylight between them in terms of capability. I ultimately chose the **Nano** because of its smaller footprint.

---

## Hardware Design

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



