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
#include "header_file.h"

using namespace ns3;

std::vector<double>tcp_path;
std::vector<double>udp_path;
std::vector< double >time_dvec;

NS_LOG_COMPONENT_DEFINE ("Assignment_4");

// Increase the data rate at specific times for obesrvations
void
IncRate (Ptr<netlab_app> my_app, DataRate rate_flow, FlowMonitorHelper *help_fm, Ptr<FlowMonitor> flow_monitoring, int is_graph_capture)
{
  my_app->rate_of_change(rate_flow);
  if(is_graph_capture){
    std::map<FlowId, FlowMonitor::FlowStats> stat_flow = flow_monitoring->GetFlowStats();
      Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (help_fm->GetClassifier());
      double final_tcp = 0,final_udp= 0;
      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator statistics = stat_flow.begin (); statistics != stat_flow.end (); ++statistics)
      {
        Ipv4FlowClassifier::FiveTuple tuple= classing->FindFlow (statistics->first);
        double throughput_val =  statistics->second.rxBytes * 8.0 / (statistics->second.timeLastRxPacket.GetSeconds()-statistics->second.timeFirstTxPacket.GetSeconds())/1024/1024 ;

        if(tuple.protocol == 6){
          final_tcp += throughput_val;

        }
        if(tuple.protocol == 17){
          final_udp += throughput_val;
        }

      }
      std::cout << final_tcp<< "::"<<final_udp<<"\n";
      tcp_path.push_back(final_tcp);
      udp_path.push_back(final_udp);

  }
    return;
}


int main (int argc, char *argv[])
{
    // Set configuration
  Time::SetResolution (Time::NS);
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));

    // Datasets for the graph plots
    Gnuplot2dDataset plot_dataset[5];
    for(int i = 0; i < 5; i++){
        plot_dataset[i].SetStyle (Gnuplot2dDataset::LINES_POINTS);
    }

 // Varying buffer size from 10*1500 to 800*1500 gradually
 // Till 100*1500, increasing by 12*1500
 // Till 400*1500, increasing by 120*1500
 // Till 800*1500, increasing by 150*1500
for(int size_buffer=10*1500;size_buffer<=800*1500;)
{
    NS_LOG_INFO ("For Visualization!");
    CommandLine commandLine;
    commandLine.Parse (argc, argv);

    NS_LOG_INFO ("Create Nodes.");
    NodeContainer nodeContainer;
    nodeContainer.Create (8);

 //creating node containers which creates two nodes with a point-to-point channel between them
    NodeContainer node_0node_3 = NodeContainer (nodeContainer.Get (0), nodeContainer.Get (3)); //H1 - R1
    NodeContainer node_1node_3 = NodeContainer (nodeContainer.Get (1), nodeContainer.Get (3)); //H2 - R1
    NodeContainer node_2node_3 = NodeContainer (nodeContainer.Get (2), nodeContainer.Get (3)); //H3 - R1
    NodeContainer node_3node_4 = NodeContainer (nodeContainer.Get (3), nodeContainer.Get (4)); //R1 - R2
    NodeContainer node_4node_5 = NodeContainer (nodeContainer.Get (4), nodeContainer.Get (5)); //R2 - H4
    NodeContainer node_4node_6 = NodeContainer (nodeContainer.Get (4), nodeContainer.Get (6)); //R2 - H5
    NodeContainer node_4node_7 = NodeContainer (nodeContainer.Get (4), nodeContainer.Get (7)); //R2 - H6

    //installs internet stacks on our two nodes
    InternetStackHelper internet;
    internet.Install (nodeContainer);

    NS_LOG_INFO ("Create Channels.");
    PointToPointHelper helper;
    //settings attributes for p2p connections between nodes and routers
    helper.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
    helper.SetChannelAttribute ("Delay", StringValue ("10ms"));
    NetDeviceContainer device_0device_3 = helper.Install (node_0node_3);    // H1 - R1
    NetDeviceContainer device_1device_3 = helper.Install (node_1node_3);    // H2 - R1
    NetDeviceContainer device_2device_3 = helper.Install (node_2node_3);    // H3 - R1
    NetDeviceContainer device_4device_5 = helper.Install (node_4node_5);    // R2 - H4
    NetDeviceContainer device_4device_6 = helper.Install (node_4node_6);    // R2 - H5
    NetDeviceContainer device_4device_7 = helper.Install (node_4node_7);    // R2 - H6

    //settings attributes for p2p connections between routers R1 - R2

    // p2p.SetQueue ("ns3::DropTailQueue","Mode", StringValue ("QUEUE_MODE_PACKETS"),"MaxPackets", UintegerValue (quesize));
    helper.SetQueue ("ns3::DropTailQueue<Packet>", "MaxSize", QueueSizeValue (QueueSize ("100p"))); // p in 100p stands for packets
    // p2p.SetQueue("ns3::DropTailQueue","Mode",EnumValue (DropTailQueue::QUEUE_MODE_BYTES),"MaxBytes",UintegerValue (125000));
    helper.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
    helper.SetChannelAttribute ("Delay", StringValue ("100ms"));
    NetDeviceContainer device_3device_4 = helper.Install (node_3node_4);

    //creates interfaces and assigns IP addresses for the point-to-point devices.

    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper address_helper;
    address_helper.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_0interface_3 = address_helper.Assign (device_0device_3);

    address_helper.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_1interface_3 = address_helper.Assign (device_1device_3);

    address_helper.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_2interface_3 = address_helper.Assign (device_2device_3);

    address_helper.SetBase ("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_3interface_4 = address_helper.Assign (device_3device_4);

    address_helper.SetBase ("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_4interface_5 = address_helper.Assign (device_4device_5);

    address_helper.SetBase ("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_4interface_6 = address_helper.Assign (device_4device_6);

    address_helper.SetBase ("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer interface_4interface_7 = address_helper.Assign (device_4device_7);

    NS_LOG_INFO ("Enable static global routing.");
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


    /*
    Creating connections between the nodes
    3 TCP Connections and 3 UDP Connections as follows:
    H1 to H6 = TCP
    H1 to H2 = TCP
    H5 to H6 = TCP
    H3 to H4 = UDP
    H2 to H3 = UDP
    H4 to H5 = UDP
    */

    //TCP - H1 to H6
    //create sockets using the class ns3::TcpSocketFactory

    uint16_t port = 8081;
    //destination node to receive TCP connections and data
    Address addr1 (InetSocketAddress (interface_4interface_7.GetAddress (1), port));
    PacketSinkHelper helper1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn1 = helper1.Install (nodeContainer.Get (7));
    sink_conn1.Start (Seconds (0.));
    sink_conn1.Stop (Seconds (15.));

    //creating socket at host
    Ptr<Socket> tcp1 = Socket::CreateSocket (nodeContainer.Get (0), TcpSocketFactory::GetTypeId ());
    tcp1->SetAttribute("SndBufSize",  ns3::UintegerValue(size_buffer));
    tcp1->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    //creating a netlab_app object conn1
    Ptr<netlab_app> conn1 = CreateObject<netlab_app> ();
    conn1->initialize_setup (tcp1, addr1, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (0)->AddApplication (conn1);
    //setting start and stop time of app
    conn1->SetStartTime (Seconds (1.));
    conn1->SetStopTime (Seconds (15.));


    // UDP - H4 to H3
    port = 8082;
    Address addr2 (InetSocketAddress (interface_2interface_3.GetAddress (0), port));
    PacketSinkHelper helper2 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn2 = helper2.Install (nodeContainer.Get (2));
    sink_conn2.Start (Seconds (0.));
    sink_conn2.Stop (Seconds (15.));

    Ptr<Socket> udp2 = Socket::CreateSocket (nodeContainer.Get (5), UdpSocketFactory::GetTypeId ());
    udp2->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    Ptr<netlab_app> conn2 = CreateObject<netlab_app> ();
    conn2->initialize_setup (udp2, addr2, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (5)->AddApplication (conn2);
    conn2->SetStartTime (Seconds (1.));
    conn2->SetStopTime (Seconds (15.));


    //TCP - H1 to H2
    port = 8083;
    Address addr3 (InetSocketAddress (interface_1interface_3.GetAddress (0), port));
    PacketSinkHelper helper3("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn3 = helper3.Install (nodeContainer.Get (1));
    sink_conn3.Start (Seconds (0.));
    sink_conn3.Stop (Seconds (15.));

    Ptr<Socket> tcp3 = Socket::CreateSocket (nodeContainer.Get (0), TcpSocketFactory::GetTypeId ());
    tcp3->SetAttribute("SndBufSize",  ns3::UintegerValue(size_buffer));
    tcp3->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    Ptr<netlab_app> conn3 = CreateObject<netlab_app> ();
    conn3->initialize_setup (tcp3, addr3, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (0)->AddApplication (conn3);
    conn3->SetStartTime (Seconds (1.));
    conn3->SetStopTime (Seconds (15.));

    //TCP - H5 to H6
    port = 8084;
    Address addr4 (InetSocketAddress (interface_4interface_7.GetAddress (1), port));
    PacketSinkHelper helper4("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn4 = helper4.Install (nodeContainer.Get (7));
    sink_conn4.Start (Seconds (0.));
    sink_conn4.Stop (Seconds (15.));

    Ptr<Socket> tcp4 = Socket::CreateSocket (nodeContainer.Get (6), TcpSocketFactory::GetTypeId ());
    tcp4->SetAttribute("SndBufSize",  ns3::UintegerValue(size_buffer));
    tcp4->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    Ptr<netlab_app> conn4 = CreateObject<netlab_app> ();
    conn4->initialize_setup (tcp4, addr4, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (6)->AddApplication (conn4);
    conn4->SetStartTime (Seconds (1.));
    conn4->SetStopTime (Seconds (15.));

    //UDP - H2 to H3
    port = 8085;
    Address addr5 (InetSocketAddress (interface_3interface_4.GetAddress (0), port));
    PacketSinkHelper helper5 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn5 = helper5.Install (nodeContainer.Get (2));
    sink_conn5.Start (Seconds (0.));
    sink_conn5.Stop (Seconds (15.));

    Ptr<Socket> udp5 = Socket::CreateSocket (nodeContainer.Get (1), UdpSocketFactory::GetTypeId ());
    udp5->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    Ptr<netlab_app> conn5 = CreateObject<netlab_app> ();
    conn5->initialize_setup (udp5, addr5, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (1)->AddApplication (conn5);
    conn5->SetStartTime (Seconds (1.));
    conn5->SetStopTime (Seconds (15.));


    //UDP - H4 to H5
    port = 8086;
    Address addr6 (InetSocketAddress (interface_4interface_6.GetAddress (1), port));
    PacketSinkHelper helper6 ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sink_conn6 = helper6.Install (nodeContainer.Get (6));
    sink_conn6.Start (Seconds (0.));
    sink_conn6.Stop (Seconds (15.));

    Ptr<Socket> udp6 = Socket::CreateSocket (nodeContainer.Get (5), UdpSocketFactory::GetTypeId ());
    udp6->SetAttribute("RcvBufSize",  ns3::UintegerValue(size_buffer));

    Ptr<netlab_app> conn6 = CreateObject<netlab_app> ();
    conn6->initialize_setup (udp6, addr6, 1500, 1000000, DataRate ("20Mbps"));
    nodeContainer.Get (5)->AddApplication (conn6);
    conn6->SetStartTime (Seconds (1.));
    conn6->SetStopTime (Seconds (15.));


    //Helper to enable IP flow monitoring on a set of Nodes.
    FlowMonitorHelper helper_flow;
    Ptr<FlowMonitor> monitor = helper_flow.InstallAll();

    if(size_buffer == 10*1500){
      //scheduling increase rate function over time to see udp flow rate's effect on tcp/udp throughput.
      //changing the udp flow rate for conn5 only and seeing it's effect on other flows
      Simulator::Schedule (Seconds(2.0), &IncRate, conn5, DataRate("30Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(3.0), &IncRate, conn5, DataRate("40Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(4.0), &IncRate, conn5, DataRate("60Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(5.0), &IncRate, conn5, DataRate("80Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(6.0), &IncRate, conn5, DataRate("90Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(7.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(8.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(10.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,1);
      Simulator::Schedule (Seconds(15.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,1);
    }
    else {
      //scheduling udp flow rate gradually over time from 20Mbps to 100Mbps
      Simulator::Schedule (Seconds(2.0), &IncRate, conn5, DataRate("30Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(3.0), &IncRate, conn5, DataRate("40Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(4.0), &IncRate, conn5, DataRate("60Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(5.0), &IncRate, conn5, DataRate("80Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(6.0), &IncRate, conn5, DataRate("90Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(7.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(8.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(10.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,0);
      Simulator::Schedule (Seconds(15.0), &IncRate, conn5, DataRate("100Mbps"), &helper_flow, monitor,0);
    }

    NS_LOG_INFO ("Run Simulation.");
    Simulator::Stop (Seconds(15.0));
    //executing the simulation
    Simulator::Run ();

    monitor->CheckForLostPackets ();

    //Classifies packets by looking at their IP and TCP/UDP headers.
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (helper_flow.GetClassifier ());

    //object that monitors and reports back packet flows observed during a simulation
    std::map<FlowId, FlowMonitor::FlowStats> statistics = monitor->GetFlowStats ();

    double x = 0, y = 0, through_udp=0,through_tcp=0;

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator it = statistics.begin (); it != statistics.end (); ++it)
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow (it->first);
        // Calculating Throughput for different flows
        double t = it->second.rxBytes * 8.0 / (it->second.timeLastRxPacket.GetSeconds() - it->second.timeFirstTxPacket.GetSeconds())/1024/1024;
        x += t;
        y += t * t ;
        // If the connection is using TCP protocol.
        if(tuple.protocol == 6){
          through_tcp += t;
        }
    }
    // Calculating UDP throughput
    through_udp = x - through_tcp;
    double fair_id = (x * x)/ (6 * y) ;
    plot_dataset[0].Add (size_buffer/1500, fair_id);

    plot_dataset[1].Add(size_buffer/1500, through_udp);

    plot_dataset[2].Add(size_buffer/1500, through_tcp);


    std :: cout << " FairnessIndex: " << fair_id << std :: endl;
    monitor->SerializeToXmlFile("lab-1.flowmon", true, true);
    Simulator::Destroy ();

    // Changing buffer size
    if(size_buffer < 100*1500) {
        size_buffer+=12*1500;
    }
    else {
        if(size_buffer < 400*1500){
            size_buffer+=120*1500;
        }else{
            size_buffer+=150*1500;
        }
    }
  }

/***************************************/

  // Creating plot

  Gnuplot graph1 ("buffvsfairness");
  graph1.SetTitle ("buffvsfairness");
  graph1.SetTerminal ("png");
  graph1.SetLegend ("BufferSize", "FairnessIndex");
  graph1.AppendExtra ("set xrange [0:800]");

  // Adding dataset to the plot

  graph1.AddDataset (plot_dataset[0]);
  std :: ofstream output1 ("BufferSize-vs-FairnessIndex.plt");
  graph1.GenerateOutput (output1);
  output1.close ();

/***************************************/

  Gnuplot graph2 ("buffvsudp");
  graph2.SetTitle ("buffvsudpthroughput");
  graph2.SetTerminal ("png");
  graph2.SetLegend ("BufferSize", "udpthroughput");
  graph2.AppendExtra ("set xrange [0:800]");

  graph2.AddDataset (plot_dataset[1]);

  std :: ofstream output2 ("BufferSize-vs-UDPthroughput.plt");
  graph2.GenerateOutput (output2);
  output2.close ();

/***************************************/

  Gnuplot graph3 ("buffvstcp");
  graph3.SetTitle ("buffvstcpthroughput");
  graph3.SetTerminal ("png");
  graph3.SetLegend ("BufferSize", "tcpthroughput");
  graph3.AppendExtra ("set xrange [0:800]");

  graph3.AddDataset (plot_dataset[2]);
  std :: ofstream output3 ("BufferSize-vs-TCPthroughput.plt");
  graph3.GenerateOutput (output3);
  output3.close ();

/***************************************/

  time_dvec.resize(9);
  time_dvec[0] = 2; time_dvec[1] = 3; time_dvec[2] = 4; time_dvec[3] =5; time_dvec[4] = 6; time_dvec[5] = 7; time_dvec[6] = 8; time_dvec[7] = 10; time_dvec[8] = 15;
  for(int i =0 ;i < 9; i++){
      plot_dataset[3].Add(time_dvec[i],tcp_path[i]);
    plot_dataset[4].Add(time_dvec[i],udp_path[i]);

  }

  Gnuplot graph4 ("udpvstcpthroughput");
  graph4.SetTitle ("Effect of UDP flow on tcpthroughput");
  graph4.SetTerminal ("png");
  graph4.SetLegend ("Time", "TCPthroughput");

  graph4.AddDataset (plot_dataset[3]);

  std :: ofstream output4 ("Time-vs-TCPthroughput.plt");
  graph4.GenerateOutput (output4);
  output4.close ();

/***************************************/

  Gnuplot graph5 ("udpvsudpthroughput");
  graph5.SetTitle ("Effect of UDP flow on tcpthroughput");
  graph5.SetTerminal ("png");
  graph5.SetLegend ("Time", "UDPthroughput");

  graph5.AddDataset (plot_dataset[4]);

  std :: ofstream output5 ("Time-vs-UDPthroughput.plt");
  graph5.GenerateOutput (output5);
  output5.close ();

  NS_LOG_INFO ("Done.");
}
