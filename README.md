# 🚀 How to Use
-Choose your hardware setup (Nano or UNO).

-Nano sketches are in folders prefixed with Alternator_.

-UNO sketches are in folders prefixed with Battery_.

-Upload the correct sketch from the folder that matches your test:

-For voltage-to-frequency conversion, use files in *_VoltageToFrequency folders.

-For switching between battery & alternator, use files in Battery_SwitchingBattery&Alternator.

-For testing optocouplers with interrupts, use files in *_TestingInteruptWithOptocouplers.

-Refer to the "Code Explanation" files for a breakdown of logic where provided.

# Recommended workflow:
Start with Testing codes → Verify signals (optocouplers, voltage dividers).
Move to Voltage Reader codes → Ensure proper voltage readings.
Progress to Voltage-to-Frequency codes → Convert readings for communication.
Finally, test Switching logic → Automatic alternator/battery switching with RGB indicators.

# Project Code Navigation Guide
This branch contains different Arduino sketches and explanations related to alternator and battery monitoring, voltage-to-frequency conversion, and switching logic.
The files are grouped into folders for easier navigation.

