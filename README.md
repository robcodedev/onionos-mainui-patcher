# OnionOS MainUI Binary Patcher

A Python tool that adds support for favorite folders, custom rows in game lists, game metadata,
optimized performance and other improvements to an original OnionOS `MainUI` executable.

**Version 1.2**

> [!WARNING]
> This is an unofficial binary patcher. Keep an untouched copy of the original MainUI executables
> and back up `/Roms/favourite.json` before installation.

## Quick installation

Place `onionos_mainui_patcher.py` and a clean original MainUI file in the same directory, then for
Miyoo Mini Plus run:

```sh
python3 onionos_mainui_patcher.py MainUI-354*
```

This applies all patches and creates:

```text
MainUI-354-clean.patched
MainUI-354-expert.patched
```

Rename the newly patched binaries and move them (without the trailing `.patched` suffix) to
`/.tmp_update/bin/` on the SD card, overwriting the old binaries. Keep the original binaries
somewhere safe.

> [!NOTE]
> Python 3.10 or newer is required.
>
> - MainUI-354 for Miyoo Mini Plus
> - MainUI-283 for Miyoo Mini
> - MainUI-285 for Miyoo Mini Flip (OnionOS v4.5-dev only, otherwise MainUI-354)
>
> Adjust the command accordingly, or build for all `MainUI*` files.
>
> MainUI binaries are available from the OnionUI repository:
> <https://github.com/OnionUI/Onion/tree/main/static/build/.tmp_update/bin>

## First-install step

Boot your device and choose **Refresh Roms** once from MainUI (SELECT context menu). This rebuilds
existing per-system cache databases so Arcade display-name sorting, database indexes, normalized
paths, and other rebuild-time optimizations are fully applied.

## Recommended configs

Either adjust by using the updated tweaks and themeSwitcher binaries from the project or just
manually update the config files as described below.

### Updated tweaks and themeSwitcher binaries

Copy the pre-built tweaks and themeSwitcher binaries along with their dependencies from
`/tools/build/.tmp_update/` into the `/.tmp_update/` folder on your SD card. Make sure to create
backups of your old binaries before overwriting.

You can find the new configurations options located under Appearance.

### Manually configuration

Recommended runtime configuration values:

```text
/.tmp_update/config/.romListRows        10
/.tmp_update/config/.romListFontSize    22
/.tmp_update/config/.romListTitleScroll 700,120
```

Paths beginning with `/.tmp_update/` in this README are shown relative to the root of the 
SD card for readability. MainUI accesses the same files through the mounted runtime path 
`/mnt/SDCARD/.tmp_update/`.

The main-menu patch additionally reads the structured file:

```text
/.tmp_update/config/main-menu.json
```

Restart MainUI or reboot your device after changing these files.

## Current scope

The default build improves these main areas:

- ROM-list layout, navigation, sorting, title scrolling, and thumbnail handling
- Favorite folders, ordering, persistence, and return behavior
- Recent-list loading, retention, App exclusion, single-entry removal, and consistent list/detail previews
- ROM database, XML, Arcade-name, shell-operation, rebuild-path safety, and background-status performance
- Wi-Fi connect-command quoting for SSIDs/passphrases containing spaces or shell metacharacters
- Game-detail titles, metadata, navigation, and layout
- Main-menu layout and shortcuts, Onion Search Favorite-state handling, theme/dialog behavior, and selected settings fixes

## Theme-author compatibility guide

The patched MainUI remains compatible with themes that provide only the normal stock assets. The
optional settings and assets below are scoped to the **active theme**. With
`skip-inactive-theme-configs` enabled, MainUI still enumerates the top-level `/mnt/SDCARD/Themes`
directory during startup but reads and parses `config.json` only for the selected external theme.
Patch-added theme state is staged by exact theme path, reset before active-theme resolution, and
populated only from the exact selected theme. A setting in one installed theme cannot become the
live setting for another theme.

A theme can combine the two game-list settings in its top-level `config.json`:

```json
{
  "gamelist": {
    "bold": false,
    "iconLeftMargin": 12
  }
}
```

- **Game-list icon left margin:** `gamelist.iconLeftMargin` is optional and belongs to
  `patch-rom-list-rows`. It is an absolute theme-author control for the aligned folder/game icon
  layout and is valid at every supported row count, including the stock six-row layout. `0` is a
  valid value and permits artwork to begin at the list edge; values above 300 clamp to 300. Missing,
  negative, or invalid values retain the established row-count-aware geometry. One value controls
  both folder and game rows; MainUI keeps their existing visual alignment internally rather than
  exposing separate margins. The title start and usable title width move with the icon layout.
- **Game-list font weight:** `gamelist.bold` belongs to `patch-theme-list-font-bold`. Missing,
  invalid, or `true` retains the stock bold game-list style; `false` uses a private normal-style
  game-list font. ROM, Favorites, Recent, and contextual Search rows are affected. Language,
  Systems, Apps, Settings, titles, Game Details, keyboard text, and other independently loaded fonts
  are not restyled.
- **Game-list selection backgrounds:** for a configured row count `N` from 6 through 20, ROM,
  Favorites, and Recent lists first try `skin/bg-list-s_N.png` and otherwise use the ordinary
  `skin/bg-list-s.png`. Numbered variants should retain the stock asset width, horizontal alignment,
  and transparency treatment. Their effective row height is `floor(360 / N)`.
- **Popup selection background:** context-menu selection rows optionally use
  `skin/bg-list-popup-s.png`. If it is missing or cannot be decoded, the already loaded normal
  selection surface remains in use. Make this asset the same width as the theme's
  `bg-pop-menu-1.png` through `bg-pop-menu-6.png`; a 60 px height matching the normal popup row is
  the safest choice. This asset is the highlighted-row strip, while the numbered
  `bg-pop-menu-N.png` files remain the popup container backgrounds.
- **Wide game-icon spacer layouts:** a deliberate `skin/icon-game.png` spacer is recognized only
  when it is at least **120 px wide** and at least **3 times as wide as it is tall**. Its complete
  horizontal width is preserved and only vertical excess is center-cropped. The preserved width is
  used for title geometry, and Favorite markers automatically use the far-right placement for these
  layouts so a fixed marker lane does not collide with the spacer. Normal icons retain ordinary
  row-count-aware behavior. Theme-resize tooling should use the same `width >= 120 && width >=
  3*height` rule and leave qualifying spacer assets unchanged.
- **Numbered popup backgrounds:** `bg-pop-menu-1.png` through `bg-pop-menu-6.png` should use a common
  width, border placement, transparency treatment, and horizontal padding. Heights may differ to
  preserve the common popup row pitch. Missing larger assets can still be synthesized by the popup
  background compatibility patch from the highest readable smaller numbered asset.
- **Settings icons:** themes should supply `skin/icon-theme.png` for the restored **Themes** row and
  `skin/fixit.png` for the restored **Tweaks** row in Settings.

### Safe decorative margins and borders

The 640x480 MainUI screen reserves a 360 px game-list band between the title and tips regions:

```text
y =   0..59    title/top region
y =  60..419   game-list viewport
y = 420..479   tips/bottom region
```

The areas above `y=60` and from `y=420` downward are therefore outside game-list row drawing, though
the title/tips UI itself can of course use them. Inside the 360 px list viewport there is **no
row-count-independent top or bottom gutter**: the row height changes with `N`, and selection/icon
artwork may use the full effective row height. Decorative borders that must never intersect a list
row should stay outside that 360 px band. Borders inside `bg-list-s_N.png` should be designed within
that row's own `floor(360/N)` height rather than relying on a large unused vertical margin.

Horizontally, do not assume the stock 20 px left/right inset is always free. Dense row counts already
reduce normal outer padding to a 15 px minimum, and `gamelist.iconLeftMargin` can deliberately move
the icon/title layout farther left, including all the way to zero. If a theme needs a permanent left
border of `B` pixels, choose `iconLeftMargin` large enough to reserve `B` plus the desired visual gap.
On the right, keep important border artwork conservative: normal dense-row geometry can approach the
15 px outer inset, Favorite markers can occupy the far-right lane, and wide-spacer layouts can use
more horizontal space than stock. Test border-heavy themes at the smallest and largest row counts
they intend to support.

Top-level main-menu labels may wrap to two centered lines inside a 136-pixel safe width. A language
value consisting of one space can still hide a top-level main-menu label; the same value no longer
creates a blank built-in SELECT-menu action because context labels use embedded fallbacks. Dialog
action labels use the theme hint style when that selector is enabled, and the improved Game Details
layout uses the documented 360-pixel right column. Test unusually large fonts, borders, icons, and
preview art on-device because MainUI crops rather than dynamically redesigning theme assets.

## Requirements

Applying the patches requires only `onionos_mainui_patcher.py` and Python 3.10 or newer. The release
ZIP contains source and documentation only; it does not include a proprietary MainUI binary or
separate compiled helper files.

- Python **3.10 or newer**
- A clean original OnionOS MainUI binary
- An untouched backup of that binary
- A backup of `/Roms/favourite.json` before using Favorite-related changes
- OnionOS 4.2.2 or newer

## Supported input hashes

The following complete SHA-256 hashes are recognized reference inputs:

- MainUI-283-clean `6b01276a6292fd7061e0b2576322a52ada65b755562f97bf7656b174d475866f`
- MainUI-283-expert `6948b5310dda6513b9e8fa2519d90c5287fc1fd06b4f668a7f2f205406281d28`
- MainUI-285-clean `f693044f031f39e5f30ec7f81b501a705db698d98a72806aa531c6974361cf89`
- MainUI-285-expert `eabc6333999ebfafbe5cf4f9a34648918a3c8947a5c8fd6ad51e0a035452e7d5`
- MainUI-354-clean `98c85f6c573bdeabd3762e8d9b596f354014e666cc873d0f758cbf3752620c94`
- MainUI-354-expert `3bd1fef7fd9bd215bb9e335b6be1101fdff510590ba0d9ca0a9707edc5d9718a`

The complete input hash is advisory rather than the only compatibility check. An unknown hash
receives a prominent warning and is accepted only when the ELF structure, imports, protected data,
hook signatures, and original instructions are still compatible.

## Safety

Current safety checks include exact stock-instruction guards, a centralized RTTI/vtable
Action-ownership map, structural/call-graph verification for important statically linked routines,
exhaustive host properties for persisted ROM-window geometry, SQL quote-pair truncation checks,
COUNT-cache ownership/key models, title-scroll/config arithmetic tests, ARM builder/allocator
self-tests, patch-selection dependency tests, and a final re-parse of the completed ELF layout
before publication. Link-sensitive protected direct-call guards use an ARM `BL` decoder that
validates the link bit instead of accepting a same-target plain `B`; generic control-flow decoding
still supports both branch forms where that distinction is intentionally irrelevant. Module-level
constants named like high-signal patch scaffolding (`*_VA`, `*_WORD`, `*_WORDS`, `*_STOCK`,
`*_STOCK_WORD`, `*_CALL`, `*_SITE`, and `*_SIG`) are AST-linted and must have a real code reference
outside their definition; comments and string literals do not count. Module-level constants whose
value is a mangled C++ import symbol (`_Z...`) are subject to the same fail-closed rule. The retained
`DBCachedTextMenu::deleteRow` function signature is pinned uniquely at its protected VA, and the
shared TextMenu keymap finder consumes its retained surrounding signature while separately requiring
the translator instruction to be a link-sensitive ARM `BL`. A broader diagnostic audit can report
all definition-only module-level uppercase constants, including intentional provenance/state-layout
records, without failing normal builds. The patcher also fails closed if protected stock call maps or
stack-slot provenance change. The forward and backward ROM asynchronous window loaders are validated
at their resolved bodies as well as at their calling `BL` sites: the patcher pins their prologue and
callee-saved `r4` contract, direction-specific selected/window arithmetic, and shared worker-construction
call shape before any row/page helper is allowed to use them.
The recursive ROM rebuild hardening similarly pins its scan functions, allocation/cleanup ownership,
filename capacities, recursive argument forwarding, and each modified formatter's stock provenance.
The connect-time Wi-Fi quoting patch pins both connect-function entries plus every original formatter,
executor, command-buffer handoff, format string, and input-local source before redirecting them.

> [!NOTE]
> **Maintainer headroom:** the current all-patches 354 layout leaves about 1,084 bytes in the finite
> stock executable gap. Large or position-independent new helpers should continue to go in the
> appended R-X segment rather than consume this gap. The large `patch()` orchestration function is
> structural debt; any future refactor should be proven with byte-identical patched output before
> and after rather than combined with runtime changes.

## Available patches

57 registered selectors, all enabled by default. The complete all-patches build is the normal
integrated configuration; partial `--include` / `--exclude` builds are supported primarily for
diagnosis and experimentation.


| Patch name | Purpose | Runtime configuration |
|---|---|---|
| `patch-rom-list-rows` | Configurable row count and geometry, row-specific `bg-list-s_N.png` theme selection background with stock fallback, active-theme `gamelist.iconLeftMargin` (0..300) for the aligned folder/game icon layout, Favorite/Recent `icon-game.png` parity with centered row-height clipping, wide-spacer compatibility using the >=120 px and >=3:1 rule with automatic far-right Favorite markers, row-scaled horizontal geometry with outer padding clamped at 15 px and post-icon spacing clamped at 5 px, first-frame preview geometry correction, fixed preview-safe Favorite-marker lane by default with a presence-only legacy dynamic-position switch for normal icons, whole-renderer tagged game-list title clipping to one row-count-aware right edge on the actual destination surface, with Favorite-marker narrowing in fixed mode, custom-row first-presentation readiness barrier, total-aware restored-window normalization with descending-range hardening, restored-window/deep-position warm-up, corrected L2/R2 destination loading, bounded one-row edge-wrap destination readiness, bounded repaint recovery, and one completion-generation-gated stock preview refresh at the pre-geometry cover check | `.romListRows`, theme `config.json` |
| `patch-rom-list-font-size` | Optional font-size override for ROM, Favorite, Recent, and contextual-Search result lists; established pointer tags remain authoritative and exact `DBCachedTextMenu` is only a post-miss fallback | `.romListFontSize` |
| `fix-contextual-search` | Contextual Search fixes: confirming-A release filter, first-result start in the normal integrated row-patch build, `Console: term` title spacing at both stock builders, deterministic case-configured A–Z order, contrast-aware bright-yellow/purple substring highlighting with stable selected geometry, white keyboard key caps, and marker-gated direct return to the source list after launching a game from Search | `.romListCaseSensitiveSort` |
| `patch-rom-list-title-scroll` | Delayed seamless loop-marquee scrolling for overflowing selected titles, 60 px loop gap, guarded NEON preview compositing, and stable-preview synthetic-frame housekeeping skip; 33 ms repaint target with ~20 Hz effective cadence measured on MainUI-354 hardware | `.romListTitleScroll` |
| `patch-rom-list-letter-jump` | R1/L1 next/previous visible-initial navigation; resilient one-window-at-a-time ROM scanning resumes on completion/idle or another shoulder press, preserves unrelated actions, and cancels if selection changes; short-name Arcade rebuilds bind resolved display labels to SQLite `disp` | Refresh Roms once for Arcade |
| `patch-rom-list-end-jump` | Hold UP then press L2 for first item; hold DOWN then press R2 for final item, including sidecar-backed Favorite menus | None |
| `patch-theme-list-font-bold` | Read `gamelist.bold` for game-list row fonts | Theme `config.json` |
| `skip-inactive-theme-configs` | Keep the stock `/Themes` enumeration loop but skip every non-selected theme before child-path construction, file access, JSON parsing, or the theme callback; unresolved/default selections fail open to the complete stock scan | None |
| `fix-game-list-rapid-navigation` | Clear stale acceleration history; accelerated Up/Down follows the effective visible row count when the row patch is enabled | None |
| `limit-rom-list-thumbnail-cache` | Cap decoded game-preview caching at 64 surfaces | None |
| `skip-rom-preview-surface-copy` | Remove redundant self-format surface clones from preview and theme loading | None |
| `cache-list-skin-images` | Cache the first-theme decode of repeated `bg-list-s[_N].png`, `ic-favorite-mark.png`, and `icon-TF.png` assets; each window still receives an independent surface clone, so stock destruction/runtime theme reload ownership is unchanged | None |
| `skip-identity-thumbnail-rescale` | Reuse a normalized preview surface when requested width already equals source width; true downscales keep the stock scaler | None |
| `optimize-favourite-refresh-sweep` | Skip the mode-2 O(n) deferred-delete row sweep on ordinary updates and run the unchanged stock sweep only after the verified pending-delete setter fires | None |
| `optimize-rom-database-deletion` | Collapse the thirteen normal ROM-cache deletion calls while preserving the distinct stock P2EXT fallback; payload is appended R-X | None |
| `optimize-direct-shell-operations` | Replace trivial startup/launch shell-outs with direct file/sysfs operations and remove the redundant pre-`fopen` `__xstat` | None |
| `skip-rom-artwork-directory-scan` | Skip the root `Imgs` directory before recursion in both the rebuild pre-count (Scanning total) and the real `cache6.db` scan; ordinary/deeper game subdirectories keep the stock path | None |
| `throttle-background-status-polling` | Poll battery and Wi-Fi immediately then every five seconds; skip `/proc` when Wi-Fi is disabled and use one process walk when enabled | None |
| `fix-wifi-network-shell-quoting` | Shell-quote connect-time `wpa_cli` SSID/PSK values so spaces and shell metacharacters remain literal; scanning is unchanged and oversized commands fail without execution | None |
| `optimize-burst-cpu-governor` | Save/restore the active governor around startup, ROM-cache rebuilds, and MainUI launch preparation/teardown using nested bursts and a recovery marker | None |
| `optimize-rom-database-rebuild` | Bulk-load derived `cache6.db` rebuilds with disabled durability, memory temp storage, an approximately 8 MiB page cache, and deferred browse-index construction when the index selector is also active | None |
| `fix-rom-rebuild-path-safety` | Harden recursive scan-based `cache6.db` rebuild path construction with a 4096-byte shared path buffer, bounded formatting, explicit filename-scratch termination, and allocation-failure gates; overlong entries are skipped rather than truncated | None |
| `optimize-miyoogamelist-file-checks` | Bounded transient filename set for ordinary root-level XML import checks, with stock `access()` fallback | None |
| `suppress-miyoogamelist-entry-timing` | Remove two loop-local `gettimeofday` calls while retaining whole-import timing | None |
| `allow-missing-miyoogamelist-image-tags` | Import games whose `<image>` tag is missing or empty; missing/null-text tags explicitly clear the reused image-path string so an entry cannot inherit the previous game's image | None |
| `cache-favourite-lookups-per-page` | Page-local Favorite cache when folders are excluded; automatically superseded by the folder patch's revalidated exact-path hash | None |
| `add-favourite-folders-json` | Keep stock `favourite.json` flat while a durable sidecar supplies folders, stable assignments, positional ordering, private action translations, instant move refresh, same-boot root viewport restore before the first slot-8 preview update (with the existing draw/input one-shot retained as a fallback), input normalization, cut reset, and a leading `..` row in every non-root Favorite folder | None |
| `sort-favourite-folder-a-z` | Add a translated Sort A-Z action that persists deterministic current-folder ordering while retaining folders before ordinary Favorite rows; requires `add-favourite-folders-json` | None |
| `restore-favourite-folder-after-game-exit` | Restore the launched row inside its nested Favorite folder after game exit; requires `add-favourite-folders-json` | None |
| `add-recent-remove-from-list` | Insert translated ID 52 immediately below Start; keep the stock game erase-key path and remove App/non-game records by bounded launch/label identity through MainUI's iterator erase and writer | None |
| `optimize-recent-list-loading` | Use the label already saved in `recentlist.json` and bypass both cold `GetGameName` calls during Recent-row construction | None |
| `fix-recent-preview-paths` | Normalize Search-origin Recent games to the source emulator + real ROM for duplicate comparison/insertion, filter exact `launch="setstate"` pseudo-records before retained slots are consumed, and make Recent list preview, initial RIGHT-open Details, and Up/Down detail refresh use one explicit-PNG-or-source-image-directory thumbnail rule | None |
| `fix-search-favourite-status` | Normalize Onion Search's synthetic `launch.sh:/real/rom` identity across Favorite star checks, Add/Remove popup choice, removal, and new Favorite storage; new records keep the stock schema but use the source emulator launcher + real ROM, while legacy Search-shaped rows remain readable | None |
| `use-rom-database-display-names` | Treat the `disp` column in an existing `cache6.db` as authoritative and never call `GetGameName` while browsing, even when the stored title looks like an Arcade short name | None |
| `extend-recent-list-to-50` | Inspect at most the first 200 parsed `recentlist.json` records, retain the first 50 unique Recent identities in top-of-file order, and ignore later duplicates during reconstruction | None |
| `exclude-apps-from-recent-list` | Prevent live AppAction launches from entering Recent and skip stale type-3 AppAction rows while reconstructing `recentlist.json` | None |
| `add-parent-folder-to-rom-lists` | Add a leading `..` row to every non-root ordinary SQLite-backed ROM directory; root browsing and root pagination counts keep the simple indexed query forms, while search lists remain unchanged | None |
| `suppress-rom-database-debug-logging` | Hot-path development logging (ROM database, Recent, preview, Favourite refresh, menu teardown): suppress sixteen unconditional development/debug output calls and remove five timing-only `gettimeofday` calls | None |
| `skip-rom-database-refresh-sync` | Skip global `sync()` after ROM-cache deletion and after both rebuild paths | None |
| `cache-rom-list-counts` | Cache repeated positive ROM-list COUNT results in RAM; cache hits bypass SQLite cleanup while real queries use `sqlite3_reset()` to release read locks without destroying the prepared statement | None |
| `reuse-onion-arcade-name-lookup` | Read Onion's Arcade-name file once into a packed process-local hash cache, bypass per-ROM `std::map` lookup, and reuse the first language/pinyin result | OnionOS 4.2.2 or newer; automatically includes `use-rom-database-display-names` |
| `optimize-rom-list-database-indexes` | Add the runtime-selected BINARY/NOCASE browse index; defer its creation until after bulk insertion when the rebuild optimizer is also active | Refresh Roms once |
| `patch-key-repeat-settings` | Global SDL key-repeat delay and interval | `.mainUIKeyRepeat` |
| `fix-game-detail-title-on-open` | Show the correct full title immediately when opening game details | None |
| `fix-game-detail-title-wrap-width` | Wrap detail titles by measured pixel width using the active font, without forced hyphens or a dropped final character | None |
| `show-gamelist-metadata-in-game-details` | Show bounded genre, 0-10 rating, and a page-scrollable description using the detail system-name font family/size as its base; synthetic Onion Search rows do not traverse back to source-console XML | Immediate-directory `gamelist.xml` only |
| `skip-folders-in-game-detail-navigation` | Skip folders in detail Up/Down; use game-only list/detail counters; suppress the normal N/N counter on Favorite and ordinary folder rows | None |
| `fix-sleep-timer-left-right` | Make Sleep Timer Left cycle opposite to Right | None |
| `patch-main-menu-layout` | Read-only `main-menu.json` visibility, launcher-aware Refresh/Search/Tweaks fallback, restored external Themes/Tweaks rows directly below Display in Settings, ordered SELECT shortcuts, built-in context-label fallback, custom launchers, transient-window state filtering around external handoffs, startup restoration of a hidden Recent list, top-level-only two-line labels, and a session-only four-shoulder reveal | `main-menu.json`, `.showRecents`, `.showExpert`, language ID 407 |
| `fix-invalid-main-menu-state` | Use Game instead of the `wrongbeef` fallback | None |
| `fix-game-list-context-menu-background` | Use the correct three-row background for the ordinary three-item game-list context menu | None |
| `patch-context-menu-selection-background` | Use optional active-theme `skin/bg-list-popup-s.png` for PopupWindow selection rows, with the existing list-selection surface as fallback | Theme asset |
| `fix-dialog-action-theme-style` | Use the theme hint font/color for generic and confirmation-dialog action labels, including Shutdown | None |
| `fix-case-sensitive-game-list-sorting` | Use a marker file to switch between case-insensitive and stock case-sensitive sorting | `.romListCaseSensitiveSort` |
| `improve-game-details` | Improve the complete detail layout, trim outer ASCII spaces only while drawing the system label, align the counter/Favorite row and previews, and widen metadata | None |
| `prevent-favourite-label-collisions` | Refuse new Favorite entries with an existing exact label | None |
| `escape-apostrophes-in-rom-paths` | Escape apostrophes before paths enter MainUI SQL queries | None |
| `--include PATCH` | Switch to an include-only build and add a named patch; repeat or use commas | N/A |
| `--exclude PATCH` | Remove a named patch from the selected set; repeat or use commas | N/A |
| `--patch-all` | Explicitly select every patch; equivalent to the no-selector default | Uses applicable config files |
---

## Patcher file

[`onionos_mainui_patcher.py`](./onionos_mainui_patcher.py)

This package contains source and documentation only. It intentionally does not contain a prepatched
MainUI binary.

Show the complete command-line help or the patcher version:

```sh
python3 onionos_mainui_patcher.py --help
python3 onionos_mainui_patcher.py --version
```

Routine input, compatibility, and filesystem failures are reported as one concise `[ERROR]` line
with exit status 1 rather than a Python traceback. `KeyboardInterrupt` and argparse's normal
help/usage exits are not converted into patcher errors.

During patching, progress is printed as ordinary newline-terminated, immediately flushed
`[PROGRESS]` lines. The expensive patch-body pass is divided into real sub-stages (ROM-list layout,
navigation, theme/font, cache/list, main menu, Recent, Favorite helpers, game details, metadata,
database/count, and system/background work), so the percentage continues to advance during the
longest part of an all-patches build. The broad active-theme and game-font signature searches also
expose internal 12/25/37/50/62/75/87/100-percent scan checkpoints instead of appearing frozen while
they walk the executable. If any one sub-stage still takes more than about two seconds, the current
stage is repeated with a low-frequency `working` heartbeat. Its timer is reset whenever the progress
stage changes, so `working, 2s on this step` means time spent in the currently displayed step rather
than cumulative time since the build began. This deliberately avoids carriage-return-only progress
displays that can disappear in launchers or captured logs.

## Patch selection

### Default: apply everything

With no include selector, every patch is selected:

```sh
python3 onionos_mainui_patcher.py MainUI-354-clean
```

The default includes every behavior and tradeoff, including long-title scrolling, the option that
skips ROM-cache `sync()` barriers, and the Onion-specific packed Arcade-name cache patch. The
default build therefore requires OnionOS 4.2.2 or newer. OnionOS 4.2.2 already contains the same
file-backed `libgamename.so` source used by 4.3.1-1. When using an older Onion release, the stock
Miyoo library, or another `libgamename.so`, exclude `reuse-onion-arcade-name-lookup`.

`--patch-all` remains available as an explicit equivalent:

```sh
python3 onionos_mainui_patcher.py --patch-all MainUI-354-clean
```

> [!WARNING]
> Only the complete all-patches build is normally tested as an integrated configuration. Every
> include-only or excluded combination is experimental. After a successful partial build, the CLI
> prints this warning as its final output.

### Embedded-core offset safeguards

Embedded helper symbols are flat `.text` offsets, not raw ELF VMAs. The linker discards
`.ARM.exidx*` and `.ARM.extab*`, `.text` starts at zero, and the patcher validates real ARM entry
instructions before publishing output.

### Exclude unwanted patches

Use `--exclude` once per patch or pass comma-separated names:

```sh
python3 onionos_mainui_patcher.py \
  --exclude patch-rom-list-title-scroll \
  --exclude skip-rom-database-refresh-sync \
  MainUI-354-clean
```

Equivalent comma-separated form:

```sh
python3 onionos_mainui_patcher.py \
  --exclude patch-rom-list-title-scroll,skip-rom-database-refresh-sync \
  MainUI-354-clean
```

Patch names are listed in the Available patches table below. Exclusions always win, including when
combined with `--patch-all` or `--include`.

### Include only selected patches

Using any `--include` value switches to an include-only build:

```sh
python3 onionos_mainui_patcher.py \
  --include patch-rom-list-rows,patch-rom-list-font-size \
  MainUI-354-clean
```

`--exclude` is applied after the include set is built.

### Dependency behavior

`improve-game-details` requires `fix-game-detail-title-on-open`.

- Including the improved-detail patch automatically includes the title fix.
- Explicitly excluding the title fix also excludes the improved-detail patch.

`reuse-onion-arcade-name-lookup` requires `use-rom-database-display-names`.

- Including the packed Arcade rebuild-name cache automatically includes the browse-time DB-title
  bypass.
- Explicitly excluding `use-rom-database-display-names` also excludes the packed Arcade cache. This
  prevents the cache optimization from removing the rebuild-time `GetGameName` warm-up only to defer
  the same cold initialization cost until the first Arcade browse.

If exclusions remove every patch, the script reports that nothing remains and creates no output.

## Output behavior

By default, patched output is written as:

```text
INPUT.patched
```

For example:

```text
MainUI-354-clean.patched
```

Using `--inplace` replaces the input and creates this backup first:

```text
INPUT.pre-mainui-patches.bak
```

The backup is created only if it does not already exist. Keep a separate untouched backup as well.

Each individual output and newly created in-place backup is first written to a checked temporary
file in the same directory, flushed with `fsync()`, assigned the source mode, and published with
`os.replace()`. This prevents a failed or interrupted final write from exposing a partially written
MainUI at the destination path. Directory metadata is also flushed when the host filesystem supports
it. This does not provide transactional rollback across multiple input files.

The script prints the selection mode, exclusions, selected patches, and each applied change.
Successful runs finish with an explicit `[SUCCESS]` summary and generated output paths. `--inplace`
is described directly in `--help`, including its backup behavior.

## Quick examples

Apply all patches, which is the normal and recommended starting point:

```sh
python3 onionos_mainui_patcher.py MainUI-354-clean
```

Include only a few selected patches:

```sh
python3 onionos_mainui_patcher.py \
  --include patch-rom-list-rows,patch-rom-list-font-size,optimize-recent-list-loading \
  MainUI-354-clean
```

Apply all patches except a few selected patches:

```sh
python3 onionos_mainui_patcher.py \
  --exclude patch-rom-list-title-scroll,skip-rom-database-refresh-sync \
  MainUI-354-clean
```

## External OnionOS source work included

The updated tweaks and themeSwitcher binaries and their respective source diffs are included in this 
project. View the contents of `/tools/src/` and `/tools/build/` in this project.

## External OnionOS source work not included

For a folder-aware Favorite repair/sort/organize tool, the manual source work belongs in:

```text
Onion/static/build/App/romscripts/favorites/favfix.sh
Onion/static/build/App/romscripts/favorites/favsort.sh
Onion/static/build/App/romscripts/favorites/favsort2.sh
Onion/static/build/App/romscripts/favorites/favfolders.sh
SearchFilter/src/common/GameJsonEntry.hpp
SearchFilter/src/tools/main.cpp
SearchFilter/src/tools/tools.hpp
```

Those projects must be applied and built independently; they are documentation references only and
are not copied into this package.

## Configurable main menu and SELECT shortcuts

Patch name:

```text
patch-main-menu-layout
```

The patch replaces only the type-0 MainUI main-menu builder and its SELECT popup. It reads
`/mnt/SDCARD/.tmp_update/config/main-menu.json` once per MainUI process. MainUI never creates,
rewrites, repairs, reorders, or otherwise modifies this file. Restart MainUI to reload changes.

### File format

The root object accepts five properties:

```text
menu      object controlling visible main-menu sections
settings  object or array controlling Settings entries and their order
context   object or array controlling SELECT-menu entries and their order
custom    object defining custom1, custom2 and custom3
hotkey    boolean enabling the L1+R1+L2+R2 temporary reveal; default true
```

For `menu` and object-form `context`, only the JSON literal `true` enables an entry. `false`, a
missing key, a string, a number, or null is treated as false. Object-form `settings` is intentionally
different: it starts from the current Settings defaults, so an omitted Settings key remains enabled,
literal `false` hides that row, and literal `true` keeps it enabled. The one exception at root level is
`hotkey`: it defaults to true when omitted. When present, only the literal `true` keeps the chord
enabled; `false`, a string, a number, or null disables it.

Unknown keys and unknown custom properties are ignored. The bounded parser also tolerates `//`, `/*
... */`, and `#` comments, trailing commas, and ordinary JSON escapes. The maximum file size is 128
KiB and the maximum nesting depth is 16.

A complete editable example is:

```json
{
  "hotkey": true,
  "menu": {
    "favorites": true,
    "recents": false,
    "games": true,
    "apps": true,
    "settings": false,
    "expert": false
  },
  "settings": {
    "shutdown": true,
    "brightness": true,
    "wifi": true,
    "display": true,
    "themes": true,
    "tweaks": true,
    "language": true,
    "sound": true,
    "sleep": true,
    "about": true
  },
  "context": {
    "refresh": true,
    "games": true,
    "settings": true,
    "tweaks": true,
    "themes": false,
    "shutdown": false,
    "search": false,
    "custom1": false,
    "custom2": true,
    "custom3": false
  },
  "custom": {
    "custom1": {
      "label": "Do this",
      "launch": "/mnt/SDCARD/Some_Custom_App_Or_Script/script.sh",
      "type": 3
    },
    "custom2": {
      "label": "Do something else",
      "launch": "/mnt/SDCARD/Some_Other_Custom_App_Or_Script/script.sh",
      "type": 3
    }
  }
}
```

If the file is missing, unreadable, oversized, or malformed, the compatibility defaults are:

```text
Main menu:    Favorite, Games, Apps, Settings
Settings:     Shutdown, Brightness, WIFI, Display, Themes, Tweaks, Change language,
              Menu sound, Sleep timer, About device (subject to the availability gates below)
SELECT menu:  Refresh all roms
              Search  (only when /mnt/SDCARD/App/Search/launch.sh exists)
              Tweaks  (only when /mnt/SDCARD/App/Tweaks/launch.sh exists)
Hotkey:       enabled
```

### Main-menu sections

Recognized `menu` keys are:

```text
recents  favorites  games  apps  settings  expert
```

Recognized object-key order is the visible main-menu order. For example, this object displays Games
before Favorite and Apps:

```json
{
  "menu": {
    "games": true,
    "favorites": true,
    "apps": true
  }
}
```

A duplicate alias keeps the first position in the object and updates that section's final true/false
state. A valid JSON file is not allowed to produce zero visible top-level sections: when `menu` is
missing, empty, or resolves to all false values, only the main-menu portion falls back to Favorite,
Games, Apps, and Settings. The parsed context menu, custom actions, and hotkey setting are retained.
Legacy `.showRecents` and `.showExpert` markers are applied after that fallback only when the
corresponding JSON key was omitted. This prevents an empty menu definition from collapsing to only
Recent or Expert because one of those marker files exists, while an explicit `true` or `false`
value remains authoritative.

One to four visible sections are sized using their actual count. Five or six sections retain the
stock four-column paging width. Left/right wrapping therefore operates only across real rows; hidden
slots are not treated as a second page.

When MainUI is restarted after an external tool changes the visible top-level section count, a stale
first-page end index from the previous layout can otherwise hide newly enabled sections until the
selection enters one of them. The patch repairs that shortened visible range immediately before the
replacement top-level menu draws. The repair is gated by the exact replacement main-menu window
pointer; stock `MainMenuWindow` users such as the two-row Games/system grid retain their original
page geometry and draw path.

Only the six top-level main-menu row labels are eligible for this wrapping. Their exact
`std::string` text pointers are recorded when the replacement main menu is built. The two shared
MainUI draw call sites are still intercepted, but every text pointer that is not one of those six
recorded labels is passed to the stock renderer with its original arguments. Systems, game-console
menus, Apps, Settings, dialogs, and every other menu therefore retain stock label drawing.

Each tracked main-menu label is measured at draw time with MainUI's active menu font. The title area
is centered inside at most 156 pixels of the button, with a further 10-pixel safety inset on both
sides, so wrapping and drawing use at most 136 pixels. This leaves room for borders or other
decoration embedded in theme artwork. Text is wrapped to at most two centered lines. The wrapper
prefers a space that keeps both lines inside the safe width; for a single long word it falls back to
a UTF-8 code-point boundary and does not insert hyphens.

For two-line labels, the patch measures both rendered line heights and places the second line
immediately after the first line's complete rendered height. No additional artificial line gap is
inserted. Neither line is compressed to half of MainUI's original one-line rectangle, and the
complete two-line block is centered around the original label midpoint. This keeps spacing dependent
on the active font while avoiding both overlap and a double-spaced appearance.

The patch does not attempt to change MainUI's stock boot-direct return selection. On some
installations, booting directly into Favorite or another section and then pressing B can briefly
show the first main-menu row before MainUI restores another selection. This is a cosmetic stock
behavior and is intentionally left for a separate future investigation.

After JSON parsing, readable zero-byte OnionOS markers remain compatibility fallbacks:

```text
/mnt/SDCARD/.tmp_update/config/.showRecents
/mnt/SDCARD/.tmp_update/config/.showExpert
```

A marker enables its section only when the corresponding `menu` key was omitted. An explicit
`"recents": false` or `"expert": false` therefore hides that section even when an old marker remains
on the card; explicit `true` also remains authoritative. Missing, unreadable, oversized, or malformed
JSON-and valid JSON that omits either key-continues to honor the matching marker. A marker-enabled
omitted section is appended after the last recognized key when the declared menu remains active. Missing,
malformed, empty, and all-false fallbacks retain stock order. MainUI never deletes either marker,
preserving compatibility with unpatched MainUI and
other OnionOS components that still consume them.

The selector also performs one startup compatibility check before MainUI can read Recents. If
`/mnt/SDCARD/Roms/recentlist.json` is absent while `/mnt/SDCARD/Roms/recentlist-hidden.json` exists,
the hidden file is renamed back to `recentlist.json` in the same directory. If the normal file
already exists, the hidden file is left untouched. MainUI never merges or rewrites either file as
part of this check. This behavior is part of `patch-main-menu-layout`; it is not a separate
selector.

The supported 354 clean and Expert binaries differ at only two instructions: the Expert variant
calls the ordinary row constructor and row-adder for the Expert button, while the clean variant has
NOPs at those two call sites. The section implementation and Expert system vector are otherwise
identical. When `patch-main-menu-layout` is active, those call sites are normalized and the
replacement builder supplies the Expert row itself, producing byte-identical patched output from
both supported inputs. An Expert page that is still empty therefore indicates that the SD card does
not currently supply Expert system records, not that another Expert implementation is NOPed out in
the clean binary.

This JSON control could eventually remove the need for separate clean and Expert MainUI binaries: a
future OnionOS release could enable or disable the Expert section through `main-menu.json` instead.
For current OnionOS releases, keep supporting both binary variants and the existing Tweaks marker
behavior.

### Settings entries

Recognized `settings` keys are:

```text
shutdown  brightness  wifi  display  themes  tweaks  language  sound  sleep  about
```

Object form is a default-on override/reorder map. Recognized keys mentioned in the object keep their
object order; omitted Settings rows remain enabled and are appended afterward in the normal Settings
order. Literal `false` hides a mentioned row and literal `true` keeps it enabled. This means, for
example, that `{"themes": false, "tweaks": false}` hides only Themes and Tweaks while leaving the
other normal Settings rows intact. A duplicate key keeps its first position and updates that row's
final enabled state. As with `context`, a string array is also accepted as an explicit allowlist when
only selected enabled rows and their order are wanted:

```json
{
  "settings": ["display", "themes", "tweaks", "brightness", "wifi", "sleep", "about"]
}
```

When the root object has no `settings` property, the patch reproduces the current patched Settings
order: **Shutdown**, **Brightness**, **WIFI**, **Display**, **Themes**, **Tweaks**, **Change language**,
**Menu sound**, **Sleep timer**, and **About device**. An empty object therefore produces those same
defaults. Array form remains an explicit allowlist, but Settings is never allowed to resolve to zero
usable rows: an empty array, an all-false object, or a selection containing only unavailable rows
falls back to the safe current defaults instead of constructing a zero-row Settings menu.

Visibility remains bounded by the capabilities MainUI actually has. `"wifi": true` does not create
a Wi-Fi row in a MainUI binary whose stock Settings builder does not support that row. `language` is
shown only when MainUI reports more than one available language. `themes` and `tweaks` are shown
only when `/mnt/SDCARD/App/ThemeSwitcher/launch.sh` and `/mnt/SDCARD/App/Tweaks/launch.sh` respectively
exist. The other recognized rows reuse their stock Settings targets and handlers.

The `sleep` Settings key controls MainUI's existing **Sleep timer** row; it is not an immediate
standby command.

### SELECT-menu entries

Recognized `context` keys are:

```text
refresh  search  recents  favorites  games  apps  settings  expert  themes  tweaks
shutdown  custom1  custom2  custom3
```

Object key order is the popup order. Recognized false entries stay registered but hidden. Duplicate
keys keep their first position and update that entry's true/false state. When a valid file contains
a `context` property, omitted keys are not invented: `search` is therefore off unless explicitly
enabled. When the root object has no `context` property at all, it uses the same launcher-aware fallback as
a missing file: **Refresh all roms**, plus **Search** and/or **Tweaks** only when the corresponding
external launcher exists. An explicitly present empty object or array remains empty.

As a permissive alternative, `context` may be an array of names. Array entries are enabled and shown
in array order:

```json
{
  "context": ["refresh", "themes", "tweaks"]
}
```

`recent` and the Favorite spellings `favorite`, `favourite`, `favorites`, `favourites`, and `favs`
are accepted as aliases. At most fourteen recognized context entries exist.

`shutdown` opens MainUI's stock Shutdown confirmation dialog directly. There is no immediate
Sleep/standby context action: Onion's hardware POWER-button suspend lifecycle is owned outside
MainUI, so the patch does not attempt to reproduce it. The separate `settings.sleep` key still
controls MainUI's ordinary **Sleep timer** Settings row.

When root `hotkey` is true or omitted, pressing L1+R1+L2+R2 together while the main menu is active
reveals every registered false context entry, immediately opens the popup, and keeps those entries
visible until that MainUI process exits. Set `"hotkey": false` at the root to disable the chord and
leave the four shoulder buttons on their stock input path.

The patch hooks the shared `PopupWindow` image-load call, so numbered fallback applies to ordinary
game-list, Favorite, Recent, folder, and replacement main-menu popups whenever they request a
numbered popup background.

The patch recognizes:

```text
skin/bg-pop-menu-1.png
...
skin/bg-pop-menu-6.png
```

Theme creators can provide any or all of these files. Every numbered asset should use identical
overall width, transparency treatment, trailing decoration, and horizontal padding; only the number
of 60-pixel item rows should vary. The supplied theme assets are 70, 130, and 190 pixels high for
one, two, and three rows: `60 × rows` plus a ten-pixel bottom decoration. Equivalent four-, five-,
and six-row assets are therefore 250, 310, and 370 pixels high.

The popup viewport is capped at six visible rows. For a request for `bg-pop-menu-N.png`, the shared
loader first tries the exact active-theme asset. If it is absent, it searches downward and uses the
highest readable smaller numbered asset. Fallback synthesis copies the source pixels directly:
complete 60-pixel row bodies are tiled in order and the trailing decoration is moved to the final
bottom edge. Direct copying preserves the source alpha channel; it avoids the SDL 1.2 behavior where
alpha-blitting onto a newly transparent surface can leave the expanded artwork invisible. Popup
width is never changed. The guarded `PopupWindow` divisor hook also caps layout at six rows, so
entries beyond row six stay in the menu and scroll normally instead of compressing every row.

The replacement main-menu context popup passes MainUI's exact original path objects at `0x1530E8`,
`0x1530A0`, `0x1530B8`, and `0x1530D0` for rows one through four. Newly embedded strings with
identical text do not receive the same stock theme-resolution behavior and can leave `PopupWindow`
on its narrow 120-pixel fallback rectangle. The four stock path strings and the shared loader call
are provenance-guarded before output. Five- and six-row paths remain private additions because stock
MainUI has no originals for them.

This means a three-item popup uses the active theme's `bg-pop-menu-3.png` when present, and a
four-item popup can use `bg-pop-menu-3.png` when `bg-pop-menu-4.png` is missing. No theme switch or
setting is required after installing the patched binary; restart MainUI so the new binary is loaded.

### Built-in and custom actions

The optional `search` entry uses the same verified stock AppAction handoff as the other built-in
applications. Its label uses translation ID 153 and its fixed launcher is the Search package
installed by Onion:

```text
search  -> /mnt/SDCARD/App/Search/launch.sh           (type 3)
Themes  -> /mnt/SDCARD/App/ThemeSwitcher/launch.sh   (type 3)
Tweaks  -> /mnt/SDCARD/App/Tweaks/launch.sh          (type 3)
```

The Search package must be installed for the external shortcut to work. These built-in launchers are
hard coded and do not use the `custom` object. Implicit fallback entries are launcher-aware, so a
missing Search or Tweaks package does not create a dead default row. Explicit JSON remains
authoritative.

### External Themes and Tweaks in Settings

`patch-main-menu-layout` also restores two stock-hidden Settings rows. The original hidden Themes row
keeps its stock label/icon and launches `/mnt/SDCARD/App/ThemeSwitcher/launch.sh`. The adjacent
hidden Fixes row is shown as **Tweaks** and launches `/mnt/SDCARD/App/Tweaks/launch.sh`. Each row is
omitted when its external launcher is unavailable.

Theme authors should provide `skin/icon-theme.png` for Themes and `skin/fixit.png` for Tweaks.

The rows keep their original stock Settings destination IDs and are redirected at the real Settings
dispatch handlers. The AppAction result is propagated back through the Settings input path so the
external application starts immediately when A is pressed. This avoids the deferred-launch behavior
of earlier experimental implementations. This main-menu selector does not itself alter the stock
`/mnt/SDCARD/Themes` startup scan. The separate `skip-inactive-theme-configs` selector can retain
that scan loop while filtering out inactive themes before their config files are touched.

The restored rows are inserted immediately after **Display**, so the visible order is Display,
Themes, Tweaks, followed by the remaining stock Settings rows. Their original hidden construction
sites remain untouched. There is no separate cross-restart return marker for these Settings
launchers. When MainUI serializes window state before the external handoff, only a Settings record
with stock type `7` and string-title value `-1` is normalized to stock numeric Settings title ID
`15`. Other window types and titles are serialized unchanged. This preserves the **Settings** header
when MainUI is restored after Themes or Tweaks exits without introducing a persistent marker.

The same selector also hardens MainUI's generic `/tmp/state.json` boundary for transient windows.
Popup and confirmation windows use negative type values, but MainUI's generic restore path cannot
reconstruct those records as the original transient class. A negative-type live window is therefore
skipped when state is serialized, and any stale negative-type record already present in the state
file is ignored during restore. Normal non-negative section windows are unchanged. This prevents a
transient SELECT/context popup from being reconstructed after an external application handoff as
an empty ordinary page with the popup's title.

A custom entry is usable only when both `label` and `launch` are non-empty strings. Labels are
copied literally and are not translated. The bounded limits are 127 UTF-8 bytes for a label and 511
UTF-8 bytes for a launch path. Missing or non-integer `type` defaults to 3. Built-in and custom
launch rows use a real stock AppAction object, the verified 128-byte stock-compatible app
configuration, and MainUI's normal AppAction execute path.

Section shortcuts use MainUI's stock `CallbackAction` class-the same callback ABI used by **Refresh
all roms**-instead of a custom action vtable. The callback stores the requested stock section and
returns popup success `1`, allowing the popup to close through its native path. The patch posts one
harmless MainUI wake event. The first controller event immediately after popup release reproduces
the exact branch used by these configurable top-level rows at `0x27878..0x2792C`: allocate a
284-byte section window, call the string-title constructor at `0x2E978` with the translated title,
section target and zero row argument, invoke activation vtable slot 5, push the window through the
stock stack helper, and return zero. This keeps the safe close-before-push ordering without waiting
for another user input or periodic UI event. Hidden destinations remain supported. Themes, Tweaks,
and custom scripts continue to use the normal application handoff.

`search` uses stock translation ID 153. The six section shortcuts use the same stock IDs as the main
menu: `0` Expert, `1` Favorites, `2` Games, `15` Settings, `18` Recents, and `107` Apps. For
built-in SELECT-menu entries only, a missing value, an empty value, or exactly one ASCII space falls
back to the embedded English label. This is intentionally separate from the top-level main-menu
labels, where a single-space translation can still be used by themes as a label-hiding tweak. Themes
uses stock translation ID 125.

**Tweaks requires private translation ID 407** in every translated `.lang` JSON file:

```json
"407": "Tweaks"
```

The patch searches the normal Miyoo/Onion language directories and falls back to the embedded
English label when the private ID is missing, empty, or exactly one space. MainUI's stock fixed
translation table is not extended.

## Configurable ROM-list row count

Patch name:

```text
patch-rom-list-rows
```

Adds configurable visible rows to ROM, Favorites, and Recent lists.

Runtime file:

```text
/.tmp_update/config/.romListRows
```

Example for ten rows:

```sh
10
```

Rules:

```text
missing, unreadable, or invalid -> 6 rows
minimum                       -> 6 rows
maximum                       -> 20 rows
row height                    -> floor(360 / rows)
```

For ROM, Favorites, and Recent game lists, the same selector also makes the active theme row-count
aware. For an effective row count `N`, MainUI first looks for:

```text
skin/bg-list-s_N.png
```

inside the **current active theme**. For example, ten rows use `skin/bg-list-s_10.png`. If that
numbered file is missing, unreadable, or cannot be decoded as an image, MainUI falls back to the
normal stock `skin/bg-list-s.png` load. The fallback therefore keeps MainUI's existing
current-theme/default-skin behavior.

Only the ROM/Favorites/Recent constructor paths are tagged for this lookup. Other screens and menus
that use `skin/bg-list-s.png` continue to use that exact stock filename and loader behavior. The
numbered asset is never substituted globally.

The patch also adjusts:

- initial and refreshed list loading windows;
- forward and backward loading;
- restored list focus;
- selection-bar source cropping;
- vertical clipping for oversized folder, game, and favorite icons;
- shoulder-button page movement;
- the established full-window top request for the effective row count;
- a bounded index-zero and restored-window readiness wait so the first draw normally receives
  populated labels;
- loading-completion repaint scheduling so newly loaded labels do not remain visually stuck as the
  `LOADING` placeholder.

Stock MainUI stores `pos`, `start`, and `end` independently in `/appconfigs/romwinidx.json`; a
record written with one visible-row count can therefore describe an incompatible window when later
consumed with another. The same persisted source-console state can also be presented to a newly
created contextual Search result list whose total is smaller. Stock `TextMenu::restoreState`
validates `pos` independently from the `start`/`end` pair, so it can accept a deep selected index
while rejecting the viewport around it. This can leave an exact `DBCachedTextMenu` with a selection
outside the first worker window and produce `LOADING` rows even when the SQL data itself is valid.

Before the stock restore/load path, the row patch now normalizes only an **exact
`DBCachedTextMenu`**. When its published total is positive, positive saved values are bounded to the
current final index, stale spans are re-anchored around the selected position, and a fully
non-negative descending pair (`start > end`) is repaired before MainUI computes
`end - start + 1` for the worker request. Negative values retain stock sentinel semantics, and a
non-positive/unpublished total leaves the saved triple untouched. The stock restore virtual,
asynchronous worker, and item accessor remain stock-owned.

The repair is deliberately validity-focused rather than presentation-maximizing. It guarantees a
bounded, non-descending requested range for the protected DBCached path, but it does **not** always
shift a short valid range upward to fill the bottom of the screen. For example, with 50 total rows
and 10 visible rows, a valid repaired `start=49, end=49` may leave blank lines below the last item
even though a full final page could use `start=40, end=49`. A future refinement could additionally
cap `start` at `max(0, total - rows_per_page)` and recompute the range while preserving the selected
row. That bottom-fill normalization is not applied in this release.

The patch does not delete `/appconfigs/romwinidx.json` automatically.

### Persisting the current ROM-list position

MainUI's stock `romwinidx.json` writer can otherwise serialize stale navigation geometry because the
live `DBCachedTextMenu` position is not necessarily published to the in-memory `romwinidx` model on
every Up/Down movement, and Onion's external power handling can terminate MainUI before its normal
late exit-save path runs.

The row patch therefore persists navigation at **list lifecycle boundaries**, not on each navigation
key press:

- on a normal real ROM-list exit, MainUI's existing `MenuWindow` snapshot first publishes the live
  `pos`, `start`, and `end`; the patch then serializes the stock in-memory `romwinidx` vector;
- immediately before the normal game/app handoff save, the patch asks the current top real ROM list
  to perform that same stock snapshot in snapshot-only mode, then leaves the existing stock writer
  in control;
- on the first MainUI power-down event, while MainUI is still alive, the patch snapshot-only
  captures the current real ROM list, serializes `romwinidx.json`, and calls the already imported
  `sync()` before continuing through MainUI's stock power-event handling.

There is **no additional work on Up/Down/L/R navigation** and no SD-card write per movement. The
persistence helpers require the expected `MenuWindow`, mode 5 or 17, and an exact
`DBCachedTextMenu`. Contextual Search (translation/title ID 153) is deliberately excluded so Search
result geometry cannot overwrite the source console's saved ROM-list position. If the first power
event occurs while no qualifying real ROM list is the active top window, the extra power-save helper
does not write anything; positions already serialized by a prior real list exit remain available.


### Row-count-aware horizontal padding

Outer padding remains at MainUI's stock 20 pixels through nine rows. The post-icon gap starts
reducing at eight rows; outer padding starts reducing at ten rows:

```text
rows        outer row padding    post-icon gap
6..7        stock 20 px          stock 15 px
8           stock 20 px          14 px
9           stock 20 px          13 px
10          19 px                12 px
11          18 px                11 px
12          17 px                10 px
13          16 px                 9 px
14          15 px                 8 px
15          15 px                 7 px
16          15 px                 6 px
17..20      15 px                 5 px
```

The icon destination and text start remain row-kind-aware, but every tagged game-list title uses one
absolute right edge derived from the actual destination SDL surface width minus the outer row padding.
Folders, normal games, wide-spacer games, and rows without an icon therefore share the same right
boundary even though their left X positions differ. At 14 through 20 rows on the normal 640-pixel
destination surface, for example, that boundary is `x=625` and leaves exactly 15 pixels on the right.

MainUI's final stock title blit uses a null source rectangle and does not treat the caller's destination
rectangle width as a hard clip. The row patch therefore temporarily intersects the actual destination
surface clip with this shared right edge around the complete stock title renderer, then restores the
original clip immediately afterward. The selected-title marquee uses the same right edge; an active
Favorite-marker boundary may narrow it further. Static and scrolling titles therefore reserve the same
row-count-aware right padding instead of ending at different pixel boundaries.

The dense-row game-title-width hook also fails closed when MainUI temporarily has no current
`TextItem`: it uses the normal row-height icon slot without dereferencing an item or surface. This
keeps the custom-row path safe during transient/empty draw states as well as ordinary populated rows.

The established active/previous game-list identity guards keep unrelated `TextMenu` screens stock.
The row-patch state records the currently tagged game-list pointer at `state+0x40` and the previously
tagged game-list pointer at `state+0x44`. When a different tagged list becomes current, the old active
pointer can move to the previous slot; this primarily protects transition lifecycles such as contextual
Search, where the source ROM list may temporarily remain the remembered previous object before it is
promoted back to active.

The static shared-edge clip and both selected-title marquee stages use the same active-or-previous
identity rule. The row-title marker that arms scrolling and the low-level scrolling blit each accept a
TextMenu matching either `state+0x40` or `state+0x44`. This matters when returning from contextual
Search: hardware testing confirmed that MainUI can expose the source console list again while it is
still the remembered previous object. The draw path only recognizes that object; it does not promote
`state+0x44` back to active or otherwise mutate the Search lifecycle while rendering.

> [!NOTE]
> **Maintainer review note:** the shared-edge clip reads the tagged row's destination `SDL_Surface`
> width for every accepted row without first testing that surface pointer for NULL. This moves the first
> `r5` dereference earlier than older baselines, where a comparable dereference was reached mainly through
> later Favorite-marker handling. Stock MainUI's row renderer already stores and ultimately blits to that
> same destination surface without a survivable NULL path, so a NULL destination was already fatal; the
> patch changes where such an invalid state would fail, not whether it can be rendered safely. No NULL
> destination has been observed on tested hardware.

### Theme-configurable game-list icon left margin

The same `patch-rom-list-rows` selector reads an optional numeric `iconLeftMargin` member from the
active theme's top-level `gamelist` object:

```json
{
  "gamelist": {
    "iconLeftMargin": 12
  }
}
```

Rules:

```text
missing setting          -> established row-count-aware geometry
negative/non-number      -> established row-count-aware geometry
0..300                   -> explicit absolute theme-author margin
above 300                -> clamp to 300
```

The setting is valid even with six rows. One value controls the aligned folder/game icon layout;
MainUI preserves the renderer-specific compensation needed for those two stock paths to remain
visually aligned. The title start and available title width are adjusted with the icon layout, so a
theme author does not need a second text-margin setting.

This is particularly useful for themes with decorative vertical borders. A value of zero is valid
and allows the layout to reach the left edge; a border-heavy theme can instead reserve its desired
left-side artwork width explicitly. When the setting is absent, the established default geometry is
used unchanged.

Patch-added theme settings are isolated by exact theme path. During MainUI's installed-theme scan,
`gamelist.bold` and `gamelist.iconLeftMargin` are staged only in per-theme records and cannot update
live renderer state. At active-theme initialization, live patched theme state is reset to defaults
and populated only from an exact selected-theme path match. A value in another installed theme
therefore cannot spill into the active theme.

### Initial ROM-list readiness

Ordinary ROM-folder setup retains the established full-window request, while the redundant stock
index-zero preload block remains bypassed. Persisted-window normalization runs before the stock
restore call so the requested visible range matches the current row geometry without replacing the
worker or draw lifecycle.

For both the index-zero top window and restored deeper positions, the ROM-list load wrapper gives
the worker up to 180 ms to populate the complete visible window. It checks readiness through
MainUI's stock item accessor and yields in 2 ms intervals. A non-null row whose primary label is
still exactly `LOADING` is treated as not ready, matching the established L2/R2 page-wait rule. The
wrapper returns immediately when all visible rows are resident.

The wait is strictly bounded. The L2/R2 page-wait wrapper also rejects a negative first-visible
index before calling the stock item accessor. If the visible window is still incomplete after
180 ms, MainUI continues with its stock progressive TextMenu drawing and the existing immediate
and bounded completion repaints. The draw vtable remains stock; there is no paint gate and no path
that intentionally leaves the list body blank behind the preview image.

Normal non-wrap Up/Down, L1/R1, L2/R2, Search, Favorites, Recent, and the loading-progress display
keep their existing paths. The ordinary ROM-list initial load vtable slot is wrapped, and exact
one-row edge wraps use the separate bounded destination-readiness path described below.

### One-row edge-wrap readiness

Stock one-row ROM navigation changes the selected index and starts the asynchronous forward/backward
window loader before publishing the new viewport `start`/`end` fields. On an opposite-edge wrap this
can briefly draw `LOADING` rows even though the worker fills them immediately afterward, especially
in very large database-backed systems or database layouts whose opposite edge is not resident.

The row patch therefore intercepts only the verified one-row loader calls for exact wraps:

```text
Down from final row -> first row: check destination 0 .. rows-1
Up from first row   -> final row: check destination max(0,total-rows) .. total-1
```

The stock destination worker is started first. The wrapper then derives the destination directly
from the new selected index, published total, and effective row count; it deliberately does not use
the still-stale viewport `start`/`end` fields. Readiness is checked through MainUI's stock item
accessor, with null items and exact `LOADING` labels treated as pending. The wrapper yields in 1 ms
intervals for at most 60 ms and returns immediately when the complete destination window is ready.
An empty or unpublished total fails open to the original loader path.

> [!NOTE]
> **Maintainer review note:** one non-blocking hardening opportunity remains documented for this
> edge-wrap helper. The Down-wrap gate currently rejects `total == 0`, while the published total is
> treated as a signed field elsewhere in stock MainUI. For every normal `total >= 1` value the existing
> unsigned destination clamp is equivalent to the intended signed result, but a hypothetical negative
> total could bypass that clamp and make the gate inspect `0 .. rows-1`. Static analysis does not
> establish that this stock field can never become negative; no negative value has been observed on
> tested hardware. A future cleanup can reject `total <= 0` or use an explicitly signed clamp.

Every non-wrap one-row Up/Down movement remains on the original loader path with no added wait. The
60 ms bound is only a presentation barrier; it does not change the SQL query, database index policy,
worker implementation, or completion-repaint machinery. In addition to pinning the one-row `BL`
destinations, the patcher now verifies the resolved forward loader at `0x241A0` and backward loader at
`0x242DC` directly, including their stock callee-saved `r4` contract and direction-specific worker
setup. A changed loader body therefore fails closed before output is published.

Shoulder buttons move by the configured number of rows in game lists. Shared non-game menus keep
their stock behavior.

The Language selector is deliberately excluded. It keeps its stock six visible rows, 60-pixel row
height, selection geometry, and shoulder-button behavior regardless of the ROM-list row setting.

### Theme icon note

Oversized row icons are vertically center-cropped to the active row height; they are not scaled by
MainUI. The clipping covers the normal folder/item icon, `skin/icon-game.png`, and
`skin/ic-favorite-mark.png`. Favorite and Recent game rows receive the same active-theme
`skin/icon-game.png` surface as ordinary SQLite-backed ROM rows, so all three list types share the
same theme-author geometry.

A normal `icon-game.png` keeps the established row-count-aware icon slot. A deliberate horizontal
layout/spacer asset is recognized only when **both** of these conditions are true:

```text
width >= 120 px
width >= 3 * height
```

Qualifying spacer assets keep their complete horizontal width; only vertical excess is
center-cropped when necessary. That width participates in the game-title start and available-text
width calculations. Favorite rows automatically move `ic-favorite-mark.png` to the far-right marker
placement for a wide spacer, and Favorite-title clipping uses the same rule, avoiding the fixed
marker lane that would otherwise sit inside the spacer layout. The marquee treats a preview boundary
as an obstruction only when that boundary is physically to the right of the selected title start.

Baseline regression safety for this stock-six path is automated rather than dependent on manually
installing a wide-spacer theme for every release. The patcher pins the generated title-scroll helper
so it must read the icon surface height before reusing the surface-pointer register for width, and it
host-sweeps the centered square-crop arithmetic for all supported row counts, including the 61-70 px
stock-six edge range. New spacer/layout behavior still requires device testing when its semantics are
changed; unchanged baselines do not require repeating a special wide-spacer asset test solely to catch
these two known failure classes.

Companion theme-resize tooling should apply the same spacer test and leave qualifying
`icon-game.png` assets unchanged. Ordinary icons continue through the normal row-count resize path.
Backups of altered theme files remain recommended.

### First-presentation preview geometry and Favorite-marker lane

The containing game-list window already decides whether the selected row reserves MainUI's
250-pixel preview pane. When a preview is active, the child TextMenu uses an x-origin of `250`; when
the selected game has no preview, the child list uses x-origin `0` and can use the full width.

Stock MainUI computes that decision before drawing the child list but stores the resulting x-origin
only **after** the child draw. The row patch mirrors MainUI's exact existing preview/no-preview
decision immediately before the child draw and leaves the original post-draw assignment unchanged.
This keeps the first visible frame in sync with later frames.

Favorite-marker placement then has two runtime modes for normal game icons. By default the marker
uses the same preview-safe lane regardless of whether the selected game currently has a thumbnail.
The marker formula remains stock except that its x-origin term is fixed at `250`, so the marker's
right edge is constant and its left edge automatically follows the decoded `ic-favorite-mark.png`
width. A qualifying wide `icon-game.png` spacer automatically overrides that fixed lane and uses the
far-right placement instead; Favorite-title clipping follows the same wide-spacer decision.

To restore the original dynamic preview/no-preview marker position, create this presence-only file:

```text
/.tmp_update/config/.romListDynamicFavPos
```

The file contents are ignored. Restart MainUI after adding or removing it. With the file present,
Favorite markers follow the live child-list x-origin exactly as before: preview rows use the
preview-safe position and no-preview rows use the far-right full-width position.

All tagged game-list rows are clipped around the **complete stock row-title renderer** at the shared
row-count-aware right edge described above. MainUI's renderer can emit up to three text blits for one
title, so the patch does not try to patch each blit or enlarge the caller's rectangle. Instead it saves
the clip rectangle from the exact destination `SDL_Surface*` already passed to the row renderer,
intersects only its right edge with `surface_width - outer_padding`, runs the complete stock title
renderer, and restores the exact original clip immediately afterward. In fixed Favorite-marker mode,
that common bound can be narrowed further to 6 pixels before the actual marker left edge.
`SDL_GetClipRect` and `SDL_SetClipRect` are resolved once at runtime through MainUI's existing `dlsym`
import. If either symbol is unavailable, the call fails open to stock rendering.

The two temporary `SDL_Rect` objects remain four 16-bit fields; no `SDL_Surface` internals, glyph
surfaces, source/destination blit rectangles, or title strings are rewritten. The Favorite bound uses
the marker surface's decoded width, so it does not assume a particular Favorite PNG size or row count.
Dynamic marker compatibility mode changes marker placement as documented above, but the base
row-count-aware title right edge remains in force for tagged game lists.

### L2/R2 page-loading correction

MainUI changes the selected index before starting the asynchronous ROM-window load. Its stock ROM
wrappers then choose the loader from the shoulder direction:

```text
L2/page up   -> backward window
R2/page down -> forward window
```

That does not match the viewport TextMenu draws. A page-up destination above the old viewport
becomes the **first** visible row and needs the destination plus following rows. A page-down
destination below the old viewport becomes the **last** visible row and needs preceding rows plus
the destination. The stock calls can appear to work when enough rows remain cached from the previous
page, but fail at boundaries: L2 from a top-row selection can expose `LOADING`/`LADDAR`, while R2
from a bottom-row selection can appear to need a second press.

The row patch redirects only the two ROM page-wrapper loader calls:

```text
L2/page up   -> forward window
R2/page down -> backward window
```

The page-step arithmetic, selection index, L2/R2 actions, sounds, and non-ROM lists remain
unchanged. The patcher verifies both wrapper call sequences and fails closed if either shape or
target differs.

The bounded completion-repaint hardening remains: ROM row-data completion posts the stock repaint
and schedules one-shot checkpoints at 120 ms and 360 ms. Timer creation is checked; if
`SDL_AddTimer()` returns NULL, the same timer creation is retried once. A timer callback whose
`SDL_PushEvent` is rejected retries every 60 ms until accepted. These redraw completed data but do
not substitute for loading the correct destination window. No sleeps, event filtering, synthetic
navigation, or permanent polling are used.

Restart MainUI after changing the row value because it is cached for the process lifetime.

### Preview-completion handling

The patch keeps the stock immediate repaint and bounded 120 ms fallback. The additional path is guarded
by two process-local generation counters:

1. The asynchronous preview-completion callback increments `produced_generation`.
2. At the stock cover check, the shared preview window compares it with `consumed_generation`.
3. If there is a new completion and the cover is still null, MainUI's stock preview updater runs once.
4. The normal draw then reloads the cover and continues through stock cover geometry and blitting.

The hook is deliberately placed before cover dimensions are read. If stock already attached the cover,
the generation is consumed without another updater call. If the guarded call is too early to find the
cache entry, normal stock updates and the retained 120 ms repaint remain available.

The shared preview-window path is used by ROM, Recent, flat Favorite, and folder-backed Favorite
views. Physical-device testing is authoritative for perceived thumbnail timing.

---

## Parent row in ordinary ROM directories

Patch name:

```text
add-parent-folder-to-rom-lists
```

This selector is separate from Favorite folders. It replaces only the normal `ppath` list and count
formats and adds a narrow GameAction entry wrapper. A non-root directory receives one synthetic row:

The patch also corrects the successful Delete ROM recount: stock SQLite counts only physical rows,
so in a non-root directory it adds one to the count passed to the menu length setter for the
synthetic `..` row. Root counts remain unchanged.

```text
id=0, disp="..", path="", imgpath="", type=2, ppath=<current>, pinyin="", cpinyin=""
```

Stock database rows use type `1` for folders and type `0` for games, so type `2` sorts first and
uses the stock folder icon/construction branch. The execute wrapper checks `GameAction+0x58`; value
`2` returns `1`, matching one B-level close, while every other value replays the original `push
{r4-r6,fp,lr}` and continues at `0x18980`.

The normal query's `snprintf` call at `0x2378C` is redirected through a small root-aware formatter.
For root `ppath='.'`, it restores MainUI's original simple `SELECT ... WHERE ppath=... ORDER BY ...
LIMIT ...` query and original argument order. This lets the `(ppath,type DESC,disp)` browse index
serve the common root list directly instead of running the synthetic-parent `UNION ALL` query and
temporary sort even though no `..` row is needed. Runtime BINARY/NOCASE selection is preserved. Only
real subfolders use the compact parent-row query and its local table/ppath vararg swap.

Search uses separate stock queries and is unchanged. The normal COUNT formatter is root-aware too:
`ppath='.'` uses the original simple COUNT shape (or the simple two-column game/folder COUNT when
`skip-folders-in-game-detail-navigation` is active), while only real subfolders use the
synthetic-parent CTE. The non-root query adds exactly one to the physical row total and, with the
game-detail folder-skip patch, one to the folder tally. This preserves game-only ordinals and
suppresses the list counter while `..` is selected without paying the parent-CTE overhead at system
root.

## Configurable game-list font size

Patch name:

```text
patch-rom-list-font-size
```

Adds an optional font-size override for ROM, Favorites, Recent, and contextual Search-result lists.
Language, Systems, Apps, popup menus, and other unrelated menus keep the normal theme font.

Runtime file:

```text
/.tmp_update/config/.romListFontSize
```

Example:

```sh
22
```

Rules:

```text
missing, unreadable, invalid, or non-positive -> theme font size
minimum valid override                        -> 8
maximum valid override                        -> 60
```

The patch preserves the established corrected recognition order:

1. Compare the current object against the established active game-list pointer.
2. Compare it against the established previous game-list pointer and perform the established
   active/previous promotion when it matches.
3. Only after both pointer checks miss, test for exact `DBCachedTextMenu` vptr `0x153D48` and apply
   the override to contextual Search results.

Favorites and Recent therefore remain on the established pointer-tag path and are never gated by a
class test. Untagged popup context menus miss both pointer slots and are not `DBCachedTextMenu`, so
they retain the theme font. Focused font-only builds preserve the established Favorites/Recent
pointer-first behavior.

Before output, the patcher validates the complete `DBCachedTextMenu` identity chain-vtable header,
typeinfo, RTTI name `16DBCachedTextMenu`, and the sole direct constructor call at `0x2337C` to
`0x25240`. A changed identity or additional constructor owner fails closed.

Restart MainUI after changing the value because it is cached for the process lifetime.

## Contextual Search fixes

Patch name:

```text
fix-contextual-search
```

The selector is enabled by default and owns only contextual per-console Search behavior.

### Initial result position and DBCached restore safety

In the normal integrated build, where `patch-rom-list-rows` is also enabled, the exact contextual
Search `MenuWindow` is marked only during its stock activation. Before the first result worker is
requested, the exact `DBCachedTextMenu` restore helper recognizes that construction and initializes:

```text
selected = 0
start    = 0
end      = min(configured_rows, result_total) - 1
```

This makes contextual Search open on its first result instead of inheriting a deep source-console
selection. The same row-patch helper also bounds stale source `pos/start/end` values against the
current Search result total and repairs a concrete descending range before the worker sees it.
The guard requires the contextual-Search construction state, exact `MenuWindow` identity/title,
exact `DBCachedTextMenu` vtable, and a positive published total. If any condition misses, stock
restore behavior is retained. Include/exclude partial builds remain experimental; excluding
`patch-rom-list-rows` also removes this pre-worker first-page normalization.

### Confirmation-A release

After contextual `SearchAction` returns success at exact post-run site `0x19BB0`, one state flag is
armed. At central SDL decoder hook `0x1777C`, only a matching repeated Space/A keydown and the
paired Space/A keyup are consumed. The keyup clears the state; another key cancels it. The flag is
armed only after the Search keyboard closes, so keyboard OK and the shared keyboard translator are
untouched.

### Result title

Both stock contextual-title builders load the same separator. The patch changes both loads from `:`
to `: `, producing:

```text
Console: Search term
```

### Deterministic result order

Both contextual Search query variants use stable ordering:

```text
ORDER BY type DESC, disp, path
```

or, when case-insensitive sorting is selected:

```text
ORDER BY type DESC, disp COLLATE NOCASE, path COLLATE NOCASE, path
```

The same runtime marker as the general game-list sort selector is honored:

```text
/.tmp_update/config/.romListCaseSensitiveSort
```

Missing or unreadable marker means case-insensitive A–Z order. A readable marker selects
case-sensitive/BINARY order. The path fields are tie-breakers only, making equal visible names
deterministic without changing their labels.

### Match highlighting

Contextual Search keeps MainUI's optional matching-substring rendering. The three exact stock
highlight-color loads in the shared row-title helper are redirected to a small contrast selector:

- light theme list text normally implies a dark background, so MainUI's stock bright yellow is
  retained;
- dark theme list text normally implies a light background, so the match uses mid-dark purple
  `#7B2CBF`.

The heuristic uses `R + 2*G + B` from the active theme's `list.color`, with 512 as the midpoint.
Highlighting is retained while a Search result is unselected. Once selected, the row temporarily
supplies an empty match key to MainUI's stock title helper, so the complete title is rendered as one
normal selection-color surface. This removes fragmented highlighting from the active row and keeps
the normal long-title marquee path available. The highlight returns when the row is no longer
selected.

### Post-game B returns directly to the source list

The lifecycle distinction is:

- without launching a game, B from Search results keeps stock behavior and returns to the existing
  Search keyboard;
- after launching a game from those results and returning, MainUI first restores the Search results,
  and the later B from those results skips the no-longer-useful Search parent and returns directly
  to the source game list.

The patch uses a narrow same-boot marker:

```text
/tmp/mainui-context-search-postgame
```

After contextual `SearchAction` succeeds, MainUI records the exact Search parent in process-local
state and removes any stale marker. At the verified successful game-launch return site `0x1853C`, a
marker is created only while that contextual Search session is active. The marker survives MainUI's
process restart but naturally disappears on reboot.

The shared navigation-stack pop hook at `0x3CF10` clears only the process-local active Search
pointer when the exact remembered parent is removed. It intentionally does not remove the marker.

The common game-launch continuation at `0x37360` remains stock. The production helper instead hooks
the original branch at `0x37290`, immediately after MainUI has performed the first stock pop used to
close the restored results. It acts only when:

- the closed-window result is `1` or `4`, the two stock first-pop cases;
- the same-boot post-game marker exists;
- the newly exposed top object is exact `MenuWindow`; and
- its translation/title ID is 153, `Search`.

Only then is the marker consumed and exactly one additional stock vector pop performed. The object
that MainUI just removed is not dereferenced after teardown. If any condition misses, execution
continues through the untouched stock path.

This produces:

```text
game exit -> restored Search results -> B -> source console ROM list
```

Normal Search remains:

```text
Search results -> B -> Search keyboard
```

A new successful contextual Search confirmation removes any stale marker, so an abandoned old
session cannot affect a later Search.

### Search keyboard key colors

The keyboard key characters normally inherit the theme's `list.color`. That is unsafe because the
keyboard key background is dark and themes cannot independently configure its color. A dark
`list.color` can therefore make the key caps unreadable even though ordinary list text is correct
for the theme.

The patch retains the exact `ImeWindow` key-cap renderer call at `0x140AEC` unchanged. Immediately
before the existing text helper, it replaces that call's `SDL_Color` argument with `#FFFFFF`, then
tail-calls the original renderer. It does not redirect the shared list-color global and does not
change:

- ordinary ROM/Favorite/Recent/list text;
- the Search input field;
- titles or hints;
- keyboard button artwork;
- non-key-cap text draws.

The exact renderer call target and its six argument-setup instructions are fail-closed guards.

## Consistent Favorite identity for Onion Search results

Patch name:

```text
fix-search-favourite-status
```

Onion's external Search app is presented to MainUI as a synthetic console. Its generated database stores
a result path as a combined source launcher plus real ROM path:

```text
/mnt/SDCARD/Emu/SYSTEM/launch.sh:/mnt/SDCARD/Roms/SYSTEM/game.ext
```

Stock MainUI uses that synthetic representation at some boundaries but later splits it when a real
`GameAction` is constructed. That creates several inconsistent identities for the same game: a live
Search row can show a Favorite star but rebuild without it, the SELECT popup can offer **Add Favorite**
for an already-favorited game, Remove can target the wrong encoded path, and a Search-created Favorite
can retain the Search launcher instead of the source emulator identity.

This selector gives those Search-only boundaries one canonical identity. For an encoded Search result,
the real ROM suffix is used for membership checks and removal, and a newly stored Favorite keeps the
stock four-field schema while writing the source emulator launcher plus the real ROM path. No `imgpath`
field is added to `favourite.json`. Ordinary ROM rows remain unchanged. Legacy Search-shaped Favorite
records are still accepted: exact encoded membership is tried as a compatibility fallback, and when the
folder-aware Favorite core constructs such a row it resolves the source emulator and real ROM in memory
without rewriting the old record solely for migration.

With the folder-aware Favorite core enabled, Favorite `GameAction`s also receive a usable detail-image
argument when the stock Favorite record has no `imgpath`: the source emulator's normal image directory
is used. This keeps list preview and RIGHT-open/Up-Down Game Details on the same source artwork without
adding a non-stock field to the Favorite file.

## Skip inactive theme configuration parsing at startup

Patch name:

```text
skip-inactive-theme-configs
```

Stock MainUI opens `/mnt/SDCARD/Themes`, enumerates every installed theme directory, builds each
`<theme>/config.json` path, opens and parses the JSON, and runs the theme callback before it later
loads `/mnt/SDCARD/system.json` and resolves which theme is actually active. That means startup work
grows with every installed theme even though MainUI only needs the selected theme for normal use.

This selector keeps the stock top-level `/Themes` directory loop and its lifecycle, but filters each
directory entry **before** MainUI constructs the child config path. A small read-only pre-pass of
`/mnt/SDCARD/system.json` extracts only the selected external theme path into bounded private state.
The normal stock `system.json` loader still runs later at its original location and retains ownership
of the real MainUI settings/theme initialization.

For a valid selected path under `/mnt/SDCARD/Themes/`:

```text
selected theme entry:
    stock path construction -> stat/access -> fopen -> JSON parse -> theme callback

all other theme entries:
    next readdir immediately
```

Inactive theme directories are therefore never opened by this scan, and their `config.json` files
are not statted, opened, read, parsed, or passed to the theme callback. The parent
`/mnt/SDCARD/Themes` directory is still opened and enumerated so the stock scan control flow itself
remains intact.

The pre-pass mirrors MainUI's stock JSON parser ownership: the raw parse allocation is converted to
the normal JSON object, the top-level `theme` string is read, and both parser-owned layers are
released through the same stock cleanup routines. The copied path is capped at 255 bytes.

This is deliberately fail-open. If `system.json` is missing or malformed, the `theme` field is not a
string, the selected path is unavailable, too long, outside `/mnt/SDCARD/Themes/`, or does not name
exactly one direct child theme directory (with an optional trailing slash), the private selection
remains unresolved and the complete stock all-theme scan runs unchanged. The built-in `./` default
therefore retains stock behavior rather than guessing an external theme.

This optimization changes which installed theme objects MainUI constructs at startup, so physical
hardware testing remains the release gate. In particular, test cold boot, ROM-list entry/navigation,
Favorites, Recent, Settings, external ThemeSwitcher handoff/return, and repeated MainUI restarts with
multiple installed themes.

---

## Theme-configurable bold style for game-list row fonts

Patch name:

```text
patch-theme-list-font-bold
```

This option implements the theme setting requested in [OnionUI/Onion issue
#42](https://github.com/OnionUI/Onion/issues/42).

It reads an optional boolean `bold` member from a top-level `gamelist` object in the active theme's
`config.json`:

```json
{
  "gamelist": {
    "bold": false
  }
}
```

Rules:

```text
missing setting        -> true
true                   -> bold (stock behavior)
false                  -> normal/non-bold
invalid non-boolean    -> true
```

The setting is applied to game-list row fonts used by ROM, Favorites, Recent, and Search-result
lists. Language, Systems, Apps, Settings, title, detail, keyboard, and other independently loaded
fonts remain outside its scope.

`TTF_SetFontStyle` mutates a `TTF_Font` object globally, so an explicit `false` does **not** restyle
MainUI's shared theme font. The patch instead opens/caches a private game-list font at the active
theme font path and size, applies normal style only to that private object, and routes the verified
game-list font users to it. When `patch-rom-list-font-size` is selected at the same time, the private
size-override font receives the requested style as well. Stock shared-font bold calls remain
stock-owned and cannot leak a normal style into titles or unrelated menus.

The parser reads only the top-level theme `gamelist.bold` value. A separate per-system JSON field
also named `gamelist` contains a string/path and is not the theme style object; `list.bold` is not a
compatibility alias. During the installed-theme scan, patched values are staged by exact theme path
and never written directly to live font state. Active-theme initialization resets the live setting
and applies only an exact selected-theme match, so another installed theme cannot affect the active
font weight. Missing or invalid values deliberately retain stock bold. Restart MainUI or switch
themes after editing `config.json`.

---

## Cross-action rapid-navigation reset

Patch name:

```text
fix-game-list-rapid-navigation
```

MainUI keeps a vector of recently repeated translated actions. When more than 25 entries remain in
that vector, ordinary Up/Down dispatch switches from the one-row virtual method to the same
page-step method used by L2/R2. Stock MainUI's page step is six entries. Repeated presses of the
same action within 200 ms are supposed to build that history.

The stock mismatch path is defective: when another action arrives within 200 ms, it neither appends
the new action nor clears the old history. A long R2/L2 sequence, or a long run in the opposite
direction, can therefore leave more than 25 entries active. A subsequent single Up or Down action
may then be sent to the page-step method even though that action itself was not rapidly repeated.

This patch redirects only the verified action-mismatch branch to MainUI's existing reset block. The
result is:

```text
same action repeated within 200 ms -> acceleration remains available
different action                  -> stale history is cleared
ordinary Up/Down after shoulders  -> one-row method, unless that same action is repeated
```

It does not filter or coalesce SDL events, alter the event queue, add sleeps, or change the
configured SDL key-repeat delay and interval.

The acceleration distance follows the movement method actually used by the list:

```text
patch-rom-list-rows enabled  -> effective configured visible row count (6..20)
patch-rom-list-rows excluded -> stock six-entry distance
```

This is not a second configuration read. The accelerated path and L2/R2 paging share the same
signature-validated TextMenu methods, and the row patch already substitutes its cached, validated
row count only when the active object is a ROM, Favorites, Recent, or Search-result game list.
Language and unrelated TextMenu users retain the stock distance.


### Known screenshot navigation issue

Taking a screenshot with **MENU + POWER** can leave several SDL key-repeat events queued while PNG
encoding and SD-card synchronization occupy the system. MainUI timestamps those events when it later
processes them, not when they were generated. The backlog can therefore drain with apparent gaps of
only a few milliseconds, rapidly rebuilding the navigation-history vector and moving the cursor to
the top or bottom of a list.

The proposed narrow mitigation is to reject history-extension samples separated by less than **15
ms**, while still updating the last-processed tick. That threshold remains below the minimum
configurable legitimate repeat interval of 30 ms. It is documented but not applied in this release:
the shared rapid-navigation path affects every TextMenu user, and physical-device regression testing
is required before changing it.

---

## Bounded decoded thumbnail cache for large ROM lists

Patch name:

```text
limit-rom-list-thumbnail-cache
```

This option mitigates [MainUI-issues #7](https://github.com/OnionUI/MainUI-issues/issues/7), where
browsing hundreds of artwork-bearing games can become progressively slower and eventually crash
MainUI.

The clean binary uses one generic mutex-protected LRU insertion routine for several caches. Its
stock eviction test runs only while the existing size is greater than 1000, then inserts the new
value, so the effective ceiling is 1001 entries. The game-preview worker decodes and scales artwork
to fit inside MainUI's 250-by-360 preview area before placing the resulting SDL surface in that
cache.

This patch changes the effective limit to **64 decoded preview surfaces**, but only when the cache
object's cleanup callback is MainUI's dedicated game-preview `SDL_FreeSurface` wrapper. Other
callers of the shared generic LRU retain the stock 1001-entry behavior.

Implementation behavior:

```text
stock game-preview cache ceiling  -> 1001 surfaces
patched game-preview cache ceiling -> 64 surfaces
other generic MainUI caches        -> unchanged
eviction order                      -> stock least-recently-used order
surface cleanup                     -> stock SDL_FreeSurface callback
mutex/worker behavior               -> unchanged
```

A maximum-size 250x360 32-bit preview surface requires about 360,000 bytes before allocator
overhead. Sixty-four such surfaces therefore bound the decoded preview cache near 22 MiB. Real
artwork can use less memory after aspect-ratio scaling, but source decode buffers, theme assets,
MainUI data, and allocator fragmentation still consume additional memory.

The patch does **not** enlarge MainUI's 128 MiB SD-backed swap file. Increasing swap would only
postpone exhaustion while increasing slow SD-card paging and write traffic; reducing the retained
decoded working set through the existing LRU eviction path addresses the problem directly instead.

The cache limit is fixed and has no runtime configuration file. Leaving the flag off preserves the
stock cache ceiling.

---

## Remove redundant preview and theme surface clones

Patch name:

```text
skip-rom-preview-surface-copy
```

MainUI's generic image helper performs:

```c
source = IMG_Load(path);
copy = SDL_ConvertSurface(source, source->format, 0);
SDL_FreeSurface(source);
return copy;
```

The conversion requests the source's own pixel format, so it allocates and copies a complete surface
without changing the requested format. The patch redirects all five verified direct calls to that
wrapper to MainUI's existing `IMG_Load` PLT entry:

```text
0x001cf84  shared 250x360 game-preview loader
0x0020070  lazy theme/image load
0x0123b60  skin resolver primary path
0x0123bec  skin resolver fallback path
0x0123c20  skin resolver absolute-path path
```

The three callers of the 250x360 preview loader are the ROM-list preview worker and the
initial/refreshed game-detail preview paths. The shared skin resolver is called from about 95 stock
sites and loads roughly eighty `skin/*.png` assets during normal startup/theme initialization, so
its three internal redirects cover those assets without editing every caller.

For previews, the surrounding stock code still performs its explicit non-32-bit normalization,
scaling, ownership, and `SDL_FreeSurface` cleanup. Theme assets receive the surface returned by
`IMG_Load` directly instead of an equivalent self-format clone; their ownership and later cleanup
remain unchanged.

The patch deliberately does **not** substitute `SDL_DisplayFormatAlpha`:

- MainUI does not import that symbol, so it is not a direct PLT-target swap.
- SDL 1.2 documents that `SDL_DisplayFormatAlpha` creates another converted surface, so the original
  and converted allocations still coexist during conversion.
- It forces a display-alpha layout and depends on the active video surface, which is a broader
  semantic change than removing a proven self-format clone.

Implementation behavior:

```text
patched call sites               -> 0x001cf84, 0x0020070,
                                    0x0123b60, 0x0123bec, 0x0123c20
stock target                     -> MainUI wrapper at 0x00123a38
patched target                   -> imported IMG_Load PLT entry
changed original byte positions  -> 15
injected code / BSS / file growth -> none
standalone output SHA-256        -> 139806fe5a7351464d64c2cbe9e5d89be9e5d639f1a5e5d38406b2c51864706c
```

The patch is independent from the 64-entry cache limit. Using both removes transient self-copy
allocations from preview and theme loading while bounding the retained decoded preview working set.

---

## Optimized ROM cache-database deletion

Patch name:

```text
optimize-rom-database-deletion
```

MainUI's `removeAllDBFiles` function contains fourteen direct calls to imported `system()`. The
first thirteen form the normal per-system ROM-cache deletion sequence. The final call is a distinct
stock path that executes:

```sh
rm -rf /mnt/P2EXT/.dbs/*
```

The patch changes only the first thirteen calls:

- the first normal call branches to one ABI-aligned helper;
- the next twelve normal calls become ARM NOPs;
- the fourteenth P2EXT call remains byte-for-byte stock and still targets `system@plt`.

The normal helper executes one consolidated cleanup command:

```sh
rm -f /mnt/SDCARD/Roms/*/*_cache* /mnt/SDCARD/Roms/PORTS/Shortcuts/Shortcuts_cache6.db
```

The explicit PORTS Shortcuts target is part of this same existing bulk-cleanup call because its
cache is one directory deeper than the normal per-system `*_cache*` pattern. No extra cleanup hook
or additional `system()` call is introduced.

The helper and command are allocated in the appended read/execute segment. The patcher explicitly
compares the complete 256-byte ASCII whitespace/parser table at `0x001547B0` against the clean input
before writing the result.

Stock refresh gating, C++ cleanup, logging, and the final `sync()` call remain unchanged unless
separately selected patches alter them.

## Direct startup and launch operations

Patch name:

```text
optimize-direct-shell-operations
```

The verified trivial shell commands are replaced with direct imported operations:

- startup `/tmp/stay_awake`: `open(O_CREAT|O_WRONLY)` and `close`;
- startup governor write: `open`, `write`, and `close` when the burst patch is excluded;
- both launch variants' `/tmp/no_audioserver` creation: direct `open` and `close`;
- both matching removals: imported `unlink`.

The system-directory scan's unused `__xstat` existence probe at `0x121820` is replaced with success;
the immediately following `fopen` and existing null branch remain the authoritative file check. When
`optimize-burst-cpu-governor` is also selected, it supersedes only the stock startup governor write;
all marker and `__xstat` changes remain active.

## Search-compatible pinyin generation

MainUI's stock pinyin/cpinyin generation is intentionally retained for every locale during
`cache6.db` construction. OnionOS SearchFilter searches the database's `pinyin` field for each
keyword, so replacing non-CJK pinyin values with empty strings makes those rebuilt databases
incompatible with the external Search feature.

Skipping pinyin generation outside `ch`/`cht` remains technically possible as a rebuild-time
optimization, but it is deliberately **not** applied by this patcher because database compatibility
takes priority. The separate Arcade-name cache may still reuse one already-generated pinyin result
for MainUI's second language slot; it does not leave the database pinyin field empty.

## Artwork-directory recursion skip

Patch name:

```text
skip-rom-artwork-directory-scan
```

MainUI performs **two recursive tree walks** during a scan-based cache rebuild. The first is a
pre-count pass whose result feeds the total shown by **Scanning...**. The second is the real
`cache6.db` builder. Both walks must exclude the artwork directory; otherwise the pre-count can
enumerate every image, inflate the Scanning denominator, and retain much of the filesystem cost even
when the DB-building walk itself does not insert artwork.

The patch guards both directory branches immediately after MainUI has classified the current
`dirent` as a directory and checked the leading-dot rule, but **before child-path construction and
before the recursive call**. If the system-root entry name is exactly `Imgs`, the helper restores
the registers it borrowed and jumps directly to that walk's existing next-`readdir` continuation.
Otherwise it recreates the one overwritten stock instruction and returns to the original path.

The two stock walks have different recursion controls, so this is deliberately **not documented as
an all-depth name blacklist**. The pre-count function carries an explicit recursive-call flag and
suppresses `Imgs` only on its top-level system scan. The DB-building function uses a stock subfolder
flag that prevents its recursive child call from descending into further directories; therefore the
guarded `Imgs` branch is reachable only from the top-level DB scan in the supported binaries. In
practical Onion layouts this means the system-root artwork directory is never opened/enumerated by
either rebuild walk, while nested game-directory handling remains stock.

The patcher also proves the surrounding control-flow provenance before writing either hook. For the
pre-count walk, its directory block must belong to the guarded recursive function and its later `bl`
must target that same function. For the DB walk, the stock `cmp d_type,#DT_REG` must still be
present, its non-file branch must target the guarded directory block, and the later recursive `bl`
must target the guarded DB-scan function. A supported binary whose local bytes still resemble the
hook block but whose scan routing has changed is therefore rejected rather than patched.

## Recursive ROM rebuild path safety

Patch name:

```text
fix-rom-rebuild-path-safety
```

The scan-based `cache6.db` rebuild allocates one shared path buffer and passes it recursively through
MainUI's directory walker. Stock MainUI sizes that buffer as the ROM-root string length plus 256
bytes, then uses unbounded `sprintf` for three different path constructions. The same allocation is
reused at deeper directory levels, so ordinary filesystem-component limits do not guarantee that the
complete recursive path fits. The image-path formatter can also start from a different root than the
ROM path used to size the allocation.

This selector makes the rebuild path explicitly bounded:

- the shared path allocation is fixed at **4096 bytes**;
- all three verified recursive path builders use `snprintf(..., 4096, ...)` through compact wrappers;
- a negative formatter result or a required length of 4096 bytes or more is treated as failure;
- a failed/overlong entry is cleared and skipped through that call site's existing loop/cleanup path;
- paths are never silently truncated and then inserted into `cache6.db`;
- the 128-byte filename scratch is explicitly terminated after stock `strncpy(...,127)`, so a
  127-byte-or-long directory entry cannot make the later `strrchr` scan beyond the scratch buffer;
- both rebuild allocations are checked before their first use. A failed path-buffer allocation exits
  through the existing rebuild return path; a failed 512-byte SQL-buffer allocation frees the first
  allocation through the verified stock cleanup path.

The patch intentionally changes only the scan-based rebuild's temporary construction buffers. It does
not enlarge stored database fields, change the filesystem path used to launch a game, or reinterpret
an overlong name. An entry whose complete temporary path does not fit the 4096-byte bound is omitted
from that rebuild rather than represented under a truncated identity.

The patcher fails closed on the surrounding recursive function identities, allocation call/cleanup
shape, filename-scratch capacities, recursive buffer/capacity forwarding, all three stock path-format
strings and `sprintf` call targets, and the folder-row temporary-string destructor/skip continuation.
This is deliberately stricter than checking only the three modified instructions because an incorrect
failure continuation in this function could corrupt a derived `cache6.db` or leak rebuild state.

## Throttled battery and Wi-Fi status work

Patch name:

```text
throttle-background-status-polling
```

The stock hotplug thread still wakes every second, preserving unrelated one-second work. A four-byte
counter runs the real battery and Wi-Fi status functions on the first tick and every fifth tick
thereafter; MainUI reuses its last parsed globals between real polls.

For Wi-Fi state:

- the in-memory enabled flag is checked first;
- disabled Wi-Fi returns stock state `1` without opening `/proc`;
- enabled Wi-Fi performs one bounded stack-buffer `/proc` walk and checks each numeric process
  cmdline for both `wpa_supplicant` and `hostapd`, returning stock states `2` or `3`;
- the startup guard is reordered so the same cheap enabled flag gates its existing `wpa_supplicant`
  scan.

The adjacent battery/Wi-Fi implementation remains unchanged: both probes run immediately and then
every five seconds while the thread itself still wakes once per second. No new battery sample
filtering, charging-state handling, or battery-icon override is added. A single already reported red
icon above 50% was not reproduced, so this package still does not claim a battery-display fix.

The external `axp_test` battery backend is deliberately retained because no direct hardware/sysfs
replacement was proven for all supported Miyoo variants.

## Wi-Fi connect-command quoting

Patch name:

```text
fix-wifi-network-shell-quoting
```

MainUI's Wi-Fi **scan** path is unchanged. The affected stock code runs only after the user chooses a
network and connects. Stock MainUI formats the selected SSID, and for secured networks the entered
PSK, into a shell command before calling `system()`. The stock format contains backslash-escaped
double quotes; after `/bin/sh -c` parses the command those do not provide a shell quoting boundary
around the interpolated value. Spaces can therefore split a legitimate SSID/passphrase into multiple
shell words, while characters such as `$`, backticks, `;`, `&`, `*`, parentheses, or embedded quote
syntax can be interpreted by the shell instead of passed literally to `wpa_cli`.

The selector replaces only the three verified connect-time formatter/executor pairs:

```text
open network:     set_network 0 ssid <selected SSID>
secured network:  set_network 0 ssid <selected SSID>
secured network:  set_network 0 psk  <entered passphrase>
```

A compact helper builds the complete command in its own **512-byte stack buffer**. The untrusted value
is enclosed in POSIX-shell single quotes, and an embedded apostrophe is represented by the standard
close/escaped-apostrophe/reopen sequence. Literal double quotes remain around the value delivered as
the final `wpa_cli` argument, preserving the stock `wpa_supplicant` value semantics. The helper calls
the already imported `system()` only after the complete command has been assembled successfully. A
NULL input/prefix/function pointer or a value that would exceed the bounded command buffer returns
failure without executing a partial command.

This fixes both the ordinary functional case (for example an SSID or passphrase containing spaces)
and the command-injection class without changing Wi-Fi discovery, list rendering, password entry, or
`wpa_cli` itself. The patcher provenance-checks both connect function entries, the stock SSID/PSK
format strings, all three `sprintf` and `system` targets, command-buffer handoff instructions,
format-pointer loads, and the exact SSID/PSK input locals before redirecting the sites.

The retained 276-byte ARM helper is generated from `WIFI_SAFE_COMMAND_C_SOURCE`. Its reference build
uses LLVM/Clang 17 with the same ARMv7-A hard-float ARM-mode freestanding flags documented in the
embedded-core section below.

## Nested temporary CPU-governor bursts

Patch name:

```text
optimize-burst-cpu-governor
```

The patch uses direct `open`/`read`/`write`/`close`/`unlink` calls and a 32-byte BSS state
containing a reference count, saved-token length, saved governor token, and recovery-marker state.

Burst boundaries are:

- startup acquire at the verified stock governor call; release immediately before the normal window
  loop begins;
- both full ROM-cache rebuild functions, with normal-return and C++ exception cleanup releases;
- both verified game/app launch action paths through MainUI teardown, with release before normal or
  fatal process exit.

The previous governor is read rather than assumed to be `ondemand`. Nested acquisitions cannot
restore while an outer burst remains active. `/tmp/mainui-governor` stores the saved token before
`performance` is written. Successful restoration removes it. Failed restoration or interruption
leaves it for the next MainUI startup to retry. Marker or sysfs-write failures fail open; the launch
wrapper immediately releases when a safely recoverable burst cannot be established. The patch does
not intentionally hold `performance` throughout emulator gameplay.

## Faster ROM cache-database rebuild transactions

Patch names:

```text
optimize-rom-database-rebuild
optimize-rom-list-database-indexes
```

MainUI has two independent `cache6.db` rebuild paths: importing `miyoogamelist.xml` and scanning the
ROM directory. The rebuild optimizer redirects the two verified stock `BEGIN TRANSACTION;` pointer
loads to:

```sql
PRAGMA synchronous=OFF;
PRAGMA journal_mode=OFF;
PRAGMA temp_store=MEMORY;
PRAGMA cache_size=-8000;
BEGIN TRANSACTION;
```

The settings are connection-local and affect only derived-cache rebuilding. `temp_store=MEMORY`
keeps temporary b-trees and the deferred index sort away from the SD card where SQLite permits it.
The negative cache size requests an approximately 8 MiB page cache for the rebuild connection.
Ordinary browse connections and queries are unchanged.

`PRAGMA locking_mode=EXCLUSIVE` is deliberately omitted. It returns a result row and changes
connection-lifetime locking behavior; neither interaction has been proven safe for MainUI's embedded
SQLite wrapper and cache lifetime.

When both selectors are active, the two schema pointer sites remain on MainUI's stock table-only
schema. The importer therefore performs all INSERTs without maintaining an index b-tree. At each
verified end-of-import site, an appended ARM wrapper:

1. recovers the still-live system table name from the caller frame;
2. selects the same BINARY or NOCASE index definition used by the runtime case-sort marker;
3. formats and executes `CREATE INDEX mainui_rom_browse ...` through MainUI's stock SQLite wrapper;
4. executes `END TRANSACTION;` in a separate call regardless of index-creation success.

The separate commit is intentional: index-build failure leaves a slower but usable unindexed cache
instead of abandoning the transaction. Existing stock browse SQL continues to work without the
optional index.

Selector behavior remains independent:

- rebuild optimizer only: the four PRAGMAs and transaction begin are applied, with stock schema and
  stock commit;
- database-index selector only: the table-plus-index schema is used;
- both together: table, bulk INSERTs, deferred runtime-selected index, then commit.

The SQL strings and wrappers live in appended R-X. The complete 256-byte protected table at
`0x001547B0` is still asserted byte-for-byte. MainUI 354 embeds SQLite 3.30.1 and routes these
statements through its existing `sqlite3_get_table` wrapper. Existing databases require Refresh Roms
to receive the index. These are derived caches; after power loss during a rebuild, run Refresh Roms
again if MainUI does not rebuild the interrupted cache automatically.

## Missing or empty miyoogamelist image tags

Patch name:

```text
allow-missing-miyoogamelist-image-tags
```

Stock MainUI exits the importer loop when `<image>` is absent and separately rejects an empty
extracted image string. The patch verifies the importer signature and changes only two ARM branches:

```text
0x0002119C  missing <image>: continue at path/name validation instead of importer exit
0x00021250  empty image:    accept the record after path/name validation
```

The resulting database row uses an empty image path. No placeholder image is invented, and malformed
entries still pass through MainUI's existing path/name checks. Preview loading already has a
no-image path. This selector is independent from the import timing, file-check, transaction, and
database-index optimizations.

This patch concerns stock `miyoogamelist.xml` import into `cache6.db`. It does not change the
separate `gamelist.xml` reader used by the optional game-detail metadata feature.

---

## Bounded miyoogamelist ROM-file checks

Patch name:

```text
optimize-miyoogamelist-file-checks
```

Before the XML entry loop, the patch scans the ROM directory into one bounded transient filename
set: a 512 KiB allocation, 8192 open-addressed slots, and at most 6000 admitted filenames. Ordinary
root-level ASCII paths use the set instead of one `access(..., F_OK)` call per XML entry.

Safety fallbacks remain local and explicit. Allocation failure, capacity exhaustion, non-ASCII
names, nested paths, unusual path forms, and setup failure all use MainUI's stock `access()` check.
Normal and exception cleanup both free the transient allocation.

The selector is active again. It is independent of the cold-cache transaction correction and the
database browse-index selector.

---

## Suppressed per-entry miyoogamelist timing calls

Patch name:

```text
suppress-miyoogamelist-entry-timing
```

The XML importer contains four direct calls to `gettimeofday`:

```text
0x00020cb4  whole-import start, retained
0x000212b8  per-entry start, NOP
0x000215c8  per-entry end, NOP
0x00021704  whole-import end, retained
```

The patch resolves `gettimeofday` symbolically and requires those exact four call positions relative
to the verified importer before replacing only the two loop calls with ARM `mov r0,r0` NOPs. The
surrounding stock arithmetic remains. The retained outer pair continues to produce MainUI's overall
import timing line.

For measurement, temporarily launch MainUI with stdout and stderr captured in tmpfs:

```sh
./MainUI > /tmp/mainui.log 2>&1
```

Do not write the benchmark log to the SD card because that adds storage traffic to the path being
measured. Onion's normal `./MainUI 2>&1 > /dev/null` ordering discards stdout but retains stderr;
the overall timing line is normally on stdout.

The two suppressed calls save two system calls per XML entry. This is expected to be a small
secondary gain. No directory-set optimization is required for this behavior.

---

## Cached Favorite-star lookups per page

Patch name:

```text
cache-favourite-lookups-per-page
```

MainUI opens `/mnt/SDCARD/Roms/favourite.json` once while constructing a database-backed ROM page,
but its stock `isFavourite` helper then rewinds and scans that `FILE*` separately for every row. A
ten-row page with no matching favorites therefore performs ten complete line-by-line scans, ten 8
KiB temporary allocations, and ten frees before the page can finish constructing.

This patch uses a verified unused word in that page-fetch function's existing stack frame for a
page-local raw-file buffer pointer; it does not expand the frame. Immediately after the stock
`fopen`, an appended ARM helper:

1. seeks to the end and obtains the file length;
2. accepts files no larger than 2 MiB;
3. allocates `length + 1` bytes;
4. rewinds and reads the complete file once;
5. appends a NUL byte and publishes the pointer only after a complete read.

Every row then passes that buffer as the first argument to MainUI's existing `isFavourite` helper.
This reuses MainUI's own quoted-path construction and `strstr` matching rather than introducing a
JSON parser or a new path-escaping implementation. The already-open `FILE*` is still passed as the
second argument and remains available for fallback.

If `fopen`, `fseek`, `ftell`, allocation, or the complete `fread` fails-or if `favourite.json`
exceeds 2 MiB-the page-local pointer stays null, the file is rewound, and each row follows the
original stock scan. An empty file is cached as an empty NUL-terminated buffer. The buffer is
cleared and freed before the stock `fclose`, and the verified C++ exception cleanup path frees it as
well.

Because the cache exists only for one page construction, no favorite add/remove invalidation hooks
are required. A subsequent page fetch reads the current file again, so changes made by MainUI or by
an external editor become visible on the next page load. Favorites and Recent builders that do not
use this database page-fetch path remain stock.

MainUI's otherwise-unused in-memory branch contains a diagnostic that prints the complete buffer for
every lookup (`isFavourite search %s in buf(%s)`). The cache patch NOPs only that buffer-specific
`printf`; retaining it would repeatedly format the full JSON file and could cancel much of the
intended gain. File-based fallback diagnostics are unchanged.

Expected work reduction for a ten-row page is:

```text
stock:    10 rewinds + up to 10 complete fgets scans + 10 allocations/frees
patched:   1 bounded file read + 10 in-memory quoted-path searches + 1 free
```

The kernel may already cache the file's blocks, so this does not necessarily eliminate ten physical
SD reads. The primary saving is repeated stdio parsing, allocation, loop, and string-search overhead
on MainUI's CPU. Small favorite files may show little visible difference; collections containing
hundreds or thousands of favorites should benefit more. Device timing is still required.

---

## Sidecar-backed Favorite folders

Patch name:

```text
add-favourite-folders-json
```

The stock file remains authoritative only for Favorite membership:

```text
/mnt/SDCARD/Roms/favourite.json
```

Favorite root re-entry restoration runs after MainUI's stock host setup and before the first 
preview update. The patch intercepts the verified mode-aware slot-8 helper call at `0x2FB4C`. 
For mode 2 only, it consumes the existing saved-root one-shot immediately before the Favorite 
refresh helper and stock preview updater at `0x2F6C4`; non-Favorite modes continue directly. 
This is late enough to run after host construction but early enough that the first preview-path 
derivation sees the remembered Favorite row. The existing draw/input restore remains a fallback. 
No extra wake is queued because the current wake has not drawn yet.

The patch keeps folder-specific data in a separate sidecar:

```text
/mnt/SDCARD/Roms/favourite-folders.json
```

This separation is deliberate. OnionOS `src/keymon/keymon.c` watches the stock Favorite file and
invokes `tools favfix` after Favorite changes. A stock-schema cleanup may remove unknown properties,
but it cannot remove the folder hierarchy or assignments because those are no longer stored there.

MainUI loads the stock newline-delimited Favorite records into a bounded model and uses an
open-addressing hash of exact `rompath` values for O(1) star checks. It separately loads and repairs
the sidecar, then overlays folders and assignments onto the stock model. A Favorite without an
assignment appears at root, so newly added stock Favorites require no custom write hook.

When SELECT opens the normal ROM-list context menu, the patch revalidates the Favorite model and
checks the selected GameAction's persistent ROM identity at the two sites that actually determine the
visible action. Ordinary ROMs use their exact path. When `fix-search-favourite-status` is also enabled,
Search's encoded `launch.sh:<real ROM>` identity first keeps exact legacy compatibility and then retries
the canonical real-ROM suffix. At `0x30108`, a small dispatcher supplies translation ID 55
(**Add Favorite**) or private 405 (**Remove Favorite**). The stock translation call at `0x3010C` is
also dispatched: ID 55 continues through MainUI's stock translator, while ID 405 is resolved by the
Favorite core's private-language loader with embedded English fallback. At `0x30138`, a matching
dispatcher tail-calls the stock `AddFavoriteAction` constructor (`0x3A054`) or stock
`RemoveFavoriteAction` constructor (`0x3A110`). This visible selector has been verified on hardware.

The mutation path uses the RTTI/vtable-derived map. `RemoveFavoriteAction::run` is `0x19ACC`;
`0x1A44C` belongs to `DeleteRomAction`. The patch hooks the verified Remove run entry. For an
ordinary ROM popup, `RemoveFavoriteAction::field_8` is the selected TextItem, and its GameAction is
dereferenced at `TextItem+0x18`; the exact ROM path then comes from the GameAction string at
`+0x24`. The hook removes that exact record from `favourite.json`, clears any sidecar assignment,
and publishes both models through their existing checked writers. With the Search Favorite selector
enabled, an exact encoded lookup is retained for legacy rows and a miss may retry the canonical real-ROM
suffix before falling back to stock. It also clears the renderer's real live-star flag at
**TextItem+0x1D** and posts the normal wake/repaint event. `TextItem+0x1E` is the stock remove-request
flag and is deliberately not used as the membership source. Custom Favorite-folder rows retain their
existing menu-aware removal path. If the object cannot be resolved safely, the hook falls back to stock.

Normal ROM deletion is deliberately kept separate. Stock `DeleteRomAction::run` at `0x1A44C` keeps
its normal dispatch to the ordinary ROM vtable `+0x30` -> `DBCachedTextMenu::deleteRow @ 0x24A14`.
Device logging identified `SQLITE_BUSY`, not a wrong action dispatcher, as the failure. The lock
originated in the positive ROM-list COUNT path: the result had been copied while its prepared
statement remained in a read transaction.

The ownership-safe implementation sends a count-cache hit directly to the successful continuation
and never touches statement cleanup. A genuine positive COUNT query calls `sqlite3_reset()` on the
valid statement, releasing the read transaction without destroying MainUI's prepared-statement
object. Single-ROM deletion then uses stock `unlink()`, stock SQLite execution and stock delete-row
control flow exactly once. The RAM count cache is invalidated only after the stock DeleteRom action
returns.

Apostrophe handling is independent of that lock fix. The DELETE and COUNT formatters double
apostrophes only in a temporary SQL-only copy; `unlink()` receives the original filesystem path and
the stack argument stores at `0x24AEC` and `0x24BEC` remain stock. Favorite-folder custom deletion
remains isolated to the copied Favorite-list vtable.

### Controls inside Favorites

```text
A              enter the selected folder, activate `..`, or launch the selected game/App/RApp
B              leave the current folder; at root, leave Favorites normally
X              stock MainUI Search
Hold UP + L2   select the first Favorite row
Hold DOWN + R2 select the final Favorite row
SELECT         open context-sensitive Favorite actions
```

Every non-root Favorite folder shows a folder-icon `..` row first. A on that row returns one level;
root deliberately has no such row. The parent row is not a sidecar folder and is never serialized.

The Select menu uses MainUI's stock popup/list components. Move is deliberately first:

```text
No pending move + row:     Move selected
Pending move:              Move here
Below maximum depth:       Create folder
Ordinary selected row:     Sort A-Z
Ordinary selected row:     Remove Favorite
Folder selected only:      Rename folder
Folder selected only:      Delete folder
```

`Sort A-Z` is deliberately omitted when the selected row is a folder.

The seven action labels are looked up lazily from the active `.lang` file using private IDs 400-406:

```text
400  Create folder
401  Move selected
402  Move here
403  Rename folder
404  Delete folder
405  Remove Favorite
406  Sort A-Z
```

Add the same keys to the active language JSON when translated labels are desired:

```json
"400": "Create folder",
"401": "Move selected",
"402": "Move here",
"403": "Rename folder",
"404": "Delete folder",
"405": "Remove Favorite",
"406": "Sort A-Z"
```

The parser reads at most 128 KiB, accepts bounded JSON strings and UTF-8/JSON escapes, and caches
only these seven labels. Missing or empty IDs fall back directly to embedded English. It searches
the normal Miyoo/Onion language directories. MainUI's stock fixed translation table and startup
language loop are not extended.

**Sort A-Z** is shown only for an ordinary selected Favorite and affects the complete active
Favorite folder. The synthetic `..` row is excluded. Folder rows remain before ordinary rows, and
each group is sorted independently by ASCII case-insensitive label with the original byte sequence
as a deterministic tie-break. The rewritten order is persisted in `favourite-folders.json` using the
same checked write, file `fsync`, atomic rename, and directory flush as other sidecar edits. Sorting
clears any pending Move selected state before the menu refresh.

### Direct Recent labels

With `optimize-recent-list-loading` enabled, both verified name-resolution callsites inside the
Recent builder use the label already present in the parsed `recentlist.json` record. The replacement
is the single ARM instruction `mov r0,r1` at each callsite: the saved label pointer is returned in
the register where the stock caller expects the resolved display name. No ROM database is queried,
and this selector itself does not change launch path, image path, type, App mapping, Favorite membership,
or row-action construction. A malformed record with an empty label remains empty rather than triggering the
expensive global name resolver.

### Consistent Recent identity and previews

Patch name:

```text
fix-recent-preview-paths
```

Recent records can contain two useful `imgpath` shapes. Some launches save a complete PNG path, while
others save a bare display/name value that is not itself a usable image pathname. Stock MainUI consumes
those records inconsistently: the Recent list derives artwork from the source system, while Game Details
uses the saved action image string. Initial RIGHT-open and later Up/Down refresh also travel through
different detail paths. A record can therefore show artwork in the list but `thumb-default.png` in
Details, or the reverse; moving Up then Down can appear to repair the detail image because the later
refresh takes a different path from initial construction.

This selector gives all three Recent thumbnail consumers one rule:

```text
saved imgpath contains an explicit .png path
    -> use that exact PNG

otherwise
    -> use the source system image directory and derive the PNG from the real ROM path
```

The rule is applied to the Recent list preview, the verified ordinary GameAction RIGHT-open path, and
the later Game Details Up/Down refresh. The unrelated alternate detail-construction path is left stock
because it uses a different action/config layout rather than the ordinary game `GameAction::imgpath`.
The exact Recent TextMenu identity is cleared when ROM/Favorite constructors take over and again when
that tracked TextMenu is destroyed, eliminating stale-address reuse across later lists.

The same selector also normalizes Onion Search's Recent identity. SearchFilter can save a game as
`<source launch.sh>:<real ROM>` while the record launcher names the synthetic Search app. Before both
live insertion and file reconstruction, that pair is converted to the source emulator launcher plus
real ROM path. A Search launch and a direct-console launch of the same game therefore compare as one
Recent identity and keep normal move-to-front/first-occurrence semantics instead of occupying two rows.
Exact `launch == "setstate"` pseudo-records are rejected before `Add2RecentList`, so they cannot consume
one of the retained game slots. Existing JSON is not rewritten merely by loading it; canonicalization
is applied to the in-memory Recent model and to new records written by later launches.

### Reading and retaining 50 Recent entries

With `extend-recent-list-to-50` enabled, MainUI inspects at most the first **200 successfully
parsed** records from the top of `recentlist.json`. This keeps file loading bounded even if a stale
or hidden Recent file has grown to hundreds or thousands of lines, while giving duplicate-heavy
files enough headroom to reconstruct a useful 50-row Recent list. Records after the first 200
successfully parsed lines are not read during that load.

The two verified `Add2RecentList` compare instructions retain indexes 0 through 49 instead of 0
through 19. During **file reconstruction only**, the duplicate path is changed so the first/top
occurrence of a matching Recent identity wins. A later duplicate is treated as a successful no-op
instead of removing and reinserting the earlier row. Once 50 rows have been retained, later unique
records from the remaining part of the 200-record window are also ignored rather than evicting an
earlier top-of-file entry. When `exclude-apps-from-recent-list` is also enabled, stale type-3
AppAction records are skipped before `Add2RecentList`, so they do not consume any of those 50
retained slots. With `fix-recent-preview-paths` enabled, exact `launch == "setstate"` pseudo-records
are filtered at the same pre-retention boundary, and Search/direct launches that normalize to the same
source emulator + ROM identity are deduplicated as one game. Later valid records inside the 200-record
scan window can therefore continue filling the retained vector up to 50 real unique games.

This load-only rule does not change live launch behavior. If a game that is already in Recent is
actually launched again, MainUI keeps its stock move-to-front behavior. The JSON format, row
builder, removal action, and writer remain unchanged. Malformed records are not counted toward the
200 parsed record ceiling.

### Keeping applications out of Recent

With `exclude-apps-from-recent-list` enabled, the verified live AppAction call to MainUI's
`Add2RecentList` writer is skipped. Game launches use a separate call site and remain unchanged, so
new Recent records are created for games but not Apps, Themes, Tweaks, or custom entries launched
through a stock-compatible AppAction.

The same selector also filters stale application records while `recentlist.json` is reconstructed.
The reader already has the parsed record type immediately before `Add2RecentList`; a guarded hook
skips only `type == 3` records through MainUI's normal per-record cleanup. Those records remain
untouched on disk and still count toward the 200-record read ceiling, but they do not occupy one of
the 50 retained Recent rows. This lets an old duplicate/app-heavy file fill the visible list with up
to 50 game entries from the first 200 parsed records.

When `optimize-burst-cpu-governor` is also enabled, the patcher routes the live AppAction call
through a small acquire-only wrapper. This preserves the temporary launch-performance burst while
deliberately omitting the Recent insertion. The selector does not change the JSON format, game
retention limit, or game launch path.

### Removing one item from Recent

With `add-recent-remove-from-list` enabled, press SELECT on a Recent row and choose **Remove from
list**. The row uses stock translation ID 52 and is inserted immediately below **Start**.

ROM rows retain MainUI's verified `GameAction +0x24` erase key and stock remover at `0x120798`. App
rows snapshot the AppAction launch string at `AppAction +0x08 -> app config +0x48`; wrapped or
otherwise unknown non-game rows snapshot the selected TextItem's visible saved label. At activation,
the patch scans at most 10,000 Recent records. App identity is compared with the saved `rompath`
string at record `+0x18`, with the saved launch string at `+0x48` as a compatibility fallback.
Generic non-game identity is compared with the record display label at offset zero. No stored record
type is assumed or required.

A matched non-game record is converted through MainUI's iterator constructor at `0x1244D4`. MainUI's
list erase routine at `0x12450C` receives the converted node pointer itself, matching the stock ABI.
The patch then invokes the stock `recentlist.json` writer and removes exactly the selected visible
TextMenu row through the verified single-item method at `0x2079C`. This deliberately avoids the
stock key remover for Apps: App records normally save their launch identity in `rompath`, while the
stock remover searches the separate `imgpath` field at record `+0x30`, which is commonly empty. The
stock clear-all method is never used.

The popup uses the stock three-row background, matching its three visible actions without the extra
fourth-row height and padding. **Clear recent game list** remains beneath the new item.

Move and reorder remain Select-only so X remains Search. The source is snapshotted by stable
identity: exact `rompath` for a ROM, folder ID for a folder, or `type|launch|label` for an App/RApp
fallback. **Move here** places the cut entry at the selected row position in the current directory.
For a same-folder downward move, the patch compensates for the source row disappearing before the
anchor is evaluated; this also permits the selected final row to become the true final position. If
the selected row is the other type, a folder clamps to the final folder position and an ordinary
item clamps to the first ordinary-item position. Enter a folder before choosing **Move here** to
move the cut entry into that folder. Menu pointers and transient model indices are not persistent
move identities. A folder cannot be moved into itself or one of its descendants, and nesting remains
capped at three levels.

Create and rename input is normalized before duplicate checks and storage: leading whitespace is
removed, then leading `>` and `.` marker characters (and following whitespace) are removed, and
trailing whitespace is trimmed. Empty results, `/`, control characters, overlong names, and
duplicate sibling names are rejected. This applies only to new keyboard input; legacy sidecar names
such as `.` and `..` remain readable.

After **Move selected**, registered Favorite menus refresh immediately so the visual `>` marker
appears without waiting for another input cycle. **Move here** uses the same immediate-presentation
rule: after the sidecar is published, the Favorite model is reloaded and registered menus are
refreshed in the same action, so the relocated row appears without waiting for another key event.

After a successful **Create folder**, the existing deferred Favorite refresh is retained. The newly
appended folder row is armed as the per-menu restore target, so when the keyboard closes and the
rebuilt list is presented, that new folder is selected and the viewport is normalized to make it
visible. The folder is not entered automatically. Failed, duplicate, invalid, or cancelled creates
do not change the selection.

When B leaves the Favorite root, its live selected row and first visible row are copied into the
existing Favorite process-local BSS state **before** stock MainUI starts tearing down the list. The
same two integers are also written as a tiny validated record to `/tmp/mainui-favourite-root-view`.
This is a same-boot fallback, not SD-backed configuration: it exists specifically because launching
a game from another system can relaunch MainUI and therefore clear the process-local BSS copy. The
destructor keeps a fallback capture for other teardown paths, but normal section exit no longer
depends on that late lifecycle point.

On re-entry, the in-memory values remain authoritative. Only if they are absent does the Favorite
loader read the `/tmp` record. `fav_load()` still does **not** write menu geometry directly while
stock host setup can overwrite it; it marks the viewport as pending and the one-shot restore runs
from the first registered root Favorite draw (with first input as a fallback), immediately before
normal list handling. Both selected row and first visible row are clamped only if the rebuilt list
requires it.

When that draw-time restore changes the selected row, the patch queues exactly one ordinary SDL user
event so MainUI immediately performs another normal frame with the corrected selection. This lets
the stock preview path observe and request the restored game's thumbnail without synthesizing an
Up/Down action, playing a navigation sound, changing the row again, or calling private thumbnail
internals. No event is queued when the selection was already correct, when input itself performed the
fallback restore, or while the post-game folder return chain is authoritative.

The game-return chain still takes precedence. The `/tmp` record is intentionally retained for
subsequent MainUI relaunches during the same boot and is naturally discarded with `/tmp` on reboot.

Leaving the Favorite root also clears only the transient cut identity and visual `>` marker. It does
not delete or move an entry. Re-entering Favorites therefore starts with no pending move.

Deleting a folder reparents its direct child folders and direct item assignments to the deleted
folder's parent rather than deleting Favorites. Removing an ordinary Favorite rewrites only the
stock file and drops its sidecar assignment. Clearing Favorites removes stock entries and
assignments while retaining the empty folder structure.

### Sidecar schema

The sidecar is bounded, line-oriented JSON. The first line is a header:

```json
{"schema":1,"generation":42}
```

Folder records use immutable IDs and parent IDs:

```json
{"kind":"folder","id":"f_7a1c2e10","parent":"","name":"Shmups","order":0}
```

Item records use stable identity plus stock action type:

```json
{"kind":"item","key":"/mnt/SDCARD/Roms/ARCADE/ddp2.zip","type":5,"folder":"f_7a1c2e10","order":3}
```

An empty `parent` or `folder` denotes root. Renaming a folder changes only its display name; its ID
and every assignment remain stable. Duplicate IDs are rejected during repair, orphan parents and
assignments are returned to root, and duplicate item identities retain one valid assignment. The
implementation bounds the sidecar to 256 folders, 10,000 item assignments, an 8 MiB file, and three
folder levels.

If the sidecar is missing or invalid, MainUI presents the stock Favorite set as a flat root list. A
corrupt sidecar therefore cannot remove or rewrite stock Favorites.

### Arranging Favorite folders manually

Exit MainUI before editing `/mnt/SDCARD/Roms/favourite-folders.json` on a computer. The file is
newline-delimited JSON: keep the schema header first, then folder and item records. Folder IDs are
stable; rename by changing `name`, move a folder by changing `parent`, move an item by changing
`folder`, and use `order` to control positions among siblings. Root is represented by an empty
`parent` or `folder` value.

Use valid UTF-8 JSON, keep every folder ID unique, and do not assign a folder beneath itself or one
of its descendants. Invalid parents or assignments are repaired back to root when the file is
loaded.

A powerful browser-based **HTML Favorite Editor** is also available at 
[github.com/robcodedev/onionos-favorites-editor](https://github.com/robcodedev/onionos-favorites-editor) for faster bulk organization.

### Sidecar loading behavior

The patch does not import custom `folder` or `fpath` properties from `favourite.json`. If no valid
sidecar exists, the folder tree starts empty and every stock Favorite is displayed at root.

The one-time stock-file backup path is retained for destructive stock Favorite operations such as
removal or clear, but folder creation, rename, delete, ordering, and moves never write to
`favourite.json`.

### Favorite backup files

The two backup files have different purposes and creation rules:

- `/mnt/SDCARD/Roms/favourite.json.pre-folders.bak` is created immediately before the first
  destructive rewrite of the stock `favourite.json`, such as removing a Favorite or clearing all
  Favorites. It is created only when `favourite.json` exists. Once present, it is never overwritten.
  Folder-only create, rename, delete, move, paste, reorder, and Sort A-Z operations do not create or
  update this backup because they do not rewrite the stock Favorite file.
- `/mnt/SDCARD/Roms/favourite-folders.json.bak` is the sidecar's last-known-good read fallback.
  After the temporary sidecar has been checked, renamed into place, and its directory synchronized,
  the patch attempts to create or replace this `.bak` with the newly committed sidecar content. This
  therefore happens after each successful sidecar publication that changes folders, ordering, or
  assignments. The main sidecar publication remains successful even if the later backup-file write
  itself fails.

### Folder row ABI and navigation

A Favorite folder row is allocated at the stock `GameAction` size and initialized by MainUI's
constructor at VA `0x39948`. The confirmed ordinary-folder convention is used:

```text
action +0x54: 0        folder/non-detail byte
action +0x58: nonzero  folder discriminator
action +0x5c:          stock std::string containing the complete folder path
```

The row retains a copied stock GameAction vtable with only execute redirected. The redirected
execute method performs the complete stock folder transaction synchronously: allocate a 284-byte
FolderWindow, call `0x2E978`, set its path with `0x3AAFC`, call vtable slot 5 to create/load the
attached type-2 Favorite menu, push the FolderWindow with `0x3B2D8`, and return zero. The custom
type-2 loader still supplies sidecar-backed rows through the existing hook at `0x37E88`.

No menu pointer is pushed as a window. No delayed `service_nav_request` push is used. No child row's
action pointer is temporarily cleared. An empty logical folder remains at zero rows rather than
manufacturing an item with a null action pointer. B uses MainUI's stock FolderWindow close/pop
lifecycle. This architecture matches the stock ownership sequence statically; device testing is
still required for the closed MainUI C++/SDL runtime.

## One-shot return after a Favorite-folder game launch

Patch name:

```text
restore-favourite-folder-after-game-exit
```

This selector requires `add-favourite-folders-json`. The CLI automatically includes that dependency;
excluding the folder patch also excludes this selector. Direct Python use rejects the invalid
combination rather than silently building an incomplete feature.

### Launch-side behavior

Only normal GameAction objects created while a registered Favorite menu path is not `/` receive the
return vtable. Root Favorite games retain the stock vtable byte-for-byte. Apps retain stock
behavior. The wrapper calls MainUI's original GameAction execute method at `0x1897C` with the
original arguments. The bounded record is prepared before that stock call while the selected
GameAction strings and active nested menu are still live. It is retained only when the call returns
the verified launch result `3`; every other result removes the prepared record.

The fixed binary record is bounded to 1,932 bytes:

```text
magic/version/type
folder path: 256 bytes
ROM path:   1,024 bytes
launch:       512 bytes
label:        128 bytes
```

The exact ROM path is the primary row identity. Launch plus label is a fallback only when a ROM path
is empty. No cursor, viewport, folder stack, menu pointer, or transient model index is serialized.

### Return-side behavior

The next root Favorite load consumes and unlinks the bounded record. It validates the complete
target folder against the current sidecar model and resolves the launched game to a current model
entry ID before constructing any nested view. Invalid size, magic, version, root path, missing
folder, missing game, or an allocation failure leaves ordinary Favorite root active and clears the
pending return.

The first stock Favorite loader call builds `/` normally and arms a bounded reconstruction plan.
MainUI then invokes that same loader once for each additional saved Favorite window. Instead of
returning `/` for every invocation, the patch supplies the next validated component of the target
path: for example, `/`, then `/Arcade`, then `/Arcade/Favorites`. The final loader selects the
launched row by current model identity and clears the pending record. No additional root or child
windows are manually pushed during the normal return path.

The older manual child-window service remains only as a fail-safe when MainUI produces fewer loader
calls than the remembered path requires. While reconstruction is pending, drawing remains suppressed
so intermediate folders are not flashed. The patch does not mutate root into a nested host, push a
raw menu pointer, or restore from a draw callback.

Because the reconstructed stack contains exactly one real window per hierarchy level, B unwinds one
folder at a time and the next B at `/` closes Favorites. This exact runtime behavior still requires
physical-device confirmation. The return selector remains independently excludable:

```sh
python3 onionos_mainui_patcher.py \
  --exclude restore-favourite-folder-after-game-exit \
  MainUI-354-clean
```

The return redesign is isolated from the other Favorite action fixes. Each runtime stage is appended
to `` for device-side diagnosis. Physical testing is still the release gate for MainUI window
ownership and lifecycle.

### Deliberate limits

| Limit | Maximum |
|---|---:|
| Folder name | 127 UTF-8 bytes |
| Complete folder path | 255 UTF-8 bytes, including separators |
| Nesting | 3 folder levels below Favorite root |
| Total Favorite folders | 256 |
| Favorite records | 10,000 |
| Sidecar item assignments | 10,000 |
| Maximum input JSON size | 8 MiB per file |

The stock `favourite.json` file remains authoritative for membership. The sidecar cannot create a
Favorite that is absent from that file.

### Publication and recovery

Sidecar edits are written to a same-directory temporary file, flushed, `fsync`ed, and atomically
renamed over `favourite-folders.json`; the containing directory is flushed when the filesystem
supports it. If publication fails, the completed destination is not exposed. A missing or malformed
sidecar falls back to a flat Favorite root without changing `favourite.json`.

Destructive membership operations retain the patcher's one-time safety copy of `favourite.json`.

### Shared keyboard corrections

Folder naming and rename use MainUI's existing on-screen keyboard. Create and Rename keep the stock
keyboard Window identity, constructor arguments, shared active-window updater, and teardown contract.
MainUI's `ImeWindow::draw` reads that stock Window identity (translation ID 153, `Search`) immediately
before drawing the visible keyboard header. The Favorite core therefore records only the exact ImeWindow
pointer it just constructed plus the already-resolved Create/Rename label. Only the translation call in
that exact `ImeWindow::draw` header path is wrapped: while the live window pointer matches the recorded
Favorite keyboard it returns **Create folder** or **Rename folder**; every other ImeWindow tail-calls
MainUI's stock translator. The custom action destructor clears the record on confirm/cancel, and no shared
update/teardown path dereferences the keyboard Action. Search and Wi-Fi keyboards retain their stock
headers. The wrapper keeps the 24-byte SDL 1.2 event union, preserves the live font register across the
text bridge, latches the committing A press until physical release, suppresses queued repeats, and keeps
long input scrolled to its cursor end. Because the input/text hooks are shared with Wi-Fi text entry, Wi-Fi
credential flows remain part of the physical-device regression matrix.

### Focused host validation

The native harness validates only the data, transaction, and explicit request-ordering layer. It
covers:

- a missing sidecar starting with a flat stock Favorite list and no legacy migration;
- sidecar-only folder creation and stock-file byte preservation;
- moving a game root → folder → root without changing `favourite.json`;
- same-directory and cross-directory positional reorder with folders kept above ordinary items;
- a folder action publishing the sidecar path and invoking the stock FolderWindow
  constructor/load/push sequence synchronously;
- complete-view Favorite game ordinals and direct child-folder counts;
- survival of a complete external stock-file rewrite;
- immutable folder identity across rename;
- failed write, file `fsync`, and rename preserving the previous sidecar byte-for-byte; and

It passes normally and under AddressSanitizer/UndefinedBehaviorSanitizer with leak detection. These
tests do not prove ARM register preservation, stock vtable semantics, preview behavior, or
window-stack ownership on a Miyoo device.

## Suppressed hot-path development logging

Patch name:

```text
suppress-rom-database-debug-logging
```

Hot-path development logging (ROM database, Recent, preview, Favourite refresh, menu teardown) is
suppressed by this selector. It removes sixteen unconditional development/debug output calls in
total while retaining failure-only diagnostics and low-frequency summary output. Twelve are in the
hot ROM-list paths listed below; four additional calls cover Recent reconstruction, the Favourite
per-frame refresh line, and `TextMenu::freeListItem` teardown output.

Preview image processing:

```text
0x0001d008  thumb3bytes pixel %d
0x0001d0ec  copy conversion geometry
0x0001d238  bitmap_scale geometry
0x0001d2fc  process thumbnail timing
```

Directory-scan rebuild insertion:

```text
0x00022050  display/pinyin values
0x00022158  INSERT input values
```

Database page fetching:

```text
0x0002314c  count-query timing and folder
0x00023298  successful database close
0x000232f0  phase-two preparation timing
```

ROM metadata/cache loading:

```text
0x000238d8  generated SQL buffer
0x00023e5c  cache insertion values
0x00023f0c  row-step result
```

Additional development/teardown output suppressed by the same selector:

```text
0x0001b57c  Recent per-record debug printf
0x0001c17c  Favourite per-frame "refresh %s removed %d" printf
0x00020684  TextMenu::freeListItem per-row printf
0x000206c0  TextMenu::freeListItem final "end" puts
```

The two `TextMenu::freeListItem` calls are output-only. Their format loads and imported
`printf`/`puts` targets are provenance-checked, and only the calls are NOPed. Item lookup, string and
object destruction, loop progression, and all actual frees remain stock. This avoids one formatted
stdout call per menu row during teardown even when the normal Onion launcher ultimately sends stdout
to `/dev/null`.

Each ROM/debug call is found from its unique format-string pointer load, verified as a direct call to imported
`printf`, checked against the stock successor instruction, and replaced with one ARM NOP. Error-path
messages such as database open/execute failures are left intact. The one-time
`create_rom_browser_menu` line and rebuild item/total summaries also remain.

Once the preview and page timing messages are gone, five `gettimeofday` calls have no remaining
functional consumer. The patch signature-bounds the preview loader and page fetch functions,
requires exactly two and three imported timing calls respectively, and NOPs them:

```text
0x0001cf0c  preview timing
0x0001cfb4  preview timing
0x00022e08  page timing start
0x00023108  page count timing end
0x000232bc  page preparation timing end
```

The surrounding elapsed-time arithmetic remains harmless stock code, but the five system calls and
all formatting/output work are avoided.

```text
Output size:                 1,443,156 bytes
Output SHA-256:              423a20cbfc2c7015ecd5046ac2b6ac52414a4bb58e3df8ab2f9b0fa5ca3d44a6
Changed original positions:         68 bytes
File growth:                          0 bytes
Runtime BSS:                          0 bytes
```

The normal OnionOS launcher uses this redirection order:

```sh
./MainUI 2>&1 > /dev/null
```

Shell redirections are applied left to right. Stderr is first attached to the launcher's current
stdout, then only stdout is redirected to `/dev/null`. The unconditional debug `printf` calls
patched here write to stdout, so their output is already discarded in the normal launch path. This
patch therefore avoids formatting work and five timing system calls, but it should be treated as
optional cleanup rather than a demonstrated performance optimization; the real-world gain is likely
very small. It can still avoid substantial output when MainUI is started manually with stdout
directed to a file, pipe, or console. Failure diagnostics and low-frequency summaries remain
unchanged.

---

## Skipped ROM-cache global sync barriers

Patch name:

```text
skip-rom-database-refresh-sync
```

MainUI calls the process-wide `sync()` function after bulk cache-file deletion and after both
successful rebuild transactions. These calls flush unrelated dirty filesystem data as well as the
regenerated cache, and can be expensive on microSD. The patch locates the bulk-delete call inside
signature-bounded `removeAllDBFiles`, then locates the two rebuild calls through the exact `END
TRANSACTION;` references. Exactly three imported `sync` calls must match:

```text
0x0011f49c  after bulk cache-database deletion
0x000216d8  after XML/gamelist rebuild
0x00022bfc  after directory-scan rebuild
```

Only those three calls become ARM NOPs. The SQLite commit, resource cleanup, error output, and
summary timing remain stock.

```text
Output size:                 1,443,156 bytes
Output SHA-256:              4b716e3e8cd95cfbf6c7857ce1074dfcbd7cabd6cb9d9999e1271ba722a80f5b
Changed original positions:         12 bytes
File growth:                          0 bytes
Runtime BSS:                          0 bytes
```

This is a deliberate durability tradeoff limited to derived `cache6.db` deletion and rebuilding.
MainUI can immediately use the committed database through the normal kernel page cache, but a power
loss before the kernel writes it to the card may leave old, missing, incomplete, or corrupt cache
files. ROM files, `gamelist.xml`, favorites, and settings are not changed by these sites; run
**Refresh Roms** again after an interrupted refresh.

---

## ROM-list COUNT cache

Patch name:

```text
cache-rom-list-counts
```

The patch uses a one-entry RAM cache keyed by the persistent system descriptor pointer plus the
exact generated 512-byte COUNT SQL and stores only positive results. Generation-checked publication
prevents a rebuild/delete mutation from republishing an older result, and the existing
rebuild/bulk-delete/post-single-delete invalidators remain active.

The important ownership correction is the SQLite statement boundary. The cache-hit path and
real-query path are deliberately separated:

```text
cache hit:
    lookup helper writes cached count
    0x22FA0 -> 0x23278
    no SQLite statement cleanup is touched

real query miss:
    stock prepare/step/column_int
    cache store at 0x23014
    0x23024 -> reset helper
    sqlite3_reset(stmt)
    -> 0x23278
```

Cache hits must bypass statement cleanup entirely because they did not prepare a current SQLite
statement; reading the statement local on that path could instead consume stale stack data. The
patch therefore sends cache hits directly to `0x23278` and never reads `fp-0x40` on a hit. Real
queries retain ownership of their prepared statement and use the reset path shown above.

For a real query, `sqlite3_reset()` is the ownership-preserving operation: it releases the read
transaction that blocked Delete ROM while leaving the prepared statement object valid. The patcher
verifies the static reset implementation at `0x9BF08`, including its reset/rewind call graph, before
patching. It does not use the destructive finalizer at `0x9BE40`.

Single-ROM deletion still invalidates the RAM count cache only after stock `DeleteRomAction`
returns, outside `DBCachedTextMenu::deleteRow`. Apostrophe-safe DELETE/COUNT SQL remains independent
of this cache and changes only SQL-bound copies, never the filesystem path.

No temporary SD-card diagnostics are installed for this path.

---

## Existing ROM database display names

Patch name:

```text
use-rom-database-display-names
```

MainUI's ordinary SQLite ROM worker reads the visible title from column 1 (`disp`) of the existing
per-system `cache6.db`. Stock MainUI then checks a legacy per-system short-name flag and, when it is
set, calls `GetGameName(language, disp)` again before drawing the row. That second lookup is
unnecessary once the database already exists: the database is the source of truth for the list it
represents.

This default-enabled patch changes only that browse-time decision. The stock conditional branch at
`0x23A24` becomes an unconditional jump to MainUI's existing `disp` fallback at `0x23A44`. The only
direct `GetGameName` call in the ordinary cache-DB worker, at `0x23A38`, therefore becomes
unreachable. The patcher also pins the stock system-descriptor / short-name-flag provenance chain at
`0x23A14..0x23A20`, and fails closed if that chain, the worker call map, or the guarded fallback
block changes.

The rule is deliberately strict:

- if `cache6.db` exists and a ROM row is being browsed, its stored `disp` value is used exactly;
- a stored short name remains a short name; MainUI does not try to reinterpret it at browse time;
- database construction/rebuild remains free to call or replace `GetGameName` while deciding what to
  store in `disp`;
- Refresh Roms is therefore the place to correct or enrich database titles, not list rendering.

This makes cached titles intentionally authoritative until the database is rebuilt. If Onion's
Arcade-name data or another build-time naming source changes after `cache6.db` was created, browsing
continues to show the older stored `disp` value; **Refresh Roms** rebuilds the database and picks up
the newer resolved title. This is the same cache-staleness model ordinary non-Arcade systems already
have rather than a second browse-time naming layer.

Cold-device timing isolated why this matters for Arcade. On a 10-row first page whose SQLite `disp`
values were already complete titles, row construction took about 741 ms and the legacy `GetGameName`
calls consumed about 731 ms. The first call alone consumed about 731 ms and all ten calls returned
`NULL`, so MainUI ultimately displayed the database strings it already had. On the immediately
repeated visit, the same ten calls together cost about 0.5 ms after the library had been
initialized. Skipping the redundant browse-time call removes that deferred one-time initialization
from system entry rather than merely moving it elsewhere.

The bypass intentionally treats that browse-time `GetGameName` call as a pure name lookup. Static
MainUI analysis cannot prove that an alternate `libgamename.so` does not lazily initialize private
state later consumed by some unrelated component. Hardware testing with the supported Onion path
confirmed normal Arcade browsing after the bypass; users of a materially different replacement
library can exclude this selector if they require its browse-time side effects.

This patch has no helper payload and no mutable state. On the 354 reference it changes one
instruction condition field only; output size and ELF segment sizes are otherwise unchanged. A
separate final post-emission check also requires the unconditional bypass to be present whenever the
selector is enabled, preventing a future silent no-op regression.

---

## Cached Onion Arcade-name lookup

Patch name:

```text
reuse-onion-arcade-name-lookup
```

This default-enabled patch is specific to OnionOS **4.2.2 or newer** and its file-backed
`libgamename.so`. Exclude it when using the stock Miyoo library or another replacement that does not
use Onion's `arcade-rom-names.txt` semantics:

```sh
python3 onionos_mainui_patcher.py \
  --exclude reuse-onion-arcade-name-lookup \
  MainUI
```

For a short-name Arcade-style system, MainUI's rebuild path asks for the same ROM key twice:

```text
GetGameName("en.lang", key)
GetGameName("ch.lang", key)
```

Onion's library parses `/mnt/SDCARD/BIOS/arcade_lists/arcade-rom-names.txt` into a process-local
map. The patch redirects the first rebuild lookup to a small freestanding MainUI helper that reads
the bounded file once, parses it in place, builds an open-addressed FNV-1a table, and reuses the
result for subsequent ROMs. Duplicate-key handling matches Onion's last-write map assignment.

The second language lookup is replaced with `chdisp = endisp`, and the second pinyin conversion with
`pych = pyen`. Missing keys return `NULL`, matching Onion. If the packed cache cannot be initialized
because the file cannot be opened, read, sized, or allocated safely, the helper permanently falls
back to the stock Onion `GetGameName` path for that process. No cache is written to the SD card. A
publication barrier is used before the ready state is exposed.

```text
first GetGameName call:        0x00021fd0 -> packed cache helper
second GetGameName call:       0x00021fe8 -> removed/reused
second pinyin conversion:      0x00022020 -> removed/reused
embedded hash-cache core:               1,404 bytes
runtime BSS state:                         16 bytes
```

The packed reader keeps its existing safety bounds: the source file may be at most **4 MiB** and the
open-addressed table ceiling remains **131,072 slots**. Files exceeding either effective bound fall
back to the stock Onion `GetGameName` implementation for that MainUI process.

The expected gain is faster Arcade-name table construction and O(1)-average hash lookup instead of
repeated tree lookup. Ordinary `shortname=0` systems do not execute this rebuild path. This selector
**requires and automatically includes** `use-rom-database-display-names`: removing the stock
rebuild-time `GetGameName` warm-up without also bypassing browse-time lookup would simply defer the
same cold initialization cost until the first Arcade list open. If `use-rom-database-display-names`
is explicitly excluded, this selector is excluded with it. With both active, name resolution is
confined to database construction/rebuild; ordinary browsing of an existing `cache6.db` never calls
`GetGameName`.

The supported floor remains OnionOS 4.2.2. The file-backed replacement was merged into Onion's 4.2
development branch before that release. Older releases and third-party library replacements remain
outside this selector's compatibility promise.

References:

- https://github.com/OnionUI/Onion/blob/v4.2.2/src/libgamename/gamename.cpp
- https://github.com/OnionUI/Onion/blob/v4.3.1-1/src/libgamename/gamename.cpp
- https://github.com/OnionUI/Onion/pull/903
- https://github.com/OnionUI/Onion/releases/tag/v4.2.2

---

## Indexed per-system ROM cache databases

Patch name:

```text
optimize-rom-list-database-indexes
```

MainUI stores each system's scanned ROM metadata in an ordinary SQLite database named
`<SYSTEM>_cache6.db`. The relevant stock table and browse query are:

```sql
CREATE TABLE <SYSTEM>_roms (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  disp TEXT NOT NULL,
  path TEXT NOT NULL,
  imgpath TEXT NOT NULL,
  type INTEGER DEFAULT 0,
  ppath TEXT NOT NULL,
  pinyin TEXT NOT NULL,
  cpinyin TEXT NOT NULL
);

SELECT * FROM <SYSTEM>_roms
WHERE ppath=?
ORDER BY type DESC, disp
LIMIT offset,count;
```

Stock MainUI creates no application-level index for that query. SQLite therefore scans matching
table rows and builds a temporary ordering structure for each requested window. The same absence
also makes `SELECT COUNT(*) ... WHERE ppath=?` scan the table.

The patch redirects the two signature-validated schema format loads used by MainUI's XML and
filesystem rebuild paths. The replacement format keeps the stock table definition and adds one
statement to the same existing SQLite execution-wrapper call:

```sql
CREATE INDEX mainui_rom_browse
ON <SYSTEM>_roms(ppath, type DESC, disp);
```

When `fix-case-sensitive-game-list-sorting` is selected in the same build, the two refresh-schema
sites use the **same cached runtime marker** as the SELECT-query selector:

```text
marker absent or unreadable   -> create the COLLATE NOCASE index
marker exists and is readable -> create the stock BINARY index
```

The NOCASE form is:

```sql
CREATE INDEX mainui_rom_browse
ON <SYSTEM>_roms(ppath, type DESC, disp COLLATE NOCASE);
```

The marker-present form is the stock-collated `(ppath, type DESC, disp)` index shown above. Only one
index is created, so database size and refresh overhead remain bounded while the active browsing
query and rebuilt index always agree. Marker existence is cached for the MainUI process lifetime.
After adding or removing `.romListCaseSensitiveSort`, restart MainUI and run **Refresh Roms** once
so the database is rebuilt with the newly selected collation.

Effects and limits:

- normal folder browsing can use an ordered index lookup instead of a full scan plus temp sort;
- the folder-item count query can use the same index as a covering index;
- SQLite still advances past preceding rows for `OFFSET`, so distant pages are not constant-time
  seeks;
- `%term%` `pinyin`/`cpinyin` searches retain their leading-wildcard scan behavior;
- index maintenance adds some work and database size during **Refresh Roms**, but browsing is the
  repeated hot path;
- the index option itself changes no SQLite PRAGMAs, transaction boundaries, inserts, deletes, or
  query result semantics; the separate rebuild option above may be combined with it.

MainUI refresh begins with `DROP TABLE <SYSTEM>_roms`, which also removes the old index. Because the
patched schema recreates the index immediately after recreating the table, subsequent refreshes need
no separate optimizer tool. Databases that already existed before installing the patch remain
unindexed until **Refresh Roms** is run once.

The schema uses two ordinary `%s` fields: one for the table and one for the index target. At both
stock schema sites, a small ARM ABI bridge copies MainUI's table-name pointer into the fifth
variadic-argument stack slot while preserving the original `r3` argument. The fixed-collation and
runtime BINARY/NOCASE selector paths therefore supply the same table name twice without relying on
positional printf support in the device libc. The longest emitted indexed format remains below the
stock 512-byte destination for a conservatively tested 64-byte system identifier.

---

## R1/L1 next and previous initial-character navigation

Patch name:

```text
patch-rom-list-letter-jump
```

This option implements the shoulder-button navigation requested in [OnionUI/Onion discussion
#975](https://github.com/OnionUI/Onion/discussions/975).

Within ROM, Favorites, Recent, and Search-result lists:

```text
R1 -> first item in the next initial-character group
L1 -> first item in the previous initial-character group
```

Navigation wraps at the first and last groups. R2/L2 retain their existing page movement, and
Up/Down retain normal one-row navigation. L1/R1 remain stock in Language, Systems, Apps, Settings,
and unrelated menus.

Stock MainUI deliberately suppresses the ordinary navigation click for physical L1/R1. For game-list
letter jumps, this patch plays the same navigation sample used by ordinary list movement once when a
new request is accepted. Held-button repeats consumed while an asynchronous loader request is
pending do not play additional clicks.

Grouping uses the first non-space UTF-8 code point from the same primary item string that TextMenu
draws. For SQLite-backed ROM lists this is the database `disp` value; browsing does not perform an
additional `GetGameName` lookup. ASCII and Latin-1 uppercase letters are folded to their lowercase
equivalents, so `A` and `a` form one group. Other Unicode code points are compared directly without
locale-specific collation.

During **Refresh Roms**, short-name Arcade systems bind the resolved raw `GetGameName` label to
SQLite's `disp` field and use `strlen()` for the matching byte length. Ordinary systems retain
MainUI's stock filename-derived `std::string` and size helpers. The pinyin-derived local is never
used as the visible title or sort key, and the raw label and byte length are selected as one paired
operation so SQLite cannot receive mismatched storage and length.

Arcade therefore uses the same contiguous displayed-initial scan as ordinary ROM systems, Favorites,
Recent, and Search-result lists: database order, visible titles, and L1/R1 grouping all follow the
resolved display name. The reserved whole-list Arcade scanner remains disabled.

If an existing Arcade `cache6.db` contains filename-derived, short, or empty `disp` values, MainUI
displays those values exactly. Run **Refresh Roms** only if you want the rebuild path to replace
them with resolved labels and corresponding full-name sorting.

The main ROM list loads records asynchronously in windows. When the ordinary contiguous scan reaches
an unloaded index, the patch asks MainUI's stock loader for one adjacent window and immediately
restores the visible selection. The pending scan can resume from the normal row-loader SDL user event
(`action 24`), an idle event, or a later L1/R1 press. This avoids depending on one completion event
that may be coalesced or absent when the requested window is already resident.

Unrelated translated actions are passed through without immediately discarding the pending scan. The
selected row at the start of the request is retained as its origin. If an intervening action changes
the visible selection, the next event cancels the old scan before it can publish a stale destination.
Only one loader request is pending at a time, and L1/R1 repeats that merely resume the existing scan
do not replay the navigation click.

Sidecar-backed Favorite-folder menus use a custom input path and therefore do not pass through the
shared virtual-ROM key hook. Their rows are already fully resident, so the Favorite core performs a
direct cyclic scan of the current row labels, skips the synthetic `..` row, updates the normal
TextMenu selection/window, and plays the same navigation click. This path is enabled only when
`patch-rom-list-letter-jump` is selected.

After identifying the target group, the helper publishes the destination and queues the same
direction-matched stock ROM window used by the accepted scan: R1 uses the forward loader and L1 uses
the backward loader. It then posts one ordinary repaint event so a resident destination is visible
immediately even when no worker completion follows. Parsed lists are fully resident and use the same
repaint without a loader call.

The helper does not write TextMenu viewport bounds, sleep in the input handler, query SQLite during
navigation, or filter the central event queue. It uses the existing item accessor and one bounded
adjacent-window request at a time. In particular, it does not issue synchronous SQL from the input
handler against MainUI's live ROM-list database connection.

The implementation wraps only the shared logical-action translation call, consumes only actions
14/15 for the two known game-list vtables, and passes every other action and menu through unchanged.

---

## Direction-modified shoulders for first and final game-list items

Patch name:

```text
patch-rom-list-end-jump
```

Within ROM, Favorites, Recent, and Search-result lists:

```text
Hold UP, then press L2   -> first item
Hold DOWN, then press R2 -> final item
```

The order is intentional: the direction must already be held when the shoulder keydown is processed.
Pressing L2 or R2 first retains ordinary page movement, even if UP or DOWN is pressed afterward
while that shoulder remains held. It is acceptable for the initial UP or DOWN keydown to move one
ordinary row before the modified shoulder performs the exact first/final jump.

The patch wraps the same signature-checked TextMenu translator call used by the optional letter-jump
patch and asks the stock translator to copy the 20-byte SDL event it already consumed. At that
scoped hook it tracks only `SDLK_UP`, `SDLK_DOWN`, `SDLK_TAB` (L2/page up), and `SDLK_BACKSPACE`
(R2/page down) for the two known game-list vtables. It does not pump, peek, remove, coalesce,
synthesize, or otherwise modify SDL's central event queue.

A fresh L2 keydown is consumed only when UP was recorded as held first. A fresh R2 keydown is
consumed only when DOWN was recorded as held first. The accepted shoulder is latched until that same
shoulder's keyup, so key-repeat cannot issue duplicate end jumps. After release, pressing the
shoulder again while the direction remains held performs a new jump. Shoulder-first repeats remain
stock L2/R2 page actions rather than becoming a late modified jump.

L1/R1 are not decoded by this helper and therefore retain optional initial-letter navigation
unchanged. Ordinary UP/DOWN events are returned unchanged after updating the scoped held-state bits.
Unmodified L2/R2 events are also returned unchanged for normal page movement. Language, Systems,
Apps, Settings, and unrelated menus remain outside the two accepted game-list vtables.

ROM lists set the exact target selection and invoke the matching stock forward/backward loader
window. Recent and Search-result lists are complete parsed vectors and need no loader call.
Sidecar-backed Favorite menus bypass the ordinary game-list translator, so the Favorite core
independently tracks the same direction-first key order from the already-consumed SDL event and
selects row zero or the last resident row directly. Shoulder-first input remains ordinary page
movement. When the letter-jump and end-jump patches are selected together, an accepted modified
shoulder cancels the saved ordinary-list letter-scan state before applying the direct destination.

No sleep, new input thread, queue filtering, synthetic event, or writable-executable segment is
introduced. Device testing is still required to confirm physical timing and that the accepted L2/R2
event never reaches stock page movement on the target hardware.

---

## Automatic horizontal scrolling for long game titles

Patch name:

```text
patch-rom-list-title-scroll
```

This feature implements the long-title auto-scrolling behavior requested in
[OnionUI/Onion#245](https://github.com/OnionUI/Onion/issues/245).

Horizontally scrolls the selected title only when the complete rendered surface is wider than the
real unobstructed strip. MainUI's right-side preview pane is 250 pixels wide in the supported binary
and has a stock fallback left edge of `x=390`. When a theme supplies `skin/preview-bg.png`, MainUI
derives the pane edge from that surface's destination rectangle and centers the selected cover
inside it. Using the pane edge rather than the centered cover edge is important: a narrow cover must
not make text appear to have extra space inside the reserved pane.

The preview-pane boundary is consumed **only while the same live pre-draw state says a thumbnail is
actually active**. A long title on a no-thumbnail row can therefore scroll through the full stock
label box. When the selected row draws `skin/ic-favorite-mark.png`, the marker's actual left edge
minus a 6-pixel gap is also treated as an obstruction. In fixed mode that keeps the marquee clear of
the fixed marker lane; with `.romListDynamicFavPos` present, a no-thumbnail Favorite marker moves to
the far-right stock position and the marquee can use the additional space automatically.

The title stays at its normal position until the configured idle time has elapsed, then moves left
at the configured pixel speed until the complete suffix is visible immediately before the preview
pane. MainUI's internal text helper normally calls `SDL_UpperBlit` with a null source rectangle,
meaning that the complete text surface is used. For the selected overflowing title, the patch
supplies an explicitly bounded moving source rectangle: `source.x` advances and `source.w` is always
written as `complete_title_width - source.x`. This never asks SDL to read beyond the rendered text
surface. SDL_ttf leaves a small visible bearing at the left edge of the stock full-surface blit, so
the first 3 pixels of raw animation travel are consumed before `source.x` begins to advance.
MainUI's destination rectangle remains untouched, keeping the same left alignment and preventing the
final glyphs from being shifted into the preview pane. Every other label retains the stock
null-source blit. Because the affected examples can consist entirely of ASCII characters,
malformed-looking final glyphs are treated as a pixel-rectangle/clipping problem rather than as a
UTF-8 decoder change.

Runtime file:

```text
/.tmp_update/config/.romListTitleScroll
```

Accepted forms:

```text
700,120
700 120
```

The first value is the idle delay in milliseconds. The second is the horizontal speed in pixels per
second. Marquee position is derived from an absolute active-scroll wall-clock epoch rather than by
adding movement every time the renderer is entered. Repeated or nested title draws therefore resolve
to the same X position at the same timestamp, and thumbnail/no-thumbnail repaint differences cannot
multiply the configured speed.

Behavior:

```text
missing or unreadable file -> scrolling disabled
fewer than two parsed integers -> scrolling disabled
negative idle time         -> scrolling disabled
non-positive speed         -> scrolling disabled
idle time                   -> clamp to 10..30000 ms; values 0..9 become 10 ms
scroll speed                -> clamp to 5..400 pixels/second
```

The 10 ms minimum prevents marquee evaluation from becoming visible before the selected row's
preview geometry has had a chance to settle. Recommended example: wait 700 milliseconds, then
scroll at 120 pixels per second:

```sh
700,120
```

The timer and crop position reset on keyboard and joystick navigation events. Non-input SDL events
such as repeated `SDL_ACTIVEEVENT` notifications do not reset the timer. The next selected-title
draw after navigation starts again from the normal unscrolled position.

The feature is limited to tagged ROM, Favorites, Recent, and Search-result lists. The Language
selector, Systems, Apps, Settings, and unrelated menus retain stock rendering. It can be enabled
independently of the row-count and font-size patches.

The marquee uses the same active/previous game-list identity rule at both of its runtime gates. A
source console list that is re-exposed after contextual Search may still be stored as the previous
`state+0x44` object instead of the active `state+0x40` object; both the selected-row arming path and
the final scrolling blit accept either identity. No pointer promotion is performed from the draw path.

### Selected-title detection

The row renderer calls an internal row-title/layout helper three times for its supported row
variants. The patch compares the renderer's current row local directly with `TextMenu::selected`,
records the packed label box, and tail-calls the original helper. It does not infer selection from a
theme image or a focus-gated style flag.

For every selected game-list row, including contextual Search results, MainUI's optional
match-highlight key is replaced temporarily with a private empty string. This forces the stock
helper to render the original complete title as one surface instead of splitting it into separately
blitted fragments. Search matches therefore remain highlighted only while unselected. The selected
row uses the theme's normal selection color and remains eligible for the complete-surface marquee
path.

MainUI also copies long list labels through a stock `strncpy(..., 64)` scratch buffer. The patch
leaves that copy length and destination completely unchanged, preserving adjacent text-style and
color fields. Because `strncpy` does not terminate a 64-byte-or-longer source, the final scratch byte
is explicitly set to NUL. **Non-selected rows therefore display at most the first 63 bytes plus that
terminating NUL.** This is a byte limit, not a pixel-width or character-count limit; with proportional
glyphs, two equally truncated labels can visibly end at very different X positions before reaching
the common right-edge clip. That appearance is intentional and is not a different row-padding value.

The pointer chosen immediately afterward is changed only for the selected row. Selected titles are
rendered from the original `std::string::c_str()` value and can marquee-scroll through the complete
suffix without expanding or overwriting MainUI's internal scratch storage.

The low-level UTF-8 helper is hooked at its final `SDL_UpperBlit`. A one-shot selected label marker
identifies the complete title surface. The wrapper stores:

```text
selected title destination x
complete rendered title width
renderer label-box width
preview-boundary marker pending
selected favorite-marker left edge when present
```

MainUI passes `r1 = NULL` for the stock title source rectangle. The patch creates `SDL_Rect
{scroll_x, 0, complete_title_width - scroll_x, surface_height}` in writable state only when the
selected title overflows. The source rectangle is therefore valid by construction on every frame and
does not rely on the target device's SDL build to repair an over-wide rectangle.

### Exact preview-pane width

The selected title is rendered inside the nested `TextMenu` row loop, but the preview pane is drawn
later by the containing game-list window. A later non-selected row must therefore not cancel the
pending boundary marker. The marker remains pending until the first real preview-window draw
consumes it.

The selected favorite marker is drawn after the row title. Its hook records the marker's actual
destination x. On first discovery for a new selection, it requests one ordinary follow-up marquee
frame and marks that frame as fresh activity. The unchanged baseline scroll calculation then starts
a complete idle interval before moving the title. Later frames reuse the recorded boundary without
repeatedly restarting the timer.

The two structurally validated preview hooks are:

```text
0x31698  preview background (`skin/preview-bg.png`) - primary
0x31794  selected cover artwork                     - fallback
```

At the background call, the wrapper reads the original destination rectangle's `x`. At the cover
call, the caller has already stored the preview pane rectangle at `fp-0x98`; the wrapper reads that
pane `x`, not the centered cover's own destination `x`.

The dedicated `ic-favorite-mark.png` blit is handled separately. The wrapper compares the renderer
current row with `TextMenu::selected` and records the icon destination `x` only for the selected row
that actually draws the marker. When the row-count patch is enabled, the same wrapper chains into
the existing centered icon-crop helper, so the icon is not double-hooked.

On the following draw, the marquee keeps **two widths**. In the normal integrated build where
`patch-rom-list-rows` is enabled, its draw viewport uses the exact same shared right edge as the static
tagged-row clip: actual destination surface width minus the active row-count outer padding. The
selected Favorite-marker boundary may narrow that viewport further. Separately, the overflow-activation
width also considers the preview-pane edge while the same live pre-draw state says a preview is active.
This preserves the proven ROM-list activation threshold without forcing the moving title to stop at the
thumbnail pane. A title-scroll-only partial build without the row patch retains its established stock
label-box width.

Conceptually for the integrated build:

```text
right_edge = destination_surface_width - row_right_padding
draw_width = right_edge - selected_title_x
if selected_row_draws_favorite_marker:
    draw_width = min(draw_width, favorite_icon_x - 6 - selected_title_x)

activation_width = draw_width
if live_preview_active and preview_pane_x - selected_title_x > 0:
    activation_width = min(activation_width, preview_pane_x - selected_title_x)
```

The positive-width check is intentional. The preview-active flag and preview-pane coordinate are
published by different parts of MainUI, and the pane coordinate is cleared during selection/reset
transitions. A transient `preview_pane_x == 0` must therefore not produce a negative activation
width and incorrectly arm scrolling for a short title.

> [!NOTE]
> **Preview-pane geometry:** the shared hard right edge comes from the destination surface width, not
> from the preview-pane edge. With 14 rows on the normal 640-pixel surface, for example, the hard title
> edge remains `x=625` even while a preview pane is visible. The pane participates only in deciding
> whether overflow scrolling should activate; once active, the title may continue underneath the later
> preview artwork until the shared row edge (or a Favorite-marker boundary) clips it. This matches the
> current validated design and is not treated as a regression. Preview-present long titles remain a
> useful hardware regression case if this layering is changed in the future.

Scrolling is enabled when `complete_title_width > activation_width`, but once active the marquee is
rendered through `draw_width`. Therefore a non-Favorite title with a thumbnail can start scrolling at
the same point as before and then travel underneath the later preview artwork to the same row-right
boundary used by static titles. A no-thumbnail title uses the same width for activation and drawing.
Fixed-lane Favorite titles stop before the fixed marker; dynamic-mode Favorites follow the marker's live
position while retaining the common base row-right boundary.

Before the idle timeout the stock title path is retained. After the idle timeout the moving content is
treated as a continuous stream:

```text
title + 60 px blank gap + title + 60 px blank gap + ...
```

The viewport is filled with at most two bounded source rectangles per frame: the visible suffix of
the current title and, once the blank gap has passed, the visible prefix of the repeated title. The
phase is derived from the absolute active-scroll wall-clock epoch and wraps at
`complete_title_width + 60`, so the final suffix moves out at the left while the beginning re-enters
from the right without stopping. Repeated renderer entries at the same timestamp therefore resolve
to the same X position, and configured pixels-per-second motion does not depend on how often the row
renderer is entered. A sufficiently long scheduler/suspend gap rebases rather than producing a large
catch-up jump.
Consequently:

- the rightmost part of the title is fully displayed during every cycle;
- a 60-pixel blank interval (roughly ten spaces at the normal game-list font size) separates copies;
- a live preview participates in overflow activation but does not limit the active draw viewport;
- the selected Favorite marker limits the viewport at its real on-screen destination;
- smaller fonts use their actual rendered pixel width;
- each source width is explicitly bounded, avoiding malformed final glyphs on vendor SDL builds;
- navigation and selection changes reset both the existing idle timer and the loop phase.

### Preview compositing and measured cadence

The preview background and cover are unusually expensive stock software alpha composites on the
supported device. Direct device measurements on the supplied `MainUI-354-clean` binary found median
stock times of approximately **29 ms** for `skin/preview-bg.png` and **17 ms** for the selected cover.
The title-scroll selector therefore includes a narrowly guarded fixed-format ARMv7 NEON compositor
for these two existing blits.

The optimized path is used only when the runtime surfaces match the validated layout:

```text
preview source: 32-bit, 4 bytes/pixel
R/G/B/A masks:  000000ff / 0000ff00 / 00ff0000 / ff000000
screen:         32-bit, 4 bytes/pixel
R/G/B/A masks:  00ff0000 / 0000ff00 / 000000ff / ff000000
```

It locks the existing destination surface through `SDL_LockSurface`, performs the per-pixel
RGBA-to-BGRA alpha blend with NEON, unlocks the surface, and preserves the original software preview
surface. It does **not** use `SDL_DisplayFormatAlpha`, hardware-alpha preview surfaces, RLE, or a
pre-composited static thumbnail. This preserves the tested cover orientation, per-pixel transparency,
`preview-bg.png`, and the existing title-under-thumbnail layering. If the surface format, geometry,
runtime API resolution, or lock operation does not match the validated case, the wrapper tail-calls
the original `SDL_UpperBlit` with the original arguments.

On hardware the optimized median blit times were approximately **7 ms** for the panel and **5 ms**
for the cover. The remaining stable-thumbnail cadence difference was removed by skipping MainUI's
slot-8 window update **only** on a synthetic marquee repaint when the previous pre-draw state already
says the preview is live. Real input events, navigation, selection changes, initial thumbnail loading,
and ordinary redraws retain the stock update call.

The repaint scheduler itself targets **33 ms (~30 Hz)**. This is a target, not a guaranteed display
rate. On the tested MainUI-354 hardware the complete full-window draw/present path settles near
**50-51 ms (~20 Hz)** even without a thumbnail. There is no explicit 20 Hz delay or cap in the patch.
After the preview and synthetic-update optimizations, 64-sample hardware captures measured a
**50 ms median for both thumbnail and no-thumbnail scrolling**. Occasional longer 67/84/100 ms
frames can still occur on either path. Lowering the requested interval cannot force the normal
MainUI presentation loop above what the device can complete; the 33 ms target is retained to avoid
artificially limiting a faster frame while keeping scheduling overhead bounded.

### Real idle redraw

MainUI normally blocks inside `SDL_WaitEvent` while the user is idle. The patch never draws
recursively from an event wrapper.

The patcher locates one central input wait at `0x1775c` in the supported `MainUI-354-clean` binary
and leaves the other six stock wait calls untouched. While an overflowing selected title is armed,
the wrapper:

1. polls the SDL queue and returns every real event unchanged;
2. preserves the published phase for the hardware-tested A/B leave-screen paths while LEFT and actual selection changes retain reset behavior;
3. computes how much of the 33 ms repaint interval has already elapsed and sleeps only that
   remaining amount;
4. returns a zero-code `SDL_USEREVENT` when a repaint frame is due and marks that wake as synthetic;
5. lets MainUI's existing event loop perform its ordinary dispatch and draw cycle;
6. when that synthetic frame already has a stable live preview, bypasses only the central slot-8
   housekeeping callback before drawing; all real-input and non-stable-preview frames call stock.

`patch-rom-list-title-scroll` remains independently selectable, but its slot-8 skip now has an
important lifecycle relationship with the row patch's preview-completion refresh. A pending cover load
can already count as preview-active, so a synthetic marquee wake may skip slot 8 before the cover is
attached. This is safe because `LoadSurfaceWorker::complete` posts a real `SDL_USEREVENT`; that wake
does not set the synthetic flag, so the next slot-8 update runs stock and attaches the cover. When
`patch-rom-list-rows` is also enabled, its completion-generation-gated cover-check refresh is an
additional safety path. Future changes to either selector should preserve at least one of these real
completion routes.

The wrapper never calls a list draw method, `SDL_Flip`, or `SDL_UpdateRect`. Disabled, short-title,
non-game-menu, and unarmed states continue to use normal blocking `SDL_WaitEvent`.

> [!NOTE]
> **Transition behavior:** hardware testing confirms that A and B can leave an actively scrolling
> row without first snapping the outgoing title back to its home position. LEFT remains an explicit
> immediate marquee reset, and normal selection changes still reset normally. When A/B navigation
> actually changes the active game-list object, the newly shown or restored list resets its selected
> long title to the normal left-aligned position immediately instead of inheriting the previous
> list's marquee phase. **RIGHT / Game Details
> remains a known cosmetic limitation:** the outgoing title can still redraw once at its normal
> position on RIGHT key-down before the next screen appears. Several broader RIGHT-specific freeze,
> timer, and surface-identity experiments caused worse lifecycle regressions, so RIGHT is deliberately
> left unresolved for now. The issue is cosmetic only.

> [!NOTE]
> This feature appends a separate read/execute (`R-X`) ELF load segment for its configuration
> reader, timer, title wrapper, preview-pane wrapper, central-wait wrapper, and configuration
> strings. Mutable state remains in the existing writable (`RW-`) BSS. No writable segment is made
> executable.

The compact support code reuses functions already imported by MainUI: `fopen`, `fscanf`, `fclose`,
`gettimeofday`, `SDL_Delay`, and `dlsym`. `SDL_LockSurface` / `SDL_UnlockSurface` are resolved lazily
once for the guarded preview fast path. No raw Linux clock or sleep system calls are issued. If
`gettimeofday` fails, the title remains stationary instead of crashing MainUI; if either surface API
cannot be resolved, preview drawing fails open to the stock blitter.

For the supplied `MainUI-354-clean` binary, a title-scroll-only build appends **5,689 bytes** of R-X
payload and **212 bytes** of writable state. Static validation confirms hooks at `0x1ff0c`,
`0x20008`, `0x2032c`, `0x15ff0`, `0x31698`, `0x31794`, `0x1775c`, and the central slot-8 call at
`0x36fa4`. The selected favorite-marker blit at `0x204a4` records its actual left edge as an
additional obstruction boundary. When the row-count patch is also selected, the same wrapper chains
to the row-height icon clip helper before the stock fallback. The generated ELF retains separate
`R-X`, `RW-`, and appended `R-X` load segments and contains no writable-executable segment.

Device testing with the supplied `MainUI-354-clean` binary confirmed the scrolling behavior.
Recommended configuration:

```sh
700,120
```

Use a smaller list font and a title that extends only a few characters into the preview pane. Also
verify that a very long title finishes with every final character visible before the pane. Restart
MainUI after changing the configuration because values are cached for the process lifetime.

---

## Configurable global key-repeat timing

Patch name:

```text
patch-key-repeat-settings
```

Redirects MainUI's startup call to `SDL_EnableKeyRepeat` through a compact configuration wrapper.
This affects held-button repeat throughout MainUI, including ROM lists, Systems, Apps, Settings, and
game-detail views.

Runtime file:

```text
/.tmp_update/config/.mainUIKeyRepeat
```

Accepted forms use either a comma or a space:

```text
300,50
300 50
500,100
500 100
```

Use `300,50` when quicker repeats are desired. Leave the file absent, or use `500,100`, to retain
the default repeat behavior. The first value is the initial delay in milliseconds and the second is
the repeat interval. Both must be positive decimal integers, and the delay must begin at the first
byte of the file; do not add leading whitespace. A trailing newline is fine.

Behavior:

```text
missing, unreadable, or non-positive parsed values -> 500 ms delay, 100 ms interval
delay                                             -> clamp to 100..2000 ms
interval                                          -> clamp to 30..500 ms
```

Example:

```sh
300,50
```

Restart MainUI after changing the file because the values are read during startup. The recommended
quicker setting is `300,50`; the default is `500,100`. Values below the quicker recommendation are
more aggressive and can make a marginal or bouncing button release feel like a continuous hold.

## Full game-detail title immediately on open

Patch name:

```text
fix-game-detail-title-on-open
```

When Right is pressed on a game, MainUI opens a detail view. A reported bug causes the initial view
to receive the display title and secondary label in the opposite order. The title only becomes
correct after pressing Up or Down.

This patch swaps the two arguments in both initial detail-window paths so the full display title is
shown immediately.

Related reports:

- [Game info screen does not show game title - MainUI-issues
  #26](https://github.com/OnionUI/MainUI-issues/issues/26)
- [Right in Favorites initially shows only the platform - MainUI-issues
  #14](https://github.com/OnionUI/MainUI-issues/issues/14)
- [On OnionOS, is there a simple way to see the full title of a game?][full-title-reddit]

[full-title-reddit]:
  https://redd.it/1drpvnd

---

## Pixel-measured game-detail title wrapping

Patch name:

```text
fix-game-detail-title-wrap-width
```

The stock `GameDetailWindow` draws title text in a rectangle starting around `x=284` with a width of
about `350` pixels, but chooses line breaks by advancing a hard-coded **15 bytes** at a time. That
value is not a pixel width and is not even a reliable character count for UTF-8. It therefore wastes
space with some fonts and overruns the intended margin with other font sizes or glyph shapes.

This patch replaces the byte-count heuristic with measured UTF-8 text width:

1. It uses MainUI's active detail-title `TTF_Font`, so theme font changes are automatically
   reflected in wrapping.
2. It renders short candidate substrings with `TTF_RenderUTF8_Blended` and reads the resulting SDL
   surface width.
3. It limits each line to **340 pixels** inside the existing 350-pixel drawing rectangle, retaining
   a 10-pixel safety margin on the right.
4. It prefers complete word boundaries. When one word is wider than the line, it falls back to
   complete UTF-8 code-point boundaries rather than splitting inside a multibyte character.
5. It keeps MainUI's existing maximum of four displayed title lines.

The first stock 15-byte check is neutralized, and the measurement helper is called once for each
produced line rather than twice.

The same flag also corrects two problems in MainUI's stock hard-wrap logic:

- MainUI normally appends an artificial `-` whenever a forced line break does not happen after a
  space. The patch skips that suffix entirely.
- MainUI compares the final remainder against `length - 1`, which discards the last character when
  exactly one byte remains. The patch compares against the real length so the final character is
  retained.

The measurement helper uses a dedicated 1 KiB BSS scratch buffer and frees every temporary SDL
surface immediately. If the active font or UTF-8 renderer is not available, it falls back to
MainUI's original 15-byte behavior with UTF-8 continuation-byte adjustment. This layout patch
remains separate from the initial title/label argument fix above.

---

## Game-list XML metadata in the game-detail view

Patch name:

```text
show-gamelist-metadata-in-game-details
```

Adds bounded metadata below the existing game title when Right opens the detail view:

```text
Genre | 4/10
Wrapped description text...
```

Each field is optional. When `genre`, `rating`, and `desc` are all absent, the stock detail layout
remains unchanged and no placeholder text is shown.

### Data source

The patch checks only `gamelist.xml` in the selected ROM's immediate directory. It does not use
`miyoogamelist.xml` for display metadata, does not consult the system `config.json` `gamelist` key,
and does not search parent/deeper directories. Metadata is read directly on demand and is never
added to `cache6.db`.

This is separate from stock MainUI database generation: stock MainUI continues to use
`miyoogamelist.xml` normally when rebuilding `cache6.db`.

The XML scanner accepts both `<game>` and normal attributed opening tags such as `<game id="1350"
source="ScreenScraper.fr">`. It requires either `>` or whitespace after `game`, so the root
`<gameList>` element is not mistaken for a record.

`onionos_mainui_patcher.py` contains both the embedded ARM bytes and their GPL-licensed C
corresponding source, linker script, and reproducible Clang/LLD build recipe. Applying the patch
uses the verified bytes and does not require a compiler.

The selected ROM's full path is reduced to its filename and matched against XML `<path>` values
after removing a leading `./` or `/`. The index hash folds ASCII case, and every candidate is
verified with MainUI's existing case-insensitive comparison before its data is accepted.
Apostrophes, commas, spaces, accented text, and other UTF-8 bytes are kept as raw path bytes after
XML-entity decoding.

Because the lookup file is taken from the ROM's immediate directory, a `gamelist.xml` at a system
root does not provide metadata for ROMs stored in deeper subfolders.

### Onion Search console limitation

Onion's external Search feature is exposed to MainUI as an emulated console named `Search`, with its
own generated folder and `cache6.db` rather than as a live view of each source console. A Search
result can originate from `/Roms/FC`, `/Roms/PS`, or another console, but the synthetic Search row
does not make the metadata patch traverse back to that source console's `gamelist.xml`. Those XML
files are therefore not read while Game Details is opened from the Onion Search console, and
genre/rating/description metadata is normally absent there.

In theory the Search app could generate a Search-specific `gamelist.xml` or a source-path sidecar
when it builds its synthetic database. Such output would need to use paths that match the rows
MainUI exposes in the Search console. The patcher does not change the Search app or generate that
data itself. MainUI's separate stock contextual Search result list also remains outside this
immediate-directory metadata promise.

### Bounded LRU indexes and lookup lifetime

Indexes are built lazily and keyed by the **complete path** to the selected `gamelist.xml` file. On
the first use of one exact XML path, the patch reads at most **8 MiB** and builds at most **32,768**
`{path hash, file offset}` records. The temporary full-file buffer and 32,768-record work array are
freed immediately. The published record array is resized to the exact record count and is therefore
at most **256 KiB**.

The process retains up to **sixteen** XML-path entries in a least-recently-used cache. Dynamic index
allocations are additionally capped at **2 MiB**; before publishing a replacement, older indexes are
evicted until the new allocation fits. The replacement calculation clamps an inconsistent
`cache_bytes < allocation_size` base to zero, matching the defensive subtraction used when records
are dropped and avoiding unsigned wraparound. Paths and bookkeeping are stored in the patch's fixed
bounded state. The key is the full XML filename rather than a console identifier, so separate
immediate ROM subdirectories cannot alias one another.

Whenever a new detail lookup is opened, MainUI's imported `__xstat` path checks that exact XML
filename. A ready entry is reused only when file size, modification-time seconds, and
modification-time nanoseconds still match. A changed file is rebuilt in temporary storage and
replaces only its own cache entry after parsing and a second stable-file stamp check succeed. If the
changed XML is malformed or allocation fails, the temporary build and the old index are both
discarded; that file stamp is cached as unusable until it changes again, so stale metadata is never
shown.

Directories without `gamelist.xml` receive a small negative cache entry. The patch still performs
`stat()` on later detail openings, allowing a file created from a PC while MainUI is running to
become usable immediately without repeatedly attempting `fopen()` on an absent file. When all
sixteen slots are occupied, the least recently used positive, negative, or unusable entry is
replaced.

The XML file itself is never kept open. Each game lookup reopens its selected XML, seeks to one
indexed `<game>` offset, and reads at most **32 KiB** for that entry.

A successful index build prints one timing line to stdout:

```text
gamelist detail index: N entries, N us
```

Cache hits and negative-entry hits do not print a build line. Onion's normal launcher discards
stdout, so capture it temporarily in `/tmp` when measuring.

### Parsing and display limits

Only these tags are read:

```text
<genre>  maximum 255 decoded bytes
<rating> EmulationStation 0.0-1.0 value, rounded to N/10
<desc>   maximum 4095 decoded bytes
```

Named XML entities (`&amp;`, `&quot;`, `&apos;`, `&lt;`, `&gt;`) and numeric entities are decoded.
Description whitespace and embedded newlines are normalized to readable spaces. Malformed,
truncated, missing, empty, or oversized XML fails to an empty metadata state rather than blocking
the detail view.

The metadata font is opened through the same active theme font-loader path used for the detail
**system-name** line. Its size is **75%** of that system-name font size, rounded to the nearest
integer, with a minimum of **14 pixels**. Its style is explicitly reset to normal. A font file whose
glyphs are intrinsically italic will naturally still look italic.

Genre and rating form one wrapped summary block below the title and system area. It is wrapped using
real rendered widths, complete word boundaries, and UTF-8-safe splitting for an oversized token. Up
to eight summary lines are retained. The description starts eight pixels after the final summary
line.

Description wrapping uses real rendered font metrics inside the active detail column-350 pixels
alone or 360 pixels with `improve-game-details`-and prefers word boundaries. It splits oversized
tokens only at UTF-8 code-point boundaries. At most **64 wrapped lines** are retained.

Rendered graphics are also bounded. The patch caches only the summary surface and the currently
visible description page, never every line in the description. These surfaces are invalidated and
freed on game, font, scroll-page, or detail-view changes.

### Scrolling

L2 and R2 are unused by stock `GameDetailWindow` and become description page controls:

```text
L2  previous description page
R2  next description page
```

Scrolling is clamped at both ends. When the description fits on one screen-or is absent- L2/R2 are
consumed without moving or disturbing the view. Other detail-view controls are unchanged.

A very large XML may cause a visible one-time pause when its exact-path index is first built. Later
lookups within the sixteen-entry working set reopen, seek, and parse only one bounded entry,
including when Favorites alternate among systems. A seventeenth distinct XML path evicts the least
recently used entry. Device testing remains required for layout across themes, mixed-system
Favorites, CJK text, malformed XML, and long-session memory stability.

---

## Folder-free game-detail navigation and list counters

Patch name:

```text
skip-folders-in-game-detail-navigation
```

When a detail window is open, Up and Down repeatedly use MainUI's stock TextMenu movement methods
until a non-folder `GameAction` row is selected. Folder detection uses the confirmed stock word at
action offset `+0x58`. If a boundary or wrap yields no game, the original selection is restored. The
detail update hook converts the raw selected row to a game-only ordinal, including direct
Right-open, and the detail counter is not drawn for a folder row.

### Ordinary ROM-list counter

The normal ppath query preserves the complete stock row total and obtains a supplementary folder
total:

```sql
SELECT COUNT(*),SUM(type>0) FROM %s_roms WHERE ppath='%s'
```

Column 0 is returned unchanged to the stock count setter at `0x3A9B0`; it remains the pagination
total. Column 1 counts stored folder rows with `type>0`. MainUI-generated cache databases use `0`
for games and positive values for folders, so this preserves the intended folder count while
avoiding a nonzero comparison. The count is associated with the exact menu pointer in a bounded
16-entry writable map at the stock setter call site `0x23470`. This keeps parent and nested-child
counts separate.

The visible counter call at `0x31854` is wrapped. Since the stock query orders `type DESC, disp`,
all folder rows precede all games. For a game row the wrapper draws:

```text
game current = physical current - folder count
game total   = physical total   - folder count
```

For a selected ordinary folder, the draw is suppressed. If no exact menu association is found, the
wrapper falls back to the unmodified stock current/total rather than subtracting a guessed or stale
count. No filesystem child count is substituted.

### Favorite-list counter

The Favorite core owns the complete logical view and does not use MainUI's loading-progress globals
or paged SQL total. When a Favorite menu is populated, it records the number of leading non-game
rows and direct game rows. Drawing a selected game can therefore derive its one-based ordinal and
total in constant time instead of rescanning the complete registered menu every frame.

A selected Favorite folder suppresses the normal `N/N` counter and draws its direct game count as
`(N)`. That direct-child count is memoized for the selected folder, so the model is scanned only
when the selected folder changes rather than once per redraw frame. Menu rebuilds invalidate the
memoized value.

Verified current sites for MainUI-354 include:

```text
Normal count-state reset:        VA 0x22D94
Dual-column SQL pointer:         VA 0x22EE0 / 0x22EE4
Result-column reader:            VA 0x2300C
Per-menu count publisher:        VA 0x23470
Visible list-counter draw call:  VA 0x31854
Counter renderer:                VA 0x311F4
Detail ordinal/update:           VA 0x35538
Detail counter guard:            VA 0x35898
```

The loading-progress overlay at `0x18E3C`/`0x192A0` is deliberately not patched. This selector is
independent from `improve-game-details` and the XML metadata selector. Physical-device testing is
required for ordinary lists, Favorites, Search, Recent, nested B returns, selected folders, paged
systems, and detail navigation.

## Improved game-detail layout

Patch name:

```text
improve-game-details
```

This patch owns the complete right-side detail layout, including the system label:

```text
counter first visible digit: x=276, y=55
Favorite icon:              x=634-icon_width, y=60 (left-clamped to x=274)
system label:                x=274, y=98, w=360, h=30
title/metadata base:         y=130; row height=7/8 active TTF height (20-40 px)
right text rectangle:        x=274, w=360
detail preview top:          y=67 for real art and thumb-default.png
```

The counter is centered by MainUI inside a rectangle, so a fixed rectangle X cannot align all
digit-strip assets exactly. The injected helper reads the ten-digit strip width `W`, uses digit
width `D=W/10`, and sets the rectangle to `x=216+2D`. For the stock four-digit counter calculation,
its first visible digit is therefore exactly `x=276`; a guarded `x=240` fallback is used only if the
digit surface is unavailable.

Title rows use the active title font's measured `TTF_FontHeight`, scaled to seven eighths and
clamped to 20-40 pixels. Single-line height, multiline vertical advance, multiline rectangle height,
Favorite-label height, and metadata placement use the same calculated value, so spacing follows the
configured theme font rather than a fixed 25- or 35-pixel constant. The genre/rating block begins
ten pixels lower and wraps when its rendered width exceeds the detail column.

The stock "Added to favorites" text is skipped. The icon is the sole Favorite indicator and is
right-aligned to the detail text area's `x=634` edge at `y=60`, using its actual surface width and
clamping unusually wide assets to the text area's left edge. Moving the right column ten pixels left
and extending its width by ten pixels gives the title and optional XML metadata more space. The
shared detail-preview path uses an exact top coordinate of `y=67`, so a real thumbnail and
`thumb-default.png` are vertically aligned rather than receiving different results from
size-dependent centering.

MainUI's stored system/emulator label is still drawn with the normal game-list font and detail-title
color. Depending on system configuration, it may be a short internal label rather than a marketing
name. Immediately before this one draw, the patch removes leading and trailing ASCII space bytes
into a bounded stack scratch buffer. The stored `std::string`, console configuration, sort position,
database identity, and every other label use remain untouched. This specifically corrects Onion
Search's padded `" Search "` display without moving the synthetic console in system ordering.

This option automatically enables `fix-game-detail-title-on-open`, because stock MainUI's initial
constructor swaps the title and system-label arguments. No compatibility selector alias is accepted.

---

## Sleep Timer Left/Right direction

Patch name:

```text
fix-sleep-timer-left-right
```

Stock MainUI uses the same forward cycle for both Left and Right in Settings > Sleep Timer:

```text
0 -> 5 -> 15 -> 30 -> 0
```

The patch changes only the Left handler to:

```text
0 -> 30 -> 15 -> 5 -> 0
```

Related reports:

- [Sleep Timer only goes right, despite having arrows - MainUI-issues
  #1](https://github.com/OnionUI/MainUI-issues/issues/1)
- [Onion v4.0.0-rc discussion: Sleep Timer Left moves
  right](https://www.reddit.com/r/MiyooMini/comments/x3xg6e/onion_v400rc_official_prerelease/)

---

## Invalid main-menu state fallback

Patch name:

```text
fix-invalid-main-menu-state
```

MainUI can display its internal `wrongbeef` fallback when it receives an invalid menu or translation
ID, reportedly associated with a non-existing ID in `/tmp/state.json`.

The patch routes the affected invalid lookup paths to the localized **Game** entry instead.

This is intentionally non-destructive: it does not rewrite `/tmp/state.json`. It changes the
fallback used by the affected MainUI lookup paths.

Related report:

- [Why does this say
  wrongbeef?](https://www.reddit.com/r/MiyooMini/comments/1f2jtq7/why_does_this_say_wrongbeef/)

---

## Correct game-list context-menu background

Patch name:

```text
fix-game-list-context-menu-background
```

The ordinary game-list context menu contains three visible actions, but stock MainUI selects
`skin/bg-pop-menu-2.png` in the branch that constructs that menu. The patch changes only that
image-pointer load to `skin/bg-pop-menu-3.png`, which MainUI already uses for a three-row popup. No
menu actions, input handling, dimensions, or theme files are modified.

The patch is a signature-checked replacement of one ARM `MOVW` instruction. On the supported
`MainUI-354-clean` binary it changes one byte at file offset `0x20388`, does not allocate code or
runtime memory, and does not grow the file.

Related report:

- [Wrong context menu background in game list - OnionUI/Onion
  #667](https://github.com/OnionUI/Onion/issues/667)

---

## Optional context-menu selection background

Patch name:

```text
patch-context-menu-selection-background
```

Popup context menus normally reuse the full-width game/list selection asset. Because popup windows
are much narrower than the normal 640-pixel list, themes can optionally provide:

```text
skin/bg-list-popup-s.png
```

`PopupWindow` first constructs its child menu and loads the ordinary selection surface exactly as
before. The patch then tries `bg-list-popup-s.png` from the **active theme only**. On success, that
surface replaces the popup's private selection surface and remains owned/freed through the normal
TextMenu lifetime. If the optional file is missing or cannot be decoded, the existing selection
surface is left untouched, giving old themes an automatic fallback with no configuration change.

The popup's child-menu width follows the active `bg-pop-menu-N.png` background. Theme authors should
therefore make `bg-list-popup-s.png` the same width as their numbered popup backgrounds. A 60-pixel
height matching the normal popup row is the safest choice. `bg-list-popup-s.png` is the highlighted
row strip; `bg-pop-menu-1.png` through `bg-pop-menu-6.png` remain the popup container/background
assets.

---

## Theme style for dialog action labels

Patch name:

```text
fix-dialog-action-theme-style
```

MainUI has two separate action-label renderers. The compact generic two-action dialog loads the list
font and list color. The larger confirmation renderer used by Settings > Shutdown also loads the
list font but uses a separate dark action color. Both renderers must be redirected so Shutdown and
compact dialogs use the configured hint style consistently.

The patch redirects all eight signature-checked global loads across the four verified OK/Cancel
branches:

```text
font:  list/default       -> hint
color: list/dark-confirm  -> hint
```

It affects generic dialogs and the separate Settings > Shutdown confirmation dialog. It does not
alter dialog body text, backgrounds, translations, button mappings, or theme files. The gray
confirmation message uses separate stock dialog-text colors and is left unchanged. The action-label
change uses existing theme font/color globals and adds no injected code, runtime memory, or file
growth.

Related report:

- [Shutdown confirmation dialog ignores theme configuration - MainUI-issues
  #12](https://github.com/OnionUI/MainUI-issues/issues/12)

---

## Runtime-selectable game-list case sorting

Patch name:

```text
fix-case-sensitive-game-list-sorting
```

Stock MainUI orders the SQLite `disp` column using SQLite's default binary collation. This separates
uppercase and lowercase names instead of treating letters as equivalent during alphabetical sorting.

The patch installs both the stock queries and equivalent queries using:

```sql
ORDER BY type DESC, disp COLLATE NOCASE
```

At runtime it checks this marker path:

```text
/mnt/SDCARD/.tmp_update/config/.romListCaseSensitiveSort
```

Behavior:

```text
marker absent or unreadable   -> ignore letter case while sorting
marker exists and is readable -> use stock case-sensitive sorting
```

Case-insensitive sorting is therefore the patched default. To restore stock capital-before-lowercase
behavior without rebuilding MainUI, create the marker:

```sh
touch /mnt/SDCARD/.tmp_update/config/.romListCaseSensitiveSort
```

The contents are ignored, so a zero-byte file is sufficient. To switch back to case-insensitive
sorting, remove the marker:

```sh
rm -f /mnt/SDCARD/.tmp_update/config/.romListCaseSensitiveSort
```

Restart MainUI after adding or removing the marker because the choice is cached for the process
lifetime.

The runtime selector covers:

- the normal folder/game-list query;
- the `cpinyin` search-result query;
- the `pinyin` search-result query.

It does not modify the ROM database or cached display names. Favorites and Recent may use separate
in-memory ordering paths and are not claimed to be changed by this option.

Related bug report:

- [Game list sorting is case-sensitive - MainUI-issues
  #11](https://github.com/OnionUI/MainUI-issues/issues/11)

---

## Favorite duplicate-label collision guard

Patch name:

```text
prevent-favourite-label-collisions
```

A known MainUI bug can corrupt the Favorite list when two entries have identical labels, even when
they belong to different systems.

The patch takes a conservative approach. It refuses to append a new Favorite when the same exact
quoted display label already exists. If an external tool or older MainUI has already placed
duplicate labels in `favourite.json`, the loader keeps the first entry, skips the later duplicate,
and leaves the file unchanged instead of running MainUI's corrupting stock rewrite routine.

> [!CAUTION]
> Back up `/mnt/SDCARD/Roms/favourite.json` before testing this option.

This patch:

- prevents new exact-label collisions in the normal and alternate type-3 add paths;
- safely skips duplicate labels already present on disk;
- suppresses only the stock `need_reformat` flag, so rejected objects are still freed normally;
- leaves `favourite.json` byte-for-byte unchanged when a duplicate is encountered;
- does not repair a file already damaged by cumulative-object rewriting; and
- intentionally prevents two different games with the same visible label from both being added.

Related report:

- [Favorite file corruption from identical labels - OnionUI/Onion
  #61](https://github.com/OnionUI/Onion/issues/61)

---

## Apostrophe escaping in ROM and folder paths

Patch name:

```text
escape-apostrophes-in-rom-paths
```

MainUI interpolates some ROM and folder paths into single-quoted SQLite expressions. An apostrophe
in a path can terminate the string literal early.

The patch duplicates apostrophes before the path is passed into the affected SQL-query construction
sites:

```text
Joey's Games
```

becomes the SQL-safe value:

```text
Joey''s Games
```

The original C++ path string is not modified. The escaped value is written into bounded scratch
buffers used only for query construction. At the 1 KiB buffer boundary, an apostrophe is emitted
only when both bytes of its SQL `''` pair fit; truncation can therefore never leave a half-escaped
trailing quote.

The patched sites cover affected path-count, list, delete, and secondary-count queries.

Related reports:

- [Directories in PS with an apostrophe break loading - MainUI-issues
  #24](https://github.com/OnionUI/MainUI-issues/issues/24)
- [Loading issue when the directory name contains an apostrophe - OnionUI/Onion
  #1751](https://github.com/OnionUI/Onion/issues/1751)
- [Loading issue on a game directory - MainUI-issues
  #21](https://github.com/OnionUI/MainUI-issues/issues/21)

Issue #21 reports a similar endless-`LOADING` symptom but does not establish an apostrophe as the
cause. This patch only claims to fix SQL quoting for apostrophes; it may not resolve every
directory-loading failure described there.

---

## Runtime configuration-path overrides

The default paths can be changed when building a binary:

```sh
python3 onionos_mainui_patcher.py \
  --include patch-rom-list-rows \
  --config-path /custom/path/.rows \
  MainUI-354-clean
```

Available path options:

```text
--config-path
--font-config-path
--sort-config-path
--key-repeat-config-path
--title-scroll-config-path
```

The path strings are embedded into the patched binary, so use paths that exist on the target device.

## Technical implementation notes

### Injection-space layout

MainUI has a 2,976-byte file gap between its original R-X and R-W `PT_LOAD` segments. The patcher
reserves that finite area for compact helpers, initialized path/query strings, and the helper whose
PC-relative path reference requires adjacency. Larger position-independent helpers and indexed
database schemas use the appended R-X segment.

Default `--patch-all` layout on `MainUI-354-clean`:

```text
finite gap range:       0x16D220..0x16DDC0
used through:           0x16D984
used:                   1,892 bytes
remaining:              1,084 bytes
appended R-X payload: 146,484 bytes
bounded added BSS:     79,944 bytes
```

An 80-byte in-gap allocation remains reserved at the former font-selector location so unrelated
compact helpers and embedded paths retain stable addresses. The bytes at that inactive site remain
at their untouched input values. The active 104-byte font selector, dynamic row-padding helper, and
all contextual Search helpers live in appended R-X. Contextual Search uses 28 bytes of state for the
confirmation-release state, cached sort mode, reserved words, active contextual-session flag, and
exact Search-parent pointer. Search match highlighting does not store row-wrapper scratch or a
private empty match string in this state. Finite-gap headroom remains 1,084 bytes.

The dynamic row-count rewrite at `0x249B4` is omitted because it sits inside the stock
`0x2493C..0x249EC` initial-request block that `patch-rom-list-rows` already bypasses unconditionally
from `0x24938`. The instruction could never execute, so omitting it removes a misleading dead-code
guard without changing live row-count behavior.

The 68-byte first-presentation geometry helper is appended to the production R-X payload. It hooks
the verified pre-child-draw instruction at `0x31528` and mirrors the exact stock post-draw
preview/full-width decision at `0x31550..0x3158C`; that original post-draw block remains byte-for-byte
unchanged and is signature-checked before publication. Existing helper and BSS addresses remain
unchanged.

The moved helpers use only absolute addresses or ordinary ARM branches. Stock hook sites remain far
inside ARM's +/-32 MiB branch range. The key-repeat helper stays in the gap with its configuration
path immediately afterward because it uses a PC-relative `add` to that blob. Deterministic
allocation order and exact emitted-size checks remain mandatory.

The original executable segment grows only to the last remaining gap allocation. New executable
payloads are mapped in a separate R-X segment; writable BSS remains non-executable. The protected
256-byte parser/classification table at VA `0x1547B0` is checked byte-for-byte before output.

### Performance experiments not retained

Additional hardware experiments evaluated three earlier fast paths: bypassing repeated preview
preparation when the selected game and resolved preview were unchanged, skipping full row preparation
for non-selected rows already completely outside the vertical clip rectangle, and sharing one frame
timestamp across marquee timing helpers. After the lifecycle guards required for safe MainUI ownership
were retained, timed navigation and normal UI use showed no measurable or perceptible improvement.
These shortcuts are therefore not part of the production patcher. Reintroducing them should require
new profiling evidence that identifies one of these paths as a meaningful bottleneck on device.

### Rebuilding the embedded ARM cores

A compiler is **not** required to apply patches. It is needed only when verifying or intentionally
changing the five active freestanding C implementations embedded in `onionos_mainui_patcher.py`:

```text
FAVOURITE_FOLDERS_C_SOURCE
FAVOURITE_FOLDERS_LINKER_SCRIPT
MIYOOGAMELIST_DETAIL_METADATA_C_SOURCE
MIYOOGAMELIST_DETAIL_METADATA_LINKER_SCRIPT
RECENT_REMOVE_C_SOURCE
RECENT_REMOVE_LINKER_SCRIPT
MAIN_MENU_LAYOUT_C_SOURCE
MAIN_MENU_LAYOUT_LINKER_SCRIPT
WIFI_SAFE_COMMAND_C_SOURCE
```

The retained ARM payloads were generated with LLVM 17.0.0 for ARMv7-A Linux EABI, hard-float, ARM
instruction mode. Use matching Clang, LLD, and llvm-objcopy 17.0.0 binaries. Newer or vendor-patched
compilers may produce valid but byte-different output.

Export the inline sources and retained binaries:

```sh
mkdir -p build/embedded-cores
python3 - <<'PY'
import importlib.util
import sys
from pathlib import Path

module_path = Path("onionos_mainui_patcher.py").resolve()
spec = importlib.util.spec_from_file_location("onion_mainui_patcher", module_path)
module = importlib.util.module_from_spec(spec)
assert spec.loader is not None
sys.modules[spec.name] = module
spec.loader.exec_module(module)

out = Path("build/embedded-cores")
out.mkdir(parents=True, exist_ok=True)
(out / "favfolders_patch.c").write_text(module.FAVOURITE_FOLDERS_C_SOURCE)
(out / "favfolders_patch.ld").write_text(module.FAVOURITE_FOLDERS_LINKER_SCRIPT)
(out / "metadata_patch.c").write_text(module.MIYOOGAMELIST_DETAIL_METADATA_C_SOURCE)
(out / "metadata_patch.ld").write_text(module.MIYOOGAMELIST_DETAIL_METADATA_LINKER_SCRIPT)
(out / "recent_remove_patch.c").write_text(module.RECENT_REMOVE_C_SOURCE)
(out / "recent_remove_patch.ld").write_text(module.RECENT_REMOVE_LINKER_SCRIPT)
(out / "main_menu_patch.c").write_text(module.MAIN_MENU_LAYOUT_C_SOURCE)
(out / "main_menu_patch.ld").write_text(module.MAIN_MENU_LAYOUT_LINKER_SCRIPT)
(out / "favfolders_patch.embedded.bin").write_bytes(module.FAVOURITE_FOLDERS_CORE)
(out / "metadata_patch.embedded.bin").write_bytes(module.MIYOOGAMELIST_DETAIL_METADATA_CORE)
(out / "recent_remove_patch.embedded.bin").write_bytes(module.RECENT_REMOVE_CORE)
(out / "main_menu_patch.embedded.bin").write_bytes(module.MAIN_MENU_LAYOUT_CORE)
(out / "wifi_safe_command.c").write_text(module.WIFI_SAFE_COMMAND_C_SOURCE)
(out / "wifi_safe_command.embedded.bin").write_bytes(module.WIFI_SAFE_COMMAND_CORE)
PY
cd build/embedded-cores
```

Build the Favorite core:

```sh
clang --target=arm-linux-gnueabihf -march=armv7-a -mfloat-abi=hard \
  -mfpu=vfpv3-d16 -marm -O2 -ffreestanding -fno-builtin \
  -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fvisibility=hidden -ffunction-sections -fdata-sections -ffixed-r9 \
  -c favfolders_patch.c -o favfolders_patch.o
ld.lld -m armelf_linux_eabi -T favfolders_patch.ld --gc-sections \
  -u fav_action_delete -u fav_activate_menu -u fav_load -u fav_is_favourite \
  -u fav_action_is_favourite -u fav_remove_label -u fav_list_input \
  -u fav_folder_action_work -u fav_game_return_work \
  -u fav_action_work -u fav_selected_folder_count -u fav_counter_packed \
  -u fav_unregister -u fav_remove -u fav_remove_index -u fav_clear -u fav_rewrite \
  -u fav_serialize_root -u fav_sync_invalidate -u fav_dummy_open \
  -u fav_dummy_close -u keyboard_get_action -u keyboard_visible_text \
  -o favfolders_patch.elf favfolders_patch.o
llvm-objcopy -O binary --only-section=.text \
  favfolders_patch.elf favfolders_patch.bin
```

Build the Recents single-entry removal core:

```sh
clang --target=arm-linux-gnueabihf -march=armv7-a -mfloat-abi=hard \
  -mfpu=vfpv3-d16 -marm -O2 -ffreestanding -fno-builtin \
  -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fvisibility=hidden -ffunction-sections -fdata-sections \
  -c recent_remove_patch.c -o recent_remove_patch.o
ld.lld -m armelf_linux_eabi -T recent_remove_patch.ld --gc-sections \
  -u recent_remove_regular_dtor -u recent_remove_deleting_dtor \
  -u recent_remove_execute -u recent_remove_add_item \
  -o recent_remove_patch.elf recent_remove_patch.o
llvm-objcopy -O binary --only-section=.text \
  recent_remove_patch.elf recent_remove_patch.bin
```

Build the configurable main-menu core:

```sh
clang --target=arm-linux-gnueabihf -march=armv7-a -mfloat-abi=hard \
  -mfpu=vfpv3-d16 -marm -O2 -ffreestanding -fno-builtin \
  -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fvisibility=hidden -ffunction-sections -fdata-sections \
  -c main_menu_patch.c -o main_menu_patch.o
ld.lld -m armelf_linux_eabi -T main_menu_patch.ld --gc-sections \
  -u main_menu_build -u main_menu_get_action \
  -u main_menu_action_regular_dtor -u main_menu_action_deleting_dtor \
  -u main_menu_action_execute -u main_menu_finish_action \
  -u main_menu_show_context -u main_menu_draw_label -u popup_background_load \
  -u section_recents_callback -u section_favorites_callback \
  -u section_games_callback -u section_apps_callback \
  -u section_settings_callback -u section_expert_callback \
  -o main_menu_patch.elf main_menu_patch.o
llvm-objcopy -O binary --only-section=.text \
  main_menu_patch.elf main_menu_patch.bin
```

Build the Wi-Fi shell-quoting core. This helper has no external relocations because `system()` is
received as a function pointer, so its dedicated function section can be extracted directly from the
object without a linker script:

```sh
clang --target=arm-linux-gnueabihf -march=armv7-a -mfloat-abi=hard \
  -mfpu=vfpv3-d16 -marm -O2 -ffreestanding -fno-builtin \
  -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables \
  -fvisibility=hidden -ffunction-sections -fdata-sections \
  -c wifi_safe_command.c -o wifi_safe_command.o
llvm-objcopy -O binary --only-section=.text.wifi_safe_system \
  wifi_safe_command.o wifi_safe_command.bin
```

The OnionUI Miyoo Mini toolchain image is available from GitHub Container Registry:

```sh
docker pull ghcr.io/onionui/miyoomini-toolchain:latest
docker run --rm -it -v "$PWD":/root/workspace \
  ghcr.io/onionui/miyoomini-toolchain:latest /bin/bash
```

That container is useful for general Miyoo ARM C work. Exact byte-for-byte reproduction of the
retained payloads still requires the LLVM versions and flags documented here.

Build the metadata core:

```sh
clang --target=armv7-linux-gnueabihf -march=armv7-a -marm \
  -mfloat-abi=hard -mfpu=vfpv3-d16 -O2 -fPIC -ffreestanding \
  -fno-builtin -fno-stack-protector -fno-unwind-tables \
  -fno-asynchronous-unwind-tables -fno-jump-tables \
  -fno-vectorize -fno-slp-vectorize \
  -c metadata_patch.c -o metadata_patch.o
ld.lld -flavor gnu -m armelf_linux_eabi -T metadata_patch.ld \
  --no-undefined -o metadata_patch.elf metadata_patch.o
llvm-objcopy -O binary --only-section=.text \
  metadata_patch.elf metadata_patch.bin
```

`ld.lld` may warn that `_start` is absent. These ELF files are intermediate containers for flat
`.text` payloads, not standalone programs.

Verify exact reproduction and the absence of relocations:

```sh
cmp favfolders_patch.bin favfolders_patch.embedded.bin
cmp recent_remove_patch.bin recent_remove_patch.embedded.bin
cmp main_menu_patch.bin main_menu_patch.embedded.bin
cmp metadata_patch.bin metadata_patch.embedded.bin
cmp wifi_safe_command.bin wifi_safe_command.embedded.bin
readelf -h favfolders_patch.elf recent_remove_patch.elf main_menu_patch.elf metadata_patch.elf
readelf -r favfolders_patch.elf recent_remove_patch.elf main_menu_patch.elf metadata_patch.elf
```

Generated `.c`, `.ld`, object, ELF, and `.bin` files are temporary verification artifacts and must
not be included in a public release ZIP.

## Static validation scope

The patcher verifies recognized hashes, ELF structure, protected bytes, exact hook words, branch
targets, imports, retained-core hashes and entry offsets, segment permissions, non-executable stack
state, bounded BSS growth, deterministic placement, and atomic output publication.

It also performs eight inexpensive post-emission/host safety audits on the generated payload:

- odd-register-count ARM `PUSH` sites are followed through their straight-line basic block and any
  nested `BL`/`BLX` must still have an 8-byte-aligned stack;
- the three known deliberate push/pop asymmetry patterns are pinned for the integrated build, while
  partial-selector builds may legitimately omit the helpers that own them;
- aligned patch-owned writable-state literals must remain within the allocated new-BSS range;
- the four SDL functions resolved with `dlsym` retain their expected fail-open/null-guard structure
  when their owning helper is present;
- the stock-six title-scroll/game-icon helper must emit the proven height-before-width load order, so
  the icon `SDL_Surface*` cannot be overwritten by its integer width before the height dereference;
- game-icon square-crop arithmetic is host-swept across row counts 6 through 20 and source dimensions
  through 512 px, including the stock-six 61-70 px negative-offset edge range, preserving stock
  signed division-by-two rounding;
- the documented wide-spacer classifier and paired title-X/title-width geometry are host-checked across
  stock-style 6/7 rows and dense-row transitions so a wide transparent spacer cannot silently give the
  marquee a wider rectangle than the row renderer actually uses;
- Recent/Search identity and thumbnail models verify Search/direct canonicalization, pre-retention
  `setstate` filtering, explicit-PNG precedence, source-image-directory fallback, Favorite detail-image
  fallback, and Search Favorite real-ROM identity normalization.

These are deliberately cheap structural guards. They are not a full control-flow proof, generic
push/pop reachability analysis, or full register/data-flow reconstruction. The more expensive
cross-selector co-installation semantic audit remains a promotion/developer check rather than a
normal build-time operation.

Static checks cannot prove device lifecycle, SDL scheduling, SQLite performance, or
hardware-specific behavior.

## Deferred preview-path performance work

No preview-path behavior is changed in this build, but device tracing identified three useful future
optimizations that are intentionally documented rather than patched yet.

### `makeThumbPath` and large `Imgs` directories

MainUI's preview path resolver constructs the candidate PNG path and performs a synchronous
filesystem existence check before it dispatches the asynchronous image worker. The worker then opens
the same pathname again through `IMG_Load`.

Device traces show why this matters for unusually large flat artwork directories. With an Arcade
`Imgs` directory containing roughly 15,000 files, the first cold Favorite lookup into that directory
spent about 134-160 ms in the synchronous path-resolution stage, while `IMG_Load` itself took only
about 6-7 ms once the worker was allowed to start. The next image lookup in the same warmed Arcade
directory was effectively immediate. By comparison, a first lookup in a more normally sized GBC
artwork directory took about 10 ms.

This strongly suggests that the visible first-row hitch is the cold directory/path existence lookup,
not PNG decoding, scaling, Favorite parsing, or worker creation. A future experiment could move the
existence decision into the existing worker, cache a proven artwork-directory/path result, or
otherwise avoid the synchronous `__xstat` on the UI thread. Before doing so, the complete
`makeThumbPath` fallback semantics must be mapped so missing/alternative artwork behavior is not
silently changed. The desired result would be immediate navigation even if artwork from a huge
folder appears a little later.

### Favorite synthetic folders

Patcher-created Favorite folder rows currently enter MainUI's game-preview updater even though a
folder cannot have a game thumbnail. Tracing shows calls such as a synthetic `/Racing` Favorite row
being tested against the Search console's `.../App/Search/data/Imgs` context and failing. These
negative checks are usually cheap once cached, so they are not the large first-game hitch, but the
work is semantically pointless.

A future patch should recognize the existing synthetic Favorite-folder discriminator and bypass
preview-path generation entirely, while explicitly publishing the same no-preview/clear state that
stock folder handling expects so a previous game's artwork can never remain stale.

### Synthetic ordinary-ROM `..` row

The patcher-created parent `..` row in a non-root ordinary ROM directory similarly enters preview
path generation even though it cannot own artwork. Stock physical ROM folder rows observed in the
same traces already avoid this bogus preview lookup, so the synthetic parent row should eventually
follow that behavior and bypass preview generation as well. As with Favorite folders, stale-preview
clearing and worker lifecycle must be preserved exactly.

## Discarded loading-progress animation throttle

A previous test selector, `throttle-loading-progress-animation`, changed only the three-dot loading
animation sleep from 28 ms to 100 ms. In stock MainUI the immediate is at VA `0x19390`
(`mov r0,#28`) and the following call at `0x19394` targets `SDL_Delay`. The test replaced the first
instruction with `mov r0,#100`, reducing the target redraw cadence from about 36 fps to about 10 fps
while leaving the draw/blit/`SDL_Flip` path unchanged.

No noticeable device improvement was observed, so the experiment is discarded in favor of retaining
stock timing and behavior. If it is ever revisited, keep it as the same isolated one-instruction
experiment: provenance-pin `0x19390` to `0xE3A0001C`, verify the `0x19394` branch still resolves to
`SDL_Delay`, and replace only the immediate with `0xE3A00064`. Do not combine it with unrelated
loading or rendering changes until a measurable benefit is demonstrated.

## Known limitations

- Power-event persistence intentionally snapshots only an active real DBCached ROM-list window.
  Contextual Search ID 153 is excluded so Search-result geometry cannot overwrite the source
  console's saved position. If power-off begins while Search or another non-ROM window is topmost,
  the extra first-power writer is not invoked; any state already serialized on the preceding real
  list exit remains on disk.
- Persisted-window hardening is intentionally limited to exact `DBCachedTextMenu` in the row patch.
  Parsed game-list `TextMenu` and `MultiDBCachedTextMenu` retain their stock restore semantics. Their
  stock restore paths can theoretically accept individually in-range but descending `start/end`
  values; this release documents that residual risk rather than extending a shared restore hook
  without stronger lifecycle/ownership evidence.
- DBCached persisted-window repair guarantees valid bounded geometry but does not guarantee a
  visually full final page. A valid repaired `start` may remain greater than
  `max(0, total_rows - rows_per_page)`, leaving blank rows below the final item even when
  `total_rows >= rows_per_page`. This is a presentation limitation, not an invalid worker range.
- MainUI is closed source, so these changes are binary-version-specific.
- A future MainUI update may change code signatures, object layouts, or behavior.
- Runtime config values are cached or read at startup; restart MainUI after changing them.
- `--patch-all` retains 1,084 bytes in the finite stock executable/file gap; new large helpers use
  the appended R-X segment, but unusually long configurable path overrides are still bounded and can
  be rejected.
- Title scrolling wraps the renderer's row-title helper calls and uses MainUI's current-row index
  and `TextMenu::selected` to identify the selected text. The selected row's post-`strncpy(..., 64)`
  pointer choice and dedicated favorite-marker blit are also version-specific. It wraps one central
  `SDL_WaitEvent` call and uses `SDL_PollEvent`; it does not directly invoke a renderer or
  display-present function. A future MainUI renderer or input-loop layout change will require
  signature updates.
- Preview-aware width depends on the containing game-list window drawing its preview after the
  nested TextMenu has drawn the selected title. The patch preserves the pending marker across later
  non-selected rows and consumes it at the preview-background or selected-cover call. A future
  MainUI version that changes this window ordering or caller-frame layout will require updated
  signatures.
- Title-scroll configuration and timing reuse MainUI's imported `fopen`, `fscanf`, `fclose`,
  `gettimeofday`, and `SDL_Delay` functions. If `gettimeofday` fails, the title remains unscrolled.
  Because `gettimeofday` follows the system clock, a large clock adjustment while MainUI is running
  can affect one idle interval; navigation resets the timer state.
- Enabling title scrolling increases the output file size by appending an aligned `R-X` load
  segment. The patcher must find a reusable `PT_NULL` entry or a `PT_NOTE` entry whose bytes are
  already covered by another load segment; otherwise it refuses the patch. No writable segment is
  made executable.
- The marquee loops continuously as `title + 60 px blank gap + title`; it neither stops at the end 
  nor bounces.
- Folder and selection graphics are cropped, not scaled. The Language selector is intentionally
  excluded from ROM-list row and font overrides and retains its stock six-row layout and theme font.
  Search navigation temporarily tracks two game-list objects so the original list can regain the
  override when it is restored.
- Long selected Favorite-folder names can be theme-dependent near the final scroll position. The
  stock folder-icon geometry changes the usable label width, so no single theme-independent
  end-offset correction is applied.
- The Favorite-folder data/transaction layer is host-tested, but the folder row still depends on
  MainUI's closed C++ GameAction ABI and window-stack behavior. Highlighting, A entry, nested B,
  empty folders, Search, popup actions, and destruction require a full physical-device regression
  before the feature should be treated as stable.
- Favorite and ordinary ROM folder rows suppress the normal bottom-right `N/N` counter. The patch
  deliberately does not substitute a raw filesystem count.
- The sidecar is global, matching the stock global Favorite file. It does not create a separate
  folder tree for Guest mode.
- The OnionOS global search app emulates a console, so it is possible to use the built in console 
  search within global search, which might look strange. It works, but results are duplicated. 
- Ordinary ROM-folder setup keeps the established full-window request while the redundant stock top
  preload remains bypassed. Persisted-window normalization handles stale saved geometry. The load
  wrapper waits up to 180 ms in 2 ms yields for the complete visible window at index zero as well as
  at restored deeper positions, then falls back to stock progressive drawing. The draw vtable
  remains stock and the list body is never deliberately blanked behind the preview image.
- The ROM row-data completion path schedules two bounded repaint checkpoints at 120 ms and 360 ms;
  the preview completion path schedules one at 120 ms. If SDL's event queue rejects a request, that
  timer retries every 60 ms until one repaint event is accepted, then removes itself. This uses a
  fixed number of initial timers rather than a permanent polling or drawing loop.
- R1/L1 letter navigation can inspect only records exposed by MainUI's stock item accessor. Parsed
  lists are complete; asynchronous ROM lists request one adjacent stock loader window when the scan
  crosses an unloaded boundary. Short-name Arcade rebuilds bind the resolved raw `GetGameName` label
  and matching `strlen()` to SQLite `disp`, so database order and visible grouping align; the pinyin
  local is never used. The stable helper publishes the target, uses the direction-matched stock
  final loader, and posts one ordinary repaint. Unicode grouping is code-point based with
  ASCII/Latin-1 case folding, not full locale collation. If an existing Arcade cache already
  contains blank `disp` values, run Refresh Roms once after installing the corrected binary.
- End jumps use modifier-first sequential state: UP must already be held when L2 is pressed, and
  DOWN must already be held when R2 is pressed. The initial direction event can move one normal row.
  The accepted shoulder event returns action zero before TextMenu dispatch, and that shoulder
  remains latched until its keyup. Pressing the shoulder first intentionally stays normal page
  movement, including later repeat keydowns.
- The `gamelist.bold` setting applies to game-list row fonts only. Language, Systems, Apps,
  Settings, title, detail, keyboard, and other independently loaded fonts are outside its scope. The
  `list.bold` compatibility location is intentionally unsupported. `gamelist.iconLeftMargin` belongs
  to the ROM-list row patch, accepts 0..300, and controls the aligned folder/game icon layout. Both
  patch-added theme settings are resolved only for the exact active theme; values found while scanning
  other installed themes cannot become live state.
- MainUI's stock rapid-navigation tracker can retain more than 25 entries when the action changes
  within 200 ms. That stale state makes a single later Up/Down action select the page-step method.
  `fix-game-list-rapid-navigation` redirects only the verified mismatch branch to MainUI's existing
  reset block. Genuine repeated same-action acceleration is preserved. With `patch-rom-list-rows`,
  the shared page/acceleration step uses the effective visible row count; without it, the stock
  six-entry distance remains. SDL key-repeat timing is unchanged. The patch targets this verified
  stock history mechanism only.
- The 64-surface preview-cache limit is a mitigation for retained decoded artwork, not a proof that
  every possible large-library crash has the same cause. Image decoder failures, malformed PNG
  files, unrelated heap corruption, or a different MainUI build may still fail independently.
- Suppressing hot-path debug logging retains failures and low-frequency summaries but removes
  preview, page-fetch, metadata-cache, and per-ROM rebuild diagnostics. Select the patch only when
  those development traces are not needed.
- Skipping ROM-cache `sync()` calls reduces crash-time durability for derived caches. A power loss
  during or immediately after Refresh Roms can require rebuilding them again.
- The Favorite guard prevents new collisions and suppresses the destructive stock rewrite when a
  duplicate already exists. It does not reconstruct a file that is already damaged.
- The invalid-state patch changes fallback display behavior but does not rewrite the state file.
- Runtime case sorting currently targets the normal SQLite-backed game list and the two search-query
  variants; it does not promise to change every separately managed list.
- Contextual Search confirmation carry-through is handled only after successful Search return and
  only for the observed A/Space repeat/keyup pair. The Search-parent repair is restricted to an
  exact translation-153 MenuWindow with a null child pointer; physical-device regression remains
  required for Search launch/return and repeated B navigation.
- The detail-title wrapper uses temporary rendered surfaces to obtain exact UTF-8 pixel widths. This
  occurs only while rebuilding the detail title, but very long titles may take slightly longer to
  prepare than the stock byte-count loop.
- The detail overlay uses a fixed position and MainUI's stored system/emulator label, which may be
  abbreviated and may need theme-specific adjustment.
- Apostrophe escaping uses fixed-size scratch buffers and is experimental.
- This patcher modifies MainUI only. It does not patch Onion scripts, RetroArch, emulator cores,
  themes, or ROM files.

## MainUI reports referenced

These upstream reports helped identify or document the behaviors targeted by the optional patches:

- [OnionUI/Onion#245 - auto-scrolling for long selected ROM titles](https://github.com/OnionUI/Onion/issues/245)
- [OnionUI/Onion discussion #975 - R1/L1 letter navigation](https://github.com/OnionUI/Onion/discussions/975)
- [OnionUI/Onion issue #42 - theme-configurable bold list fonts](https://github.com/OnionUI/Onion/issues/42)
- [MainUI-issues #7 - large artwork-bearing library eventually crashes MainUI](https://github.com/OnionUI/MainUI-issues/issues/7)
- [OnionUI/Onion issue #667 - three-item game-list popup background](https://github.com/OnionUI/Onion/issues/667)
- [MainUI-issues #12 - shutdown dialog ignores theme configuration](https://github.com/OnionUI/MainUI-issues/issues/12)
- [#26 - Game info screen does not show game title](https://github.com/OnionUI/MainUI-issues/issues/26)
- [#24 - Apostrophe in a PS directory breaks loading](https://github.com/OnionUI/MainUI-issues/issues/24)
- [#21 - Loading issue on a game directory](https://github.com/OnionUI/MainUI-issues/issues/21)
- [#14 - Right in Favorites initially shows only the platform](https://github.com/OnionUI/MainUI-issues/issues/14)
- [#11 - Game-list sorting is case-sensitive](https://github.com/OnionUI/MainUI-issues/issues/11)
- [#1 - Sleep Timer only goes right](https://github.com/OnionUI/MainUI-issues/issues/1)

The patcher does not claim to solve every symptom in every linked report. Each feature section above
states the specific behavior it changes.

## Recovery

If MainUI does not start or behaves incorrectly:

1. Power off the device.
2. Mount the SD card on another system.
3. Restore the untouched MainUI backup.
4. Restore the backed-up `favourite.json` if Favorite membership testing changed it.
5. Re-test with one `--include` selection at a time.

## Other collection optimizations

### Smaller Latin-only theme fonts

Latin-language users of the default OnionOS stock theme can create a much smaller font file by
retaining common Latin, punctuation, and currency ranges only:

```sh
python3 -m fontTools.subset wqy-microhei.ttc \
  --font-number=0 \
  --unicodes="U+0020-007E,U+00A0-00FF,U+0100-017F,U+2000-206F,U+20A0-20BF" \
  --layout-features="kern,liga" \
  --output-file=wqy-microhei-latin.ttf
```

Copy the result to `/mnt/SDCARD/miyoo/app/wqy-microhei-latin.ttf`, then update the theme's
`config.json` so `list.font` uses that path. For the other theme font entries, use
`/mnt/SDCARD/miyoo/app/Exo-2-Bold-Italic.ttf` instead of
`/mnt/SDCARD/miyoo/app/Exo-2-Bold-Italic_Universal.ttf` when the removed character coverage is not
needed. Keep the original fonts when Chinese, Russian, or other excluded scripts are required.

### Bounded PNG preview dimensions

Keep preview images at or below **250 pixels wide by 360 pixels high**. ImageMagick's `>` geometry
flag shrinks only oversized files and preserves their aspect ratio.

Linux/macOS shell, in place:

```sh
find "/path/to/Imgs" -type f -iname '*.png' -exec \
  magick mogrify -resize '250x360>' -strip \
  -define png:compression-level=9 {} +
```

Windows PowerShell, in place:

```powershell
Get-ChildItem "D:\Roms" -Filter *.png -Recurse | ForEach-Object {
    magick $_.FullName -resize "250x360>" -strip `
        -define "png:compression-level=9" $_.FullName
}
```

These commands preserve image proportions and never enlarge smaller images.

## Changelog

### 1.2 - 2026-08-28

- **New patch: `fix-wifi-network-shell-quoting`.** Shell-quote connect-time `wpa_cli` SSID/PSK 
  values so spaces and shell metacharacters remain literal.
  
- **New patch: `skip-inactive-theme-configs`.** Inactive themes no longer have their 
  configuration parsed at startup.

- **New patch: `patch-context-menu-selection-background`.** A theme may supply
  `skin/bg-list-popup-s.png` to style the selection highlight in popup and context menus.

- **New patch: `fix-recent-preview-paths`.** Preview for items in Recents should always work, 
  even if we start a game from Search.

- **New patch: `fix-search-favourite-status`.** Favorite handling for items in Search is fixed.

- **New patch: `fix-rom-rebuild-path-safety`.** Harden recursive scan-based `cache6.db`.

- **New theme setting: `gamelist.iconLeftMargin`.** A single absolute theme-author control
  for the game/folder icon left margin.

- **Wide game-icon spacers.** A wide `skin/icon-game.png` is now treated as a deliberate 
  horizontal spacer with scroll support.

- **Creating a Favorite folder left the selection in the wrong place.** After a folder is
  successfully created, the list selection now lands on the new folder.

- **ROM-list edge wraps no longer briefly expose unloaded destination rows.** Added a 
  60 ms readiness guard. The resolved forward/backward async-loader bodies are also 
  provenance-checked directly before publication.

- **Game-list right padding is now consistent between static and scrolling titles.** Tagged 
  folder, game, and no-icon rows share one row-count-aware right edge derived from the real 
  destination surface.


- **Titles that fit could still scroll.** The game-list marquee could arm on a title short
  enough to fit, most visibly right after a selection change while artwork was still loading. 
  Titles that fit no longer scroll.

- **Very small title-scroll idle values behaved erratically.** `idle` values below 10 ms are
  clamped to 10 ms (upper limit unchanged at 30000 ms). Zero or negative still disables
  scrolling.

- **Popup windows could reappear as an empty page.** A popup or confirmation window open when
  MainUI handed off to an external application could be written to the saved UI state and then 
  restored as an ordinary empty page carrying the popup's title.

- **Settings page could lose its title after a handoff.** A Settings window saved without a
  resolved title is now restored with its correct title.

- **A non-active theme could change the active theme's appearance.** Theme settings added by
  these patches are now staged per theme and applied only on an exact active-theme path
  match, with a reset to defaults first. 

- **Hot-path development logging extended.** The suppression selector now also removes 
  `TextMenu::freeListItem` per-row/final teardown output.

- **Favorite Game Details thumbnails no longer depend on a non-stock `imgpath` field.** Folder-aware
  Favorite `GameAction`s use the source emulator image directory whenever the Favorite record does not
  provide an explicit image path.

- **Recent thumbnail/lifetime hardening completed.** The list preview, initial RIGHT-open detail path,
  and Up/Down detail refresh share one explicit-PNG-or-source-directory rule. 

- **Theme setting `gamelist.bold: false` will now fully work.** 

- **Improved documentation for theme authors.**

- **Crash-regression checks strengthened.**

### 1.1 - 2026-08-21

- **Configurable Settings menu via `main-menu.json`.** Control the visibility and order of `shutdown`, 
  `brightness`, `wifi`, `display`, `themes`, `tweaks`, `language`, `sound`, `sleep`, and `about`. Object 
  form is default-on: omitted rows keep their normal state, `false` hides a row, and `true` keeps it 
  subject to the usual capability and launcher checks.

- **Updated Tweaks app.** The companion Tweaks build now exposes the new MainUI configurable Settings 
  menu support.

- **New optional `shutdown` action for the main SELECT/context menu.** It opens MainUI's stock Shutdown 
  confirmation dialog.

- **Improved Favorite-folder keyboard titles.** Folder creation and rename now show *Create folder* 
  and *Rename folder* instead of the generic *Search* keyboard title.

- **Fixed `use-rom-database-display-names`.** The selector previously had no runtime effect, so 
  short-name/Arcade systems could still perform the expensive browse-time `GetGameName` lookup even 
  when the final display title was already stored in `cache6.db`. Browsing now treats the database 
  `disp` value as authoritative, avoiding the cold Arcade-name initialization without changing the 
  stored titles.

- **Theme guidance for the restored Settings entries.** Themes should provide `skin/icon-theme.png` for 
  **Themes** and `skin/fixit.png` for **Tweaks**.

- **Patcher safety and validation improvements.** Direct-call guards now distinguish ARM `BL` calls from
  plain `B` branches instead of validating only the destination address. The patcher also verifies that
  `use-rom-database-display-names` actually emits its expected branch change, and AST-lints module-level
  patch scaffolding so abandoned `*_VA`, `*_WORD`, `*_WORDS`, `*_STOCK`, `*_STOCK_WORD`, `*_CALL`,
  `*_SITE`, and `*_SIG` constants cannot silently remain after an implementation block is removed.
  Definition-only module constants containing mangled C++ import symbols (`_Z...`) fail the same lint.
  The retained `DBCachedTextMenu::deleteRow` and shared TextMenu keymap signatures are now wired 
  into their live ownership checks rather than existing as documentation-only scaffolding. A broader
  report-only AST audit also exposes every definition-only module-level uppercase constant for
  maintainer review without treating provenance/state-layout records as build failures. Existing
  fail-closed stock-instruction checks, ownership guards, ELF/segment validation, embedded-core checks, 
  and post-emission safety audits remain in place.

### 1.0 - 2026-08-14

Initial release.

## Build reference

Default and explicit `--patch-all` builds are byte-identical. Focused selector combinations and
fail-closed mutation tests are exercised by the patcher itself. The 283, 285, and 354 clean/expert
reference hashes remain accepted inputs.

Validated output,

```text
MainUI-283-clean & MainUI-283-expert, bytes: 1,653,820
sha256sum: af6a118c566598bac0450a22c50e3ac16f46f1ee5f9fa3ac8cb597092354e293

MainUI-285-clean & MainUI-285-expert, bytes: 1,653,820
sha256sum: 7d8a73d0a18f5a4742e2c4c57f1e2daf189494c4e1e72c141da5118e33e7c597

MainUI-354-clean & MainUI-354-expert, bytes: 1,653,820
sha256sum: 801b26fe28e5cf314e2125036f17ca974e16bfa0bceac556cccdb16420cbd20e
```
> [!NOTE]
> The patcher normalizes the clean/expert difference, so both variants produce identical output.

## License

The patcher source code and its documentation are licensed under the GNU General Public License v3.0
only (`GPL-3.0-only`). See [`LICENSE`](./LICENSE).

Copyright (C) 2026 robcodedev

OnionOS, MainUI, Miyoo firmware, and related trademarks and binaries belong to their respective
owners. This project is independent and is not affiliated with or endorsed by OnionUI or Miyoo.

## Disclaimer

Fable, Opus and Sol had to help me out on this one. It was pretty hard to pinpoint the correct patch
locations in the binary. A lot had to be changed for this to work fully, especially the auto-scroll,
row count and favorite folder features.

This project is unofficial and is not affiliated with Miyoo or the OnionUI maintainers. The patches
modify a closed-source executable and may cause crashes, data loss, or incompatibility with future
builds. Use backups and test carefully.
