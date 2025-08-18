# OpenO2
Codes during internship at Global health Informatics Institute (GHII) under OpenO2 Department

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

# 📂 Folder & File Structure
🔹 Alternator_TestingInteruptWithOptocouplers
Nano_Code_Testing_InteruptWithOptocouplers.ino
-Test code for using optocouplers with the Nano. Demonstrates interrupt-based signal handling.
🔹 Alternator_VoltageToFrequency
Code Explanation
-A written explanation of how the code works.
NanoCode_VoltageToFrequency.ino
-Converts alternator voltage readings to frequency using the Nano.
NanoCode_VoltageToFrequency_Version_2.ino
-Improved version with refinements in mapping and stability.

🔹 Alternator_VoltageToFrequencyWithCustomTimer
Code Explanation
-Documentation of custom timer-based approach.
NanoCode_VoltageToFrequencyCustomTimer.ino
-Implements a custom timer instead of tone() for more precise voltage-to-frequency conversion.

🔹 Alternator_Voltage_Reader_withVoltageDividerReverseEngineering
Alternator_Voltage_Reader_withVoltageDividerReverseEngineering.ino
-Reads alternator voltage using a voltage divider (reverse engineered approach).

🔹 Battery_Battery&AlternatorVoltageReader
UNOCode_Battery_AlternatorVoltageVoltageReader.ino
-Reads both battery and alternator voltage on the UNO and displays/logs the values.

🔹 Battery_SwitchingBattery&Alternator
Code Explanation
-Overview of the switching logic.
UNOCode_SwitchingBattAndAlt.ino
-Basic code for switching between battery and alternator.
UNOCode_SwitchingBattAndAlt_Version_2.ino
-Improved version with refined conditions.
UNOCode_SwitchingBattAndAlt_Version_3.ino
-Adds fast failover logic if no alternator data is received.
UNOCode_SwitchingBattAndAlt_Version_3.1_with_RGB.ino
-Version 3.1 with RGB LED status indicators (Battery = Red, Alternator = Green).
UNOCode_SwitchingBattAndAlt_Version_3_with_RGB.ino
-Another variation of Version 3 with RGB indicators.

🔹 Battery_TestingInteruptWithOptocouplers
UNO_Code_Testing_InteruptWithOptocouplers.ino
-Test code for UNO handling optocoupler signals via interrupts.

🔹 Battery_VoltageToFrequency
UnoCode_VoltageToFrequency.ino
-Maps battery voltage to frequency for transmission/processing.

🔹 Battery_VolyageToFrequencyWithAveraging
UNOCode_VolyageToFrequency_Averaging.ino
-Adds averaging to smooth out noisy battery voltage-to-frequency conversion.


