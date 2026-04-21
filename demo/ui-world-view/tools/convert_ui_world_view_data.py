#!/usr/bin/env python3
#  Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
#  SPDX-License-Identifier: Apache-2.0
"""Convert Natural Earth reference data into ui-world-view text asset."""

from __future__ import annotations

import argparse
import datetime as dt
import math
from dataclasses import dataclass
from pathlib import Path

import shapefile  # type: ignore[import-not-found]
from PIL import Image, ImageDraw  # type: ignore[import-not-found]

REPO_ROOT = Path(__file__).resolve().parents[2]
LAND_PATH = REPO_ROOT / "temporary_data/110m_physical/ne_110m_land.shp"
COUNTRIES_PATH = REPO_ROOT / "temporary_data/110m_cultural/ne_110m_admin_0_countries.shp"
OUTPUT_PATH = REPO_ROOT / "demo/ui-world-view/data/world-map.txt"
ZOOM_SIZES = [(120, 60), (240, 120), (480, 240), (960, 480)]
LABEL_PADDING_BY_ZOOM = {
    0: (4, 2),
    1: (3, 1),
    2: (2, 1),
    3: (1, 0),
}


@dataclass(slots=True)
class CountryLabelCandidate:
    name: str
    lon: float
    lat: float
    labelrank: int
    population: float
    area: float


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--land", type=Path, default=LAND_PATH, help="Input land polygon shapefile.")
    parser.add_argument("--countries", type=Path, default=COUNTRIES_PATH, help="Input countries shapefile.")
    parser.add_argument("--output", type=Path, default=OUTPUT_PATH, help="Generated output text file.")
    parser.add_argument("--check", action="store_true", help="Verify that the generated file is up to date.")
    return parser.parse_args()


def unwrap_ring(points: list[tuple[float, float]]) -> list[tuple[float, float]]:
    if not points:
        return []
    result = [points[0]]
    previous_lon = points[0][0]
    for lon, lat in points[1:]:
        while lon - previous_lon > 180.0:
            lon -= 360.0
        while previous_lon - lon > 180.0:
            lon += 360.0
        result.append((lon, lat))
        previous_lon = lon
    return result


def split_parts(shape: shapefile.Shape) -> list[list[tuple[float, float]]]:
    indices = list(shape.parts) + [len(shape.points)]
    return [shape.points[indices[i]: indices[i + 1]] for i in range(len(indices) - 1)]


def project(lon: float, lat: float, width: int, height: int) -> tuple[int, int]:
    x = round(((lon + 180.0) / 360.0) * (width - 1))
    y = round(((90.0 - lat) / 180.0) * (height - 1))
    return max(0, min(width - 1, x)), max(0, min(height - 1, y))


def draw_ring(draw: ImageDraw.ImageDraw, ring: list[tuple[float, float]], width: int, height: int) -> None:
    if len(ring) < 3:
        return
    unwrapped = unwrap_ring(ring)
    min_lon = min(lon for lon, _ in unwrapped)
    max_lon = max(lon for lon, _ in unwrapped)
    for shift in (-360.0, 0.0, 360.0):
        shifted_min = min_lon + shift
        shifted_max = max_lon + shift
        if shifted_max < -180.0 or shifted_min > 180.0:
            continue
        polygon = [project(lon + shift, lat, width, height) for lon, lat in unwrapped]
        draw.polygon(polygon, fill=1)


def render_land_mask(land_reader: shapefile.Reader, width: int, height: int) -> list[list[bool]]:
    image = Image.new("1", (width, height), 0)
    draw = ImageDraw.Draw(image)
    for shape in land_reader.shapes():
        for ring in split_parts(shape):
            draw_ring(draw, ring, width, height)
    pixels = image.load()
    mask = [[bool(pixels[x, y]) for x in range(width)] for y in range(height)]
    for row in mask:
        if row:
            # Keep the antimeridian visually open. Some polygon fills otherwise leave a false land seam in column 0.
            row[0] = False
    return mask


def polygon_area(ring: list[tuple[float, float]]) -> float:
    if len(ring) < 3:
        return 0.0
    area = 0.0
    for index, (x1, y1) in enumerate(ring):
        x2, y2 = ring[(index + 1) % len(ring)]
        area += x1 * y2 - x2 * y1
    return area / 2.0


def polygon_centroid(ring: list[tuple[float, float]]) -> tuple[float, float]:
    area = polygon_area(ring)
    if math.isclose(area, 0.0):
        xs = [x for x, _ in ring]
        ys = [y for _, y in ring]
        return (sum(xs) / max(len(xs), 1), sum(ys) / max(len(ys), 1))
    cx = 0.0
    cy = 0.0
    for index, (x1, y1) in enumerate(ring):
        x2, y2 = ring[(index + 1) % len(ring)]
        cross = x1 * y2 - x2 * y1
        cx += (x1 + x2) * cross
        cy += (y1 + y2) * cross
    factor = 1.0 / (6.0 * area)
    return cx * factor, cy * factor


def preferred_name(record: dict[str, object]) -> str:
    for field in ("NAME", "ABBREV", "SOVEREIGNT"):
        value = str(record.get(field, "")).strip()
        if value:
            return value
    return "?"


def extract_label_candidates(countries_reader: shapefile.Reader) -> list[CountryLabelCandidate]:
    candidates: list[CountryLabelCandidate] = []
    for shape_record in countries_reader.iterShapeRecords():
        record = shape_record.record.as_dict()
        parts = [unwrap_ring(part) for part in split_parts(shape_record.shape)]
        if not parts:
            continue
        largest = max(parts, key=lambda ring: abs(polygon_area(ring)))
        area = abs(polygon_area(largest))
        if math.isclose(area, 0.0):
            continue
        lon, lat = polygon_centroid(largest)
        candidates.append(
            CountryLabelCandidate(
                name=preferred_name(record),
                lon=lon,
                lat=lat,
                labelrank=int(float(record.get("LABELRANK", 99) or 99)),
                population=float(record.get("POP_EST", 0.0) or 0.0),
                area=area,
            )
        )
    candidates.sort(key=lambda item: (item.labelrank, -item.population, -item.area, item.name))
    return candidates


def label_rect(
        candidate: CountryLabelCandidate, width: int, height: int, zoom_index: int
) -> tuple[int, int, int, int] | None:
    x, y = project(candidate.lon, candidate.lat, width, height)
    rendered_x = x * 2
    label_width = len(candidate.name)
    left = rendered_x - label_width // 2
    right = left + label_width
    horizontal_padding, vertical_padding = LABEL_PADDING_BY_ZOOM[zoom_index]
    if left < 0 or right > width * 2 or y < 0 or y >= height:
        return None
    return (
        left - horizontal_padding,
        max(y - vertical_padding, 0),
        right + horizontal_padding,
        min(y + vertical_padding + 1, height),
    )


def overlaps(bounds1: tuple[int, int, int, int], bounds2: tuple[int, int, int, int]) -> bool:
    return not (
            bounds1[2] <= bounds2[0] or bounds2[2] <= bounds1[0] or bounds1[3] <= bounds2[1] or bounds2[3] <= bounds1[1]
    )


def filter_labels(
        candidates: list[CountryLabelCandidate], width: int, height: int, zoom_index: int
) -> list[tuple[int, int, str]]:
    accepted: list[tuple[int, int, str]] = []
    occupied: list[tuple[int, int, int, int]] = []
    for candidate in candidates:
        rect = label_rect(candidate, width, height, zoom_index)
        if rect is None:
            continue
        if any(overlaps(rect, existing) for existing in occupied):
            continue
        x, y = project(candidate.lon, candidate.lat, width, height)
        accepted.append((x, y, candidate.name))
        occupied.append(rect)
    return accepted


def encode_row(row: list[bool]) -> str:
    spans: list[str] = []
    current = False
    count = 0
    for pixel in row:
        if pixel == current:
            count += 1
            continue
        spans.append(f"{count}{'L' if current else 'S'}")
        current = pixel
        count = 1
    spans.append(f"{count}{'L' if current else 'S'}")
    return " ".join(spans)


def generate_output(land_path: Path, countries_path: Path) -> str:
    land_reader = shapefile.Reader(str(land_path))
    countries_reader = shapefile.Reader(str(countries_path))
    labels = extract_label_candidates(countries_reader)
    lines = [
        "# ui-world-view generated world map data",
        f"# generator: {Path(__file__).relative_to(REPO_ROOT).as_posix()}",
        f"# source land: {land_path.relative_to(REPO_ROOT).as_posix()}",
        f"# source countries: {countries_path.relative_to(REPO_ROOT).as_posix()}",
        f"# generated at: {dt.datetime.now(dt.timezone.utc).isoformat()}",
        "format ui-world-view-map 1",
        "",
    ]
    for zoom_index, (width, height) in enumerate(ZOOM_SIZES):
        mask = render_land_mask(land_reader, width, height)
        zoom_labels = filter_labels(labels, width, height, zoom_index)
        lines.append(f"zoom {zoom_index} {width} {height}")
        for row in mask:
            lines.append(f"row {encode_row(row)}")
        lines.append(f"labels {len(zoom_labels)}")
        for x, y, text in zoom_labels:
            lines.append(f"label {x} {y} {text}")
        lines.append("endzoom")
        lines.append("")
    return "\n".join(lines)


def main() -> None:
    args = parse_arguments()
    output_text = generate_output(args.land, args.countries)
    if args.check:
        existing = args.output.read_text(encoding="utf-8")
        if existing != output_text:
            raise SystemExit("The generated ui-world-view map data is out of date.")
        return
    args.output.write_text(output_text, encoding="utf-8")


if __name__ == "__main__":
    main()
