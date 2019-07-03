# raycore
Copyright (c) 2019 Liam Wynn

A simple raycasting based game engine

# Overview
raycore is a simple game engine. The backbone of the engine is a technique
called "raycasting". Please read [this Wikipedia article](https://en.wikipedia.org/wiki/Ray_casting)
for a quick summary. For an even quicker summary, raycasting is a way
to simulate 3D environments. It was most famously used in the game
Wolfenstein 3D by iD Software in the early 90's.

## How do I run raycore?
To run raycore, you need to have SDL2 (Simple Direct Media Layer) installed.

Once you've got that installed, make sure you are in the root of the project.
When you are there, run the command

```make all```

Provided there are no errors, you should be able to navigate to the `bin/` folder.
Then run

```./raycore.out```

You should now be able to run the application!

# Issue Summary
Please navigate to the [Issue tab](https://github.com/wynnliam/raycore/issues) for the
most up-to-date list of issues. Nonetheless, I'll try my best to keep a list here for
quick viewing.

Current outstanding issues:
- LICENSE needs to be added
- Original raycasting system needs to be imported.

# Licensing
This program is licensed uder the "MIT License". Please see the `LICENSE` in the source distribution
of this software for license terms.

# A brief history of raycore
In brief, raycore is the project for my CS 461p class at Portland State Unviersity.
However, raycore has a history before thic class. In the Spring of 2018, I implemented
a raycasting-based renderer for CS 410p (Full Stack Web Development). Indeed, it
was a fully front-end web app program. It was written in C, which was compiled into
Javascript (using emscripten).

When this class ended, I continued tuning the system. I occasionally added features here
and there. In the Fall of 2018, I began working on the raycaster again for another class.
This time it was CS 446 (Advance Topics in Machine Learning). My goal was to implement a
texture generating system using a Generative Adversarial Neural Network. Ultimately, I
abandoned that model for a Variational Autoencoder. This project is still ongoing. When it
is more complete, I shall post it somewhere on Github.

Between the Winter and Spring of 2019, I started refering to the raycaster as "Spaceman".
The vision was to put my renderer to use and make a simple walking simulator. The premise
was you were to be a spaceman that just... walked around. You see, my renderer couldn't
do anything other than walk around. "Spaceman" was to be a more complete tech demo.

Now, for CS 461p, my last class at Portland State, I wanted to build something more complete.
So my goal is to build a complete engine, which I have hence dubbed "raycore".
