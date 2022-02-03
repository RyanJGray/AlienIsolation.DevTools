#include "ShortGuid.h"

#include "Menu_Log.hpp"

#include <iostream>

using namespace CATHODE;
using namespace CATHODE::DataTypes;

__declspec(noinline)
int __fastcall CATHODE::ShortGuid::hShortGuid(DataTypes::ShortGuid* _this, void* /*_EDX*/, char* node_name)
{
	// Prevent the node/parameter from being registered.
    /*if (reinterpret_cast<int>(_this) == 0x01751888)
    {
        const int ret = RegisterNode(reinterpret_cast<void*>(0x0175188C), node_name);
        logger.AddLog("[Engine] Request to register node %s swapped for node %s!\n", node_name, "on_false");
        return ret;
    }
    else if (reinterpret_cast<int>(_this) == 0x0175188C)
    {
        const int ret = RegisterNode(reinterpret_cast<void*>(0x01751888), node_name);
        logger.AddLog("[Engine] Request to register node %s swapped for node %s!\n", node_name, "on_true");
        return ret;
    }
    else if (reinterpret_cast<int>(_this) == 0x017518C8)
    {
        const int ret = RegisterNode(reinterpret_cast<void*>(0x017518CC), node_name);
        logger.AddLog("[Engine] Request to register node %s swapped for node %s!\n", node_name, "on_false");
        return ret;
    }
    else if (reinterpret_cast<int>(_this) == 0x017518CC)
    {
        const int ret = RegisterNode(reinterpret_cast<void*>(0x017518C8), node_name);
        logger.AddLog("[Engine] Request to register node %s swapped for node %s!\n", node_name, "on_true");
        return ret;
    }*/

    //unsigned char *nodeID = static_cast<unsigned char*>(_this);

    // This call to the original function *must* be performed to populate the ShortGuid in the object for the current node.
    const int ret = ShortGuid(_this, node_name);

    //ShortGuid nodeShortGuid = static_cast<ShortGuid*>(_this)[0];

    /*char* paramString[100];

    iNumRegisteredNodes++;
    sprintf_s(reinterpret_cast<char*>(paramString), 0x100, "[%d] _this: 0x%p; _EDX: 0x%p; nodeID: %02x-%02x-%02x-%02x; param_1: %s\n", iNumRegisteredNodes, _this, _EDX, nodeID[0], nodeID[1], nodeID[2], nodeID[3], param_1);
    printf_s("%s", reinterpret_cast<char*>(paramString));*/

    //registeredNodesFile << reinterpret_cast<char*>(paramString);

    return ret;
}
