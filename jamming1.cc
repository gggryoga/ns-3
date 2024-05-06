#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
    // コマンドライン引数の処理
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // 初期化
    NodeContainer nodes;
    nodes.Create(2); // 2つのノードを作成

    // Wi-Fi PHYレイヤの設定
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211g);

    // Wi-Fi MACレイヤの設定
    WifiMacHelper wifiMac;
    wifiMac.SetType("ns3::AdhocWifiMac");

    // Wi-Fiデバイスのインストール
    NetDeviceContainer devices = wifi.Install(wifiMac, nodes);

    // インターネットスタックの有効化
    InternetStackHelper internet;
    internet.Install(nodes);

    // IPアドレスの割り当て
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    // エコークライアントの設定
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9); // ポート番号9
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    // エコーサーバの設定
    UdpEchoServerHelper echoServer(9); // ポート番号9

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // ジャマーの設定
    Ptr<PacketSink> jammer = CreateObject<PacketSink>();
    nodes.Get(1)->AddDevice(jammer);

    jammer->SetStartTime(Seconds(4.0)); // 通信開始後にジャミングを開始
    jammer->SetStopTime(Seconds(8.0));  // 通信終了前にジャミングを停止

    // シミュレーション終了時間の設定
    Simulator::Stop(Seconds(10.0));

    // シミュレーションを実行
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
