// WfcPatcher's NoSSL change as Fulldex applies it in memory (frontend/wfc_nossl.h): the WFC
// URL strings lose the "s", the lone scheme comparison and unrelated URLs are left alone.
#include "wfc_nossl.h"

#include <cstdio>
#include <cstring>

int main()
{
    char b[192] = {};
    strcpy(b, "https://%s/download");
    strcpy(b + 40, "https://nas.nintendowifi.net/acc");
    strcpy(b + 90, "https://");
    strcpy(b + 120, "https://example.com/page");
    strcpy(b + 160, "http://%s/already");
    const int fixed = WfcNoSsl::Apply((unsigned char*)b, sizeof b);
    int wrong = 0;
    auto expect = [&](const char* got, const char* want) { if (strcmp(got, want) != 0) { std::printf("wrong: %s (wanted %s)\n", got, want); wrong++; } };
    expect(b, "http://%s/download");
    expect(b + 40, "http://nas.nintendowifi.net/acc");
    expect(b + 90, "https://");
    expect(b + 120, "https://example.com/page");
    expect(b + 160, "http://%s/already");
    if (fixed != 2) { std::printf("patched %d, wanted 2\n", fixed); wrong++; }
    return wrong ? 1 : 0;
}
