#!/usr/bin/env python3
"""Deprecated entrypoint.

Architecture decision: native C++ HTTP server is the single server on port 3004.
This script no longer exposes HTTP endpoints and no longer launches cpp_engine_server.
No task lifecycle, no auto-restart, no lifecycle decision is implemented here.
"""

import os
import sys


def main() -> int:
    launch_mode = os.environ.get("CODEIA_LAUNCH_MODE", "")
    host = os.environ.get("CPP_ENGINE_HOST", "127.0.0.1")
    port = int(os.environ.get("CPP_ENGINE_PORT", "3004"))

    print("[control_server.py] DEPRECATED: cet entrypoint est désactivé.", file=sys.stderr)
    print(
        "[control_server.py] Utiliser directement le serveur natif: "
        f"./build/bin/cpp_engine_server --host {host} --port {port}",
        file=sys.stderr,
    )
    print(
        f"[control_server.py] launch_mode={launch_mode!r} (attendu: 'orchestrator')",
        file=sys.stderr,
    )
    return 3


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"[control_server.py] erreur: {exc}", file=sys.stderr)
        raise SystemExit(1)
