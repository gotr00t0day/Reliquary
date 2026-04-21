# Reliquary

Scans shell and application history files across all users on a Linux system for credentials, tokens, secrets, and authenticated network requests left in plaintext.

## What It Does

Enumerates every user under `/home/`, then reads their history files looking for two categories of sensitive data:

**1. Credential patterns** — lines containing keywords like `password`, `token`, `secret`, `api_key`, `auth`, `credential`, `private_key`, `access_key`, MySQL `IDENTIFIED BY` clauses, and MongoDB `db.auth()` calls.

**2. Authenticated curl/wget commands** — lines where a user ran a network request and passed credentials inline:
- `-H "Authorization: Bearer <token>"`
- `-H "X-Api-Key: <key>"`
- `--header="Authorization: ..."`
- `-u user:password` / `--user user:password`
- `--oauth2-bearer <token>`
- `wget --header="Authorization: ..."`

## Build

```bash
g++ reliquary.cpp -o reliquary -std=c++20 -pthread
```

## Usage

```bash
./reliquary
```

Prints all matching lines to stdout, one per line. Redirect to a file to save results:

```bash
./reliquary > hits.txt
```

## Notes

- Requires read access to `/home/<user>/` directories — run as root for full coverage
- History files are almost never treated as sensitive by users, making tokens found here likely still valid and unrotated
- `zsh_history` with `EXTENDED_HISTORY` enabled also includes timestamps per line, which does not affect matching
