#!/usr/bin/env python3
"""Mechanical Qt3 -> Qt6 replacements for the Ananas metadata editor sources.

This handles the unambiguous class renames only.  Anything whose API changed
(collections, Q3Sql*, layout/table methods, ...) is left for manual porting so
that the compiler points at the real work.

Usage: port-designer-q3.py <file.cpp|file.h> [...]
"""

import re
import sys

# Longest names first so that e.g. Q3ListViewItem is not caught by Q3ListView.
RENAMES = (
    ("Q3CheckTableItem", "QTableWidgetItem"),
    ("Q3ListViewItem", "QTreeWidgetItem"),
    ("Q3IconViewItem", "QListWidgetItem"),
    ("Q3ListView", "QTreeWidget"),
    ("Q3IconView", "QListWidget"),
    ("Q3MainWindow", "QMainWindow"),
    ("Q3TextEdit", "QTextEdit"),
    ("Q3TextStream", "QTextStream"),
    ("Q3Table", "QTableWidget"),
    ("Q3PopupMenu", "QMenu"),
    ("Q3ToolBar", "QToolBar"),
    ("Q3GroupBox", "QGroupBox"),
    ("Q3Frame", "QFrame"),
    ("Q3GridLayout", "QGridLayout"),
    ("Q3VBox", "QVBoxLayout"),
    ("Q3HBox", "QHBoxLayout"),
    ("Q3WidgetStack", "QStackedWidget"),
    ("Q3FileDialog", "QFileDialog"),
    ("Q3Url", "QUrl"),
)

PLAIN = (
    ("Qt::WType_TopLevel", "Qt::WindowFlags()"),
    ("QString::null", "QString()"),
    # Qt3 QString API removed in Qt5/Qt6.
    (".stripWhiteSpace()", ".trimmed()"),
    (".local8Bit()", ".toLocal8Bit()"),
    (".lower()", ".toLower()"),
    (".upper()", ".toUpper()"),
    # QTextEdit/QPlainTextEdit fields: text()/setText() -> toPlainText()/setPlainText().
    ("eDescription->text()", "eDescription->toPlainText()"),
    ("eDescription->setText(", "eDescription->setPlainText("),
    ("eFormSource->text()", "eFormSource->toPlainText()"),
    ("eFormSource->setText(", "eFormSource->setPlainText("),
    ("eModule->text()", "eModule->toPlainText()"),
    ("eModule->setText(", "eModule->setPlainText("),
    ("eClientModule->text()", "eClientModule->toPlainText()"),
    ("eClientModule->setText(", "eClientModule->setPlainText("),
    ("eServerModule->text()", "eServerModule->toPlainText()"),
    ("eServerModule->setText(", "eServerModule->setPlainText("),
    # QAction::activated() -> triggered() in Qt5/Qt6.
    ("SIGNAL(activated())", "SIGNAL(triggered())"),
)

# Drop Qt3Support / Q3 includes; the modern include is added by hand.
INCLUDE_RE = re.compile(r"#include\s*[<\"](?:q3|Qt3Support)[A-Za-z0-9_./]*[>\"]\n")


def convert(text):
    for old, new in RENAMES:
        text = re.sub(r"\b%s\b" % re.escape(old), new, text)
    for old, new in PLAIN:
        text = text.replace(old, new)
    text = re.sub(r"\bTRUE\b", "true", text)
    text = re.sub(r"\bFALSE\b", "false", text)
    text = INCLUDE_RE.sub("", text)
    return text


def main(paths):
    for path in paths:
        with open(path, encoding="utf-8") as fh:
            text = fh.read()
        new = convert(text)
        if new != text:
            with open(path, "w", encoding="utf-8") as fh:
                fh.write(new)
            print("converted %s" % path)
        else:
            print("unchanged %s" % path)
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.stderr.write("usage: port-designer-q3.py <file> [...]\n")
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))
