#!/usr/bin/env python3
"""Convert a MySQL dump (as shipped with the inventory scheme) into a
SQLite-compatible INSERT-only script.

Only the data is kept: the internal (SQLite) database structure is created by
the application from the scheme metadata.  MySQL specifics are dropped
(backticks, LOCK/UNLOCK, ENGINE, /*! ... */ comments) and MySQL string escapes
are translated to SQLite (\\" -> ", \\' -> '').

The generated INSERTs carry an explicit column list taken from the dump's
CREATE TABLE statements.  The internal scheme tables can have more columns
than the old dump (the metadata evolved), so SQLite then fills the extra
columns with their defaults instead of failing on a value-count mismatch.

Usage: port-demo-sql.py <mysql-dump.sql> <out.sql>
"""

import re
import sys


def table_columns(data):
    """Map table name -> list of columns, from the CREATE TABLE statements."""
    columns = {}
    for m in re.finditer(r"CREATE TABLE\s+`([^`]+)`\s*\((.*?)\)\s*ENGINE", data, re.DOTALL):
        name, body = m.group(1), m.group(2)
        cols = re.findall(r"^\s*`([^`]+)`\s+\w", body, re.MULTILINE)
        if cols:
            columns[name] = cols
    return columns


def convert_statement(stmt):
    # MySQL escapes a double quote inside single-quoted strings; SQLite treats
    # it literally.  A single quote is escaped by doubling it.
    stmt = stmt.replace('\\"', '"')
    stmt = stmt.replace("\\'", "''")
    stmt = stmt.replace("\\\\", "\\")
    return stmt


def main(src, dst):
    with open(src, encoding="utf-8", errors="replace") as fh:
        data = fh.read()

    columns = table_columns(data)
    out = []

    for line in data.splitlines():
        stripped = line.strip()
        m = re.match(r"INSERT INTO\s+`([^`]+)`\s+VALUES\s+(.*)", stripped, re.IGNORECASE)
        if not m:
            continue
        table, values = m.group(1), m.group(2)
        values = convert_statement(values)
        if not values.endswith(";"):
            values += ";"
        if table in columns:
            collist = ", ".join(columns[table])
            out.append("INSERT INTO %s (%s) VALUES %s" % (table, collist, values))
        else:
            out.append("INSERT INTO %s VALUES %s" % (table, values))

    with open(dst, "w", encoding="utf-8") as fh:
        fh.write("\n".join(out) + "\n")
    print("wrote %d INSERT statement(s) to %s" % (len(out), dst))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.stderr.write("usage: port-demo-sql.py <mysql-dump.sql> <out.sql>\n")
        sys.exit(2)
    main(sys.argv[1], sys.argv[2])
