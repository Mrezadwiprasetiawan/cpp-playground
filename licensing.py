import os

header = '''\
/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
'''

def has_license(content):
  return "GNU General Public License" in content

def process_file(filepath):
  with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()
  if not has_license(content):
    with open(filepath, 'w', encoding='utf-8') as f:
      f.write(header + '\n\n' + content)
    print(f"Header added: {filepath}")
  else:
    print(f"Already licensed: {filepath}")

for root, _, files in os.walk('.'):
  for file in files:
    if file.endswith(('.cxx', '.hxx')):
      process_file(os.path.join(root, file))
