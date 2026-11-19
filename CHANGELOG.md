# ToDoOs
## Unreleased
- Fix: todoos exits with 0 on success and 1 when the command fails (it always exited with 1), so
  `todoos … && …` chains work. A missing theme is now a notice, not an error.
- Zsh sync wrapper (`Linux/Zsh/zsh_todoos_sync`): `todoos`, `t`, `ta`… now return todoos' exit code
  instead of the background unison call's (always 0). Reload it after upgrading.
- **Breaking:** `move-to {list} {ids}` / `copy-to {list} {ids}` are replaced by
  `move {ids} --to {list}` / `copy {ids} --to {list}` (short option `-t`, aliases `mv` / `cp`).
- Help: archive flag shown as `-A` (was wrongly `-a`); `describe` no longer lists flags it ignores.
- **Breaking (data):** archived and deleted items now get a 5-char id (`<4-char id><[a-z]>`), so they
  no longer use up the 4-char ids of the active list. Descriptions follow the renamed item.
  `restore` gives back the original 4-char id when free, otherwise a new one (printed).
  The ID column widens to 7 chars when showing archived/deleted items.
  **Migrate existing data before upgrading:** run `scripts/migrate-archive-ids.py --dry-run`, then
  `scripts/migrate-archive-ids.py` on one machine, let the sync finish, then install the new binary
  everywhere. Existing archived ids become `<id>a`, deleted ids `<id>d` (next free letter on conflict).
  Until then, todoos refuses to run and asks for the migration (temporary check on the current list).
- Move, copy and duplicate are rejected for archived/deleted items (restore them first).
- Auto-archive on completion only applies to active items (it used to duplicate archived ones).
- Fix: id generation no longer returns a colliding id after 50 failed attempts; it raises an error.

## v.0.1.0
- Initial pre-release version
