## Basic Unit Test
Basic Unit Test (BUT) is a simple unit-test driver. The idea is collect related unit tests into a test suite that is compiled and linked into a shared library (DLL, .so, whatever). The test driver is a just a command-line tool that accepts paths to shared libraries. It will load them and run the tests within.

Test libraries (DLLs) are expected to export one function, `BUTTestSuite *but_test_suite()` that returns the address of the `BUTTestSuite` object defined therein.

It currently builds only on Windows with Visual Studio 2017, 2019, or 2020. It will probably build and run on other operating systems with other C compilers, I just haven't included build scripts/tools for those, yet.

## How to Build
The initial build system relies on Visual Studio (2017, 2019, or 2022) and the `.cmd` scripts under `build/cmd/`. `all.cmd` builds both the exceptions library, the test driver,`but.exe`, and unit tests for both. `exceptions.cmd` and `but.cmd` build their respective components. The `exceptions.cmd` script will also build `but.exe` if the `test` option is passed in so its unit tests can be executed. All build artifacts are written to various subdirectories of the `target` directory.

Those three build scripts accept the following commands:

- `build`: build the project. May be combined with release or debug (default).
- `debug`: build without optimizations (default).
- `release`: build with optimizations.
- `cleanall`: delete all build artifacts for the configured compiler.
- `clean`: delete all build artifacts for the debug/release configuration.
- `cleanplat`: delete all build artifacts for the 32-bit/64-bit configuration.
- `x64`: specifies a 64-bit build (default).
- `x86`: specifies a 32-bit build.
- `win32`: specifies a 32-bit build.
- `test`: build the current configuration and run all unit tests.
- `timed`: run ctime for timed builds. The .ctm files are written to the metrics folder. N.B.: build ctime first.
- `vs2017`: use Visual Studio 2017. The scripts will search for MS Build, Pro, and Community Edition in that order.
- `vs2019`: use Visual Studio 2019. The scripts will search for MS Build, Pro, and Community Edition in that order.
- `vs2022`: use Visual Studio 2022. The scripts will search for MS Build, Pro, and Community Edition in that order.
- `verbose`: Display details of steps during the build process.
- `trace`: Display the values of these options.-

## Run the Examples
I still need to create some examples outside of the code itself. In the meantime, open a Windows command prompt, navigate to the root of the repo and run `build\cmd\all.cmd test` to build and execute all of the unit tests for the exceptions library and the BUT test driver itself. You can run `build\cmd\all.cmd test clean` to first delete the build artifacts, then rebuild everything and run the tests. The order of `test` and `clean` doesn't matter. Similarly, to create a release build and run all of the tests, run `build\cmd\all.cmd test release`. To delete all build artifacts, simply run `build\cmd\all.cmd cleanall`.

## Project Status
It works. Examples and build scripts to use clang/llvm instead of VS/MSBuild will follow before too long.

## Notes

- `config.cmd` sets environment variables for debug and release compiler and linker options.
- `options.cmd` processes the commands for the build scripts.
- `setup.cmd` sets some environment variables based on some of the commands passed to the build scripts.
- `shell.cmd` has common/default install paths for supported versions of Visual Studio and MSBuild. I will replace these hardcoded paths with the output of `vswhere.exe`, a tool for locating and listing installed versions of VS and MSBuild.
