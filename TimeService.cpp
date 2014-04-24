#include "TimeService.h"
#include "Common.h"

namespace MyZWave {
  bool TimeService::IsNight() {
    int hour = GetHour();
    return (0 < hour && hour < 6);
  }

  bool TimeService::IsMorning() {
    int hour = GetHour();
    return (6 < hour && hour < 12);
  }

  int TimeService::GetHour() {
    struct tm * timeInfo;

    time_t currentTime = time(NULL);

    timeInfo = localtime(&currentTime);

    return timeInfo->tm_hour;
  }
}
