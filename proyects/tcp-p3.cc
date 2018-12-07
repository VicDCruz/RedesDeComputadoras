/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Network topology
//
//       n0 ----------- n1
//            500 Kbps
//             5 ms
//
// - Flow from n0 to n1 using BulkSendApplication.
// - Tracing of queues and packet receptions to file "tcp-bulk-send.tr"
//   and pcap tracing available when tracing is turned on.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpBulkSendExample");

int
main (int argc, char *argv[])
{

  bool tracing = false;
  uint32_t maxBytes = 0;
  uint32_t tamCola = 20;

//
// Allow the user to override any of the defaults at
// run-time, via command-line arguments
//
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
  cmd.Parse (argc, argv);

//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (6);

  NS_LOG_INFO ("Create channels.");

  NodeContainer n0n1 (nodes.Get (0), nodes.Get (1));
  NodeContainer n2n0 (nodes.Get (2), nodes.Get (0));
  NodeContainer n3n0 (nodes.Get (3), nodes.Get (0));
  NodeContainer n4n0 (nodes.Get (4), nodes.Get (0));
  NodeContainer n5n0 (nodes.Get (5), nodes.Get (0));

//
// Explicitly create the point-to-point link required by the topology (shown above).
//
  PointToPointHelper pointToPoint;

  pointToPoint.SetQueue("ns3::DropTailQueue","MaxPackets",
        UintegerValue(tamCola));
  pointToPoint.SetDeviceAttribute ("Mtu", UintegerValue (1500));

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));

  PointToPointHelper p20;
  p20.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  p20.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p20.SetQueue("ns3::DropTailQueue","MaxPackets",
      UintegerValue(tamCola));
  p20.SetChannelAttribute ("Delay", TimeValue(MilliSeconds (2)));

  PointToPointHelper p30;
  p30.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  p30.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p30.SetQueue("ns3::DropTailQueue","MaxPackets",
      UintegerValue(tamCola));
  p30.SetChannelAttribute ("Delay", TimeValue(MilliSeconds (2)));

  PointToPointHelper p40;
  p40.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  p40.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p40.SetQueue("ns3::DropTailQueue","MaxPackets",
      UintegerValue(tamCola));
  p40.SetChannelAttribute ("Delay", TimeValue(MilliSeconds (2)));

  PointToPointHelper p50;
  p50.SetDeviceAttribute ("DataRate", StringValue ("500Kbps"));
  p50.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p50.SetQueue("ns3::DropTailQueue","MaxPackets",
      UintegerValue(tamCola));
  p50.SetChannelAttribute ("Delay", TimeValue(MilliSeconds (2)));

  NetDeviceContainer dev01 = pointToPoint.Install (n0n1);
  NetDeviceContainer dev20 = p20.Install (n2n0);
  NetDeviceContainer dev30 = p30.Install (n3n0);
  NetDeviceContainer dev40 = p40.Install (n4n0);
  NetDeviceContainer dev50 = p50.Install (n5n0);

//
// Install the internet stack on the nodes
//
  InternetStackHelper internet;
  internet.Install (nodes);

//
// We've got the "hardware" in place.  Now we need to add IP addresses.
//
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (dev01);
  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4.Assign (dev20);
  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4.Assign (dev30);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4.Assign (dev40);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4.Assign (dev50);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  NS_LOG_INFO ("Create Applications.");

//
// Create a BulkSendApplication and install it on node 0
//
  uint16_t port;  // antes era port=9

  for (port=9; port < 13; port++ ) {
    BulkSendHelper source ("ns3::TcpSocketFactory",
          InetSocketAddress (i.GetAddress (1), port));
          
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
          
    // Instala fuente en nodos
    ApplicationContainer sourceApps = 
          source.Install (nodes.Get (port-7));
          sourceApps.Start (Seconds (0.0));
    sourceApps.Stop (Seconds (60.0));

    // Crear el destino (el pozo) en el nodo 1
    PacketSinkHelper sink ("ns3::TcpSocketFactory",
            InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (nodes.Get (1));
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (70.0));
  }

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = 
      DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = 
      monitor->GetFlowStats ();

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i =  
        stats.begin (); i != stats.end (); ++i)
  {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
    if (t.sourceAddress=="10.1.2.1" && 
        t.destinationAddress == "10.1.1.2")
    {
      std::cout << "Fuente " << i->first  <<  "  Goodput: " 
          << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() – i->second.timeFirstTxPacket.GetSeconds())/1024/1024 
          << " Mbps\n";
    }
    else  if (t.sourceAddress=="10.1.3.1"&& 
        t.destinationAddress == "10.1.1.2" )
    {
      std::cout << "Fuente " << i->first  <<  "  Goodput: " 
          << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() – i->second.timeFirstTxPacket.GetSeconds())/1024/1024 
          << " Mbps\n";
    }
    else  if (t.sourceAddress=="10.1.4.1"&& 
        t.destinationAddress == "10.1.1.2" )
    {
      std::cout << "Fuente " << i->first  <<  "  Goodput: " 
          << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() – i->second.timeFirstTxPacket.GetSeconds())/1024/1024 
          << " Mbps\n";
    }
    else  if (t.sourceAddress=="10.1.5.1"&& 
        t.destinationAddress == "10.1.1.2" )
    {
      std::cout << "Fuente " << i->first  <<  "  Goodput: " 
          << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() – i->second.timeFirstTxPacket.GetSeconds())/1024/1024 
          << " Mbps\n";
    }
  }

//
// Set up tracing if enabled
//
  if (tracing)
    {
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("tcp-bulk-send.tr"));
      pointToPoint.EnablePcapAll ("tcp-bulk-send", false);
    }

//
// Now, do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
}
