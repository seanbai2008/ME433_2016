ME433 Advanced Mechatronics
---------------------------------
Assignment 16

<img src="https://github.com/seanbai2008/ME433_2016/blob/master/HW16/figure/1.png">


Basically in my final design does not includes the laser cut layout. All the structure are 3D printed.
I used 3 sliders. I have 3 sliders in the app. Each of them controls the threasholds of RPG values. I programmed the phone to sent "COM" to the pic.

Pic recevied the "COM" which is ranging from 0-640 then it does proportional control to produce the PWM for both wheels. When COM is less than 300, then the PWM for the right wheel increases linearly with COM decreases. In the meantime, the PWM for the left wheel decreases linearly as COM drops. Similarly, when COM is larger than 360, then the PWM for the left wheel increases linearly with COM decreases. In the meantime, the PWM for the right wheel decreases linearly as COM drops.


