#!/usr/bin/env python3
#  Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
#  SPDX-License-Identifier: Apache-2.0
import os
import subprocess
import sys
from pathlib import Path


def run_clang_format(directories):
    """Run clang-format on specified directories."""
    print("Running clang-format...")
    for directory in directories:
        dir_path = Path(directory)
        if not dir_path.exists():
            print(f"Warning: Directory '{directory}' does not exist, skipping...")
            continue

        cpp_files = list(dir_path.rglob("*.cpp")) + list(dir_path.rglob("*.hpp"))
        for file in cpp_files:
            print(f"  Formatting {file}")
            subprocess.run(["clang-format", "-i", str(file)], check=True)
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

    # Step 1: Run clang-format
    run_clang_format(directories)

    # Step 2: Run generate_header_files.py
    run_script("generate_header_files.py")

    # Step 3: Run cleanup_includes.py
    run_script("cleanup_includes.py")

    print("All pre-commit tasks completed successfully.")


if __name__ == "__main__":
    main()
