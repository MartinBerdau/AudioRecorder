/*
Copyright 2020 Martin Berdau

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This File creates a char time label from ellapsedMillis used on the display.

*/

#include "RunningTimeLabel.h"

RunningTimeLabel::RunningTimeLabel()
{
}

void RunningTimeLabel::updateLabel(int tPassedMilli, char* TimerVal){
    TimerVal[0] = char(int(tPassedMilli/3600000/10)+'0');
    TimerVal[1] = char(int(tPassedMilli/3600000%10)+'0');
    TimerVal[3] = char(int(tPassedMilli/60000/10%6)+'0');
    TimerVal[4] = char(int(tPassedMilli/60000%10)+'0');
    TimerVal[6] = char(int(tPassedMilli/1000/10%6)+'0');
    TimerVal[7] = char(int(tPassedMilli/1000%10)+'0');
}
