#!/usr/bin/env python3
# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0
"""Generate the common box-frame character combination data."""

from __future__ import annotations

import argparse
from collections import Counter
from pathlib import Path

from lib.box_drawing import (
    Attributes,
    POSITION_NAMES,
    POSITION_WEIGHTS,
    all_characters,
    build_attributes_to_character,
    build_character_attributes,
    combine_attributes,
    choose_combination_result,
    has_lines,
    is_center_only,
    score_candidate,
)
from lib.cpp_render import generated_cpp_source


LIBRARY_ROOT = Path(__file__).resolve().parent.parent
GENERATOR_PATH = "erbsland-cpp-color-term/utilities/generate_common_box_frame_style.py"
OUTPUT_PATH = LIBRARY_ROOT / "src" / "erbsland" / "cterm" / "impl" / "CommonBoxFrameCombinationStyle_data.cpp"
ATTRIBUTE_SHIFT = 4
BOX_DRAWING_START = 0x2500
BOX_DRAWING_END = 0x257F
UNSUPPORTED_INDEX = 0xFF
MINIMUM_IMPROVEMENT = 100


def is_box_drawing(character: str) -> bool:
    """Test if a character is in the Unicode box-drawing block."""
    return BOX_DRAWING_START <= ord(character) <= BOX_DRAWING_END


def pack_attributes(attributes: Attributes) -> int:
    """Pack attributes into a compact 4-bit-per-position integer."""
    value = 0
    for index, position in enumerate(POSITION_NAMES):
        value |= int(getattr(attributes, position)) << (index * ATTRIBUTE_SHIFT)
    return value


def compact_result(
    current: str,
    overlay: str,
    character_attributes: dict[str, Attributes],
    attributes_to_character: dict[Attributes, str],
    characters: list[str],
) -> str:
    """Return the result produced by the compact runtime algorithm."""
    current_attributes = character_attributes[current]
    overlay_attributes = character_attributes[overlay]
    if is_center_only(current_attributes) and has_lines(overlay_attributes):
        return overlay
    if has_lines(current_attributes) and is_center_only(overlay_attributes):
        return current

    ideal = combine_attributes(current_attributes, overlay_attributes)
    exact_match = attributes_to_character.get(ideal)
    if exact_match is not None:
        return exact_match

    best_character = min(characters, key=lambda character: score_candidate(ideal, character_attributes[character]))
    best_score = score_candidate(ideal, character_attributes[best_character])
    overlay_score = score_candidate(ideal, overlay_attributes)
    if best_score + MINIMUM_IMPROVEMENT < overlay_score:
        return best_character
    return overlay


def generate_matrix(characters: list[str]) -> tuple[list[str], Counter[str]]:
    """Generate the reference result matrix rows and a summary of selection modes."""
    character_attributes = build_character_attributes(characters)
    attributes_to_character = build_attributes_to_character(character_attributes)
    result_rows: list[str] = []
    selection_counts: Counter[str] = Counter()
    for current in characters:
        row: list[str] = []
        for overlay in characters:
            result, selection = choose_combination_result(
                current=current,
                overlay=overlay,
                character_attributes=character_attributes,
                attributes_to_character=attributes_to_character,
                characters=characters,
            )
            row.append(result)
            selection_counts[selection] += 1
        result_rows.append("".join(row))
    return result_rows, selection_counts


def verify_compact_algorithm(characters: list[str]) -> None:
    """Verify the compact runtime algorithm against the reference generator rules."""
    character_attributes = build_character_attributes(characters)
    attributes_to_character = build_attributes_to_character(character_attributes)
    for current in characters:
        for overlay in characters:
            reference_result, _ = choose_combination_result(
                current=current,
                overlay=overlay,
                character_attributes=character_attributes,
                attributes_to_character=attributes_to_character,
                characters=characters,
            )
            compact = compact_result(current, overlay, character_attributes, attributes_to_character, characters)
            if compact != reference_result:
                raise AssertionError(
                    f"Compact algorithm mismatch for {current!r} + {overlay!r}: "
                    f"{compact!r} != {reference_result!r}"
                )


def render_integer_array(
    values: list[int],
    *,
    indent: str = "        ",
    entries_per_line: int = 12,
    suffix: str = "U",
    width: int = 2,
) -> list[str]:
    """Render an integer list for a C++ array initializer."""
    result: list[str] = []
    for start in range(0, len(values), entries_per_line):
        chunk = values[start:start + entries_per_line]
        result.append(indent + ", ".join(f"0x{value:0{width}X}{suffix}" for value in chunk) + ",")
    return result


def render_attribute_array(values: list[int]) -> list[str]:
    """Render packed attributes as C++ array entries."""
    return render_integer_array(values, entries_per_line=8, suffix="U", width=7)


def special_index_entries(characters: list[str]) -> list[tuple[str, int]]:
    """Build the special non-box code point lookup entries."""
    return [(character, index) for index, character in enumerate(characters) if not is_box_drawing(character)]


def render_data_function(
    name: str,
    result_type: str,
    values: list[int],
    *,
    entries_per_line: int = 12,
    width: int = 2,
) -> list[str]:
    """Render one generated data accessor function."""
    storage_type = result_type.split("::")[-1]
    lines = [
        f"auto CommonBoxFrameCombinationStyle::{name}() noexcept -> std::span<const {result_type}> {{",
        f"    static constexpr std::array<{storage_type}, {len(values)}U> data{{{{",
    ]
    lines.extend(
        render_integer_array(values, indent="        ", entries_per_line=entries_per_line, suffix="U", width=width)
    )
    lines.extend(
        [
            "    }};",
            "    return data;",
            "}",
            "",
        ]
    )
    return lines


def render_output(characters: list[str], selection_counts: Counter[str]) -> str:
    """Render the generated C++ source file."""
    character_attributes = build_character_attributes(characters)
    attributes_to_character = build_attributes_to_character(character_attributes)
    packed_attributes = [pack_attributes(character_attributes[character]) for character in characters]
    exact_entries = sorted(
        (pack_attributes(attributes), ord(character)) for attributes, character in attributes_to_character.items()
    )
    box_offset_to_character_index = [UNSUPPORTED_INDEX] * (BOX_DRAWING_END - BOX_DRAWING_START + 1)
    for index, character in enumerate(characters):
        if is_box_drawing(character):
            box_offset_to_character_index[ord(character) - BOX_DRAWING_START] = index

    special_entries = special_index_entries(characters)
    summary_lines = [
        (
            f"Generated from {sum(selection_counts.values())} directed combinations: "
            f"{selection_counts['exact']} exact, "
            f"{selection_counts['best']} weighted matches, "
            f"{selection_counts['overlay']} overlay fallbacks, "
            f"{selection_counts['center-overwrite']} center overwrites."
        ),
        (
            f"Compact runtime data: {len(characters)} characters, {len(exact_entries)} exact attribute entries, "
            f"{len(box_offset_to_character_index)} box lookup entries."
        ),
    ]
    body_lines = [
        '#include "CommonBoxFrameCombinationStyle.hpp"',
        "",
        "#include <array>",
        "",
        "namespace erbsland::cterm {",
        "",
    ]
    body_lines.extend(
        render_data_function(
            "boxOffsetToCharacterIndex", "uint8_t", box_offset_to_character_index, entries_per_line=12, width=2
        )
    )
    body_lines.extend(
        render_data_function(
            "characters", "CommonBoxFrameCombinationStyle::CodePointData",
            [ord(character) for character in characters], entries_per_line=8, width=4
        )
    )
    body_lines.extend(
        render_data_function(
            "attributes", "CommonBoxFrameCombinationStyle::AttributeData", packed_attributes,
            entries_per_line=8, width=7
        )
    )
    body_lines.extend(
        render_data_function(
            "exactAttributes", "CommonBoxFrameCombinationStyle::AttributeData", [entry[0] for entry in exact_entries],
            entries_per_line=8, width=7
        )
    )
    body_lines.extend(
        render_data_function(
            "exactCharacters", "CommonBoxFrameCombinationStyle::CodePointData", [entry[1] for entry in exact_entries],
            entries_per_line=8, width=4
        )
    )
    body_lines.extend(
        render_data_function(
            "positionWeights", "uint8_t", [POSITION_WEIGHTS[position] for position in POSITION_NAMES],
            entries_per_line=len(POSITION_NAMES), width=2
        )
    )
    body_lines.extend(
        render_data_function(
            "specialCodePoints", "CommonBoxFrameCombinationStyle::CodePointData",
            [ord(character) for character, _ in special_entries], entries_per_line=8, width=4
        )
    )
    body_lines.extend(
        render_data_function(
            "specialCharacterIndexes", "uint8_t", [index for _, index in special_entries], entries_per_line=8, width=2
        )
    )
    body_lines.extend(["}", ""])
    return generated_cpp_source(GENERATOR_PATH, summary_lines, body_lines)


def parse_arguments() -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="Verify that the generated file is up to date.")
    return parser.parse_args()


def main() -> None:
    """Run the generator."""
    args = parse_arguments()
    characters = all_characters()
    verify_compact_algorithm(characters)
    _, selection_counts = generate_matrix(characters)
    output = render_output(characters, selection_counts)

    if args.check:
        existing_output = OUTPUT_PATH.read_text(encoding="utf-8")
        if existing_output != output:
            raise SystemExit("The generated common box frame style data file is out of date.")
        return

    OUTPUT_PATH.write_text(output, encoding="utf-8")


if __name__ == "__main__":
    main()
