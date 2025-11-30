# PID Motor Control Project

## Introduction

This project is the next step in my exploration of robotics. Unlike my last project—which focused on higher-level motion control and kinematics in a simulated 2-DOF robotic arm—this one leans much more into the hardware side.

I wanted to use this README as more than just installation instructions. Consider it a small project showcase: a place where I walk through my design decisions, what I learned, the challenges I hit, and the fun (and sometimes frustrating) bits of actually wiring, tuning, and debugging real hardware.

If you *do* want to clone this and build your own version, the setup instructions are at the bottom. But if you’re reading this because I’ve applied for a role and you want a look into what I’ve been building in my own time—welcome. This is for you.

Could I put all of this on a personal website? Sure. But nobody really visits personal websites anymore, and I’d rather keep everything in one place, close to the code.

---

## Why a PID Controller?

If you’ve glanced around my GitHub, you’ve probably noticed that my interests tend to orbit two areas: **C++** and **robotics**. I’m still early in this space and feeling out where I’d like to go, but one thing is consistent: I’m curious, and I like understanding how things work all the way down to the hardware.

My last project was almost entirely software-driven. I designed the kinematics and motion logic, but didn’t actually build the physical arm. This time I wanted to get my hands on sensors, encoders, and a microcontroller—to bridge the gap between simulation and something I can actually see and touch.

The idea for this project came while watching my Roomba navigate around my living room. As it moved from hardwood to a rug, I caught myself thinking:

> *How does it maintain a consistent speed across totally different surfaces?*

A few Google searches later I ended up deep in the world of **PID controllers**.

A PID controller lets you maintain stability in almost any measurable variable: **speed**, **position**, **orientation**, **pressure**, **temperature**—you name it. They’re fundamental to many robotic systems, and building one felt like the right way to start working with real hardware and learning how software interacts with the physical world.

---

