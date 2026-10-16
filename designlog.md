## Concepts learnt

This project has taught me:
* How to capture and decode a byte and translate it to something useful
* How to drive a DC motor that needs much more power than a microcontroller can provide
* Using a L293D Darlington Pair IC chip and why they are important as they provide a lot of current
* Concept of back emf from a DC motor suddenly stopping

## Issues Encountered

*The slow down before changing direction logic was added after I burned on side of the IC due to the back emf destroying 1 of the 2 transistors pairs on my IC.
*The IR receiver would miss inputs given at extreme angles and so you must be within its stated datasheet FOV to communicate with it via the remote
