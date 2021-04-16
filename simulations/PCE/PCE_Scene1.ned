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
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedVisualizer;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;

import agentquartet.linklayer.Splitter;
import agentquartet.node.DDoSBot;
import agentquartet.applications.agentquartet.DetectAgent.Detector;
import agentquartet.node.DefenceRouter;
import agentquartet.node.AQClient;
import agentquartet.node.AQServer;

network PCE_Scene1
{
    types:
        channel C extends ThruputMeteringChannel
        {
            parameters:
                datarate = 10Mbps;
                delay = 5ms;
                per = 0;
                ber = 0;
                //thruputDisplayFormat = default("p b u");
        }
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        clientAgent: AQClient {
            @display("p=162,422;i=block/app2");
        }
        cloudServer1: StandardHost {
            @display("p=597,565;i=device/lan-ring");
        }
        cloudServer2: StandardHost {
            @display("p=694,589;i=device/lan-ring");
        }
        cloudServer3: StandardHost {
            @display("p=797,576;i=device/lan-ring");
        }
        cloudServer4: StandardHost {
            @display("p=878,537;i=device/lan-ring");
        }
        cloudServer5: StandardHost {
            @display("p=926,446;i=device/lan-ring");
        }
        cloudServer6: StandardHost {
            @display("p=926,355;i=device/lan-ring");
        }
        ddosBot1: DDoSBot {
            @display("p=218,84");
        }
        ddosBot2: DDoSBot {
            @display("p=577,63");
        }
        detectionAgent1: Detector {
            @display("p=274,368");
        }
        etherSwitch1: EtherSwitch {
            @display("p=234,483");
        }
        etherSwitch2: EtherSwitch {
            @display("p=298,148");
        }
        etherSwitch3: EtherSwitch {
            @display("p=457,116");
        }
        targetHost1: StandardHost {
            @display("p=137,530");
        }
        targetHost2: StandardHost {
            @display("p=234,565");
        }
        router1: DefenceRouter {
            @display("p=380,400");
        }
        router2: DefenceRouter {
            @display("p=434,246");
        }
        router3: DefenceRouter {
            @display("p=597,369");
        }
        router4: Router {
            @display("p=699,400");
        }
        router5: Router {
            @display("p=699,502");
        }
        router6: Router {
            @display("p=821,442");
        }
        router7: Router {
            @display("p=395,180");
        }
        serverAgent: AQServer {
            @display("p=616,450;i=block/app2");
        }
        splitter1: Splitter {
            @display("p=308,442");
        }
        standardHost1: StandardHost {
            @display("p=162,132");
        }
        standardHost2: StandardHost {
            @display("p=298,54");
        }
        standardHost3: StandardHost {
            @display("p=402,30");
        }
        standardHost4: StandardHost {
            @display("p=496,30");
        }

    connections:
        clientAgent.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        targetHost1.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        targetHost2.ethg++ <--> Eth100M <--> etherSwitch1.ethg++;
        etherSwitch1.ethg++ <--> Eth100M <--> splitter1.ethg++;
        detectionAgent1.ethg++ <--> Eth100M <--> splitter1.ethg++;
        router1.ethg++ <--> Eth100M <--> splitter1.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> router3.ethg++;
        router2.ethg++ <--> Eth100M <--> router3.ethg++;
        standardHost1.ethg++ <--> Eth100M <--> etherSwitch2.ethg++;
        standardHost2.ethg++ <--> Eth100M <--> etherSwitch2.ethg++;
        standardHost3.ethg++ <--> Eth100M <--> etherSwitch3.ethg++;
        standardHost4.ethg++ <--> Eth100M <--> etherSwitch3.ethg++;
        ddosBot1.ethg++ <--> Eth100M <--> etherSwitch2.ethg++;
        ddosBot2.ethg++ <--> Eth100M <--> etherSwitch3.ethg++;
        etherSwitch2.ethg++ <--> Eth100M <--> router7.ethg++;
        etherSwitch3.ethg++ <--> Eth100M <--> router7.ethg++;
        router7.ethg++ <--> Eth100M <--> router2.ethg++;
        cloudServer1.ethg++ <--> Eth100M <--> router5.ethg++;
        cloudServer2.ethg++ <--> Eth100M <--> router5.ethg++;
        cloudServer3.ethg++ <--> Eth100M <--> router5.ethg++;
        cloudServer4.ethg++ <--> Eth100M <--> router6.ethg++;
        cloudServer5.ethg++ <--> Eth100M <--> router6.ethg++;
        cloudServer6.ethg++ <--> Eth100M <--> router6.ethg++;
        router5.ethg++ <--> Eth100M <--> router4.ethg++;
        router6.ethg++ <--> Eth100M <--> router4.ethg++;
        router4.ethg++ <--> Eth100M <--> router3.ethg++;
        serverAgent.ethg++ <--> Eth100M <--> router4.ethg++;
}