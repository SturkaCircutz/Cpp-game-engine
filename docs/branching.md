# Branching Strategy

This repository uses a lightweight industrial workflow suited for a small C++ engine project.

## Shared branches

- `master` stores the most stable revision
- `develop` collects tested feature work for the next version
- `release/<version>` is used for release preparation

## Working branches

- `feature/<topic>` for new features
- `hotfix/<topic>` for urgent production fixes
- `chore/<topic>` for tooling, docs, and maintenance

## Merge rules

- Merge feature branches into `develop`
- Merge release branches into `master`
- Back-merge release and hotfix changes into `develop`

## Suggested first release line

- `release/0.1.0` for the first playable prototype milestone
