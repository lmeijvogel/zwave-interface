#include "Manager.h"
#include "Options.h"
#include "Log.h"
#include "ManagerFactory.h"
#include "Driver.h"

namespace MyZWave {
  ManagerFactory::ManagerFactory(std::string port, void (*OnNotification)(OpenZWave::Notification const*, void*)) :
    port(port),
    OnNotification(OnNotification)
  {}

  void ManagerFactory::Create() {
    // Create the OpenZWave Manager.
    // The first argument is the path to the config files (where the manufacturer_specific.xml file is located
    // The second argument is the path for saved Z-Wave network state and the log file.  If you leave it NULL
    // the log file will appear in the program's working directory.
    OpenZWave::Options::Create( "../../../config", "", "" );
    OpenZWave::Options::Get()->AddOptionInt( "SaveLogLevel", OpenZWave::LogLevel_Error );
    OpenZWave::Options::Get()->AddOptionInt( "QueueLogLevel", OpenZWave::LogLevel_Error );
    OpenZWave::Options::Get()->AddOptionInt( "DumpTrigger", OpenZWave::LogLevel_Error );
    OpenZWave::Options::Get()->AddOptionInt( "PollInterval", 500 );
    OpenZWave::Options::Get()->AddOptionBool( "IntervalBetweenPolls", true );
    OpenZWave::Options::Get()->AddOptionBool("ValidateValueChanges", true);
    OpenZWave::Options::Get()->AddOptionInt("DriverMaxAttempts", 3);
    OpenZWave::Options::Get()->Lock();

    OpenZWave::Manager::Create();
    // Add a callback handler to the manager.  The second argument is a context that
    // is passed to the OnNotification method.  If the OnNotification is a method of
    // a class, the context would usually be a pointer to that class object, to
    // avoid the need for the notification handler to be a static.
    OpenZWave::Manager::Get()->AddWatcher( OnNotification, NULL );

    OpenZWave::Manager::Get()->AddDriver( port );

  }

  void ManagerFactory::Destroy() {
    OpenZWave::Manager::Get()->RemoveDriver( "/dev/ttyUSB0" );
    OpenZWave::Manager::Get()->RemoveWatcher( OnNotification, NULL );

    OpenZWave::Manager::Destroy();
    OpenZWave::Options::Destroy();
  }
}
