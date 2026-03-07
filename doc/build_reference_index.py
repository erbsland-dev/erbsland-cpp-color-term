# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0


import re
from pathlib import Path

ref_path = Path(__file__).parent / "chapters" / "reference"
src_path = Path(__file__).parent.parent / "src" / "erbsland" / "cterm"

re_entry = re.compile(r".. doxygen\w+:: erbsland::cterm::(\w+)")


def extract_entry_map(reference_path: Path) -> dict[str, str]:
    entry_map = {}
    for file in reference_path.glob("*.rst"):
        content = file.read_text()
        for match in re_entry.finditer(content):
            entry_map[match.group(1)] = file.stem
    return entry_map


def find_type_name(declaration: str) -> str | None:
    if match := re.match(r"(?:enum\s+class|enum|class|struct)\s+(\w+)\b", declaration):
        return match.group(1)
    if match := re.match(r"using\s+(\w+)\b", declaration):
        return match.group(1)
    return None


def extract_briefs_from_header(content: str) -> dict[str, str]:
    brief_map = {}
    lines = content.splitlines()
    index = 0
    while index < len(lines):
        line = lines[index]
        if not line.startswith("/// "):
            index += 1
            continue
        brief = line.removeprefix("/// ").strip().replace("`", "")
        index += 1
        while index < len(lines) and lines[index].startswith("///"):
            index += 1
        while index < len(lines) and not lines[index].strip():
            index += 1
        while index < len(lines) and lines[index].lstrip().startswith("template "):
            index += 1
            while index < len(lines) and not lines[index].strip():
                index += 1
        if index >= len(lines):
            break
        declaration = lines[index].strip()
        if type_name := find_type_name(declaration):
            brief_map[type_name] = brief
        index += 1
    return brief_map


def extract_brief_map(source_path: Path) -> dict[str, str]:
    brief_map = {}
    for file in source_path.glob("*.hpp"):
        brief_map.update(extract_briefs_from_header(file.read_text()))
    return brief_map


def is_title_underline(line: str) -> bool:
    stripped = line.strip()
    return bool(stripped) and len(set(stripped)) == 1 and stripped[0] in "*=~-^\"`#+:."


def extract_title_and_paragraph(content: str) -> tuple[str, str]:
    lines = content.splitlines()
    title = ""
    paragraph = ""
    index = 0
    while index < len(lines):
        line = lines[index]
        if line == "..":
            index += 1
            while index < len(lines) and (not lines[index].strip() or lines[index].startswith(" ")):
                index += 1
            continue
        if not line.strip():
            index += 1
            continue
        if (
            index + 2 < len(lines)
            and is_title_underline(line)
            and lines[index + 1].strip()
            and is_title_underline(lines[index + 2])
        ):
            title = lines[index + 1].strip()
            index += 3
            break
        if index + 1 < len(lines) and lines[index].strip() and is_title_underline(lines[index + 1]):
            title = lines[index].strip()
            index += 2
            break
        index += 1
    while index < len(lines):
        line = lines[index].strip()
        if not line:
            index += 1
            continue
        if line.startswith(".. "):
            index += 1
            continue
        paragraph_lines = [line]
        index += 1
        while index < len(lines) and lines[index].strip():
            if is_title_underline(lines[index]):
                break
            paragraph_lines.append(lines[index].strip())
            index += 1
        paragraph = " ".join(paragraph_lines).replace("`", "")
        break
    return title, paragraph


def extract_topic_map(reference_path: Path) -> list[tuple[str, str, str]]:
    topics = []
    for file in sorted(reference_path.glob("*.rst")):
        if file.name == "index.rst":
            continue
        title, paragraph = extract_title_and_paragraph(file.read_text())
        topics.append((title, file.stem, paragraph))
    return topics


def print_alphabetical_section(entry_map: dict[str, str], brief_map: dict[str, str]) -> None:
    print("Alphabetical")
    print("============")
    print()
    print(".. list-table::")
    print("    :header-rows: 1")
    print("    :widths: 30 70")
    print("    :width: 100%")
    print()
    print("    *   - Name")
    print("        - Description")
    for name in sorted(entry_map.keys()):
        page = entry_map[name]
        brief = brief_map.get(name, "")
        print(f"    *   - :doc:`{name}<{page}>`")
        print(f"        - {brief}")


def print_topic_section(topics: list[tuple[str, str, str]]) -> None:
    print()
    print("By Topic")
    print("========")
    print()
    print(".. list-table::")
    print("    :header-rows: 1")
    print("    :widths: 30 70")
    print("    :width: 100%")
    print()
    print("    *   -   Topic")
    print("        -   Description")
    for title, page, paragraph in topics:
        print(f"    *   -   :doc:`{title} <{page}>`")
        print(f"        -   {paragraph}")


def main() -> None:
    entry_map = extract_entry_map(ref_path)
    brief_map = extract_brief_map(src_path)
    topics = extract_topic_map(ref_path)
    print_alphabetical_section(entry_map, brief_map)
    print_topic_section(topics)


if __name__ == "__main__":
    main()
