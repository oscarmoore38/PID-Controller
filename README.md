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

> *How does it maintain a consistent speed across totally different surfaces?*

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

## Hardware Used

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



