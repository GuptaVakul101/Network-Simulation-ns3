#include <fstream>
#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/gnuplot.h"

using namespace ns3;

// Coded an Application so we could take that Socket and use it during simulation

class netlab_app : public Application
{
public:

  netlab_app ();
  virtual ~netlab_app();

  void initialize_setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void rate_of_change(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void next_pkt_sched (void);
  void pkt_send (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

// Constructor for netlab_app class

netlab_app::netlab_app ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

// Destructor for netlab_app class

netlab_app::~netlab_app()
{
  m_socket = 0;
}

//allow the Socket to be created at configuration time

void
netlab_app::initialize_setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

//required to start sending data during the simulation.
void
netlab_app::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  pkt_send ();
}

//required to stop sending data during the simulation.
void
netlab_app::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

// starts data flow
void
netlab_app::pkt_send (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      next_pkt_sched ();
    }
}

//schedule the send packet function tnext
void
netlab_app::next_pkt_sched (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &netlab_app::pkt_send, this);
    }
}

//change rate of our app
void
netlab_app::rate_of_change(DataRate newrate)
{
   m_dataRate = newrate;
   return;
}