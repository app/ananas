#!/usr/bin/env python3
"""Normalize the Qt4-format .ui files of the Ananas metadata editor for Qt6.

The designer's own forms are already Qt4 .ui files, but they still use Qt3
widget classes (Q3MainWindow, Q3Table, ...) and Qt3-only properties, which Qt6
uic/QFormBuilder cannot handle.  This script rewrites them in place:

  * Qt3 widget classes -> modern Qt6 classes
  * Q3MainWindow's child QWidget -> centralwidget
  * drops Q3 <customwidget> declarations and Qt3-only properties
  * drops <pixmapfunction> (Qt6 has no Q3 pixmap factory)

Usage: port-designer-ui.py <file.ui> [<file.ui> ...]
"""

import re
import sys

CLASS_MAP = {
    "Q3MainWindow": "QMainWindow",
    "Q3TextEdit": "QTextEdit",
    "Q3Table": "QTableWidget",
    "Q3ToolBar": "QToolBar",
    "Q3GroupBox": "QGroupBox",
    "Q3Frame": "QFrame",
    "Q3WidgetStack": "QStackedWidget",
    "Q3IconView": "QListWidget",
}

# Qt3-only properties with no Qt6 equivalent (values are dropped).
DROP_PROPERTIES = (
    "numRows", "numCols", "maxItemWidth", "maxItemTextLength", "itemsMovable",
    "label", "resizeMode", "focusStyle", "usesTextLabel", "drawFrame",
    "clicked", "textLabel", "movingEnabled", "newLine", "margin",
    "midLineWidth", "textFormat",
)

# Qt3 enum values used inside <enum> elements.
ENUM_MAP = {
    "Q3Table::Multi": "QAbstractItemView::MultiSelection",
    "Q3Table::Single": "QAbstractItemView::SingleSelection",
    "Q3Table::NoSelection": "QAbstractItemView::NoSelection",
}

# Qt3 type names that appear in signal/slot signatures.
TYPE_MAP = {
    "Q3IconViewItem": "QListWidgetItem",
    "Q3ListViewItem": "QTreeWidgetItem",
}


def convert(ui):
    for old, new in CLASS_MAP.items():
        ui = ui.replace('class="%s"' % old, 'class="%s"' % new)

    for old, new in ENUM_MAP.items():
        ui = ui.replace(old, new)
    ui = ui.replace("Q3Table::", "QAbstractItemView::")

    for old, new in TYPE_MAP.items():
        ui = ui.replace(old, new)

    if 'class="QMainWindow"' in ui:
        # Q3MainWindow had a single QWidget child; Qt4+ QMainWindow calls it
        # centralwidget and uic generates setCentralWidget() for it.
        ui = ui.replace('<widget class="QWidget" name="widget" >',
                        '<widget class="QWidget" name="centralwidget" >')

    # Drop Q3 custom widget declarations (class starts with Q3).
    ui = re.sub(r"\s*<customwidget>\s*<class>Q3[^<]*</class>.*?</customwidget>",
                "", ui, flags=re.DOTALL)

    ui = re.sub(r"\s*<pixmapfunction>[^<]*</pixmapfunction>", "", ui)

    # Drop <include> hints that name removed Qt3/Qt3Support headers.
    ui = re.sub(r'\s*<include[^>]*>\s*(?:q3|Q3|qworkspace|QWorkspace|Qt3Support)[^<]*</include>',
                "", ui)

    for prop in DROP_PROPERTIES:
        ui = re.sub(r'\s*<property name="%s"\s*>.*?</property>' % prop,
                    "", ui, flags=re.DOTALL)

    # readOnly is valid on QLineEdit/QTextEdit but not on QTableWidget; drop it
    # only inside table blocks (which contain no nested widgets).
    def strip_table_readonly(match):
        return re.sub(r'\s*<property name="readOnly"\s*>.*?</property>', "",
                      match.group(0), flags=re.DOTALL)

    ui = re.sub(r'<widget class="QTableWidget".*?</widget>',
                strip_table_readonly, ui, flags=re.DOTALL)

    return ui


def main(paths):
    for path in paths:
        with open(path, encoding="utf-8") as fh:
            ui = fh.read()
        new = convert(ui)
        if new != ui:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(new)
            print("converted %s" % path)
        else:
            print("unchanged %s" % path)
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.stderr.write("usage: port-designer-ui.py <file.ui> [...]\n")
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))
