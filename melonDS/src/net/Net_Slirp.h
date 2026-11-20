/*
    Copyright 2016-2025 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#ifndef NET_SLIRP_H
#define NET_SLIRP_H

#include "types.h"
#include "FIFO.h"
#include "Platform.h"
#include "NetDriver.h"

#include <libslirp.h>

#include <atomic>

#ifdef __WIN32__
    #include <ws2tcpip.h>
#else
    #include <poll.h>
#endif

struct Slirp;

namespace melonDS
{
class Net_Slirp : public NetDriver
{
public:
    explicit Net_Slirp(const Platform::SendPacketCallback& callback) noexcept;
    Net_Slirp(const Net_Slirp&) = delete;
    Net_Slirp& operator=(const Net_Slirp&) = delete;
    Net_Slirp(Net_Slirp&& other) noexcept;
    Net_Slirp& operator=(Net_Slirp&& other) noexcept;
    ~Net_Slirp() noexcept override;

    int SendPacket(u8* data, int len) noexcept override;
    void RecvCheck() noexcept override;

    // Nintendo WFC through a community server (WfcServers.h): when `on`, the names under
    // nintendowifi.net are answered by that server instead of the real, long-closed ones.
    // FORWARD (`forward`) sends the game's own DNS query to `primary` (host byte order) and
    // answers with what it says, trying `alternate` if it does not reply. DIRECT gives every
    // name `primary`. Read by the network thread, set from the UI thread.
    static void SetWFCRedirect(bool on, bool forward, u32 primary, u32 alternate) noexcept
    {
        // (The addresses first, so the mode that reads them is the last word.)
        WFCPrimary.store(primary, std::memory_order_relaxed);
        WFCAlternate.store(alternate, std::memory_order_relaxed);
        WFCMode.store(!on ? 0 : forward ? 2 : 1, std::memory_order_release);
    }
private:
    static std::atomic<int> WFCMode; // 0 off, 1 direct, 2 forward
    static std::atomic<u32> WFCPrimary;
    static std::atomic<u32> WFCAlternate;
    static constexpr int PollListMax = 64;
    static const SlirpCb cb;
    static int SlirpCbGetREvents(int idx, void* opaque) noexcept;
    static int SlirpCbAddPoll(int fd, int events, void* opaque) noexcept;
    static ssize_t SlirpCbSendPacket(const void* buf, size_t len, void* opaque) noexcept;
    void HandleDNSFrame(u8* data, int len) noexcept;

    Platform::SendPacketCallback Callback;
    pollfd PollList[PollListMax] {};
    int PollListSize = 0;
    FIFO<u32, (0x8000 >> 2)> RXBuffer {};
    u32 IPv4ID = 0;
    Slirp* Ctx = nullptr;
};
}
#endif // NET_SLIRP_H
