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

package agentquartet.simulations.tests.old;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedVisualizer;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;
import inet.node.inet.Router;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;
import agentquartet.node.DDoSBot;
import agentquartet.node.DefenceRouter;
import agentquartet.applications.agentquartet.DetectAgent.Detector;
import agentquartet.linklayer.Splitter;

network TwoNodeNetwork
{
    parameters:

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        sendhost: StandardHost {
            @display("p=150,60");
        }
        recvhost: StandardHost {
            @display("p=350,60");
        }
        router: Router {
            @display("p=250,60");
        }
    connections:
        sendhost.ethg++ <--> Eth100M <--> router.ethg++;
        recvhost.ethg++ <--> Eth100M <--> router.ethg++;
}

network TestBot{
	parameters:
	
	submodules:
	    configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        bot: DDoSBot {
            @display("p=150,60");
        }
        recvhost: StandardHost {
            @display("p=350,60");
        }
        router: DefenceRouter{
            @display("p=250,60");
        }
    connections:
        bot.ethg++ <--> Eth100M <--> router.ethg++;
        recvhost.ethg++ <--> Eth100M <--> router.ethg++;

}

network TestDefenceRouter
{
    parameters:

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        sendhost: StandardHost {
            @display("p=150,60");
        }
        recvhost: StandardHost {
            @display("p=350,60");
        }
        router: DefenceRouter {
            @display("p=250,60");
        }
    connections:
        sendhost.ethg++ <--> Eth100M <--> router.ethg++;
        recvhost.ethg++ <--> Eth100M <--> router.ethg++;
}

network TestL3Deploy
{
    parameters:

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        sendhost: StandardHost {
            @display("p=150,30");
        }
        recvhost: StandardHost {
            @display("p=150,210");
        }
        router1: Router {
            @display("p=150,90");
        }
        router2: Router {
            @display("p=150,150");
        }
        mitigator: Router {
            @display("p=250,90");
        }
    connections:
        sendhost.ethg++ <--> Eth100M <--> router1.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;
        router2.ethg++ <--> Eth100M <--> recvhost.ethg++;
        router1.ethg++ <--> Eth100M <--> mitigator.ethg++;
        router1.ethg++ <--> Eth100M <--> mitigator.ethg++;
}

network TestL2Deploy
{
    parameters:

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        sendhost: StandardHost {
            @display("p=150,30");
        }
        recvhost: StandardHost {
            @display("p=150,210");
        }
        switch1: EtherSwitch {
            @display("p=150,90");
        }
        router1: Router {
            @display("p=150,150");
        }
        mitigator: Router {
            @display("p=250,90");
        }
    connections:
        sendhost.ethg++ <--> Eth100M <--> switch1.ethg++;
        switch1.ethg++ <--> Eth100M <--> mitigator.ethg++;
        switch1.ethg++ <--> Eth100M <--> mitigator.ethg++;
        switch1.ethg++ <--> Eth100M <--> router1.ethg++;
        recvhost.ethg++ <--> Eth100M <--> router1.ethg++;    
}

network TestSplitter
{
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        standardHost: DDoSBot {
            @display("p=100,130");
        }
        standardHost1: StandardHost {
            @display("p=400,130");
        }
        router: Router {
            @display("p=300,130");
        }
        detector: Detector {
            @display("p=200,30");
        }
        splitter: Splitter {
            @display("p=200,130");
        }
    connections allowunconnected:
        standardHost.ethg++ <--> Eth100M <--> splitter.ethg++;
        splitter.ethg++ <--> Eth100M <--> router.ethg++;
        detector.ethg++ <--> Eth100M <--> splitter.ethg++;
        standardHost1.ethg++ <--> Eth100M <--> router.ethg++;
}

network TestWirelessAndWiredNetwork
{
    parameters:

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
        etherSwitch: EtherSwitch {
            @display("p=430,125");
        }
        router: Router {
            @display("p=340,125");
        }
        wirelessHost1: WirelessHost {
            @display("p=150,70");
        }
        accessPoint: AccessPoint {
            @display("p=250,125");
        }
        standardHost1: StandardHost {
            @display("p=520,70");
        }
        standardHost2: StandardHost {
            @display("p=520,180");
        }
        wirelessHost2: WirelessHost {
            @display("p=150,180");
        }
    connections:
        accessPoint.ethg++ <--> Eth100M <--> router.ethg++;
        etherSwitch.ethg++ <--> Eth100M <--> router.ethg++;
        standardHost1.ethg++ <--> Eth100M <--> etherSwitch.ethg++;
        standardHost2.ethg++ <--> Eth100M <--> etherSwitch.ethg++;
}
