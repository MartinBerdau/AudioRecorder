// Header kommt noch

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
