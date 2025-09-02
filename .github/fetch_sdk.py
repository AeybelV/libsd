#!/usr/bin/env python3
import sys
import os
import json
import subprocess
import shlex

# The script is provided a json string as a cli arg
# The json is the SDK fields in mcus.json
# The CI file will automatically filter out and select the required SDK field
sdk = json.loads(sys.argv[1]) if len(sys.argv) > 1 else {"method": "none"}


def run(cmd, **kw):
    """
    A little helper that runs commands, also prints it to stdout
    Args:
        cmd (): command to run
        **kw: args to pass to command
    """
    print("+", cmd)
    subprocess.check_call(cmd, shell=True, **kw)


m = sdk.get("method", "none")

# Matches the desire fetch method
if m == "git":
    path = sdk["path"]
    os.makedirs(os.path.dirname(path), exist_ok=True)

    if not os.path.isdir(os.path.join(path, ".git")):
        run(f"git clone --depth=1 {shlex.quote(sdk['repo'])} {shlex.quote(path)}")

    ref = sdk.get("ref")
    if ref:
        run(
            f"git -C {shlex.quote(path)} fetch --depth=1 origin {shlex.quote(ref)} || true"
        )
        run(f"git -C {shlex.quote(path)} checkout {shlex.quote(ref)} || true")

    if sdk.get("submodules"):
        run(f"git -C {shlex.quote(path)} submodule update --init --recursive")

elif m == "archive":  # Tested with a dummy archive, needs more rigours testing
    path = sdk["path"]
    os.makedirs(path, exist_ok=True)

    url = sdk["url"]
    strip = int(sdk.get("strip_components", 0))

    # Fetches to archive to tmp, then extracts it. So far only support tar
    # The "ouch" utility s rather nice for decompressing all kinds of format though, maybe
    # use it in the future?
    run(f"curl -L {shlex.quote(url)} -o /tmp/sdk.tgz")
    run(f"tar -xf /tmp/sdk.tgz -C {shlex.quote(path)} --strip-components={strip}")
elif m == "submodule":
    # Ensure submodules are present (checkout step may already do this)
    run("git submodule update --init --recursive")
else:
    print("No SDK fetch required.")
