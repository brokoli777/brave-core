#!/usr/bin/env python3

from os.path import dirname

import os
import sys

def main():
    src_path, dst_path = sys.argv[1:]
    os.makedirs(dirname(dst_path), exist_ok=True)
    with open(src_path, 'r') as src:
        contents = src.read()
    to_replace = '<target namespace="Google.Policies"'
    assert to_replace in contents, contents
    replace_with = '<target namespace="Brave.Policies"'
    new_contents = contents.replace(to_replace, replace_with)
    with open(dst_path, 'w') as dst:
        dst.write(new_contents)

if __name__ == "__main__":
    main()
