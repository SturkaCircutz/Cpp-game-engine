# Contributing

## Branch model

- `master`: stable code that is safe to clone and build
- `develop`: integration branch for upcoming work
- `feature/<name>`: short-lived branches for individual tasks
- `chore/<name>`: tooling, repository, CI, and build maintenance
- `fix/<name>`: normal bug fixes that are not urgent production incidents
- `release/<version>`: release hardening branches
- `hotfix/<name>`: urgent fixes that must land quickly

## Branch naming guide

- Use `feature/*` for engine capabilities and player-facing behavior changes
- Use `chore/*` for CMake, CI, dependency setup, docs, and repository structure
- Use `fix/*` for ordinary defects in gameplay, rendering, or engine logic
- Use `hotfix/*` only for critical issues that must be patched immediately

Examples:

- `feature/chunk-system`
- `feature/block-interaction`
- `feature/player-collision`
- `chore/windows-build-fix`
- `chore/github-actions`
- `fix/camera-drift`
- `hotfix/startup-crash`

## Recommended workflow

1. Branch from `develop` for normal feature work.
2. Open a pull request back into `develop`.
3. Stabilize a version on `release/<version>`.
4. Merge validated release changes into `master`.

## Commit style

Use short imperative commit messages.

Examples:

- `Add chunk storage skeleton`
- `Implement player collision`
- `Fix camera pitch clamp`
- `Configure Windows build for GLFW`
- `Add GitHub Actions build workflow`

## Code style

- Keep comments in English
- Prefer small focused classes
- Avoid adding heavy dependencies without a clear reason
- Keep the engine buildable with CMake
