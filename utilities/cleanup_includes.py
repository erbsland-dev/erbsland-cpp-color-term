# Copyright (c) 2024-2025 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0


import argparse
import re
from operator import attrgetter
from pathlib import Path


class ScriptError (Exception):
    """
    Script error used for error reporting.
    """
    pass


class Include:
    INCLUDE_CATEGORY_PATTERNS = [
        (re.compile(r'^"[^.][^/]+"'), 1),
        (re.compile(r'^"[^./]+/.+"'), 2),
        (re.compile(r'^"\.\./[^.].+"'), 3),
        (re.compile(r'^"\.\./\.\./.+"'), 4),
        (re.compile(r'^<.*\.h(pp)?>'), 5),
        (re.compile(r'^<[^.]+>'), 6),
        (re.compile(r'.*'), 7),
    ]

    def __init__(self, path: str, is_global: bool):
        self.path = path
        self.is_global = is_global
        self.check_for_problems()
        self.group_id = self.get_group_id()
        self.sort_path = self.get_sort_path()

    def __eq__(self, other):
        if not isinstance(other, Include):
            return False
        return self.sort_path == other.sort_path and self.is_global == other.is_global

    def check_for_problems(self):
        if self.path.startswith('src/') or self.path.startswith('erbsland/'):
            raise ScriptError(f'Include statement with invalid path: {self.path}')

    def get_group_id(self) -> int:
        include_name = self.get_include_name()
        for pattern, priority in self.INCLUDE_CATEGORY_PATTERNS:
            if pattern.match(include_name):
                return priority
        return 7

    def get_include_name(self) -> str:
        if self.is_global:
            return f'<{self.path}>'
        return f'"{self.path}"'

    def get_sort_path(self) -> str:
        return self.get_include_name().casefold()


class WorkingSet:
    """
    The working set for this script.
    """

    EXCLUDED_NAMES = [
        'fwd.hpp',
        'all.hpp'
    ]

    RE_INCLUDE_BLOCK_H = re.compile(r"""
        # Skip a BOM
        \ufeff?
        # Skip copyright block
        (?://.*\n)+(?:[ \t]*\n)*
        # Skip pragma
        \#pragma[ \t]+once[ \t]*\n
        # Skip initial empty lines
        ((?:[ \t]*\n)*
        # Capture a continous block of include statements.
        (?:\#include.+\n|[ \t]*\n)+)
    """, re.VERBOSE)

    RE_INCLUDE_BLOCK_CPP = re.compile(r"""
        # Skip a BOM
        \ufeff?
        # Skip copyright block
        (?://.*\n)+(?:[ \t]*\n)*
        # Skip initial include statement
        \#include.+\n
        # Skip initial empty lines
        ((?:[ \t]*\n)*
        # Capture a continous block of include statements
        (?:\#include.+\n|[ \t]*\n)+)
    """, re.VERBOSE)

    RE_INCLUDE_LINE = re.compile(r"""
        \#include\s+(<|")([^>"]+)(>|")
    """, re.VERBOSE)

    def __init__(self):
        """
        Create a new empty working set.
        """
        self.verbose = False                       # If verbose messages shall be shown
        self.project_dir = Path()                  # The project directory
        self.src_dir = Path()                      # The directory with the sources.

    def process_file(self, path: Path, is_header: bool):
        if self.verbose:
            print(f'Processing file: {path.relative_to(self.src_dir)}')
        text = path.read_text(encoding='utf-8')
        if is_header:
            if '#pragma once' not in text:
                raise ScriptError('Missing "#pragma once" in header file!')
            match = self.RE_INCLUDE_BLOCK_H.match(text)
        else:
            match = self.RE_INCLUDE_BLOCK_CPP.match(text)
        if not match or not match.group(1).strip():
            if self.verbose:
                print('  no include block found.')
            return
        include_list: list[Include] = []
        for line in match.group(1).splitlines():
            line_match = self.RE_INCLUDE_LINE.match(line)
            if not line_match:
                continue
            inc_path = line_match.group(2)
            if inc_path.startswith('src/erbsland/'):
                # solve CLion refactoring issue with wrong include paths.
                target_path = self.src_dir / inc_path.replace('src/erbsland/', '')
                rel_to_path_parts = path.relative_to(self.src_dir).parts
                rel_to_target_parts = target_path.relative_to(self.src_dir).parts
                if not rel_to_path_parts or not rel_to_target_parts:
                    raise ScriptError('Unexpected empty paths')
                # remove common directories.
                while rel_to_path_parts and rel_to_target_parts and rel_to_path_parts[0] == rel_to_target_parts[0]:
                    rel_to_path_parts = rel_to_path_parts[1:]
                    rel_to_target_parts = rel_to_target_parts[1:]
                # Create a relative path.
                up_count = max(0, len(rel_to_path_parts) - 1)
                inc_path = '../' * up_count + '/'.join(rel_to_target_parts)
            include = Include(inc_path, line_match.group(1) == '<')
            if include in include_list:
                print(f'  ignored duplicate include statement: {line}')
                continue
            include_list.append(include)
        if not include_list:
            if self.verbose:
                print('Ignoring file without relevant includes.')
            return
        include_list.sort(key=attrgetter('group_id', 'sort_path'))
        last_group_id = 0
        new_block = '\n\n'
        if include_list:
            for entry in include_list:
                if last_group_id > 0 and last_group_id != entry.group_id:
                    new_block += '\n'
                if entry.is_global:
                    start_quote = '<'
                    end_quote = '>'
                else:
                    start_quote = '"'
                    end_quote = '"'
                new_block += f'#include {start_quote}{entry.path}{end_quote}\n'
                last_group_id = entry.group_id
            new_block += '\n\n'
        else:
            new_block = ''
        new_text = text[0:match.start(1)]
        new_text += new_block
        new_text += text[match.end(1):]
        if text != new_text:
            if self.verbose:
                print('  overwriting changed file')
            path.write_text(new_text, encoding='utf-8')

    def scan_code_files(self):
        paths = list(self.src_dir.rglob('*.hpp'))
        paths.extend(self.src_dir.rglob('*.tpp'))
        paths.extend(self.src_dir.rglob('*.cpp'))
        paths.sort(key=lambda p: str(p).casefold())
        for path in paths:
            if path.name in self.EXCLUDED_NAMES:
                continue
            self.process_file(path, path.name.endswith(('.hpp', '.h', '.hxx', '.tpp')))

    def parse_cmd(self):
        """
        Parse the command line arguments.
        """
        parser = argparse.ArgumentParser(
            description='Cleanup/sort #include statements.')
        parser.add_argument('-v', '--verbose', action='store_true')
        args = parser.parse_args()
        if args.verbose:
            self.verbose = True
        # No arguments for this script.
        self.project_dir = Path(__file__).parent.parent
        self.src_dir = self.project_dir / 'src'
        if not self.src_dir.is_dir():
            raise ScriptError('The source dir does not exist.')
        self.src_dir = self.src_dir / 'erbsland'

    def run(self):
        """
        Run this script.
        """
        self.parse_cmd()
        self.scan_code_files()


def main():
    try:
        ws = WorkingSet()
        ws.run()
    except ScriptError as err:
        exit(err)


if __name__ == '__main__':
    main()
