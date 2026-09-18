#!/usr/bin/env python3
"""Convert the Qt3 .ui forms embedded in an Ananas scheme .cfg to the Qt4/Qt6
format understood by QFormBuilder.

An Ananas scheme stores its dialog forms as escaped Qt Designer XML inside
<dialogform> elements.  Schemes authored with the Qt3-era designer still carry
the Qt3 .ui format (<!DOCTYPE UI><UI version="3.3">), which Qt6's QFormBuilder
cannot parse.

This script extracts every non-empty <dialogform>, runs the Qt4 `uic3 -convert`
converter on it, normalizes the result for Qt6 and writes it back.  It must run
where `uic3` is available (the Qt4 image).

Usage: port-cfg-ui.py <path-to-cfg>
"""

import os
import re
import subprocess
import sys
import tempfile
from xml.sax.saxutils import unescape

# saxutils.unescape() only handles &amp; &lt; &gt; by default; some schemes also
# escape the quotes.
UNESCAPE_ENTITIES = {"&quot;": '"', "&apos;": "'"}


def escape_text(text):
    # The original files escape only '&' and '<' (not '>'); keep that style.
    return text.replace("&", "&amp;").replace("<", "&lt;")


def normalize(ui):
    # Qt6 has no QLayoutWidget.
    ui = ui.replace("QLayoutWidget", "QWidget")

    # Drop the stale Qt3 custom widgets entirely.
    ui = re.sub(
        r"\s*<customwidget>\s*<class>Q3(?:Frame|Table)</class>.*?</customwidget>",
        "",
        ui,
        flags=re.DOTALL,
    )

    # wDBTable is a QTableWidget wrapper now.
    ui = re.sub(
        r"(<customwidget>\s*<class>wDBTable</class>\s*<extends>)Q3Table(</extends>)",
        r"\1QTableWidget\2",
        ui,
        flags=re.DOTALL,
    )

    # Qt3 pixmap function / Qt3Support includes.
    ui = re.sub(r"\s*<pixmapfunction>[^<]*</pixmapfunction>", "", ui)
    ui = re.sub(r"\s*<include[^>]*>Qt3Support/[^<]*</include>", "", ui)

    # Any leftover direct Q3 widget references.
    ui = ui.replace('class="Q3Frame"', 'class="QFrame"')
    ui = ui.replace('class="Q3Table"', 'class="QTableWidget"')
    ui = ui.replace('class="Q3GroupBox"', 'class="QGroupBox"')
    ui = ui.replace('class="Q3DateEdit"', 'class="QDateEdit"')

    # Qt3 Q3Table::focusStyle has no equivalent on QTableWidget.
    ui = re.sub(r"\s*<property name=\"focusStyle\">.*?</property>", "", ui, flags=re.DOTALL)

    # Qt3 <cstring> element -> Qt4 <string>.
    ui = re.sub(r"<cstring\s*/>", "<string/>", ui)
    ui = ui.replace("<cstring>", "<string>").replace("</cstring>", "</string>")

    return ui


def run_uic3(raw):
    with tempfile.TemporaryDirectory() as tmp:
        src = os.path.join(tmp, "in.ui")
        out = os.path.join(tmp, "out.ui")
        with open(src, "w", encoding="utf-8") as fh:
            fh.write(raw)
        subprocess.check_call(["uic3", "-convert", src, "-o", out])
        with open(out, encoding="utf-8") as fh:
            return fh.read()


def process_block(inner):
    raw = unescape(inner, UNESCAPE_ENTITIES)
    # Qt3 forms use <!DOCTYPE UI>/<UI ...>; Qt4/Qt6 forms use lowercase <ui>.
    if re.search(r"<!DOCTYPE\s+UI\b|<\s*UI\b", raw):
        raw = run_uic3(raw)
    elif "<ui" not in raw:
        return None  # empty or unknown
    return escape_text(normalize(raw))


def main(path):
    with open(path, encoding="utf-8") as fh:
        data = fh.read()

    pattern = re.compile(r"(<dialogform\b[^>]*>)(.*?)(</dialogform>)", re.DOTALL)
    converted = 0

    def repl(match):
        nonlocal converted
        new = process_block(match.group(2))
        if new is None:
            return match.group(0)
        converted += 1
        return match.group(1) + new + match.group(3)

    data = pattern.sub(repl, data)
    with open(path, "w", encoding="utf-8") as fh:
        fh.write(data)

    print("processed %d dialogform(s) in %s" % (converted, path))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.stderr.write("usage: port-cfg-ui.py <path-to-cfg>\n")
        sys.exit(2)
    main(sys.argv[1])
