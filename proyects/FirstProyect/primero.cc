
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

// Agregado para obtener parametros de la línea de comando
#include <iostream>	//std::cout
#include <string>	//std::string, std::getline();

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("PrimeroScratch");
// Poner en terminal: export NS_LOG=PrimeroScratch=info

int main (int argc, char *argv[]) {
    NS_LOG_INFO("Inicializando proyecto");

    CommandLine cmd;
    cmd.Parse (argc, argv);

    // Obtener param. del usuario
    NS_LOG_INFO("Obteniendo parámetro de usuario");
    // printf("\n# paquetes a intercambiar: "); 
    // string buffer;
    // getline(cin, buffer);
    // cout<<"Se ingreso: \""<<buffer<<"\""<<endl;
    NS_LOG_INFO("Cast de String a Int");
    // int numPackage = system(buffer.c_str());
    int numPackage = 5;

    Time::SetResolution (Time::NS);
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NS_LOG_INFO("Creando 2 nodos");
    NodeContainer nodes;
    nodes.Create (2);

    NS_LOG_INFO("Uniendo nodos");
    PointToPointHelper pointToPoint;
    NS_LOG_INFO("Definiendo ancho de banda");
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));

    NS_LOG_INFO("Creando un contenedor");
    NetDeviceContainer devices;
    devices = pointToPoint.Install (nodes);

    NS_LOG_INFO("Instalando los nodos");
    InternetStackHelper stack;
    stack.Install (nodes);

    NS_LOG_INFO("Generando las direcciones IP");
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");

    NS_LOG_INFO("Asignando las direcciones a los dispositivos");
    Ipv4InterfaceContainer interfaces = address.Assign (devices);

    NS_LOG_INFO("Iniciando paquetes de echo");
    UdpEchoServerHelper echoServer (9);

    ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (10.0));

    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (numPackage));
    // USAR, sino: ./waf --run "scratch/primero --ns3::UdpEchoClient::MaxPackets=10"
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    NS_LOG_INFO("Instalando Nodos en el ApplicationContainer");
    ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("scratch/myfirst.tr"));

    pointToPoint.EnablePcapAll ("scratch/myfirst");

    NS_LOG_INFO("Corriendo simulación");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO("Finalizando simulación");
    return 0;
}
