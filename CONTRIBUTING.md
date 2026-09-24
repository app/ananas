# Contributing to Ananas

## Branches

Work on short-lived topic branches off `master`, named `<type>/<slug>` using the
same types as the commits:

| Prefix      | Use for                          | Example                        |
| ----------- | -------------------------------- | ------------------------------ |
| `feat/`     | new functionality                | `feat/123-snap-package`        |
| `fix/`      | bug fix                          | `fix/empty-metadata-crash`     |
| `build/`    | build system, packaging          | `build/snapcraft-packaging`    |
| `ci/`       | CI workflows                     | `ci/deb-pipeline`              |
| `docs/`     | documentation                    | `docs/contributing`            |
| `refactor/` | internal rework, no behaviour    | `refactor/split-cfg-parser`    |
| `chore/`    | maintenance, dependencies        | `chore/bump-qt`                |
| `test/`     | tests only                       | `test/designer-roundtrip`      |

Use lowercase and hyphens, do not put `master`/`main` in the name, and add the
issue id when useful. Keep one concern per branch and delete it after it is
merged.

## Commits

- Conventional Commits, English (`feat:`, `fix:`, `build:`, `docs:`,
  `refactor:`, `chore:`, `ci:`).
- Prefer several focused commits over one large one.
- Never commit secrets or build artifacts.

## Building and testing

Everything is built, tested and packaged inside containers; no host Qt6/CMake
toolchain is needed:

```sh
bash tools/scripts/smoke.sh                # build + tests
bash tools/scripts/build-deb-worktree.sh   # .deb packages
bash tools/scripts/build-snap-worktree.sh  # snap package
```

See [`tools/README.md`](tools/README.md) for details.

## Pull requests

Open the pull request against `master`. GitHub Actions then runs:

- `ci` — verification build and the test suite;
- `snap` and `deb` — package builds, but only when the pull request touches the
  packaging inputs (a check that the packages still build).

Packages are published as CI artifacts only from `master`; pull requests publish
nothing. See [`tools/GITHUB-CI.md`](tools/GITHUB-CI.md).

## License

Ananas is released under the GNU General Public License v2. By contributing you
agree that your changes are licensed under the same terms.
