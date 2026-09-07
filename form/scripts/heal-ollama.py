#!/usr/bin/env python3
"""Loopback model transport. Form selects roles and judges proposed edits."""
import json
import sys
import urllib.request


def call(path, payload=None):
    data = None if payload is None else json.dumps(payload).encode()
    request = urllib.request.Request("http://127.0.0.1:11434/api/" + path, data=data, headers={"Content-Type": "application/json"})
    # No proxy, alternate host, model download, or cloud-model admission.
    with urllib.request.build_opener(urllib.request.ProxyHandler({})).open(request, timeout=110) as response:
        return json.load(response)


if __name__ == "__main__":
    try:
        models = call("tags").get("models", [])
        local = [m["name"] for m in models if m.get("size", 0) > 0 and not m.get("remote_host") and "cloud" not in m["name"].lower()]
        if sys.argv[1] == "list":
            print("\n".join(local))
        elif sys.argv[1] == "generate" and sys.argv[2] in local:
            result = call("generate", {"model": sys.argv[2], "prompt": sys.stdin.read(24577), "stream": False, "options": {"temperature": 0, "num_predict": 512}})
            if result.get("done") is not True:
                raise ValueError("incomplete-generation")
            print(result.get("response", ""), end="")
        else:
            raise ValueError("model-not-observed-local")
    except Exception as exc:
        print("local-model-unavailable:" + str(exc), file=sys.stderr)
        sys.exit(1)
