# LED-Audio-Visualizer

audio-visualizer.ino - contians the code I wrote to control two strips of LEDs  
                       using an Arduino Uno and a Sparkfun Spectrum Shield. I 
                       manually chose the colors by using a color picker on 
                       Google and used a series of if statements to set the RGB
                       values based on the current frequency of the music, and 
                       then shifted the LED colors over in groups of 5. New colors 
                       were slowly introduced to the LED strip by first taking up a
                       single LED, then two LEDs, then three, all the way up until 
                       the color spanned 5 LEDs, and then were slowly faded out of 
                       the strip by doing the opposite. Colors introduced on one
                       end of the LED strip travel across the LED strip in groups 
                       of 5 LEDs.
                       
arduino-case-top.stl & arduino-case.stl - STL files that I created using OnShape
                                          to contain the Arduino Uno and Sparkfun
                                          Spectrum Shield, which are also included
                                          in my STL repo.
