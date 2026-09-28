#!/usr/bin/env python3
import re
from pathlib import Path

libfile = Path('parsed_liberty.txt')
gatefile = Path('files/gate1.v')

if not libfile.exists():
    print('parsed_liberty.txt not found; run the parser first')
    raise SystemExit(1)
if not gatefile.exists():
    print('gate file not found:', gatefile)
    raise SystemExit(1)

# Parse parsed_liberty.txt to extract cell -> area
cell_area = {}
with libfile.open() as f:
    lines = [l.rstrip('\n') for l in f]

i = 0
while i < len(lines):
    line = lines[i].strip()
    m = re.match(r'^Cell:\s+(\S+)\s+\(pins:\s*\d+\)', line)
    if m:
        name = m.group(1)
        area = None
        j = i + 1
        while j < len(lines) and lines[j].startswith('  '):
            l = lines[j].strip()
            if l.startswith('area='):
                try:
                    area = float(l.split('=',1)[1])
                except:
                    pass
            j += 1
        if area is not None:
            cell_area[name] = area
        i = j
    else:
        i += 1

if not cell_area:
    print('No cell areas found in parsed_liberty.txt')

# Read gate file and count instances whose type is in cell_area
counts = {}
inst_re = re.compile(r'^\s*([A-Za-z0-9_]+)\s+([^;\(]+)\s*\(')
with gatefile.open() as f:
    for ln in f:
        m = inst_re.match(ln)
        if m:
            typ = m.group(1)
            if typ in cell_area:
                counts[typ] = counts.get(typ,0) + 1

# Now compute totals
total_area = 0.0
print('Cell counts and area contributions:')
for typ, cnt in sorted(counts.items(), key=lambda x: -x[1]):
    area = cell_area.get(typ, 0.0)
    contrib = area * cnt
    total_area += contrib
    print(f'{typ}: count={cnt}, area={area}, contribut={contrib:.6f}')

print('\nTotal area = {:.6f}'.format(total_area))

# Also print cells in gates that have no area info
missing = [t for t in counts if t not in cell_area]
if missing:
    print('\nWarning: some cell types found in gate file have no area info in parsed_liberty.txt:')
    for t in missing:
        print(' -', t)
