# MessageThrottler
Implementation of Message Throttling using SlidingWindow Mechanism.

# Build instructions.
- Ensure g++ compiler along with support for boost and c++ standard thread library is present. Then just run the below   
  commands.
  
  cmake CMakeLists.txt
  make clean
  make
  ./ThrottleDemo

Read the code and play around with configuration passed to Application object app to tune and change the parameters to study the behaviour.Reducing eviction Time will demonstrate how many messages were evicted to 
terminal output [ std::cout ].

# Author : Anand Kulkarni

Anand Kulkarni 28/07/2018 - Look at the build script carefully. We have incorporated the support for log4cpp [ thread safe logging ], xerces-c for supporting reading config 
xml file for parameters.We need in total three Libraries for building the code. -lCppConfigManager -lxerces-c -llog4cpp. The cppConfgManager library has to be picked from
other git repo namely the CppConfigManager [ https://github.com/anandkulkarnisg/CppConfigManager ].
