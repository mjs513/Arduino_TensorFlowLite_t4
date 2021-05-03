# Arduino_TensorFlowLite_t4
Arduino TensorFlowLite for the T4

This is a sligthly modified version of the Arduino TensorFlowLite library that can be downloaded from the Arduino Libraries manager.  Two changes had to made:
1. cmsis_gcc.h was added to the ....\Arduino_TensorFlowLite_t4\src\tensorflow\lite\micro\tools\make\downloads\cmsis\CMSIS\Core\Include directory
2. include <string.h> add to ...\Arduino_TensorFlowLite_t4\src\tensorflow\lite\micro\tools\make\downloads\kissfft\kissfft.c

Still have a few compiler warnings but still works.

The following examples have been updated to work with T4x
1.  hello_world.ino
2.  micro_speech.ino - will need the audio sheild with microphone.  Thanks to @andrewpiepenbrink PJRC forum user for the output handler: https://forum.pjrc.com/threads/57441-Tensorflow-on-Teensy?highlight=tensorflow
3.  magic_wand.ino using the SparkFun_LIS2DH12 accelerometer

remaining 2 examples may be updated at some point.
