#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Resize the current theme's list assets to match a configured ROM-list row count.
# The three icon assets use one-time .bak originals. bg-list-s.png is never
# modified; for non-stock row counts it is used to generate bg-list-s_N.png.
# Usage: rescale_theme_list_icons.sh THEME_PATH MAX_HEIGHT ROW_COUNT

set -u

PNGRESIZE="${PNGRESIZE:-/mnt/SDCARD/.tmp_update/bin/pngresize}"

theme_path=${1:-}
max_height=${2:-}
row_count=${3:-}

if [ -z "$theme_path" ]; then
    echo "Missing theme path" >&2
    exit 2
fi

case "$max_height" in
    ''|*[!0-9]*)
        echo "Invalid max height: $max_height" >&2
        exit 2
        ;;
esac
case "$row_count" in
    ''|*[!0-9]*)
        echo "Invalid row count: $row_count" >&2
        exit 2
        ;;
esac

if [ "$max_height" -le 0 ]; then
    echo "Invalid max height: $max_height" >&2
    exit 2
fi
if [ "$row_count" -lt 6 ] || [ "$row_count" -gt 20 ]; then
    echo "Invalid row count: $row_count" >&2
    exit 2
fi

# Remove exactly one trailing slash so relative paths remain predictable.
theme_path=${theme_path%/}

restore_one()
{
    relative=$1
    file="$theme_path/$relative"
    backup="$file.bak"

    # Six rows is the stock geometry. Restore only backups that already
    # exist; do not create new backups and do not invoke pngresize.
    [ -f "$backup" ] || return 0
    cp -p "$backup" "$file"
}

if [ "$row_count" -eq 6 ]; then
    status=0
    restore_one "skin/icon-folder.png" || status=1
    restore_one "skin/icon-game.png" || status=1
    restore_one "skin/ic-favorite-mark.png" || status=1
    # bg-list-s.png is the untouched stock source and needs no restore.
    exit "$status"
fi

if [ ! -x "$PNGRESIZE" ]; then
    echo "pngresize not found or not executable: $PNGRESIZE" >&2
    exit 3
fi

png_width()
{
    # PNG IHDR width is the four-byte big-endian value at offset 16.
    # BusyBox od is available in Onion's runtime environment.
    set -- $(od -An -tu1 -j16 -N4 "$1" 2>/dev/null)
    [ "$#" -eq 4 ] || return 1
    echo $(( $1 * 16777216 + $2 * 65536 + $3 * 256 + $4 ))
}

resize_icon()
{
    relative=$1
    file="$theme_path/$relative"
    backup="$file.bak"

    if [ ! -f "$backup" ]; then
        [ -f "$file" ] || return 0
        if ! cp -p "$file" "$backup"; then
            echo "Could not create backup: $backup" >&2
            return 1
        fi
    fi

    width=$(png_width "$backup") || {
        echo "Could not read PNG width: $backup" >&2
        return 1
    }
    [ "$width" -gt 0 ] || return 1

    tmp="$file.tmp.$$"
    rm -f "$tmp"

    # Default pngresize mode preserves aspect ratio, fits within the
    # original width x requested row height, and never upscales.
    if ! "$PNGRESIZE" --quiet \
            "$backup" "$tmp" "$width" "$max_height"; then
        rm -f "$tmp"
        return 1
    fi

    if ! mv -f "$tmp" "$file"; then
        rm -f "$tmp"
        return 1
    fi
    return 0
}

resize_row_background()
{
    source="$theme_path/skin/bg-list-s.png"
    target="$theme_path/skin/bg-list-s_${row_count}.png"

    # do not resize if already present
    [ ! -f "$target" ] || return 0

    # Keep bg-list-s.png untouched and do not create a .bak for it.
    [ -f "$source" ] || return 0

    width=$(png_width "$source") || {
        echo "Could not read PNG width: $source" >&2
        return 1
    }
    [ "$width" -gt 0 ] || return 1

    tmp="$target.tmp.$$"
    rm -f "$tmp"

    # Preserve the source width while clamping only the height.
    if ! "$PNGRESIZE" --quiet --stretch-no-upscale \
            "$source" "$tmp" "$width" "$max_height"; then
        rm -f "$tmp"
        return 1
    fi

    if ! mv -f "$tmp" "$target"; then
        rm -f "$tmp"
        return 1
    fi
    return 0
}

status=0
resize_icon "skin/icon-folder.png" || status=1
resize_icon "skin/icon-game.png" || status=1
resize_icon "skin/ic-favorite-mark.png" || status=1
resize_row_background || status=1

exit "$status"
