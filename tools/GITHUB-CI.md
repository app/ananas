# GitHub Actions CI

Continuous integration for Ananas lives in `.github/workflows/`. Everything is
built inside containers, so the runners only need Docker; no host Qt6/CMake
toolchain is involved.

## Workflows

| Workflow  | File               | What it does                                             |
| --------- | ------------------ | -------------------------------------------------------- |
| `ci`      | `.github/workflows/ci.yml`  | Verification build: CMake + `ananas-test` (`smoke.sh`)   |
| `snap`    | `.github/workflows/snap.yml`| Builds the `.snap` in the Snapcraft OCI image            |
| `deb`     | `.github/workflows/deb.yml` | Builds the `.deb` packages in the Qt6 image              |

## When the workflows run

| Event                                   | `ci` (verify) | `snap` / `deb` (build packages) | Artifacts uploaded |
| --------------------------------------- | ------------- | ------------------------------- | ------------------ |
| Pull request, source changes            | yes           | no                              | no                 |
| Pull request, packaging changes         | yes           | yes (build check only)          | no                 |
| Push to `master` (source or packaging)  | yes           | yes                             | **yes**            |
| Manual `workflow_dispatch`              | yes           | yes                             | only on `master`   |

Packages are never published from a pull request: a PR that touches the
packaging inputs only proves that the packages still build. Artifacts are
uploaded when `github.ref == 'refs/heads/master'` (this covers a push to
`master` and a manual run started from `master`).

Push triggers are limited to `master`, so topic branches (`<type>/<slug>`, see
`CONTRIBUTING.md`) run only through pull requests.

## Path sets

Pull requests run the packaging workflows only when the packaging inputs change:

- `snap/**`, `build/ubuntu/**`, `debian`
- `tools/scripts/build-snap*.sh`, `tools/scripts/build-deb*.sh`,
  `tools/docker/**`
- `.snapcraftignore`, `**/CMakeLists.txt`, `VERSION`,
  `.github/workflows/**`

The `ci` workflow and the `master` push triggers use the wider set
(`src/**`, `applications/**`, `translations/**`, `snap/**`, `build/**`,
`tools/**`, `**/CMakeLists.txt`, `VERSION`, `.snapcraftignore`,
`.github/workflows/**`).

## Artifacts

Built from `master` and kept for the default 90 days:

- `ananas-snap-amd64` — `dist/ananas_<version>_amd64.snap`
- `ananas-deb-amd64` — `dist/ananas_*.deb` and `dist/libqdataschema_*.deb`

The snap version is `<VERSION>+git<YYYYMMDD>.<short-sha>`; the build scripts
compute it and pass it to Snapcraft as `ANANAS_VERSION`.

## Running the same checks locally

```sh
bash tools/scripts/smoke.sh                # same as the ci workflow
bash tools/scripts/build-snap-worktree.sh  # same as snap
bash tools/scripts/build-deb-worktree.sh   # same as deb
```

The worktree scripts build exactly what is checked out (they do not use
`git archive`), which is what a CI runner needs. `build-deb-worktree.sh` builds
the `ananas-qt6-builder` image on demand; `build-snap-worktree.sh` pulls the
official Snapcraft image automatically.

## Adding or changing a workflow

1. Keep triggers and path filters in sync with the table above.
2. Prefer the `tools/scripts/*` wrappers over inlining container commands, so
   local and CI runs stay identical.
3. Update this file (and, if user-facing, `tools/README.md`).

## Not implemented

- Publishing to the Snap Store or to a package repository.
- arm64 builds: `snap.yml` contains a commented `remote-build` job that needs
  Launchpad credentials.
- Cross-workflow caching of the container images.
