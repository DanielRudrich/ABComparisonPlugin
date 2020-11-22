/*
==============================================================================

ABComparison Plug-in
Copyright (C) 2018 - Daniel Rudrich

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/** An extension to JUCE's OSCReceiver class with some useful methods.
*/
class OSCReceiverPlus : public OSCReceiver, public ChangeBroadcaster
{
public:
    OSCReceiverPlus (int defaultPort = -1, bool shouldAutoConnect = false)
    {
        port = defaultPort;
        connected = false;
        autoConnect = shouldAutoConnect;
        if (shouldAutoConnect)
            connect();
    }

    void setAutoConnect (bool shouldAutoConnect)
    {
        if (autoConnect.exchange (shouldAutoConnect) != shouldAutoConnect && shouldAutoConnect)
            connect();
        else
            sendChangeMessage();
    }

    bool getAutoConnect() const
    {
        return autoConnect.load();
    }

    void setPort (int portNumber)
    {
        port = portNumber;
        DBG ("OSC: Port set to " << port);
        if (autoConnect.load())
            connect();
        else
            sendChangeMessage();
    }

    bool connect()
    {
        if (port == -1)
        {
            disconnect();
            connected = false;
            return true;
        }

        if (! isPositiveAndBelow (port, 65536))
        {
            sendChangeMessage();
            return false;
        }

        if (OSCReceiver::connect (port))
        {
            DBG ("OSC: Successfully opened port " << port);
            connected = true;
            sendChangeMessage();
            return true;
        }
        else
        {
            DBG ("OSC: Failed opening port " << port);
            sendChangeMessage();
            return false;
        }
    }

    bool disconnect()
    {
        if (OSCReceiver::disconnect())
        {
            DBG ("OSC: disconnected!");
            connected = false;
            sendChangeMessage();
            return true;
        }
        else
            return false;
    }

    int getPortNumber() const
    {
        return port;
    }

    bool isConnected() const
    {
        return connected.load();
    }


private:
    int port = -1;
    std::atomic<bool> connected;
    std::atomic<bool> autoConnect;
};

