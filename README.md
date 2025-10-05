# Hyperpage

![status](https://github.com/maxtek6/hyperpage/actions/workflows/pipeline.yml/badge.svg)
[![codecov](https://codecov.io/gh/maxtek6/hyperpage/branch/master/graph/badge.svg)](https://codecov.io/gh/maxtek6/hyperpage)

Fast, clean, and efficient solution for archiving and retrieving web content.

## Requirements

+ CMake
+ C++17
+ NodeJS (only used for example app)

## Usage

This project provides two components:

+ `hyperpage`: A library with an API for reading and writing archives
+ `hyperpack`: A command line tool for archiving web content

### `hyperpage`

Hyperpage is the C++ API provided by this project. It provides all of 
the interfaces required to utilize the hyperpage database:

+ `hyperpage::page`: An abstract class representing a single entry in 
the database. It provides the path, mime type, and content.

+ `hyperpage::reader`: Loads pages from the database. Given a path,
the reader will provide a pointer to a page if it exists.

+ `hyperpage::writer`: Stores pages in the database. Given a page, the
writer will create a database entry that can later be loaded by path.

### `hyperpack`

Hyperpack is a command line utility used to create a hyperpage database 
file:

```
Usage: hyperpack [--help] [--version] [--output VAR] [--verbose] directories...

Positional arguments:
  directories    Directories to scan for files to pack into the hyperpage database [nargs: 1 or more] [required]

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  -o, --output   Output file for the hyperpage database [nargs=0..1] [default: "hyperpage.db"]
  -v, --verbose  Show detailed output information
```

### Note on Overwriting

If two or more files share the same **relative subpath** (i.e., the same path within their respective parent directories), the file from the **rightmost directory** specified on the command line will overwrite the others in the final archive.

Only **exact path matches** are considered conflicts — differing subdirectories or filenames will coexist as separate entries.

#### Example

Suppose you run:
```bash
hyperpack -o output.hp dir1 dir2 dir3
```
And the directories contain:
```bash
dir1/Subdir1/index.html
dir2/Subdir2/index.html
dir3/Index.html
```
These will result in three distinct files inside the archive:
```bash
Subdir1/index.html
/Subdir2/index.html
/Index.html
```
However, if two or more directories contain the same relative path, for example:
```bash
dir1/public/index.html
dir2/public/index.html
```
then the file from dir2 (the rightmost one) will overwrite the file from dir1 in the resulting archive entry:
```bash
/public/index.html
```
### Documentation and Example

This is only intended to cover basic usage. For more info about the API,
see the [docs](https://maxtek6.github.io/docs/hyperpage). To see how hyperpage is used in a basic use case, the [example](https://github.com/maxtek6/hyperpage/tree/master/example) should be helpful. 
