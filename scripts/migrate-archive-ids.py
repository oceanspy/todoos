#!/usr/bin/env python3
"""One-shot migration: give archived / deleted items a 5-char id.

Since archived and deleted items carry `<4-char id><[a-z]>`, the active list owns the whole 4-char
id space. This script converts data written by older versions:

  - `.archive_<list>.<ext>`: 4-char id `abcd` -> `abcda`
  - `.del_<list>.<ext>`:     4-char id `abcd` -> `abcdd`
  - `descriptions/<list>/abcd.md` is renamed to follow its item

When the preferred letter is already used for that list (an id archived several times, or present
in both archive and delete), the next free letter is used, so every id stays unique across the
archive + delete lists. Suffixes are deterministic (file order), so running it on two synced
machines gives identical results. It is idempotent: a second run changes nothing.

Rollout: run it on one machine, let the sync finish, then install the new todoos binary everywhere.

Usage:
  scripts/migrate-archive-ids.py --dry-run      # show what would change
  scripts/migrate-archive-ids.py                # migrate (backup folder is printed)
  scripts/migrate-archive-ids.py --dir PATH     # data dir (default: appDirStorage from config)
"""

import argparse
import json
import re
import shutil
import sys
import time
from pathlib import Path

CONFIG_FILE = Path.home() / ".oceanspy" / "todoos" / "config.conf"
DEFAULT_DATA_DIR = Path.home() / ".todoos"
VARIANTS = {".archive_": "a", ".del_": "d"}  # archive is processed before delete
EXTENSIONS = (".csv", ".json")
LETTERS = "abcdefghijklmnopqrstuvwxyz"
LEGACY_ID = re.compile(r"^[A-Za-z0-9]{4}$")
CSV_LEGACY_ID_LINE = re.compile(r"^([A-Za-z0-9]{4})(,|\r?\n|$)")


def default_data_dir():
    if CONFIG_FILE.exists():
        for line in CONFIG_FILE.read_text(encoding="utf-8").splitlines():
            key, _, value = line.partition(":")
            if key.strip() == "appDirStorage" and value.strip():
                return Path(value.strip().strip('"')).expanduser()
    return DEFAULT_DATA_DIR


# ---- reading list files ----------------------------------------------------------------------


def read_ids(path):
    if not path.exists():
        return []
    if path.suffix == ".csv":
        lines = path.read_text(encoding="utf-8").splitlines()
        return [line.split(",", 1)[0] for line in lines if line and not line.startswith("#")]
    text = path.read_text(encoding="utf-8").strip()
    return [row[0] for row in json.loads(text) if row] if text else []


def variant_files(data_dir):
    """{(list name, extension): [(path, preferred suffix), ...]}, archive before delete."""
    lists = {}
    for prefix, suffix in VARIANTS.items():
        for path in sorted(data_dir.glob(prefix + "*")):
            if path.suffix in EXTENSIONS and path.is_file():
                list_name = path.name[len(prefix) : -len(path.suffix)]
                lists.setdefault((list_name, path.suffix), []).append((path, suffix))
    return lists


# ---- rewriting ids ---------------------------------------------------------------------------


def allocate(old_id, preferred, taken):
    """Preferred suffix, else the first free letter: unique per list across archive + delete."""
    for letter in preferred + LETTERS:
        if old_id + letter not in taken:
            taken.add(old_id + letter)
            return old_id + letter
    raise RuntimeError(f"No free suffix left for {old_id}")


def migrate_csv(path, suffix, taken):
    """Rewrites only the leading id of each data line, every other byte is kept as is."""
    changed = []
    out = []
    for line in path.read_text(encoding="utf-8").splitlines(keepends=True):
        match = CSV_LEGACY_ID_LINE.match(line)
        if line.startswith("#") or not match:
            out.append(line)
            continue
        old_id = match.group(1)
        new_id = allocate(old_id, suffix, taken)
        changed.append((old_id, new_id))
        out.append(new_id + line[len(old_id) :])
    return "".join(out), changed


def migrate_json(path, suffix, taken):
    changed = []
    text = path.read_text(encoding="utf-8").strip()
    rows = json.loads(text) if text else []
    for row in rows:
        if row and isinstance(row[0], str) and LEGACY_ID.match(row[0]):
            new_id = allocate(row[0], suffix, taken)
            changed.append((row[0], new_id))
            row[0] = new_id
    return json.dumps(rows, indent=4, ensure_ascii=False), changed


# ---- main ------------------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dir", type=Path, default=None, help="todoos data directory")
    parser.add_argument("--dry-run", action="store_true", help="only print what would change")
    args = parser.parse_args()

    data_dir = (args.dir or default_data_dir()).expanduser()
    if not data_dir.is_dir():
        sys.exit(f"Data directory not found: {data_dir}")

    print(f"Data directory: {data_dir}{'  (dry run)' if args.dry_run else ''}")

    backup_dir = data_dir / f".migrate-archive-ids-backup-{time.strftime('%Y%m%d-%H%M%S')}"
    renamed_log = []
    warnings = []
    total_ids = 0
    total_files = 0

    for (list_name, extension), files in sorted(variant_files(data_dir).items()):
        # 5-char ids already present (migrated earlier, or written by the new binary) stay reserved
        taken = {i for path, _ in files for i in read_ids(path) if len(i) == 5}
        active_ids = set(read_ids(data_dir / (list_name + extension)))
        description_dir = data_dir / "descriptions" / list_name
        described = set()  # an old id's description goes to its first occurrence only

        for path, suffix in files:
            migrate = migrate_csv if extension == ".csv" else migrate_json
            new_content, changed = migrate(path, suffix, taken)
            if not changed:
                continue

            total_files += 1
            total_ids += len(changed)
            print(f"  {path.name}: {len(changed)} id(s)")

            description_moves = []
            for old_id, new_id in changed:
                old_desc = description_dir / f"{old_id}.md"
                new_desc = description_dir / f"{new_id}.md"
                if not old_desc.exists():
                    continue
                if old_id in active_ids:
                    if old_id not in described:
                        warnings.append(f"{list_name}: {old_id} is both active and in {path.name} (now {new_id}); "
                                        f"description left with the active item")
                    described.add(old_id)
                    continue
                if old_id in described:
                    warnings.append(f"{list_name}: {old_id} appears several times; description given to the "
                                    f"first copy, not to {new_id}")
                    continue
                if new_desc.exists():
                    warnings.append(f"{list_name}: {new_desc.name} already exists; {old_desc.name} not renamed")
                    continue
                described.add(old_id)
                description_moves.append((old_desc, new_desc))
                print(f"    description {old_desc.name} -> {new_desc.name}")

            if args.dry_run:
                continue

            backup_dir.mkdir(exist_ok=True)
            shutil.copy2(path, backup_dir / path.name)
            path.write_text(new_content, encoding="utf-8")
            for old_desc, new_desc in description_moves:
                old_desc.rename(new_desc)
                renamed_log.append(f"{old_desc.relative_to(data_dir)} -> {new_desc.relative_to(data_dir)}")

    if renamed_log:
        (backup_dir / "renamed-descriptions.txt").write_text("\n".join(renamed_log) + "\n", encoding="utf-8")

    for warning in warnings:
        print(f"WARNING: {warning}")

    if total_ids == 0:
        print("Nothing to migrate.")
    elif args.dry_run:
        print(f"Would migrate {total_ids} id(s) in {total_files} file(s).")
    else:
        print(f"Migrated {total_ids} id(s) in {total_files} file(s). Backup: {backup_dir}")


if __name__ == "__main__":
    main()
