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

package agentquartet.applications.agentquartet.RouterAgent;

import inet.applications.contract.IApp;

simple RouterAgent like IApp{
	parameters:
	    @class(RouterAgent);
	    @display("i=block/app");
	    string localAddress = default("");
        int localPort = default(9881); // Router Agent
        string connectAddress = default("");
        int connectPort = default(9981);// Server Agent
	    double startTime @unit(s) = default(1s);
        double stopTime @unit(s) = default(-1s);
        string routingTableModule = default(absPath("^.ipv4.routingTable"));
        string filterRuleTableModule = default(absPath("^.ipv4.filterRuleTable"));
        
        //par of TcpAppBase
	    int timeToLive = default(-1);
	    int dscp = default(-1);
        int tos = default(-1);
        @signal[connect](type=long);
        @signal[packetSent](type=inet::Packet);
        @signal[packetReceived](type=inet::Packet);
        
    gates:
	    input socketIn @labels(TcpCommand/up);
        output socketOut @labels(TcpCommand/down);
}