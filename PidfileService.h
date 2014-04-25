#ifndef PidfileService_H_
#define PidfileService_H_
#include <stdlib.h>
#include <string>

namespace MyZWave {
  class PidfileService {
    public:
      PidfileService(std::string PidfileName);
      void CreatePidfile();
      void DeletePidfile();

    private:
      bool PidfileExists();

      std::string PidfileName;
  };
}
#endif
