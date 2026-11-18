#!/usr/bin/env python3
"""One-shot migration: add the `parent_id` column to every stored item.

Items can now be attached to a parent item. Each stored row gets a `parent_id` column right after
the id (empty = top-level item):

  before: id, value, priority, status, dueAt, closedAt, createdAt, updatedAt
  after:  id, parent_id, value, priority, status, dueAt, closedAt, createdAt, updatedAt

Every list file is converted: active `<list>`, `.archive_<list>` and `.del_<list>`, `.csv` and `.json`.
Only 8-column rows are changed (an empty `parent_id` is inserted), every other row and every other
byte is kept as is. Line order is kept, so running it on two synced machines gives identical results.
It is idempotent: a second run changes nothing. Descriptions are not touched.

Rollout: run it on one machine, let the sync finish, then install the new todoos binary everywhere.

Usage:
  scripts/migrate-parent-id.py --dry-run      # show what would change
  scripts/migrate-parent-id.py                # migrate (backup folder is printed)
  scripts/migrate-parent-id.py --dir PATH     # data dir (default: appDirStorage from config)
"""

import argparse
import json
import shutil
import sys
import time
from pathlib import Path

CONFIG_FILE = Path.home() / ".oceanspy" / "todoos" / "config.conf"
DEFAULT_DATA_DIR = Path.home() / ".todoos"
EXTENSIONS = (".csv", ".json")
LEGACY_ROW_SIZE = 8
OLD_CSV_HEADER = "# ID,VALUE,"
NEW_CSV_HEADER = "# ID,PARENT_ID,VALUE,"


def default_data_dir():
    if CONFIG_FILE.exists():
        for line in CONFIG_FILE.read_text(encoding="utf-8").splitlines():
            key, _, value = line.partition(":")
            if key.strip() == "appDirStorage" and value.strip():
                return Path(value.strip().strip('"')).expanduser()
    return DEFAULT_DATA_DIR


def list_files(data_dir):
    """Active, archive and delete list files (top level only: backup folders are skipped)."""
    return [
        path
        for path in sorted(data_dir.iterdir())
        if path.is_file() and path.suffix in EXTENSIONS
    ]


# ---- rewriting rows --------------------------------------------------------------------------


def csv_field_count(line):
    """Same rule as CsvSerializer::read: commas outside double quotes separate the fields."""
    in_string = False
    count = 1
    for char in line.rstrip("\r\n"):
        if char == '"':
            in_string = not in_string
        elif char == "," and not in_string:
            count += 1
    return count


def migrate_csv(path):
    """Inserts an empty field after the id of each 8-column line; every other byte is kept."""
    changed = 0
    out = []
    for line in path.read_text(encoding="utf-8").splitlines(keepends=True):
        if line.startswith(OLD_CSV_HEADER):
            out.append(NEW_CSV_HEADER + line[len(OLD_CSV_HEADER) :])
            continue
        if not line.strip() or line.startswith("#") or csv_field_count(line) != LEGACY_ROW_SIZE:
            out.append(line)
            continue
        # The id is 4 or 5 alphanumeric chars: it never holds a comma or a quote
        comma = line.index(",")
        out.append(line[:comma] + "," + line[comma:])
        changed += 1
    return "".join(out), changed


def migrate_json(path):
    text = path.read_text(encoding="utf-8").strip()
    rows = json.loads(text) if text else []
    if not isinstance(rows, list):
        return None, 0
    changed = 0
    for row in rows:
        if isinstance(row, list) and len(row) == LEGACY_ROW_SIZE:
            row.insert(1, "")
            changed += 1
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

    backup_dir = data_dir / f".migrate-parent-id-backup-{time.strftime('%Y%m%d-%H%M%S')}"
    total_rows = 0
    total_files = 0

    for path in list_files(data_dir):
        migrate = migrate_csv if path.suffix == ".csv" else migrate_json
        try:
            new_content, changed = migrate(path)
        except (ValueError, UnicodeDecodeError) as error:
            print(f"WARNING: {path.name} skipped, it could not be read ({error})")
            continue

        original = path.read_text(encoding="utf-8")
        if not changed and (new_content is None or path.suffix == ".json" or new_content == original):
            continue

        total_files += 1
        total_rows += changed
        print(f"  {path.name}: {changed} row(s)")

        if args.dry_run:
            continue

        backup_dir.mkdir(exist_ok=True)
        shutil.copy2(path, backup_dir / path.name)
        path.write_text(new_content, encoding="utf-8")

    if total_files == 0:
        print("Nothing to migrate.")
    elif args.dry_run:
        print(f"Would migrate {total_rows} row(s) in {total_files} file(s).")
    else:
        print(f"Migrated {total_rows} row(s) in {total_files} file(s). Backup: {backup_dir}")


if __name__ == "__main__":
    main()
