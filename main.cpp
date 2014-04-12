//-----------------------------------------------------------------------------
//
//  Main.cpp
//
//  The main program: Open a listening socket and wait for instructions.
//
//  License: Lesser GPL
//-----------------------------------------------------------------------------

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "Options.h"
#include "Manager.h"
#include "Driver.h"
#include "Node.h"
#include "Group.h"
#include "Notification.h"
#include "ValueStore.h"
#include "Value.h"
#include "ValueBool.h"
#include "Log.h"
#include "Common.h"
#include "SocketReader.h"

using namespace std;

static bool   g_initFailed = false;

typedef struct
{
  uint32      m_homeId;
  uint8      m_nodeId;
  bool      m_polled;
  list<OpenZWave::ValueID>  m_values;
}NodeInfo;

void CreateManager();
void CleanUp();
void SignalReceived(int signal);
NodeInfo *FindNodeById(uint8 id);
bool SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 value);

static list<NodeInfo*> g_nodes;
static pthread_mutex_t g_criticalSection;
static pthread_cond_t  initCond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;

MyZWave::SocketReader *socket_reader = new MyZWave::SocketReader(2014);

uint32 MyZWave::g_homeId;
//-----------------------------------------------------------------------------
// <GetNodeInfo>
// Return the NodeInfo object associated with this notification
//-----------------------------------------------------------------------------
NodeInfo* GetNodeInfo
(
  OpenZWave::Notification const* _notification
)
{
  uint32 const homeId = _notification->GetHomeId();
  uint8 const nodeId = _notification->GetNodeId();
  for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
  {
    NodeInfo* nodeInfo = *it;
    if( ( nodeInfo->m_homeId == homeId ) && ( nodeInfo->m_nodeId == nodeId ) )
    {
      return nodeInfo;
    }
  }

  return NULL;
}

void parseCommand(std::string input) {
  int nodeId, classId, index, level;

  printf("RAW: %s\n", input.c_str());
  if (sscanf(input.c_str(), "%i 0x%x 0x%x %i\n", &nodeId, &classId, &index, &level) == 4) {
    printf("Received %i, 0x%x, 0x%x, %i\n", nodeId, classId, index, level);
    NodeInfo *nodeInfo = FindNodeById(nodeId);

    if (!nodeInfo) {
      printf("!!! Did not find node %i\n", nodeId);
      return;
    }

    if (!nodeInfo->m_nodeId) {
      printf("Node unknown!\n");
      return;
    }

    pthread_mutex_lock( &g_criticalSection );

    if (SetValue(nodeInfo, classId, index, level)) {
      printf("Success!\n");
    } else {
      // This can e.g. occur when newLevel is not a byte but the valuetype is.
      printf("\n\n!!!! COULD NOT SET VALUE !!!!\n\n");
    }

    // but NodeInfo list and similar data should be inside critical section
    pthread_mutex_unlock( &g_criticalSection );
  }
}

//-----------------------------------------------------------------------------
// <OnNotification>
// Callback that is triggered when a value, group or node changes
//-----------------------------------------------------------------------------
void OnNotification
(
  OpenZWave::Notification const* _notification,
  void* _context
)
{
  // Must do this inside a critical section to avoid conflicts with the main thread
  pthread_mutex_lock( &g_criticalSection );

  switch( _notification->GetType() )
  {
    case OpenZWave::Notification::Type_ValueAdded:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        // Add the new value to our list
        nodeInfo->m_values.push_back( _notification->GetValueID() );
      }
      break;
    }

    case OpenZWave::Notification::Type_ValueRemoved:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        // Remove the value from out list
        for( list<OpenZWave::ValueID>::iterator it = nodeInfo->m_values.begin(); it != nodeInfo->m_values.end(); ++it )
        {
          if( (*it) == _notification->GetValueID() )
          {
            nodeInfo->m_values.erase( it );
            break;
          }
        }
      }
      break;
    }

    case OpenZWave::Notification::Type_ValueChanged:
    {
      // One of the node values has changed
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case OpenZWave::Notification::Type_Group:
    {
      // One of the node's association groups has changed
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case OpenZWave::Notification::Type_NodeAdded:
    {
      // Add the new node to our list
      NodeInfo* nodeInfo = new NodeInfo();
      nodeInfo->m_homeId = _notification->GetHomeId();
      nodeInfo->m_nodeId = _notification->GetNodeId();
      nodeInfo->m_polled = false;
      g_nodes.push_back( nodeInfo );
      break;
    }

    case OpenZWave::Notification::Type_NodeRemoved:
    {
      // Remove the node from our list
      uint32 const homeId = _notification->GetHomeId();
      uint8 const nodeId = _notification->GetNodeId();
      for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
      {
        NodeInfo* nodeInfo = *it;
        if( ( nodeInfo->m_homeId == homeId ) && ( nodeInfo->m_nodeId == nodeId ) )
        {
          g_nodes.erase( it );
          delete nodeInfo;
          break;
        }
      }
      break;
    }

    case OpenZWave::Notification::Type_NodeEvent:
    {
      // We have received an event from the node, caused by a
      // basic_set or hail message.
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case OpenZWave::Notification::Type_PollingDisabled:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo->m_polled = false;
      }
      break;
    }

    case OpenZWave::Notification::Type_PollingEnabled:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo->m_polled = true;
      }
      break;
    }

    case OpenZWave::Notification::Type_DriverReady:
    {
      MyZWave::g_homeId = _notification->GetHomeId();
      break;
    }

    case OpenZWave::Notification::Type_DriverFailed:
    {
      g_initFailed = true;
      pthread_cond_broadcast(&initCond);
      break;
    }

    case OpenZWave::Notification::Type_AwakeNodesQueried:
    case OpenZWave::Notification::Type_AllNodesQueried:
    case OpenZWave::Notification::Type_AllNodesQueriedSomeDead:
    {
      pthread_cond_broadcast(&initCond);
      break;
    }

    case OpenZWave::Notification::Type_DriverReset:
    case OpenZWave::Notification::Type_Notification:
    case OpenZWave::Notification::Type_NodeNaming:
    case OpenZWave::Notification::Type_NodeProtocolInfo:
    case OpenZWave::Notification::Type_NodeQueriesComplete:
    default:
    {
    }
  }

  pthread_mutex_unlock( &g_criticalSection );
}

bool g_cleaningUp = false;

//-----------------------------------------------------------------------------
// <main>
// Create the driver and then wait
//-----------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  pthread_mutexattr_t mutexattr;

  pthread_mutexattr_init ( &mutexattr );
  pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE );
  pthread_mutex_init( &g_criticalSection, &mutexattr );
  pthread_mutexattr_destroy( &mutexattr );

  pthread_mutex_lock( &initMutex );

  signal(SIGABRT, &SignalReceived);
  signal(SIGTERM, &SignalReceived);
  signal(SIGINT, &SignalReceived);

  printf("Starting MinOZW with OpenZWave Version %s\n", OpenZWave::Manager::getVersionAsString().c_str());

  CreateManager();

  // Add a callback handler to the manager.  The second argument is a context that
  // is passed to the OnNotification method.  If the OnNotification is a method of
  // a class, the context would usually be a pointer to that class object, to
  // avoid the need for the notification handler to be a static.
  OpenZWave::Manager::Get()->AddWatcher( OnNotification, NULL );

  // Add a Z-Wave Driver
  // Modify this line to set the correct serial port for your PC interface.

#ifdef DARWIN
  string port = "/dev/cu.usbserial";
#elif WIN32
        string port = "\\\\.\\COM6";
#else
  string port = "/dev/ttyUSB0";
#endif
  if( strcasecmp( port.c_str(), "usb" ) == 0 )
  {
    OpenZWave::Manager::Get()->AddDriver( "HID Controller", OpenZWave::Driver::ControllerInterface_Hid );
  }
  else
  {
    OpenZWave::Manager::Get()->AddDriver( port );
  }

  // Now we just wait for either the AwakeNodesQueried or AllNodesQueried notification,
  // then write out the config file.
  // In a normal app, we would be handling notifications and building a UI for the user.
  pthread_cond_wait( &initCond, &initMutex );

  // Since the configuration file contains command class information that is only
  // known after the nodes on the network are queried, wait until all of the nodes
  // on the network have been queried (at least the "listening" ones) before
  // writing the configuration file.  (Maybe write again after sleeping nodes have
  // been queried as well.)
  if( !g_initFailed )
  {
    // If we want to access our NodeInfo list, that has been built from all the
    // notification callbacks we received from the library, we have to do so
    // from inside a Critical Section.  This is because the callbacks occur on other
    // threads, and we cannot risk the list being changed while we are using it.
    // We must hold the critical section for as short a time as possible, to avoid
    // stalling the OpenZWave drivers.
    socket_reader->listen(&parseCommand);

    // Sleep a bit more to make sure that any messages will be sent
    sleep(1);

    OpenZWave::Driver::DriverData data;
    OpenZWave::Manager::Get()->GetDriverStatistics( MyZWave::g_homeId, &data );
    printf("SOF: %d ACK Waiting: %d Read Aborts: %d Bad Checksums: %d\n", data.m_SOFCnt, data.m_ACKWaiting, data.m_readAborts, data.m_badChecksum);
    printf("Reads: %d Writes: %d CAN: %d NAK: %d ACK: %d Out of Frame: %d\n", data.m_readCnt, data.m_writeCnt, data.m_CANCnt, data.m_NAKCnt, data.m_ACKCnt, data.m_OOFCnt);
    printf("Dropped: %d Retries: %d\n", data.m_dropped, data.m_retries);
  }

  CleanUp();

  return 0;
}

void CreateManager() {
  // Create the OpenZWave Manager.
  // The first argument is the path to the config files (where the manufacturer_specific.xml file is located
  // The second argument is the path for saved Z-Wave network state and the log file.  If you leave it NULL
  // the log file will appear in the program's working directory.
  OpenZWave::Options::Create( "../../../config/", "", "" );
  OpenZWave::Options::Get()->AddOptionInt( "SaveLogLevel", OpenZWave::LogLevel_Error );
  OpenZWave::Options::Get()->AddOptionInt( "QueueLogLevel", OpenZWave::LogLevel_Error );
  OpenZWave::Options::Get()->AddOptionInt( "DumpTrigger", OpenZWave::LogLevel_Error );
  OpenZWave::Options::Get()->AddOptionInt( "PollInterval", 500 );
  OpenZWave::Options::Get()->AddOptionBool( "IntervalBetweenPolls", true );
  OpenZWave::Options::Get()->AddOptionBool("ValidateValueChanges", true);
  OpenZWave::Options::Get()->Lock();

  OpenZWave::Manager::Create();

}

void SignalReceived(int signal) {
  printf("Stopping socket_reader\n");
  socket_reader->stop();
}

void CleanUp() {
  if (g_cleaningUp) {
    return;
  }

  g_cleaningUp = true;

  OpenZWave::Manager::Get()->RemoveDriver( "/dev/ttyUSB0" );
  OpenZWave::Manager::Get()->RemoveWatcher( OnNotification, NULL );
  OpenZWave::Manager::Destroy();
  OpenZWave::Options::Destroy();
  pthread_mutex_destroy( &g_criticalSection );

  delete socket_reader;
  exit(0);
}

NodeInfo *FindNodeById(uint8 id) {
  for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
  {
    NodeInfo* nodeInfo = *it;

    // skip the controller (most likely node 1)
    if( nodeInfo->m_nodeId == 1) continue;
    if( nodeInfo->m_nodeId == id) {
      return nodeInfo;
    }
  }

  return NULL;
}

bool SetValue(NodeInfo *nodeInfo, uint8 classId, uint8 index, uint8 value) {
  for( list<OpenZWave::ValueID>::iterator it2 = nodeInfo->m_values.begin(); it2 != nodeInfo->m_values.end(); ++it2 )
  {
    OpenZWave::ValueID v = *it2;
    if (v.GetCommandClassId() == classId && v.GetIndex() == index) {
      printf("SetValue: (0x%x, 0x%x)\n", v.GetCommandClassId(), v.GetIndex());
      OpenZWave::Manager::Get()->SetValue(v, value);
      return true;
    }
  }
  return false;
}
