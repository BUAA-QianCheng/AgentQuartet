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

#include "MitigationInfo.h"

using namespace inet;

Register_Class(MitigationInfo);

std::string MitigationInfo::str() const{
    std::ostringstream str;
    str << "mid: " << mid << " Targets: ";
    for(int i = 0, n = targets.size(); i< n ;i++)
        str << targets[i] <<" ";
    str << "Create Time: " << createTime.str();
    return str.str();
}

bool MitigationInfo::inTargets(std::string s){
    for(int i = 0, n = this->targets.size(); i < n; i++)
        if(s == targets[i])
            return true;
    return false;
}

