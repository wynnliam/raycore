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
Google will help you here, since installing it is dependent on the OS you are using.
[Go here](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/) for instructions to install
SDL2.

Please note that I developed this in a Linux environment. Thus, while I assume that
the system will work regardless of the OS you are using, I cannot guarantee this will
work on say Windows.

Once you've got that installed, make sure you are in the root of the project.
When you are there, run the command

```make all```

Provided there are no errors, you should be able to navigate to the `bin/` folder.
Then run

```./raycore.out```

You should now be able to run the application!

## Credits
[MatchaJoeJoe](https://matchajoejoe.com/) for making the awesome cosmonaut player sprite.

[Christian Vallentin](https://github.com/vallentin) for making `loadbmp`

## Developer Documentation
For a how-to on adding features or building a game using raycore, please see the
repo [wiki](https://github.com/wynnliam/raycore/wiki).

# Issue Summary
Please navigate to the [Issue tab](https://github.com/wynnliam/raycore/issues) for the
most up-to-date list of issues. Nonetheless, I'll try my best to keep a list here for
quick viewing.

Current outstanding issues:
- If you fail to load a level, the system gets stuck in a loop. Ctrl-C, 'P' key, exit button
do not work. For now, use `ps -aux` (or some equivalent command) to find the id of raycore, and
then run `kill -9 [RAYCORE_ID]` where the numeric id of raycore goes where the `[RAYCORE_ID]` is.

# Roadmap
raycore is a project that will be on-going. That is, I will like to add several features to
it that make it a more robust engine. As features come up, I'll add them here. Furthermore, as
features become more fleshed out, I will update them here. Roughly speaking, features that
become in active development will be moved into Issue Summary.

With all this said, here is the current laundry list of dream features, ordered roughly
by their priority:

* Clean up what already exists! This is the key one because right now, raycore is a mess! A lot of it
was slapped together, and if we go in with a poor foundation, this thing will inevitably implode.

* Entities. Currently, levels consist of two object types: geometry (walls, floors, ceilings, sky texture),
and "things", which are props that are rendered as sprites. Entities would be a third object in levels that
control some part of game or environment logic. These will be player spawn points, sounds, AI, etc.

* AI. This will be a rewrite of an AI system I did a few years ago with An Engine of Ice and Fire (which you
can find on my github). However, this will be even more simplistic, as I will only include the ability to find
a path from two points A to B. Everything beyond that is pretty game-dependent. This will essentially be A-star
pathfinding.

* Sound. Right now you can render environments, walk around them, and (when Entities are done) do stuff in them.
But it's like a silent movie. I would like to add a robust sound system. The plan is to use OpenAL for this,
so that I can do 3D sounds. Basically, if a gun goes off in one corner of the level, you'll only hear it
if you're close to it AND you'll hear it based on its relative position to you.

* GUI System. Basically things like menus and whatnot. Like AI, I had a system in An Engine of Ice and Fire
that worked fairly well, which I'd like to bring back and spruce up here. This is only a neccessity for when
there are more game elements to interact with. I want to avoid "dangling features" or neat features that I
don't do anything with for a long time.

* Multiplayer. I think it'd be very cool to have the ability to connect with other players in a game. This
is a faaar off dream right now, since integrating it will require a lot of rewriting. As such, I'd like to
have a more "defined" engine in place before I decide how it integrates into the system.

This list is subject to change, to say the least.

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
