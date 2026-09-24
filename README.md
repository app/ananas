# Ananas

Ananas is a free accounting automation platform for Linux and Windows. This
repository contains the whole system: the engine, the
administrator, the designer, the libraries and the bundled `qdataschema`.

Built with **Qt6** and **CMake** (qmake is no longer used).

## Layout

- `src/` — engine, libraries, plugins, designer, extensions, tests
- `src/qdataschema/` — libqdataschema
- `applications/` — the inventory business scheme
- `build/` — packaging (Debian/Ubuntu; legacy RPM/Inno/menus)
- `snap/` — Snapcraft manifest, GUI entries and wrappers
- `tools/` — build, test and packaging tooling

## Building and testing

By default the project is built, tested and packaged in a container (Podman);
no Qt6/CMake toolchain is required on the host. See
[`tools/README.md`](tools/README.md).

## License

GNU General Public License v2. See [`COPYING`](COPYING) (English) and
[`COPYING.RU`](COPYING.RU) (Russian translation).
