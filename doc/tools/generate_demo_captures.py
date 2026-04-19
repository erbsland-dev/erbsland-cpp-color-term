#!/usr/bin/env python3
# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

import argparse
import fcntl
import os
import re
import select
import signal
import struct
import subprocess
import sys
import termios
import time
from dataclasses import dataclass, field
from pathlib import Path
from tempfile import NamedTemporaryFile


LOCAL_CONVERTER_PATH = Path("/tmp/erbsland-demo-tools")
if LOCAL_CONVERTER_PATH.exists():
    sys.path.insert(0, str(LOCAL_CONVERTER_PATH))

from erbsland.ansi_convert import Terminal  # type: ignore[import-not-found]


WIDTH = 80
HEIGHT = 25
REPO_ROOT = Path(__file__).resolve().parents[2]
OUTPUT_DIR = REPO_ROOT / "doc/chapters/demos/_captures"


@dataclass(slots=True)
class TimedInput:
    delay_seconds: float
    data: bytes


@dataclass(slots=True)
class DemoCapture:
    name: str
    command: list[str]
    timeout_seconds: float = 6.0
    post_exit_grace_seconds: float = 0.3
    inputs: list[TimedInput] = field(default_factory=list)


CAPTURES = [
    DemoCapture("minimum-effort", ["./cmake-build-debug/demo-apps/minimum-effort"], timeout_seconds=2.0),
    DemoCapture("terminal-chronicle", ["./cmake-build-debug/demo-apps/terminal-chronicle"], timeout_seconds=2.0),
    DemoCapture("command-line-help", ["./cmake-build-debug/demo-apps/command-line-help", "--terminal-width=80"]),
    DemoCapture(
        "display-all-attributes",
        ["./cmake-build-debug/demo-apps/display-all-attributes"],
        timeout_seconds=2.0,
        inputs=[TimedInput(0.4, b"\n")],
    ),
    DemoCapture("display-all-colors", ["./cmake-build-debug/demo-apps/display-all-colors"], timeout_seconds=2.0),
    DemoCapture(
        "frame-color-animations",
        ["./cmake-build-debug/demo-apps/frame-color-animations"],
        inputs=[TimedInput(0.6, b"q")],
    ),
    DemoCapture(
        "frame-weaver",
        ["./cmake-build-debug/demo-apps/frame-weaver"],
        inputs=[TimedInput(1.6, b"q")],
        timeout_seconds=4.0,
    ),
    DemoCapture(
        "html-viewer",
        [
            "./cmake-build-debug/demo-apps/html-viewer",
            "--print",
            "--style=styled",
            "demo/html-viewer/data/demo-tea.html",
        ],
        timeout_seconds=2.0,
    ),
    DemoCapture(
        "key-input-demo",
        ["./cmake-build-debug/demo-apps/key-input-demo"],
        inputs=[TimedInput(0.4, b"A"), TimedInput(0.8, b"\x1b")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "log-viewer",
        ["./cmake-build-debug/demo-apps/log-viewer"],
        inputs=[TimedInput(1.8, b"q")],
        timeout_seconds=4.0,
    ),
    DemoCapture(
        "retro-plasma",
        ["./cmake-build-debug/demo-apps/retro-plasma"],
        inputs=[TimedInput(0.7, b"q")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "text-gallery",
        ["./cmake-build-debug/demo-apps/text-gallery"],
        inputs=[TimedInput(0.7, b"q")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "ui-hello-world",
        ["./cmake-build-debug/demo-apps/ui-hello-world"],
        inputs=[TimedInput(0.7, b"q")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "ui-html-viewer",
        [
            "./cmake-build-debug/demo-apps/ui-html-viewer",
            "--style=styled",
            "demo/html-viewer/data/demo-tea.html",
        ],
        inputs=[TimedInput(0.8, b"q")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "update-screen-modes",
        ["./cmake-build-debug/demo-apps/update-screen-modes"],
        inputs=[TimedInput(0.8, b"q")],
        timeout_seconds=3.0,
    ),
    DemoCapture(
        "bitmap-showcase",
        ["./cmake-build-debug/demo-apps/bitmap-showcase"],
        inputs=[TimedInput(0.7, b"q")],
        timeout_seconds=3.0,
    ),
]


def set_pty_size(fd: int, width: int, height: int) -> None:
    fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack("HHHH", height, width, 0, 0))


def indent_block(text: str) -> str:
    lines = text.rstrip("\n").splitlines()
    return "\n".join(f"    {line}" if line else "" for line in lines)


def wrap_as_rst_include(ansi_text: str) -> str:
    return "\n".join(
        [
            ".. erbsland-ansi::",
            "    :escape-char: ␛",
            "    :theme: ela-term",
            "",
            indent_block(ansi_text),
            "",
        ]
    )


def strip_unsupported_sgr_codes(ansi_text: str) -> str:
    unsupported_codes = {"9", "29"}

    def replace(match: re.Match[str]) -> str:
        codes = [code for code in match.group(1).split(";") if code and code not in unsupported_codes]
        if not codes:
            return "␛[0m"
        return f"␛[{';'.join(codes)}m"

    return re.sub(r"␛\[([0-9;]*)m", replace, ansi_text)


def capture_demo(spec: DemoCapture) -> bytes:
    master_fd, slave_fd = os.openpty()
    try:
        set_pty_size(master_fd, WIDTH, HEIGHT)
        set_pty_size(slave_fd, WIDTH, HEIGHT)
        env = os.environ.copy()
        env["TERM"] = "xterm-256color"
        env["COLUMNS"] = str(WIDTH)
        env["LINES"] = str(HEIGHT)
        process = subprocess.Popen(
            spec.command,
            cwd=REPO_ROOT,
            env=env,
            stdin=slave_fd,
            stdout=slave_fd,
            stderr=slave_fd,
            start_new_session=True,
            close_fds=True,
        )
    finally:
        os.close(slave_fd)

    chunks = bytearray()
    started = time.monotonic()
    input_index = 0
    deadline = started + spec.timeout_seconds
    grace_deadline: float | None = None

    try:
        while True:
            now = time.monotonic()
            while input_index < len(spec.inputs) and now - started >= spec.inputs[input_index].delay_seconds:
                os.write(master_fd, spec.inputs[input_index].data)
                input_index += 1

            ready, _, _ = select.select([master_fd], [], [], 0.05)
            if ready:
                try:
                    chunk = os.read(master_fd, 8192)
                except OSError:
                    chunk = b""
                if chunk:
                    chunks.extend(chunk)

            exit_code = process.poll()
            if exit_code is not None:
                if grace_deadline is None:
                    grace_deadline = now + spec.post_exit_grace_seconds
                if now >= grace_deadline:
                    break
            elif now >= deadline:
                os.killpg(process.pid, signal.SIGTERM)
                try:
                    process.wait(timeout=1.0)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait(timeout=1.0)
                grace_deadline = time.monotonic() + spec.post_exit_grace_seconds
    finally:
        os.close(master_fd)

    return bytes(chunks)


def normalize_capture(capture_bytes: bytes) -> str:
    for restore_sequence in (b"\x1b[?1049l", b"\x1b[?47l"):
        restore_index = capture_bytes.rfind(restore_sequence)
        if restore_index != -1:
            capture_bytes = capture_bytes[:restore_index]
            break
    with NamedTemporaryFile("wb", delete=False) as capture_file:
        capture_file.write(capture_bytes)
        capture_path = Path(capture_file.name)
    try:
        terminal = Terminal(width=WIDTH, height=HEIGHT, back_buffer_height=6000)
        terminal.writeFile(capture_path)
        return terminal.to_ansi(esc_char="␛").rstrip("\n")
    finally:
        capture_path.unlink(missing_ok=True)


def write_capture(spec: DemoCapture) -> Path:
    capture_bytes = capture_demo(spec)
    ansi_text = strip_unsupported_sgr_codes(normalize_capture(capture_bytes))
    output_path = OUTPUT_DIR / f"{spec.name}.rstinc"
    output_path.write_text(wrap_as_rst_include(ansi_text), encoding="utf-8")
    return output_path


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate Sphinx ANSI capture includes for demo pages.")
    parser.add_argument("names", nargs="*", help="Optional demo names to generate. Defaults to all demos.")
    args = parser.parse_args()

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    selected = set(args.names)
    specs = [spec for spec in CAPTURES if not selected or spec.name in selected]
    if selected:
        missing = sorted(selected - {spec.name for spec in specs})
        if missing:
            raise SystemExit(f"Unknown demo capture names: {', '.join(missing)}")

    for spec in specs:
        output_path = write_capture(spec)
        print(f"Generated {output_path.relative_to(REPO_ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
