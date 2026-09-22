# Ананас

Ананас — свободная платформа автоматизации учёта для Linux и Windows. Этот
репозиторий содержит всю систему: движок, администратор,
дизайнер, библиотеки и встроенную `qdataschema`.

Сборка — **Qt6** и **CMake** (qmake больше не используется).

## Состав

- `src/` — движок, библиотеки, плагины, дизайнер, расширения, тесты
- `src/qdataschema/` — libqdataschema
- `applications/` — бизнес-схема «Оперативный учёт» (inventory)
- `build/` — упаковка (Debian/Ubuntu; легаси RPM/Inno/меню)
- `tools/` — сборка, тесты и упаковка

## Сборка и тесты

См. [`tools/README.md`](tools/README.md).

## Лицензия

GNU General Public License v2. См. [`COPYING`](COPYING) (английский) и
[`COPYING.RU`](COPYING.RU) (русский перевод).
