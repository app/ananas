#!/usr/bin/env python3
"""Extract one form from an Ananas scheme .cfg into a standalone .ui file.

An Ananas scheme stores its dialog forms as escaped Qt Designer XML inside
<dialogform> elements, nested in <form id="..." name="..."> elements.  This
script pulls one form out (by numeric id or by name) and unescapes it, so it
can be opened with the Qt6 Designer wrapper or QFormBuilder.

Usage: extract-cfg-form.py <path-to-cfg> <id|name> <out.ui>
"""

import os
import re
import sys
from xml.sax.saxutils import unescape

# saxutils.unescape() only handles &amp; &lt; &gt;; schemes also escape quotes.
UNESCAPE_ENTITIES = {"&quot;": '"', "&apos;": "'"}

FORM_RE = re.compile(r"<form\b([^>]*)>(.*?)</form>", re.DOTALL)
DIALOGFORM_RE = re.compile(r"<dialogform\b[^>]*>(.*?)</dialogform>", re.DOTALL)
ATTR_RE = re.compile(r'(\w+)\s*=\s*"([^"]*)"')


def attrs(text):
    return dict(ATTR_RE.findall(text))


def select(selector):
    # Numeric selectors match the form id, everything else the form name.
    if selector.isdigit():
        return lambda a: a.get("id") == selector
    return lambda a: a.get("name") == selector


def main(path, selector, out):
    with open(path, encoding="utf-8") as fh:
        data = fh.read()

    match = select(selector)
    for form_match in FORM_RE.finditer(data):
        form_attrs = attrs(form_match.group(1))
        if not match(form_attrs):
            continue

        dialog_match = DIALOGFORM_RE.search(form_match.group(2))
        if not dialog_match:
            sys.stderr.write("form %s has no dialogform\n" % form_attrs)
            return 1

        ui = unescape(dialog_match.group(1), UNESCAPE_ENTITIES)
        with open(out, "w", encoding="utf-8") as fh:
            fh.write(ui)
        print("extracted form id=%s name=%s -> %s" % (
            form_attrs.get("id"), form_attrs.get("name"), out))
        return 0

    sys.stderr.write("form %r not found in %s\n" % (selector, path))
    return 1


if __name__ == "__main__":
    if len(sys.argv) != 4:
        sys.stderr.write("usage: extract-cfg-form.py <cfg> <id|name> <out.ui>\n")
        sys.exit(2)
    sys.exit(main(sys.argv[1], sys.argv[2], sys.argv[3]))
