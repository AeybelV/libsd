#!/usr/bin/env python3

# Copyright (c) 2025 Aeybel Varghese
# SPDX-License-Identifier: MIT

import os
import json
import sys


def dedup(seq):
    """
    Given a sequence, removes duplicates to yield on unique values
    Args:
        seq (): sequence/array to operate on

    Returns: Array containing unique values

    """
    seen = set()
    out = []
    for x in seq:
        if x not in seen:
            out.append(x)
            seen.add(x)
    return out


CFG_PATH = ".github/mcus.json"
cfg = json.load(open(CFG_PATH, "r", encoding="utf-8"))

# The mcu json file contains default options for every target, and a list
# of target with their respective per-target configs
defaults = cfg.get("defaults", {})
targets = cfg.get("targets", [])

if not targets:
    print("::error::No targets found in .github/mcus.json", file=sys.stderr)
    sys.exit(1)

# The resulting json that we are building up
include = []

for t in targets:
    t = dict(t)  # copy
    # build up the matrix json
    # merge defaults
    t["toolchain"] = t.get("toolchain", defaults.get("toolchain"))
    t["apt"] = dedup([*(defaults.get("apt", [])), *(t.get("apt", []))])
    t["cmake_args"] = [*(defaults.get("cmake_args", [])), *(t.get("cmake_args", []))]
    t.setdefault("desc", t["id"])
    t.setdefault("sdk", {"method": "none"})
    include.append(t)

# Emit a single-line JSON for the workflow output
matrix = {"include": include}
out_path = os.environ.get("GITHUB_OUTPUT")

# Print the JSON to GITHuB output if provded, or to stdout otherwise
if not out_path:
    print(json.dumps(matrix))
else:
    with open(out_path, "a", encoding="utf-8") as f:
        f.write(f"matrix={json.dumps(matrix)}\n")
