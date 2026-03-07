#!/usr/bin/env python3
# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0
"""Generate the common box-frame character combination matrix."""

from __future__ import annotations

import argparse
import unicodedata
from collections import Counter
from dataclasses import asdict, dataclass
from enum import IntEnum
from pathlib import Path


LIBRARY_ROOT = Path(__file__).resolve().parent.parent
OUTPUT_PATH = LIBRARY_ROOT / "src" / "erbsland" / "cterm" / "impl" / "CommonBoxFrameStyle.cpp"
BOX_FRAME_CHARACTER_DATA = """\
┌┬┐├┼┤└┴┘
┏┳┓┣╋┫┗┻┛
┍┯┑┝┿┥┕┷┙
┎┰┒┠╂┨┖┸┚
╒╤╕╞╪╡╘╧╛
╔╦╗╠╬╣╚╩╝
╓╥╖╟╫╢╙╨╜
╼╾╽╿╱╲╳
 ┮┱┲┭
┡┽╀╁┾┩
┟╆╈╇╅┧
┞╄╉╊╃┦
┢┶┹┺┵┪
╴╵╶╷╭╮
╸╹╺╻╰╯
│┃┊┋┆┇
─━┈┉┄┅
╎╌╏╍║═︎
∙▪▫
"""
POSITION_NAMES = ("right", "down", "left", "up", "slash", "backslash", "center")
POSITION_WEIGHTS = {
    "right": 4,
    "down": 4,
    "left": 4,
    "up": 4,
    "slash": 5,
    "backslash": 5,
    "center": 6,
}
MINIMUM_IMPROVEMENT = 100


class StrokeType(IntEnum):
    NONE = 0
    LIGHT_QUADRUPLE_DASH = 1
    HEAVY_QUADRUPLE_DASH = 2
    LIGHT_TRIPLE_DASH = 3
    HEAVY_TRIPLE_DASH = 4
    LIGHT_DOUBLE_DASH = 5
    HEAVY_DOUBLE_DASH = 6
    LIGHT = 7
    DOUBLE = 8
    HEAVY = 9


STYLE_TOKENS = {
    "LIGHT": StrokeType.LIGHT,
    "HEAVY": StrokeType.HEAVY,
    "DOUBLE": StrokeType.DOUBLE,
    "SINGLE": StrokeType.LIGHT,
}
GROUPS = {
    "UP": ("up",),
    "DOWN": ("down",),
    "LEFT": ("left",),
    "RIGHT": ("right",),
    "VERTICAL": ("up", "down"),
    "HORIZONTAL": ("left", "right"),
    "LEFT DOWN": ("left", "down"),
    "RIGHT DOWN": ("right", "down"),
    "LEFT UP": ("left", "up"),
    "RIGHT UP": ("right", "up"),
    "DOWN HORIZONTAL": ("down", "left", "right"),
    "UP HORIZONTAL": ("up", "left", "right"),
    "RIGHT VERTICAL": ("right", "up", "down"),
    "LEFT VERTICAL": ("left", "up", "down"),
}


@dataclass(frozen=True)
class Attributes:
    right: StrokeType = StrokeType.NONE
    down: StrokeType = StrokeType.NONE
    left: StrokeType = StrokeType.NONE
    up: StrokeType = StrokeType.NONE
    slash: StrokeType = StrokeType.NONE
    backslash: StrokeType = StrokeType.NONE
    center: StrokeType = StrokeType.NONE


def all_characters() -> list[str]:
    result: list[str] = []
    for character in BOX_FRAME_CHARACTER_DATA:
        if character == "\n":
            continue
        if 0xFE00 <= ord(character) <= 0xFE0F:
            continue
        if character not in result:
            result.append(character)
    return result


def apply_positions(
    attributes: Attributes,
    positions: tuple[str, ...],
    stroke_type: StrokeType,
    *,
    include_center: bool = True,
) -> Attributes:
    values = asdict(attributes)
    for position in positions:
        values[position] = stroke_type
    if include_center and stroke_type > values["center"]:
        values["center"] = stroke_type
    return Attributes(**values)


def parse_group(text: str) -> tuple[str, ...]:
    if text not in GROUPS:
        raise ValueError(f"Unsupported box-drawing group: {text!r}")
    return GROUPS[text]


def extract_style(segment: str, inherited_style: StrokeType | None) -> tuple[StrokeType, str]:
    segment = segment.replace(" SINGLE", " LIGHT")
    dash_variant: str | None = None
    for marker, variant in (
        (" QUADRUPLE DASH", "quadruple"),
        (" TRIPLE DASH", "triple"),
        (" DOUBLE DASH", "double"),
    ):
        if marker in segment:
            segment = segment.replace(marker, "")
            dash_variant = variant
            break
    words = segment.split()
    if words and words[0] in STYLE_TOKENS:
        stroke_type = STYLE_TOKENS[words[0]]
        group_text = " ".join(words[1:])
    elif words and words[-1] in STYLE_TOKENS:
        stroke_type = STYLE_TOKENS[words[-1]]
        group_text = " ".join(words[:-1])
    else:
        if inherited_style is None:
            raise ValueError(f"Cannot infer style for segment: {segment!r}")
        stroke_type = inherited_style
        group_text = segment
    if dash_variant == "quadruple":
        stroke_type = (
            StrokeType.HEAVY_QUADRUPLE_DASH if stroke_type == StrokeType.HEAVY else StrokeType.LIGHT_QUADRUPLE_DASH
        )
    elif dash_variant == "triple":
        stroke_type = StrokeType.HEAVY_TRIPLE_DASH if stroke_type == StrokeType.HEAVY else StrokeType.LIGHT_TRIPLE_DASH
    elif dash_variant == "double":
        stroke_type = StrokeType.HEAVY_DOUBLE_DASH if stroke_type == StrokeType.HEAVY else StrokeType.LIGHT_DOUBLE_DASH
    return stroke_type, group_text


def attributes_from_name(character: str) -> Attributes:
    if character == " ":
        return Attributes()
    if character == "∙":
        return Attributes(center=StrokeType.LIGHT)
    if character == "▪":
        return Attributes(center=StrokeType.HEAVY)
    if character == "▫":
        return Attributes(center=StrokeType.DOUBLE)

    unicode_name = unicodedata.name(character)
    if not unicode_name.startswith("BOX DRAWINGS "):
        raise ValueError(f"Unsupported character: {character!r}")
    box_name = unicode_name.removeprefix("BOX DRAWINGS ")

    if box_name.startswith("LIGHT ARC "):
        attributes = Attributes()
        for segment in box_name.removeprefix("LIGHT ARC ").split(" AND "):
            attributes = apply_positions(attributes, parse_group(segment), StrokeType.LIGHT, include_center=False)
        return attributes
    if box_name == "LIGHT DIAGONAL UPPER RIGHT TO LOWER LEFT":
        return Attributes(slash=StrokeType.LIGHT, center=StrokeType.LIGHT)
    if box_name == "LIGHT DIAGONAL UPPER LEFT TO LOWER RIGHT":
        return Attributes(backslash=StrokeType.LIGHT, center=StrokeType.LIGHT)
    if box_name == "LIGHT DIAGONAL CROSS":
        return Attributes(slash=StrokeType.LIGHT, backslash=StrokeType.LIGHT, center=StrokeType.LIGHT)

    attributes = Attributes()
    inherited_style: StrokeType | None = None
    for segment in box_name.split(" AND "):
        stroke_type, group_text = extract_style(segment, inherited_style)
        attributes = apply_positions(attributes, parse_group(group_text), stroke_type)
        inherited_style = stroke_type
    return attributes


def build_character_attributes(characters: list[str]) -> dict[str, Attributes]:
    return {character: attributes_from_name(character) for character in characters}


def combine_attributes(current: Attributes, overlay: Attributes) -> Attributes:
    return Attributes(
        **{
            position: max(getattr(current, position), getattr(overlay, position))
            for position in POSITION_NAMES
        }
    )


def has_lines(attributes: Attributes) -> bool:
    return any(getattr(attributes, position) != StrokeType.NONE for position in POSITION_NAMES[:-1])


def is_center_only(attributes: Attributes) -> bool:
    return attributes.center != StrokeType.NONE and not has_lines(attributes)


def score_candidate(ideal: Attributes, candidate: Attributes) -> int:
    missing_score = 0
    distance_score = 0
    extra_score = 0
    for position in POSITION_NAMES:
        ideal_stroke = getattr(ideal, position)
        candidate_stroke = getattr(candidate, position)
        weight = POSITION_WEIGHTS[position]
        if ideal_stroke != StrokeType.NONE and candidate_stroke == StrokeType.NONE:
            missing_score += weight
        elif ideal_stroke == StrokeType.NONE and candidate_stroke != StrokeType.NONE:
            extra_score += weight * (2 + int(candidate_stroke))
        elif ideal_stroke != StrokeType.NONE and candidate_stroke != StrokeType.NONE:
            distance_score += weight * abs(int(ideal_stroke) - int(candidate_stroke))
    return missing_score * 10_000 + distance_score * 100 + extra_score


def choose_result(
    current: str,
    overlay: str,
    character_attributes: dict[str, Attributes],
    attributes_to_character: dict[Attributes, str],
    characters: list[str],
) -> tuple[str, str]:
    current_attributes = character_attributes[current]
    overlay_attributes = character_attributes[overlay]
    if is_center_only(current_attributes) and has_lines(overlay_attributes):
        return overlay, "center-overwrite"
    if has_lines(current_attributes) and is_center_only(overlay_attributes):
        return current, "center-overwrite"

    ideal = combine_attributes(current_attributes, overlay_attributes)
    exact_match = attributes_to_character.get(ideal)
    if exact_match is not None:
        return exact_match, "exact"

    best_character = min(characters, key=lambda character: score_candidate(ideal, character_attributes[character]))
    best_score = score_candidate(ideal, character_attributes[best_character])
    overlay_score = score_candidate(ideal, overlay_attributes)
    if best_score + MINIMUM_IMPROVEMENT < overlay_score:
        return best_character, "best"
    return overlay, "overlay"


def cpp_u32_string_literal(text: str) -> str:
    return 'U"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'


def generate_matrix(characters: list[str]) -> tuple[list[str], Counter[str]]:
    character_attributes = build_character_attributes(characters)
    attributes_to_character = {attributes: character for character, attributes in character_attributes.items()}
    result_rows: list[str] = []
    selection_counts: Counter[str] = Counter()
    for current in characters:
        row: list[str] = []
        for overlay in characters:
            result, selection = choose_result(
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


def generate_result_index_rows(characters: list[str], result_rows: list[str]) -> list[str]:
    character_index = {character: index for index, character in enumerate(characters)}
    index_rows: list[str] = []
    for row in result_rows:
        encoded_row = "".join(f"\\x{character_index[character]:02X}" for character in row)
        index_rows.append('"' + encoded_row + '"')
    return index_rows


def render_output(characters: list[str], result_rows: list[str], selection_counts: Counter[str]) -> str:
    result_index_rows = generate_result_index_rows(characters, result_rows)
    matrix_size = len(characters) * len(characters)
    lines = [
        "// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev",
        "// SPDX-License-Identifier: Apache-2.0",
        "//",
        "// This file is generated by erbsland-cpp-color-term/utilities/generate_common_box_frame_style.py",
        "//",
        (
            "// Generated from "
            f"{sum(selection_counts.values())} directed combinations: "
            f"{selection_counts['exact']} exact, "
            f"{selection_counts['best']} weighted matches, "
            f"{selection_counts['overlay']} overlay fallbacks, "
            f"{selection_counts['center-overwrite']} center overwrites."
        ),
        "#include \"../CharCombinationStyle.hpp\"",
        "",
        "#include <memory>",
        "",
        "",
        "namespace erbsland::cterm {",
        "",
        "",
        "auto CharCombinationStyle::commonBoxFrame() noexcept -> const CharCombinationStylePtr & {",
        "    static const CharCombinationStylePtr style =",
        "        std::make_shared<MatrixCombinationStyle>(",
        f"            {cpp_u32_string_literal(''.join(characters))},",
        f"            std::string_view{{",
    ]
    for row in result_index_rows:
        lines.append(f"                {row}")
    lines.extend(
        [
            f"                , {matrix_size}}}",
            "            );",
            "    return style;",
            "}",
            "",
            "",
            "}",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="Verify that the generated file is up to date.")
    args = parser.parse_args()

    characters = all_characters()
    result_rows, selection_counts = generate_matrix(characters)
    output = render_output(characters, result_rows, selection_counts)

    if args.check:
        existing_output = OUTPUT_PATH.read_text(encoding="utf-8")
        if existing_output != output:
            raise SystemExit("The generated common box frame style file is out of date.")
        return

    OUTPUT_PATH.write_text(output, encoding="utf-8")


if __name__ == "__main__":
    main()
