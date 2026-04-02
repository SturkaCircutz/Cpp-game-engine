# Contributing

## Branch model

- `master`: stable code that is safe to clone and build
- `develop`: integration branch for upcoming work
- `feature/<name>`: short-lived branches for individual tasks
- `release/<version>`: release hardening branches
- `hotfix/<name>`: urgent fixes that must land quickly

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

## Code style

- Keep comments in English
- Prefer small focused classes
- Avoid adding heavy dependencies without a clear reason
- Keep the engine buildable with CMake
