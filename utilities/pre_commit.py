#!/usr/bin/env python3
#  Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
#  SPDX-License-Identifier: Apache-2.0
import os
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

CLANG_FORMAT_MARKER_FILE = ".clang-format-last-run"
CLANG_FORMAT_FULL_RUN_INTERVAL = 24 * 60 * 60
CLANG_FORMAT_EXTENSIONS = (".cpp", ".hpp")


def find_cpp_files(directories):
    """Find C++ source files in the specified directories."""
    cpp_files = []
    for directory in directories:
        dir_path = Path(directory)
        if not dir_path.exists():
            print(f"Warning: Directory '{directory}' does not exist, skipping...")
            continue
        cpp_files.extend(file for file in dir_path.rglob("*") if file.suffix in CLANG_FORMAT_EXTENSIONS)
    return sorted(cpp_files)


def should_run_full_clang_format(marker_path):
    """Test if clang-format should run over all source files."""
    if not marker_path.exists():
        return True
    return (time.time() - marker_path.stat().st_mtime) > CLANG_FORMAT_FULL_RUN_INTERVAL


def touch_clang_format_marker(marker_path):
    """Update the clang-format run marker file."""
    timestamp = datetime.now(timezone.utc).isoformat(timespec="seconds")
    marker_path.write_text(f"{timestamp}\n", encoding="utf-8")


def run_clang_format(directories, project_root):
    """Run clang-format on specified directories."""
    marker_path = project_root / CLANG_FORMAT_MARKER_FILE
    full_run = should_run_full_clang_format(marker_path)
    marker_mtime = marker_path.stat().st_mtime if marker_path.exists() else 0
    cpp_files = find_cpp_files(directories)
    files_to_format = cpp_files if full_run else [file for file in cpp_files if file.stat().st_mtime > marker_mtime]

    if full_run:
        print("Running clang-format for all source files...")
    else:
        print("Running clang-format for source files changed since the last run...")

    for file in files_to_format:
        print(f"  Formatting {file}")
        subprocess.run(["clang-format", "-i", str(file)], check=True)

    touch_clang_format_marker(marker_path)
    print(f"Formatted {len(files_to_format)} of {len(cpp_files)} source files.")
    print("clang-format completed.")


def run_script(script_name):
    """Run a Python script."""
    script_path = Path(__file__).parent / script_name
    if not script_path.exists():
        print(f"Error: Script '{script_name}' not found at {script_path}")
        sys.exit(1)

    print(f"Running {script_name}...")
    subprocess.run([sys.executable, str(script_path)], check=True)
    print(f"{script_name} completed.")


def main():
    """Main entry point for pre-commit tasks."""
    directories = ["src", "test/profiling", "test/unittest", "demo"]

    # CD to the project root directory
    project_root = Path(__file__).parent.parent
    os.chdir(project_root)

    # Step 1: Regenerate static frame lookup tables
    run_script("generate_common_box_frame_style.py")
    run_script("generate_frame_border_joint_chars.py")

    # Step 2: Run clang-format
    run_clang_format(directories, project_root)

    # Step 3: Run generate_header_files.py
    run_script("generate_header_files.py")

    # Step 4: Run cleanup_includes.py
    run_script("cleanup_includes.py")

    print("All pre-commit tasks completed successfully.")


if __name__ == "__main__":
    main()
