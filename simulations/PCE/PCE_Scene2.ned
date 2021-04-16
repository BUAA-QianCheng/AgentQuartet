//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package agentquartet.simulations.PCE;

import inet.common.misc.ThruputMeteringChannel;

import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedVisualizer;
import inet.node.ethernet.Eth100M;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

import agentquartet.linklayer.Splitter;
import agentquartet.node.DDoSBot;
import agentquartet.applications.agentquartet.DetectAgent.Detector;
import agentquartet.node.DefenceRouter;
import agentquartet.node.AQClient;
import agentquartet.node.AQServer;

module TinyCloudComputingCenter
{
    gates:
        inout ethg[];
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        coreRouter: Router {
            @display("p=200,40");
        }
        collectRouter: Router {
            @display("p=200,125");
        }
        connectSwitch1: EtherSwitch {
            @display("p=120,195");
        }
        connectSwitch2: EtherSwitch {
            @display("p=280,195");
        }
        server1: StandardHost {
            @display("p=70,265;i=device/lan-ring");
        }
        server2: StandardHost {
            @display("p=150,265;i=device/lan-ring");
        }
        server3: StandardHost {
            @display("p=240,265;i=device/lan-ring");
        }
        serverAgent: AQServer {
            @display("p=320,265;i=block/app2");
        }
    connections:
        ethg++ <--> coreRouter.ethg++;
        server1.ethg++ <--> Eth100M <--> connectSwitch1.ethg++;
        server2.ethg++ <--> Eth100M <--> connectSwitch1.ethg++;
        server3.ethg++ <--> Eth100M <--> connectSwitch2.ethg++;
        serverAgent.ethg++ <--> Eth100M <--> connectSwitch2.ethg++;
        connectSwitch1.ethg++ <--> Eth100M <--> collectRouter.ethg++;
        connectSwitch2.ethg++ <--> Eth100M <--> collectRouter.ethg++;
        collectRouter.ethg++ <--> Eth100M <--> coreRouter.ethg++;
}

module ExternalNetwork
{
    gates:
        inout ethg[];
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        connectRouter: Router {
            @display("p=200,40");
        }
        switch1: EtherSwitch {
            @display("p=140,130");
        }
        switch2: EtherSwitch {
            @display("p=260,130");
        }
        host1: StandardHost {
            @display("p=100,240");
        }
        host2: StandardHost {
            @display("p=170,240");
        }
        host3: StandardHost {
            @display("p=230,240");
        }
        bot: DDoSBot {
            @display("p=300,240");
        }
    connections:
        ethg++ <--> connectRouter.ethg++;
        host1.ethg++ <--> Eth100M <--> switch1.ethg++;
        host2.ethg++ <--> Eth100M <--> switch1.ethg++;
        host3.ethg++ <--> Eth100M <--> switch2.ethg++;
        bot.ethg++ <--> Eth100M <--> switch2.ethg++;
        switch1.ethg++ <--> Eth100M <--> connectRouter.ethg++;
        switch2.ethg++ <--> Eth100M <--> connectRouter.ethg++;
}

network PCE_Scene2
{
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            @display("p=50,150");
        }
        defenceRouter1: DefenceRouter {
            @display("p=459,286");
        }
        defenceRouter2: DefenceRouter {
            @display("p=490,151");
        }
        defenceRouter3: DefenceRouter {
            @display("p=625,244");
        }
        accessPoint: AccessPoint {
            @display("p=175,343");
        }
        router: Router {
            @display("p=336,344");
        }
        clientAgent: AQClient {
            @display("p=297,279;i=block/app2");
        }
        detectionAgent: Detector {
            @display("p=378,243");
        }
        wirelessHost: WirelessHost {
            @display("p=79,375");
        }
        mobileHost: WirelessHost {
            @display("p=87,305;i=misc/smartphone");
        }
        cloudCenter: TinyCloudComputingCenter {
            @display("p=446,48;i=misc/cloud_vl");
        }
        wiredNetwork: ExternalNetwork {
            @display("p=730,269;i=block/network2_l");
        }
        splitter: Splitter {
            @display("p=397,317;i=abstract/opticalswitch_s");
        }
    connections:
        defenceRouter1.ethg++ <--> Eth100M <--> defenceRouter2.ethg++;
        defenceRouter1.ethg++ <--> Eth100M <--> defenceRouter3.ethg++;
        defenceRouter2.ethg++ <--> Eth100M <--> defenceRouter3.ethg++;
        cloudCenter.ethg++ <--> Eth100M <--> defenceRouter2.ethg++;
        wiredNetwork.ethg++ <--> Eth100M <--> defenceRouter3.ethg++;
        router.ethg++ <--> Eth100M <--> splitter.ethg++;
        router.ethg++ <--> Eth100M <--> clientAgent.ethg++;
        router.ethg++ <--> Eth100M <--> accessPoint.ethg++;
        splitter.ethg++ <--> Eth100M <--> defenceRouter1.ethg++;
        splitter.ethg++ <--> Eth100M <--> detectionAgent.ethg++;
}
