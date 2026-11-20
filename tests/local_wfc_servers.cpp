// The community server list and wfc_server's forms (WfcServers.h), and the WFC names
// (WfcNames.h): both are header-only, so they are tested without the emulator.
//   g++ -std=c++17 -ImelonDS/src tests/local_wfc_servers.cpp -o /tmp/local_wfc_servers && /tmp/local_wfc_servers
#include "net/WfcServers.h"
#include "net/WfcNames.h"

#include <cassert>
#include <cstring>
#include <iostream>

using melonDS::Wfc::Mode;
using melonDS::Wfc::ParseTarget;
using melonDS::Wfc::Target;

static unsigned IPv4(unsigned a, unsigned b, unsigned c, unsigned d)
{
    return (a << 24) | (b << 16) | (c << 8) | d;
}

int main()
{
    size_t count;
    const auto* servers = melonDS::Wfc::Servers(count);
    assert(count >= 2);
    assert(std::strcmp(servers[0].key, "altwfc") == 0);
    assert(servers[0].mode == Mode::Direct);
    assert(std::strcmp(servers[1].key, "wiimmfi") == 0);
    assert(servers[1].mode == Mode::Forward);

    Target target;

    // A known key: its address, in its own mode.
    assert(ParseTarget("altwfc", target));
    assert(target.mode == Mode::Direct);
    assert(target.primary == IPv4(172, 104, 88, 237));
    assert(target.alternate == 0);

    assert(ParseTarget("wiimmfi", target));
    assert(target.mode == Mode::Forward);
    assert(target.primary == IPv4(95, 217, 77, 151));
    assert(target.alternate == IPv4(95, 217, 77, 181));

    // An address of one's own: direct, or a DNS to forward to.
    assert(ParseTarget("1.2.3.4", target));
    assert(target.mode == Mode::Direct && target.primary == IPv4(1, 2, 3, 4));

    assert(ParseTarget("dns:95.217.77.151", target));
    assert(target.mode == Mode::Forward && target.primary == IPv4(95, 217, 77, 151));

    // Not addresses.
    assert(!ParseTarget("", target));
    assert(!ParseTarget("1.2.3", target));
    assert(!ParseTarget("1.2.3.4.5", target));
    assert(!ParseTarget("300.2.3.4", target));
    assert(!ParseTarget("dns:", target));
    assert(!ParseTarget("dns:1.2.3", target));
    assert(!ParseTarget("not a server", target));

    // The picker's index: a key, or -1 for an address set by hand.
    assert(melonDS::Wfc::IndexOf("altwfc") == 0);
    assert(melonDS::Wfc::IndexOf("wiimmfi") == 1);
    assert(melonDS::Wfc::IndexOf("9.9.9.9") == -1);
    assert(melonDS::Wfc::IndexOf("") == -1);

    // The names the games ask for, and only those.
    assert(melonDS::Wfc::IsName("nas.nintendowifi.net"));
    assert(melonDS::Wfc::IsName("NAS.NINTENDOWIFI.NET"));
    assert(melonDS::Wfc::IsName("nintendowifi.net"));
    assert(melonDS::Wfc::IsName("nintendowifi.com"));
    assert(!melonDS::Wfc::IsName("evilnintendowifi.net"));
    assert(!melonDS::Wfc::IsName("nintendowifi.net.example.com"));
    assert(!melonDS::Wfc::IsName(""));
    assert(!melonDS::Wfc::IsName(nullptr));

    std::cout << "PASS: the community server list, wfc_server's forms and the WFC names\n";
}
