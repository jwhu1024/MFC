Compiling The Dev-C++ Project
------------------------------


Everything is included, so set-up should be very simple. Please note, however, that you will need to re-link libws2_32.a to the project if you put it in any directory other than C:\Dev-Cpp. Otherwise you'll get a compiler error when it doesn't find the library.


Compiling with Other Compilers
-------------------------------


You'll need to link libws2_32.a to the project, and add the following files:

1. Chat1.cpp
2. Chat1.h
3. Chat1.rc

The Chat1_private.h and Chat1_private.rc are for Dev-C++ use only.

Beyond that, I really can't help much, but it shouldn't be too much trouble to get everything working; as it's a relatively simple program :).


Copyright
----------


The source code is NOT copyrighted and you may use it, modify it, and mess around with it to your heart's content, because it's too simple to copyright. HOWEVER, this does not apply to the tutorial...