#!/usr/bin/env python3

# Copyright (c) 2025 Aeybel Varghese
# SPDX-License-Identifier: MIT

import re
import sys
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[2]
platforms = [p for p in ROOT.glob("hw/*") if p.is_dir()]
SRC = [ROOT / "include", ROOT / "src"] + platforms

LICENSE_RE = re.compile(r"SPDX-License-Identifier:\s*([A-Za-z0-9\-.+]+)")
DOXY_RE = re.compile(r"/\*\s*SPDX-License-Identifier:.*?@file\s+.*?\*/", re.S)
GUARD_START_RE = re.compile(r"^\s*#ifndef\s+([A-Z0-9_]+)\s*$")
GUARD_END_RE = re.compile(r"^\s*#endif\b.*$", re.M)


def expected_guard(path: pathlib.Path) -> str:
    rel = path.relative_to(ROOT).with_suffix("")
    return re.sub(r"[^A-Za-z0-9]", "_", str(rel).upper()) + "_H"


def check_file(p: pathlib.Path):
    txt = p.read_text(encoding="utf-8", errors="ignore")
    ok = True
    msgs = []
    if not LICENSE_RE.search(txt):
        ok = False
        msgs.append("missing SPDX license")
    return ok, msgs


def iter_files():
    for base in SRC:
        if not base.exists():
            continue
        for p in base.rglob("*"):
            if p.suffix in {".h", ".c", ".hpp", ".cpp"}:
                yield p


bad = []
for f in iter_files():
    ok, msgs = check_file(f)
    if not ok:
        bad.append((f, msgs))

if bad:
    for f, msgs in bad:
        print(f"[FAIL] {f}:\n\t" + "\n\t".join(msgs))
    sys.exit(1)
else:
    print("License checks passed.")
