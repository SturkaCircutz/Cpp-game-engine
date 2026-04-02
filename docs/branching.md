# Branching Strategy

This repository uses a lightweight industrial workflow suited for a small C++ engine project.

## Shared branches

- `master` stores the most stable revision
- `develop` collects tested feature work for the next version
- `release/<version>` is used for release preparation

## Working branches

- `feature/<topic>` for new features
- `fix/<topic>` for standard bug fixes
- `hotfix/<topic>` for urgent production fixes
- `chore/<topic>` for tooling, docs, and maintenance

## Branch selection rules

- `feature/*` changes engine behavior or adds gameplay capability
- `chore/*` improves workflow, build setup, CI, documentation, or repository structure
- `fix/*` repairs a bug without treating it as an emergency release issue
- `hotfix/*` is reserved for severe problems that should patch stable code quickly

## Example names

- `feature/chunk-system`
- `feature/block-interaction`
- `feature/render-pipeline`
- `chore/windows-build-fix`
- `chore/github-actions`
- `fix/player-movement`
- `hotfix/crash-on-startup`

## Merge rules

- Merge feature branches into `develop`
- Merge fix branches into `develop`
- Merge chore branches into `develop`
- Merge release branches into `master`
- Merge hotfix branches into `master`
- Back-merge release and hotfix changes into `develop`

## Suggested first release line

- `release/0.1.0` for the first playable prototype milestone
