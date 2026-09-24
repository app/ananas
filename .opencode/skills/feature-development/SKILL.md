---
name: feature-development
description: >-
  Use when starting a new feature or a non-trivial change in the Ananas
  project — how to plan, implement in the container, verify, document and
  commit. Triggers: new feature, refactor, large task, plan, temporary plan
  file, tmp tracker, definition of done.
---

# Feature development in Ananas

Playbook for new features and other non-trivial changes. `AGENTS.md` holds the
short, always-on rules; this skill is the detailed checklist. For snap-specific
work also load the `snapcraft-packager` skill.

## When to use

A new feature, a refactor, a packaging change, or anything touching more than a
couple of files. For a small, obvious fix the process can be abbreviated (skip
the tracker), but the container checks still apply.

## Phases

### 1. Understand

- Explore the relevant code and find analogous existing implementations; follow
  their patterns (dialogs, forms, plugins, extensions).
- Confirm the baseline builds with the container tooling
  (`bash tools/scripts/smoke.sh`).
- No file changes in this phase.

### 2. Plan (ends with a tracker file)

- Write a short plan: scope, affected files, deliverables, risks, open
  questions. Get it approved before implementing.
- **The planning phase ends with creating a temporary tracker file
  `tmp/<task-slug>.md`** used to track execution. It is a scratch file: `tmp/`
  is gitignored, the tracker is never committed, and it can be deleted once the
  task is done.
- Update the tracker in real time as steps complete; note blockers and
  decisions in it.

Suggested tracker shape:

```markdown
# <task title>

## Goal

## Decisions

## Steps

- [ ] step 1
- [ ] step 2

## Definition of done

- [ ] builds/tests pass in the container
- [ ] docs/skills updated
- [ ] focused commits prepared
```

### 3. Implement

- Make small, reviewable changes; follow the conventions of neighbouring files.
- Do not add comments unless they explain non-obvious intent.
- Keep the tracker current.

### 4. Verify

- Run the container checks relevant to the change:
  `smoke.sh`, `smoke-designer.sh`, `build-deb*.sh`, `build-snap*.sh`.
- Never build on the host (`cmake`, `make`, `qmake`, `lrelease`).
- Fix failures; do not commit a red state.

### 5. Document

- Update `tools/README.md` and/or `README.md` / `README.ru.md` when behaviour or
  workflow changes.
- When CI workflows change, update `tools/GITHUB-CI.md`.
- Add or update a skill under `.opencode/skills/` for repeatable procedures.

### 6. Commit

- Conventional Commits, English; commit only when explicitly asked.
- Split logically by concern; stage only the intended files; never stage `tmp/`.
- Review the diff and `git log` before reporting.

## Definition of done

- [ ] the change builds and the relevant checks pass in the container
- [ ] documentation and, when applicable, skills are updated
- [ ] commits are focused and follow Conventional Commits
- [ ] `git status` is clean apart from intentionally uncommitted files

## Anti-patterns

- Building with `cmake`/`make` on the host.
- Committing `tmp/` trackers or build artifacts.
- Large changes without a plan or a tracker.
- Committing secrets or keys.
