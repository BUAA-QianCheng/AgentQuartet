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

package agentquartet.simulations.tests.agentquartet;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.visualizer.integrated.IntegratedVisualizer;
import inet.node.ethernet.Eth100M;
import inet.node.inet.StandardHost;
import inet.node.inet.Router;

import agentquartet.applications.agentquartet.DetectAgent.Detector;
import agentquartet.node.AQClient;
import agentquartet.node.AQServer;
import agentquartet.linklayer.Splitter;
import agentquartet.node.DDoSBot;
import agentquartet.node.DefenceRouter;

network TestAQAgents
{
    parameters:

    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,30");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,90");
        }
        client: AQClient {
            @display("p=140,110");
        }
        server: AQServer {
            @display("p=340,110");
        }
        host: StandardHost {
            @display("p=440,190");
        }
        router: Router {
            @display("p=240,110");
        }
        router1: Router {
            @display("p=440,110");
        }
        defenceRouter1: DefenceRouter {
            @display("p=540,30");
        }
        defenceRouter2: DefenceRouter {
            @display("p=540,110");
        }
        defenceRouter3: DefenceRouter {
            @display("p=540,190");
        }
        detector: Detector {
            @display("p=139,189");
        }
        bot: DDoSBot {
            @display("p=340,190");
        }
        splitter: Splitter {
            @display("p=240,190");
        }
        router2: Router {
            @display("p=340,30");
        }
    connections:
        client.ethg++ <--> Eth100M <--> router.ethg++;
        server.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> defenceRouter1.ethg++;
        router1.ethg++ <--> Eth100M <--> defenceRouter2.ethg++;
        router1.ethg++ <--> Eth100M <--> defenceRouter3.ethg++;
        bot.ethg++ <--> Eth100M <--> splitter.ethg++;
        splitter.ethg++ <--> Eth100M <--> router.ethg++;
        splitter.ethg++ <--> Eth100M <--> detector.ethg++;
        host.ethg++ <--> Eth100M <--> defenceRouter3.ethg++;
        router.ethg++ <--> Eth100M <--> router2.ethg++;
        router1.ethg++ <--> Eth100M <--> router2.ethg++;
}


