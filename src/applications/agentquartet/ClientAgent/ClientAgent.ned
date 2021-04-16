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

package agentquartet.applications.agentquartet.ClientAgent;

import inet.applications.contract.IApp;

simple ClientAgent like IApp {
	parameters:
	    @class(ClientAgent);
	    @display("i=block/app");
	    //connections
	    string localAddress = default("");
	    int localPort = default(8381); //Client Agent
	    string connectAddress = default("");
	    int connectPort = default(9981); //Server Agent
	 	//par of TcpAppBase
	    int timeToLive = default(-1);
	    int dscp = default(-1);
        int tos = default(-1);
        //Time
        double tStart @unit(s) = default(1s);
        double tStop @unit(s) = default(-1s);
        double retransmitTimeout @unit(s) = default(3s);
        double efficacyUpdateTimeOut @unit(s) = default(1s);
        @lifecycleSupport;
       	@signal[connect](type=long);
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        @statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[packetSent](title="packets sent"; source=packetSent; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
	    //Client Area
	   	xml protectList = default(xml("<config></config>"));
	    
	gates:
	    input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
        //detector input
        input detectIn @directIn;
}