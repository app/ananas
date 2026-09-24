# Ananas — agent working agreement

Ananas is a Qt6 + CMake monorepo: the engine, the administrator, the designer,
the shared libraries, the plugins and the bundled `libqdataschema`. See
`README.md` and `tools/README.md` for the layout.

This file is the always-on working agreement for agents. Detailed,
task-specific playbooks live in skills under `.opencode/skills/` (for example
`feature-development` and `snapcraft-packager`).

## Building and checking (always in containers)

Builds, tests and packaging run inside containers — **no Qt6/CMake toolchain is
required on the host**, and host builds are not supported:

- `bash tools/scripts/smoke.sh` — CMake build + `ananas-test` under Xvfb
- `bash tools/scripts/smoke-designer.sh` — designer round-trip and app start
- `bash tools/scripts/build-deb.sh` / `build-deb-worktree.sh` — `.deb` into `dist/`
- `bash tools/scripts/build-snap.sh` / `build-snap-worktree.sh` — `.snap` into `dist/`

Do not run `cmake`, `make`, `qmake` or `lrelease` directly on the host.

## How to approach a task

- For a new feature or any non-trivial change, use plan mode first: explore the
  code, then write a short plan and get it approved before changing files.
- The planning phase ends with a temporary checklist in `tmp/<task-slug>.md`
  (see the `feature-development` skill). `tmp/` is gitignored, so trackers are
  never committed; update the file as work proceeds.
- Small, obvious changes do not need a tracker.

## Documentation and skills

- When behaviour or workflow changes, update `tools/README.md` and/or
  `README.md` / `README.ru.md`.
- Capture repeatable procedures as skills under
  `.opencode/skills/<name>/SKILL.md`.
- Available skills: `feature-development`, `snapcraft-packager`.

## Commits

- Conventional Commits, English (`feat:`, `fix:`, `build:`, `docs:`,
  `refactor:`, `chore:`, `ci:`).
- Commit only when explicitly asked; stage only the intended files; never commit
  secrets or build artifacts.
- Prefer several focused commits over one large one.

## Definition of done

- The change builds and the relevant checks pass inside the container.
- Documentation and, when applicable, skills are updated.
- `git status` is clean apart from intentionally uncommitted files; the diff has
  been reviewed.

## Language

Conversation is Russian; code, comments, commit messages and documentation are
English. See the global `~/.config/opencode/AGENTS.md`.
