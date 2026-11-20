#pragma once
// WfcPatcher's "NoSSL" change, in the emulated DS's memory instead of the player's ROM file:
// a URL string that starts "https://" has its "s" removed, so the game's DWC library uses plain
// HTTP. Wiimmfi needs this for DS games (its certificates rely on a console bug, or on this
// patch); AltWFC does not, so this is only applied for a server chosen as a DNS relay.
//
// WfcPatcher walks the ARM9, ARM7 and overlays in the ROM and replaces every "https://" with
// "http://", skipping the lone "https://" the library compares a scheme against. Here the same
// is done over the DS's main RAM, where those strings appear once the code that holds them is
// loaded, and only for strings that look like WFC URLs (they contain "%s" or "nintendowifi"),
// so nothing else on the DS is touched. A patched string no longer starts with "https://", so
// repeated scans are harmless.
#include <cstddef>
#include <cstring>

namespace WfcNoSsl
{
inline bool LooksWfc(const unsigned char* s, size_t n)
{
    const char* a = "nintendowifi";
    const size_t an = 12;
    for (size_t i = 0; i + 2 <= n; i++)
    {
        if (i + 2 <= n && s[i] == '%' && s[i + 1] == 's') return true;
        if (i + an <= n && memcmp(s + i, a, an) == 0) return true;
    }
    return false;
}

// Patches every WFC URL in `ram`; returns how many strings changed.
inline int Apply(unsigned char* ram, size_t size)
{
    if (!ram || size < 9) return 0;
    static const char needle[] = "https://";
    int patched = 0;
    for (size_t i = 0; i + 8 <= size; i++)
    {
        if (memcmp(ram + i, needle, 8) != 0) continue;
        size_t n = 0;
        while (i + n < size && n < 512 && ram[i + n]) n++;
        if (n <= 8) { i += 7; continue; }              // (a lone "https://": the scheme check)
        if (!LooksWfc(ram + i, n)) { i += 7; continue; }
        // Remove the 's' at index 4, taking the rest of the string (and its end) with it.
        memmove(ram + i + 4, ram + i + 5, n - 4);
        ram[i + n] = 0;
        patched++;
        i += n;
    }
    return patched;
}
}
