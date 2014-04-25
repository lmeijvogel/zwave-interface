#include "PidfileService.h"

#include <sys/unistd.h>
#include <stdio.h>
#include <string>

namespace MyZWave {
  PidfileService::PidfileService(std::string PidfileName) :
    PidfileName(PidfileName)
  { }

  void PidfileService::CreatePidfile() {
    pid_t pid = getpid();

    FILE *file;
    file = fopen(PidfileName.c_str(), "w");

    fprintf(file, "%i\n", pid);

    fclose(file);
  }

  bool PidfileService::PidfileExists() {
    FILE *file;

    bool result = false;

    file = fopen(PidfileName.c_str(), "r");

    if (file) {
      result = true;

      fclose(file);
    }

    return result;
  }

  void PidfileService::DeletePidfile() {
    unlink(PidfileName.c_str());
  }
}
