#pragma once
// The community servers that stand in for Nintendo WFC (closed in 2014), and how each answers the
// games' names under nintendowifi.net:
//
//   Direct  - every WFC name resolves to one address. For a server that answers all the services
//             from one box (AltWFC/WFZwei).
//   Forward - the game's own DNS query is passed to the server's DNS, so each name resolves as
//             that server says. Needed when the services are on different addresses (Wiimmfi).
//
// Kept here, with WfcNames.h, so the list and wfc_server's forms can be tested without the
// emulator.
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace melonDS::Wfc
{
enum class Mode
{
    Direct,
    Forward,
};

struct Server
{
    const char* key;       // In settings.cfg's wfc_server, and the picker's order.
    const char* name;      // In the picker.
    Mode mode;
    const char* address;   // IPv4: the server itself (Direct), or its DNS (Forward).
    const char* alternate; // Forward: tried when the first does not answer. nullptr for none.
};

inline const Server* Servers(size_t& count)
{
    // (The addresses are the servers' own, and may change: wfc_server also takes any address.)
    static const Server servers[] =
    {
        // (AltWFC/WFZwei answers every WFC name from one address, and its event list has the
        // most Mystery Gifts.)
        { "altwfc",  "ALTWFC / WFZWEI", Mode::Direct,  "172.104.88.237", nullptr },
        // (Wiimmfi's DNS answers each name with the address that serves it.)
        { "wiimmfi", "WIMMFI",          Mode::Forward, "95.217.77.151",  "95.217.77.181" },
    };
    count = sizeof(servers) / sizeof(*servers);
    return servers;
}

// A strict dotted-decimal IPv4 address, in host byte order.
inline bool ParseIPv4(const char* text, unsigned& ip)
{
    if (!text || !*text) return false;
    unsigned a = 0, b = 0, c = 0, d = 0;
    char tail = 0;
    // (The space before %c takes trailing whitespace, but still catches anything else.)
    if (std::sscanf(text, " %u.%u.%u.%u %c", &a, &b, &c, &d, &tail) != 4) return false;
    if (a > 255 || b > 255 || c > 255 || d > 255) return false;
    ip = (a << 24) | (b << 16) | (c << 8) | d;
    return true;
}

struct Target
{
    Mode mode = Mode::Direct;
    unsigned primary = 0;   // Host byte order.
    unsigned alternate = 0; // Host byte order; 0 for none.
};

// wfc_server's forms: a known key ("altwfc"), an IPv4 address (Direct), or "dns:<IPv4>" (Forward).
inline bool ParseTarget(const char* value, Target& target)
{
    if (!value || !*value) return false;

    size_t count;
    const Server* servers = Servers(count);
    for (size_t i = 0; i < count; i++)
    {
        if (std::strcmp(value, servers[i].key) != 0) continue;
        target.mode = servers[i].mode;
        target.alternate = 0;
        if (!ParseIPv4(servers[i].address, target.primary)) return false;
        if (servers[i].alternate) ParseIPv4(servers[i].alternate, target.alternate);
        return true;
    }

    if (std::strncmp(value, "dns:", 4) == 0)
    {
        target.mode = Mode::Forward;
        target.alternate = 0;
        return ParseIPv4(value + 4, target.primary);
    }

    target.mode = Mode::Direct;
    target.alternate = 0;
    return ParseIPv4(value, target.primary);
}

// The picker's index of a known key; -1 for an address set by hand.
inline int IndexOf(const char* value)
{
    if (!value) return -1;
    size_t count;
    const Server* servers = Servers(count);
    for (size_t i = 0; i < count; i++)
        if (std::strcmp(value, servers[i].key) == 0) return (int)i;
    return -1;
}
}
