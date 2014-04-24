#ifndef TimeService_H_
#define TimeService_H_

namespace MyZWave {
  class TimeService {
    public:
      bool IsNight();
      bool IsMorning();

    private:
      int GetHour();
  };
}
#endif
