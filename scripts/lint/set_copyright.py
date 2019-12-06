#!/usr/bin/env python3
#
# Copyright (C) 2018-2019 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

"""Usage: ./scripts/lint/set_copyright.py <files>"""

import re
import sys
import os
import datetime
import stat
import argparse


def is_banned(path):
    """Check if path is banned."""

    banned_paths = [
        'scripts/tests/copyright/in',
        'scripts/tests/copyright/out',
        'third_party'
    ]

    path_banned = False
    dirname = os.path.dirname(path)
    for banned_path in banned_paths:
        if dirname.startswith(banned_path):
            path_banned = True
            break

    return path_banned


def can_be_scanned(path):
    """Check whether we should scan this file"""

    allowed_extensions = [
        'cpp', 'h', 'inl', 'hpp', 'm',
        'cmake',
        'py', 'sh',
        'cl',
        'exports'
    ]

    allowed_extensions_2 = [
        'h.in', 'rc.in',
        'options.txt'
    ]

    allowed_files = [
        'CMakeLists.txt'
    ]

    path_ext = path.split('.')
    path_ok = False
    filename = os.path.basename(path)

    if not os.path.isfile(path):
        print('Cannot find file {}, skipping'.format(path))
        path_ok = False

    elif is_banned(path):
        path_ok = False

    elif filename in allowed_files:
        path_ok = True

    elif path_ext[-1].lower() in allowed_extensions:
        path_ok = True

    elif '.'.join(path_ext[-2:]) in allowed_extensions_2:
        path_ok = True

    if not path_ok:
        print('[MIT] Ignoring file: {}'.format(path))

    return path_ok


def _main():
    header_cpp = """/*
 * Copyright (C) {} Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
"""

    header_bash_style = """#
# Copyright (C) {} Intel Corporation
#
# SPDX-License-Identifier: MIT
#
"""

    cpp_sharp_lines = [
        '#pragma',
        '#include'
    ]

    parser = argparse.ArgumentParser(
        description='Usage: ./scripts/lint/set_copyright.py <files>')

    parser.add_argument('files', nargs='*')
    args = parser.parse_args()

    for path in args.files:

        # avoid self scan
        if os.path.abspath(path) == os.path.abspath(sys.argv[0]):
            continue

        if not can_be_scanned(path):
            continue

        print('[MIT] Processing file: {}'.format(path))

        gathered_lines = []
        start_year = None
        header = header_cpp
        header_start = '/*'
        header_end = '*/'
        comment_char = r'\*'

        # now read line by line
        with open(path, 'r') as fin:

            # take care of hashbang
            first_line = fin.readline()
            if not first_line.startswith('#!'):
                line = first_line
                first_line = ''
            else:
                line = fin.readline()

            is_cpp = False

            # check whether comment type is '#'
            if first_line or line.startswith('#'):
                for i in cpp_sharp_lines:
                    print('a: {} ~ {}'.format(i, line))
                    if line.startswith(i):
                        is_cpp = True
                        break

                if not is_cpp:
                    header_start = '#'
                    header_end = '\n'
                    header = header_bash_style
                    comment_char = '#'

            curr_comment = []

            is_header = None
            is_header_end = None

            # copyright have to be first comment in file
            if line.startswith(header_start):
                is_header = True
                is_header_end = False
            else:
                is_header = False
                is_header_end = True

            is_copyright = False

            while line:
                if is_header:
                    if header_end == '\n' and len(line.strip()) == 0:
                        is_header = False
                        is_header_end = True
                    elif line.strip().endswith(header_end):
                        is_header = False
                        is_header_end = True
                    elif 'Copyright' in line:
                        expr = (
                            r'^{} Copyright \([Cc]\) (\d+)( *- *\d+)?'.format(comment_char))
                        match = re.match(expr, line.strip())
                        if match:
                            start_year = match.groups()[0]
                            curr_comment = []
                            is_copyright = True
                    if not is_copyright:
                        curr_comment.append(line)
                elif is_copyright and is_header_end:
                    if len(line.strip()) > 0:
                        gathered_lines.append(line)
                        is_header_end = False
                else:
                    gathered_lines.append(line)

                line = fin.readline()

        year = datetime.datetime.now().year
        if start_year is None:
            start_year = str(year)
        elif int(start_year) < year:
            start_year += '-'
            start_year += str(year)

        # store file mode because we want to preserve this
        old_mode = os.stat(path)[stat.ST_MODE]

        os.remove(path)
        with open(path, 'w') as fout:

            if first_line:
                fout.write(first_line)

            fout.write(header.format(start_year))

            if len(curr_comment) > 0 or len(gathered_lines) > 0:
                fout.write('\n')

            if len(curr_comment) > 0:
                fout.write(''.join(curr_comment))

            contents = ''.join(gathered_lines)
            fout.write(contents)

        # chmod to original value
        os.chmod(path, old_mode)


if __name__ == '__main__':
    sys.exit(_main())
