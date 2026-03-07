#  Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
#  SPDX-License-Identifier: CC-BY-4.0

import re
import shutil
import sys
from pathlib import Path

import erbsland.conf_pygments.lexer

sys.path.insert(0, str(Path(__file__).parent))

# -- Project information -----------------------------------------------------
project = "Erbsland Color Terminal for C++"
copyright = "2026, Tobias Erbsland - Erbsland DEV"
author = "Tobias Erbsland - Erbsland DEV"
release = "1.0"

# -- General configuration ---------------------------------------------------
extensions = [
    "sphinx_rtd_theme",
    "sphinx_design",
    "sphinx_copybutton",
    "breathe",
    "erbsland.sphinx.ansi"]
templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output -------------------------------------------------
html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_css_files = [
    "custom.css",
    "ansi.css",
]
html_js_files = [
    "https://erbsland.dev/ext/fa7/js/all.min.js"
]

# -- Options for Breathe -----------------------------------------------------
_project_dir = Path(__file__).parent.parent
_original_src_dir = _project_dir / "src"
_processed_src_dir = _project_dir / "_doxygen_input"
breathe_projects = {"erbsland-color-term": _project_dir / "_build/breathe/doxygen/erbsland-color-term/xml"}
breathe_default_project = "erbsland-color-term"
breathe_projects_source = {"erbsland-color-term": (_processed_src_dir, ["erbsland/cterm"])}
breathe_doxygen_config_options = {
    "STRIP_FROM_PATH": _processed_src_dir,
    "STRIP_FROM_INC_PATH": _processed_src_dir,
    "JAVADOC_AUTOBRIEF": "yes",
    "ALIASES": "\"tested=@par Tested:^^\", \"notest=@par Not Tested:^^\", \"needtest=@par Needs Testing:^^\", \"wip=@par Work in Progress:^^\"",
    "ENABLE_PREPROCESSING": "yes",
    "MACRO_EXPANSION": "no",
}


def _swap_tr(match):
    return match.group(2) + " " + match.group(1) + " " + match.group(3)


def preprocess_sources():
    print("Preprocessing sources...")

    re_swap_tr = re.compile(r"auto\s+(.*?)\s+->\s*(.*?)\s*(override\s*[;{]|=\s*(?:0|default)\s*;|[;{])")
    re_std_helpers = re.compile(r"(?s)template\s*<>\s*\nstruct.*\{\n.*\n\};\n")
    re_macros = re.compile(r"""(?x)
        ERBSLAND_COLOR_TERM_[A-Z_]+ .*\n
    """)
    re_requires = re.compile(r"(?s)(template.*?)\s+requires\s*\(.*?\)\s*\n")
    re_one_line_specialization = re.compile(r"(?s)template<>\s+\[\[nodiscard\]\]\s+inline.*?\}\s*\n")

    # Clean output dir
    if _processed_src_dir.exists():
        shutil.rmtree(_processed_src_dir)
    _processed_src_dir.mkdir(parents=True, exist_ok=True)

    # Copy and patch files
    for src_path in _original_src_dir.rglob("*.hpp"):
        dst_path = _processed_src_dir / src_path.relative_to(_original_src_dir)
        content = src_path.read_text(encoding="utf-8")
        content = re_swap_tr.sub(_swap_tr, content)
        content = re_std_helpers.sub("", content)
        content = re_macros.sub("// macro removed\n", content)
        content = re_requires.sub("\\1/* requires removed */\n", content)
        content = re_one_line_specialization.sub("", content)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        dst_path.write_text(content, encoding="utf-8")


# -- Add the syntax highlighter ---------------------------------------------
def setup(app):
    preprocess_sources()
