# Nintendo WFC, community servers and Mystery Gifts

Nintendo Wi-Fi Connection (WFC) closed in 2014, taking the DS games' online play and Mystery
Gift downloads with it. Fulldex can point the game's WFC names at a community server so the
games' **own** online menus work again: Mystery Gift's *Get via Nintendo WFC*, the GTS, trades
and battles. Nothing is injected into saves and no gifts are unlocked in bulk: the game
receives each gift itself, one at a time, exactly as it did from Nintendo.

This page is both the player setup guide and the troubleshooting reference. If you are filing
a bug report about online features, please read the **Before reporting** section.

> The community servers are run by volunteers, not by this project. Their addresses and the
> services they offer can change. Fulldex has no account, password or payment on any of them.

---

## Quick start

1. **SETTINGS > OPTIONAL FEATURES > NINTENDO WFC** (in the game list or the pause menu) → **ON**.
2. **WFC SERVER** → **ALTWFC / WFZWEI** (default; the most complete Mystery Gift list) or
   **WIMMFI** (see [Wiimmfi and NoSSL](#wiimmfi-and-nossl) below).
3. Start a DS game and set up a Nintendo WFC connection once:
   **NINTENDO WFC SETTINGS > NINTENDO WI-FI CONNECTION SETTINGS**, choose a free connection,
   *Search for an Access Point*, choose **melonAP**, leave **Auto-Obtain IP Address / DNS** on,
   save, and let the connection test finish.
4. In the game, open **MYSTERY GIFT > RECEIVE GIFT > GET VIA NINTENDO WFC**.

Gifts arrive through the game's own delivery flow; the deliveryman in any Pokémon Center hands
the gift over, as before.

---

## The settings

| Where | Setting | Values |
| --- | --- | --- |
| Launcher **Settings > Optional Features**, and the pause menu's **OPTIONAL FEATURES** | **NINTENDO WFC** | off (default) / on |
| Same place, once NINTENDO WFC is on | **WFC SERVER** | ALTWFC / WFZWEI, WIMMFI |
| `settings.cfg` (advanced) | `extra_wfc` | `0` / `1` |
| `settings.cfg` (advanced) | `wfc_server` | `altwfc`, `wiimmfi`, an IPv4 address, or `dns:` + an IPv4 address |

`wfc_server` forms:

- `altwfc`, `wiimmfi`: a known server from the list.
- `203.0.113.10` (any IPv4): **direct** mode — every WFC name is answered with that address.
  Use this for a server that hosts all the WFC services at one address (AltWFC/WFZwei does).
- `dns:203.0.113.10`: **forward** mode — the game's own DNS query is relayed to that server's
  DNS, so each name resolves to whatever address that server says. Use this when the services
  are on more than one address (Wiimmfi's are).

Direct mode is the safest choice when you are not sure: one address answers every WFC name.

---

## What Fulldex does

The emulated DS uses melonDS's user-mode network. Fulldex answers the DS's DNS queries itself:

- With the setting on, a query for a name under `nintendowifi.net` or `nintendowifi.com` is
  answered from the chosen server instead of the closed Nintendo one. Every other name resolves
  as usual.
- This works whether the game's WFC connection uses automatic DNS or a manual DNS address, and
  replies come from the DNS server the game actually asked, so the DS accepts them.
- A DNS relay (`dns:...`/Wiimmfi) gets [WfcPatcher's NoSSL change](#wiimmfi-and-nossl) applied
  in the DS's memory too. The player's ROM file is never written to.

The names covered are the WFC set: `conntest`, `nas`, the `*.gs.nintendowifi.net` game/lobby
hosts, the Download Service (`dls1`) and game-stats hosts.

---

## Wiimmfi and NoSSL

Wiimmfi's DS service expects the game's WFC Web addresses to use **http**, not **https**. The
community tool [WfcPatcher](https://github.com/AdmiralCurtiss/WfcPatcher) changes that in a
copy of the ROM (its "NoSSL" patch). Fulldex makes the same change **in the emulated DS's
memory** whenever the chosen server is a DNS relay:

- URL strings beginning `https://` that contain `%s` or `nintendowifi` lose the `s`
  (`https://%s/download` becomes `http://%s/download`), exactly as WfcPatcher does to the ARM9,
  ARM7 and overlays.
- The lone `"https://"` used by the library to compare a scheme is left alone.
- Only strings that look like WFC URLs are touched; anything else on the DS is left as it is.
- The ROM on disk is unchanged. Achievements, save identity and your dump are unaffected.

The log shows this happening:

```
[wfc] community server wiimmfi (its DNS, per name)
[wfc] NoSSL: 6 URL(s) switched to http in memory
```

If you see `NoSSL` lines but still fail to connect, look at the error code table below.

**AltWFC / WFZwei does not need NoSSL**, which is why direct mode leaves the game alone.

---

## Error 60000: an old WFC ID

This is the most common Wiimmfi failure and it is **not** a Fulldex bug.

Nintendo's servers gave each save a **profile ID (PID)**/friend code. Wiimmfi cannot reuse it:
when the game's nickname is unknown to Wiimmfi it issues a new PID, but the save still holds the
old one, so the login stops with **60000**. Wiimmfi's [documentation](https://mkwiiki.org/wiki/Wiimmfi/Error_60000)
describes the same for Wii and DS.

Fix (one time per save):

1. Start the game and reach the title screen's main menu (without continuing).
2. **NINTENDO WFC SETTINGS > OPTIONS > ERASE NINTENDO WFC CONFIGURATION**, confirm twice.
   The system shuts down; this erases the WFC ID and connection settings.
3. Start the game again and set up the connection as in
   [Quick start](#quick-start) step 3.
4. Open **MYSTERY GIFT > RECEIVE GIFT > GET VIA NINTENDO WFC**. If the game asks to update the
   Wi-Fi User Information, choose **YES**; it will warn that Pal Pad friends and Rating Mode
   results are deleted. That is the profile update Wiimmfi needs.
5. The gift list should appear. Later attempts reuse the new profile; no further reset needed.

**What is lost:** Pal Pad friend data and Rating Mode results for that save, as the game warns.
Your Pokémon, items and story progress are not touched. **What is not lost:** the save itself.
Back up the save first if you like (the Manage Saves page has *Export Save* and the backups
folder).

---

## Error codes

| Code | What it usually means | What to try |
| --- | --- | --- |
| **60000** | Old WFC ID/PID, see [above](#error-60000-an-old-wfc-id) | Erase the WFC configuration once, reconnect, accept the user-information update |
| **20100 / 20110** | The game cannot complete the WFC login (often the connection never reached the server, or SSL was not removed) | Check the server choice: Wiimmfi needs `dns:`/forward mode and the NoSSL change; AltWFC is direct. Check the DNS setup and the connection test. |
| **52100 / 61020** | DNS or no connection at all | Recheck the WFC connection (automatic DNS), make sure NINTENDO WFC is ON, and that a VPN/router is not forcing its own DNS |
| **86420 / 86430** | NAT/connection trouble (more common for battles) | Try again, try the other server, or another network |
| **23919** | Wiimmfi refused a banned/hacked PID | Use a clean WFC ID (erase and reconnect) |
| *Connection test fails* | The game has no working WFC profile | Erase and set the connection up again (Quick start step 3) |
| *Gift list is empty* | No event is up on that server right now | Try AltWFC/WFZwei, or try later; servers rotate events |

Error numbers are the game's own; they are shown in the game's message box. Fulldex's log adds
`[wfc]` lines that say which names were answered and by which server.

---

## Before reporting a bug

Please include:

- **Which server** is chosen (ALTWFC/WFZWEI, WIMMFI, or the exact `wfc_server` value).
- **Whether the WFC ID was erased** for this save (60000 fix above).
- **The `[wfc]` lines from `game.log`** (and the rest of the log; see the bug report form for
  where it is on each platform). They show the names resolved, the NoSSL patches and the
  server reached.
- **The exact error code** in the game's message, and what step it appears at (connection test,
  login, gift download, battle...).
- Whether the same save works with the **other server** (AltWFC vs Wiimmfi). That one comparison
  tells us whether it is a server-side or game-side problem.

Please do **not** attach a ROM. Saves and logs are fine.

---

## For developers

Code map:

| File | Role |
| --- | --- |
| `frontend/optional_features.h` | the `extra_wfc` option and its help |
| `frontend/main.cpp` | `ApplyWfcSetting` (parses `wfc_server`, sets the redirect and the NoSSL flag), the `WFC SERVER` picker, the per-30-frames NoSSL scan in the emulation loop |
| `melonDS/src/net/Net_Slirp.cpp` | answers the DS's DNS; direct and forward modes; replies come from the queried server |
| `melonDS/src/net/WfcNames.h` | the WFC name rule (label boundary; tested by `tests/test_wfc_dns.py`) |
| `melonDS/src/net/WfcServers.h` | the server table and `wfc_server` forms (tested by `tests/local_wfc_servers.cpp`) |
| `frontend/wfc_nossl.h` | the in-memory NoSSL scanner |
| `docs/WFC-AND-MYSTERY-GIFTS.md` | this page |

Notes:

- The work is opt-in and off by default because it points at a third-party service.
- The switch has no user-mode Wi-Fi (`SSR_NET` is not built there), so the option is hidden on
  the Switch.
- `SetWFCRedirect` and the NoSSL flag are set on the UI thread and read by the emulation thread;
  the redirect address/mode are `std::atomic`/release-acquire, and the NoSSL scan runs on the
  emulation thread over main RAM.
- The NoSSL scan is the same change WfcPatcher makes to a ROM, but local to the session. If a
  game version keeps its URL strings somewhere main RAM does not cover, no URLs are patched and
  the log stays silent; add the game/version and the strings' whereabouts to this page.
- Adding a server: append it to `WfcServers.h` (`Mode::Direct` for one address, `Mode::Forward`
  when its own DNS must answer each name), update the two help texts and this page, and test
  both a DNS lookup and an actual in-game connection on at least one platform.
- The forward mode waits up to a second per name (then tries the alternate address); that is
  only during the first WFC connection, not per frame.

## Tested

- **Android (Pixel 3, stock US Black ROM, release build, arm64):** AltWFC direct — full Mystery
  Gift receive ("super-fast Mewtwo", gift written to `black.sav`). Wiimmfi forward with the
  in-memory NoSSL patch and a fresh WFC ID — login, distribution list, gift received. Both in
  the game's own menus.
- **Linux:** compiles and the DNS/NoSSL unit tests pass.
- **Steam Deck:** AltWFC direct — full Mystery Gift receive.
- Windows, macOS and iOS have not been exercised for this feature yet.
