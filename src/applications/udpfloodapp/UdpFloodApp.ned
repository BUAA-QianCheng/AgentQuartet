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

package agentquartet.applications.udpfloodapp;
import inet.applications.contract.IApp;

simple UdpFloodApp like IApp{
    parameters:
        string localAddr = default("");
        string destAddr = default("");
        int destPort;
        string packetName = default("UdpFlood");
        volatile int messageLength @unit(B);
        double startTime @unit(s) = default(this.sendInterval); 
        double stopTime @unit(s) = default(-1s);
        volatile double sendInterval @unit(s);
        @display("i=block/app");
        @lifecycleSupport;
        @signal[packetSent](type=inet::Packet);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        
	gates:
        input socketIn @labels(UdpControlInfo/up);
        output socketOut @labels(UdpControlInfo/down);
}


