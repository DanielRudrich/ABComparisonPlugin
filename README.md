# ABComparisonPlugin
The ABComparison is a simple channel routing plug-in for AB-comparison tests.

As per default, the plug-in can switch between **10 different input streams** with configurable **channel width** (up to 32 channels). 

There are **two switching modes**: the *exclusive solo* mode and *toggle mode*. The first one makes sure that only one choice is playing. 
The **fade-time** can be set to values between 0ms and 1000ms.

**Usage example**: Switch between three 5.1 mixes
 - create a bus with at least 3 * 6 = 18 channels
 - insert the ABComparison plugin
 - route your first 5.1 mix to channels 1 to 6
 - route your second 5.1 mix to channels 7 to 12
 - route your third 5.1 mix to - who would have guessed - channels 13 to 18
 - set number of choices to 3
 - set channel width to 6
 - select one of the coices
 - output will be routed to the first 6 output channels


Made with the [JUCE framework](http://github.com/weareroli/juce/)
