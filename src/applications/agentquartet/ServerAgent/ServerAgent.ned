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

package agentquartet.applications.agentquartet.ServerAgent;

import inet.applications.tcpapp.TcpServerHostApp;

module ServerAgent extends TcpServerHostApp {
	parameters:
	    localPort = default(9981); // port number to listen on
	    serverThreadModuleType = default("agentquartet.applications.agentquartet.ServerAgent.ServerAgentThread");
	    @display("i=block/app");
	    @class(ServerAgent);
	    xml AreaInfoList=default(xml("<config></config>"));
	    //xml sessionConfig=default(xml("<config><config/>"));
	    //@statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        //@statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
}

simple ServerAgentThread {
	parameters:
	    @display("i=block/app");
	    @class(ServerAgentThread);
		//@statistic[packetReceived](title="packets received"; source=packetReceived; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        //@statistic[endToEndDelay](title="end-to-end delay"; source="dataAge(packetReceived)"; unit=s; record=histogram,weightedHistogram,vector; interpolationmode=none);
	    
}