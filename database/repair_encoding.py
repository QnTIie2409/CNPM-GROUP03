"""Preview or repair UTF-8 text imported as Windows-1252.

Run: python database/repair_encoding.py [--apply] [--mysql PATH]
Uses config.ini. Only repairs reversible full_name/course name mojibake.
--apply saves original values locally, then updates using a HEX guard.
Does not reset tables, passwords, enrollments or attendance.
"""
import argparse
import configparser
from datetime import datetime
import json
import os
from pathlib import Path
import shutil
import subprocess


def repaired(value):
    try:
        # Some import tools preserve undefined CP1252 bytes as control codes.
        raw = bytearray()
        for character in value:
            try:
                raw.extend(character.encode('cp1252'))
            except UnicodeEncodeError:
                if 0x80 <= ord(character) <= 0x9f:
                    raw.append(ord(character))
                else:
                    raise
        candidate = raw.decode('utf-8')
    except (UnicodeEncodeError, UnicodeDecodeError):
        return value
    return candidate


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    parser.add_argument('--mysql', default=shutil.which('mysql') or
                        r'C:\xampp\mysql\bin\mysql.exe')
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    config = configparser.ConfigParser(interpolation=None)
    config.read(root / 'config.ini', encoding='utf-8-sig')
    db = config['Database']
    env = os.environ.copy()
    env['MYSQL_PWD'] = db.get('password', '')
    command = [args.mysql, '--host=' + db['host'], '--port=' + db['port'],
               '--user=' + db['username'], '--default-character-set=utf8mb4',
               '--batch', '--skip-column-names', db['dbname']]

    def query(sql):
        result = subprocess.run(command, env=env, input=sql.encode('utf-8'),
                                capture_output=True)
        if result.returncode:
            raise RuntimeError(result.stderr.decode('utf-8', errors='replace'))
        return result.stdout.decode('utf-8')

    changes = []
    for table, key, column in [('users', 'user_id', 'full_name'),
                                ('courses', 'course_id', 'name')]:
        rows = query(f'SELECT {key}, HEX({column}) FROM {table};')
        for row in rows.splitlines():
            identifier, original_hex = row.split('\t')
            original = bytes.fromhex(original_hex).decode('utf-8')
            corrected = repaired(original)
            if corrected == original:
                continue
            changes.append(dict(table=table, key=key, column=column,
                                identifier=int(identifier), original=original,
                                corrected=corrected, original_hex=original_hex))
            print(f'{table}/{identifier}: {ascii(original)} -> {ascii(corrected)}')
    print(f'{len(changes)} values need repair.')
    if not args.apply or not changes:
        return
    backup = root / 'database' / 'backups' / (
        'encoding_originals_' + datetime.now().strftime('%Y%m%d_%H%M%S') + '.json')
    backup.parent.mkdir(exist_ok=True)
    backup.write_text(json.dumps(changes, ensure_ascii=False, indent=2), encoding='utf-8')
    statements = ['SET NAMES utf8mb4;', 'START TRANSACTION;']
    for change in changes:
        corrected_hex = change['corrected'].encode('utf-8').hex()
        statements.append(
            f"UPDATE {change['table']} SET {change['column']} = "
            f"CONVERT(UNHEX('{corrected_hex}') USING utf8mb4) "
            f"WHERE {change['key']} = {change['identifier']} "
            f"AND HEX({change['column']}) = '{change['original_hex']}';")
    statements.append('COMMIT;')
    query('\n'.join(statements))
    print('Repair committed. Original values saved to ' + str(backup))


if __name__ == '__main__':
    main()
