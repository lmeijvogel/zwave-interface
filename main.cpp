//-----------------------------------------------------------------------------
//
//  Main.cpp
//
//  Minimal application to test OpenZWave.
//
//  Creates an OpenZWave::Driver and the waits.  In Debug builds
//  you should see verbose logging to the console, which will
//  indicate that communications with the Z-Wave network are working.
//
//  Copyright (c) 2010 Mal Lansell <mal@openzwave.com>
//
//
//  SOFTWARE NOTICE AND LICENSE
//
//  This file is part of OpenZWave.
//
//  OpenZWave is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//
//  OpenZWave is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenZWave.  If not, see <http://www.gnu.org/licenses/>.
//
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
#include <iostream>

using namespace OpenZWave;
using namespace std;

static uint32 g_homeId = 0;
static bool   g_initFailed = false;

typedef struct
{
  uint32      m_homeId;
  uint8      m_nodeId;
  bool      m_polled;
  list<ValueID>  m_values;
}NodeInfo;

static list<NodeInfo*> g_nodes;
static pthread_mutex_t g_criticalSection;
static pthread_cond_t  initCond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t initMutex = PTHREAD_MUTEX_INITIALIZER;

//-----------------------------------------------------------------------------
// <GetNodeInfo>
// Return the NodeInfo object associated with this notification
//-----------------------------------------------------------------------------
NodeInfo* GetNodeInfo
(
  Notification const* _notification
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

//-----------------------------------------------------------------------------
// <OnNotification>
// Callback that is triggered when a value, group or node changes
//-----------------------------------------------------------------------------
void OnNotification
(
  Notification const* _notification,
  void* _context
)
{
  // Must do this inside a critical section to avoid conflicts with the main thread
  pthread_mutex_lock( &g_criticalSection );

  switch( _notification->GetType() )
  {
    case Notification::Type_ValueAdded:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        // Add the new value to our list
        nodeInfo->m_values.push_back( _notification->GetValueID() );
      }
      break;
    }

    case Notification::Type_ValueRemoved:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        // Remove the value from out list
        for( list<ValueID>::iterator it = nodeInfo->m_values.begin(); it != nodeInfo->m_values.end(); ++it )
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

    case Notification::Type_ValueChanged:
    {
      // One of the node values has changed
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case Notification::Type_Group:
    {
      // One of the node's association groups has changed
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case Notification::Type_NodeAdded:
    {
      // Add the new node to our list
      NodeInfo* nodeInfo = new NodeInfo();
      nodeInfo->m_homeId = _notification->GetHomeId();
      nodeInfo->m_nodeId = _notification->GetNodeId();
      nodeInfo->m_polled = false;
      g_nodes.push_back( nodeInfo );
      break;
    }

    case Notification::Type_NodeRemoved:
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

    case Notification::Type_NodeEvent:
    {
      // We have received an event from the node, caused by a
      // basic_set or hail message.
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo = nodeInfo;    // placeholder for real action
      }
      break;
    }

    case Notification::Type_PollingDisabled:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo->m_polled = false;
      }
      break;
    }

    case Notification::Type_PollingEnabled:
    {
      if( NodeInfo* nodeInfo = GetNodeInfo( _notification ) )
      {
        nodeInfo->m_polled = true;
      }
      break;
    }

    case Notification::Type_DriverReady:
    {
      g_homeId = _notification->GetHomeId();
      break;
    }

    case Notification::Type_DriverFailed:
    {
      g_initFailed = true;
      pthread_cond_broadcast(&initCond);
      break;
    }

    case Notification::Type_AwakeNodesQueried:
    case Notification::Type_AllNodesQueried:
    case Notification::Type_AllNodesQueriedSomeDead:
    {
      pthread_cond_broadcast(&initCond);
      break;
    }

    case Notification::Type_DriverReset:
    case Notification::Type_Notification:
    case Notification::Type_NodeNaming:
    case Notification::Type_NodeProtocolInfo:
    case Notification::Type_NodeQueriesComplete:
    default:
    {
    }
  }

  pthread_mutex_unlock( &g_criticalSection );
}

bool g_cleaningUp = false;

void CleanUp(int signal) {
  if (g_cleaningUp) {
    return;
  }

  g_cleaningUp = true;

  printf("Cleaning up\n");
  // program exit (clean up)
  Manager::Get()->RemoveDriver( "/dev/ttyUSB0" );
  Manager::Get()->RemoveWatcher( OnNotification, NULL );
  Manager::Destroy();
  Options::Destroy();
  pthread_mutex_destroy( &g_criticalSection );

  exit(0);
}

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

  signal(SIGABRT, &CleanUp);
  signal(SIGTERM, &CleanUp);
  signal(SIGINT, &CleanUp);

  printf("Starting MinOZW with OpenZWave Version %s\n", Manager::getVersionAsString().c_str());

  // Create the OpenZWave Manager.
  // The first argument is the path to the config files (where the manufacturer_specific.xml file is located
  // The second argument is the path for saved Z-Wave network state and the log file.  If you leave it NULL
  // the log file will appear in the program's working directory.
  Options::Create( "../../../config/", "", "" );
  Options::Get()->AddOptionInt( "SaveLogLevel", LogLevel_Error );
  Options::Get()->AddOptionInt( "QueueLogLevel", LogLevel_Error );
  Options::Get()->AddOptionInt( "DumpTrigger", LogLevel_Error );
  Options::Get()->AddOptionInt( "PollInterval", 500 );
  Options::Get()->AddOptionBool( "IntervalBetweenPolls", true );
  Options::Get()->AddOptionBool("ValidateValueChanges", true);
  Options::Get()->Lock();

  Manager::Create();

  // Add a callback handler to the manager.  The second argument is a context that
  // is passed to the OnNotification method.  If the OnNotification is a method of
  // a class, the context would usually be a pointer to that class object, to
  // avoid the need for the notification handler to be a static.
  Manager::Get()->AddWatcher( OnNotification, NULL );

  // Add a Z-Wave Driver
  // Modify this line to set the correct serial port for your PC interface.

#ifdef DARWIN
  string port = "/dev/cu.usbserial";
#elif WIN32
        string port = "\\\\.\\COM6";
#else
  string port = "/dev/ttyUSB0";
#endif
  bool g_levelGiven = false;
  uint8 newLevel = 0;

  if ( argc > 1 )
  {
    char *endptr;

    newLevel = strtoul(argv[1], &endptr, 10);

    if (endptr == argv[1]) {
      printf("Error!!!");
      exit(1);
    }

    g_levelGiven = true;
  }

  if( strcasecmp( port.c_str(), "usb" ) == 0 )
  {
    Manager::Get()->AddDriver( "HID Controller", Driver::ControllerInterface_Hid );
  }
  else
  {
    Manager::Get()->AddDriver( port );
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
    int nodeId = 2;
    Manager *manager = Manager::Get();

    // The section below demonstrates setting up polling for a variable.  In this simple
    // example, it has been hardwired to poll COMMAND_CLASS_BASIC on the each node that
    // supports this setting.
    pthread_mutex_lock( &g_criticalSection );

    manager->TestNetworkNode( g_homeId, nodeId, 2 );

    for( list<NodeInfo*>::iterator it = g_nodes.begin(); it != g_nodes.end(); ++it )
    {
      NodeInfo* nodeInfo = *it;

      // skip the controller (most likely node 1)
      if( nodeInfo->m_nodeId == 1) continue;
      if( nodeInfo->m_nodeId != nodeId) continue;

      for( list<ValueID>::iterator it2 = nodeInfo->m_values.begin(); it2 != nodeInfo->m_values.end(); ++it2 )
      {
        ValueID v = *it2;

        printf("CommandClassId 0x%x: Genre 0x%x Index 0x%x (%-20s):", v.GetCommandClassId(), v.GetGenre(), v.GetIndex(), manager->GetValueLabel(v).c_str());

        // Read byte values (as an experiment)
        // Lesson: don't pass an uninitialized pointer, it will not do anything (except set the GetValueAsByte return value to false)
        uint8 value;

        if (v.GetType() == ValueID::ValueType_Byte) {
          if (manager->GetValueAsByte(v, &value)) {
            printf("%i\n", value);
          } else {
            printf("-\n");
          }
        } else {
          printf("%i\n", v.GetType());
        }

        if (g_levelGiven && v.GetCommandClassId() == 0x26 && v.GetIndex() == 0) {
          bool success = manager->SetValue(v, newLevel);

          if (!success) {
            // This can e.g. occur when newLevel is not a byte but the valuetype is.
            printf("\n\n!!!! COULD NOT SET VALUE !!!!\n\n");
          }
        }
      }
    }

    pthread_mutex_unlock( &g_criticalSection );

    // If we want to access our NodeInfo list, that has been built from all the
    // notification callbacks we received from the library, we have to do so
    // from inside a Critical Section.  This is because the callbacks occur on other
    // threads, and we cannot risk the list being changed while we are using it.
    // We must hold the critical section for as short a time as possible, to avoid
    // stalling the OpenZWave drivers.
    // At this point, the program just waits for 3 seconds (to demonstrate polling),
    // then exits

    if (!g_levelGiven) {
      while (true) {
        pthread_mutex_lock( &g_criticalSection );
        // but NodeInfo list and similar data should be inside critical section
        pthread_mutex_unlock( &g_criticalSection );
        sleep(1);
      }
    } else {
      // Don't immediately clean up, since this will stop the message thread as well,
      // preventing the command to the lights to be sent.
      sleep(1);
    }

    Driver::DriverData data;
    Manager::Get()->GetDriverStatistics( g_homeId, &data );
    printf("SOF: %d ACK Waiting: %d Read Aborts: %d Bad Checksums: %d\n", data.m_SOFCnt, data.m_ACKWaiting, data.m_readAborts, data.m_badChecksum);
    printf("Reads: %d Writes: %d CAN: %d NAK: %d ACK: %d Out of Frame: %d\n", data.m_readCnt, data.m_writeCnt, data.m_CANCnt, data.m_NAKCnt, data.m_ACKCnt, data.m_OOFCnt);
    printf("Dropped: %d Retries: %d\n", data.m_dropped, data.m_retries);
  }

  CleanUp(0);

  return 0;
}
