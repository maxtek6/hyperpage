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
Usage: hyperpack [--help] [--output VAR] directory

Positional arguments:
  directory      Directory to scan for files to pack into the hyperpage database [required]

Optional arguments:
  -h, --help     shows help message and exits
  -o, --output   Output file for the hyperpage database [nargs=0..1] [default: "hyperpage.db"]
```

It will iterate through a given directory, storing its files based on 
paths relative to the given directory. It will also detect the mime
type and store the contents of each file.

### Documentation and Example

This is only intended to cover basic usage. For more info about the API,
see the [docs](https://maxtek6.github.io/docs/hyperpage). To see how hyperpage is used in a basic use case, the [example](https://github.com/maxtek6/hyperpage/tree/master/example) should be helpful. 