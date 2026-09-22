#!/usr/bin/env python3
"""Strip the iCCP chunk from PNG files.

libpng warns about an "incorrect sRGB profile" for PNGs that carry a stale
iCCP chunk.  The Ananas splash screens have one; dropping the chunk removes
the warning without changing the pixels.

Usage: strip-png-iccp.py <file.png> [...]
"""

import struct
import sys

SIGNATURE = b"\x89PNG\r\n\x1a\n"


def strip(path):
    with open(path, "rb") as fh:
        data = fh.read()
    if data[:8] != SIGNATURE:
        return False

    out = bytearray(SIGNATURE)
    pos = 8
    removed = False
    while pos + 8 <= len(data):
        length = struct.unpack(">I", data[pos:pos + 4])[0]
        ctype = data[pos + 4:pos + 8]
        end = pos + 12 + length
        if ctype != b"iCCP":
            out += data[pos:end]
        else:
            removed = True
        pos = end
        if ctype == b"IEND":
            break

    if removed:
        with open(path, "wb") as fh:
            fh.write(out)
    return removed


def main(paths):
    for path in paths:
        print(("stripped " if strip(path) else "unchanged ") + path)
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.stderr.write("usage: strip-png-iccp.py <file.png> [...]\n")
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))
