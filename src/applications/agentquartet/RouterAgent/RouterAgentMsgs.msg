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

import inet.common.packet.chunk.Chunk;

namespace inet;

enum RouterMsgType{
	CREATE_FILTERRULE = 1;
	DELETE_FILTERRULE = 2;
	CREATE_ROUTE = 3;
	CHANGE_ROUTE = 4;
	DELETE_ROUTE = 5;
	TO_IDLE_FILTER_TYPE = 6;
	TO_DROPLIST_ONLY_FILTERTYPE = 7;
	TO_ACCEPTLIST_ONLY_FILTERTYPE = 8;
}

class RouterAgentMsg extends FieldsChunk {
    RouterMsgType type;
    string strs[];
}