\# C++ Disk Usage Analyzer



\## Overview



A Windows command-line utility inspired by the UNIX `du` command that scans directories and summarizes file sizes. Written in C++, it uses the standard filesystem library to traverse nested folders and report totals in bytes or estimated allocation blocks. The project demonstrates filesystem operations, command-line argument processing, structured data, and custom sorting.



\## Key Features



\- Scans the current working directory or a specified directory.

\- Recursively totals regular-file sizes within each immediate subdirectory.

\- Includes files directly inside the selected directory in its overall total.

\- Reports logical file sizes in bytes or estimated block counts, using a default block size of 4,096 bytes.

\- Supports a configurable block size and a summary-only output option.

\- Implements ascending size sorting and reverse ordering for current-directory reports, subject to the limitations below.

\- Provides built-in help and version information.



Block counts are calculated by rounding each file's size up to the selected block size. They are estimates, not measurements of actual filesystem allocation.



\## Technologies Used



\- \*\*Programming language:\*\* C++, configured for C++20.

\- \*\*Standard library:\*\* `std::filesystem`, `std::vector`, `std::string`, stream output, and standard algorithms.

\- \*\*Build configuration:\*\* Visual Studio C++ project files, MSVC toolset `v145`, and Windows SDK target `10.0`.

\- \*\*Platform:\*\* Windows; the source includes `Windows.h` and uses Windows-style path handling.



\## Technical Concepts Demonstrated



\- \*\*Filesystem traversal:\*\* Directory iterators inspect immediate entries, while recursive directory iterators visit files within nested subdirectories.

\- \*\*Data aggregation:\*\* File sizes are accumulated into subdirectory totals and an overall directory total.

\- \*\*Structured data:\*\* A `Folder` structure associates a filesystem path with a 64-bit unsigned size value.

\- \*\*Dynamic collections:\*\* A vector stores folder records for subsequent ordering and display.

\- \*\*Command-line parsing:\*\* Arguments select output modes, block sizes, directory targets, and help or version output.

\- \*\*Sorting algorithms:\*\* A separate module implements bubble sort using adjacent comparisons and swaps.

\- \*\*Error handling:\*\* The scanner uses filesystem error codes, permission-skipping options, and exception handling. Some failure paths still need improvement.



\## How the Program Works



1\. Read command-line arguments and update the selected options.

2\. Scan a directory when its argument is encountered, or scan the current directory if no results have been collected.

3\. Read regular-file sizes and recursively aggregate files beneath each immediate subdirectory.

4\. Store folder totals and append an overall total.

5\. Apply supported ordering options where implemented.

6\. Print the results or a summary total.



In byte mode, the program adds logical file sizes. In block mode, it adds each file's rounded-up block count.



Because explicit directories are scanned during argument parsing, place options before the directory argument.



\## Project Structure



\- `DiskUsageProject3.slnx` — Visual Studio solution.

\- `du/du.cpp` — Argument parsing, directory traversal, size calculations, and console output.

\- `du/Folder.h` — Folder record containing a path and size.

\- `du/sort.cpp` — Ascending size-sort implementation.

\- `du/sort.h` — Sorting function declaration.

\- `du/du.vcxproj` — Compiler and build configuration.

\- `du/du.vcxproj.filters` — Visual Studio file organization.



\## How to Run



\### Requirements



\- Windows.

\- Visual Studio with C++ build support and the configured MSVC `v145` toolset.

\- A compatible Windows SDK; the project targets `10.0`.



\### Build



1\. Open `DiskUsageProject3.slnx` in a Visual Studio version supporting this solution format, or open `du/du.vcxproj` directly.

2\. Select \*\*Debug / x64\*\* or \*\*Release / x64\*\*.

3\. Build the project.

4\. Open a terminal in the directory containing the generated `du.exe`.



The instructions below use PowerShell syntax.



\### Examples



Display built-in help:



```powershell

.\\du.exe --help

```



Scan the current directory using the default block size:



```powershell

.\\du.exe

```



Report logical byte totals for a directory:



```powershell

.\\du.exe -b "C:\\ExampleFolder"

```



Display only the overall byte total:



```powershell

.\\du.exe -sb "C:\\ExampleFolder"

```



Estimate block counts using 1,024-byte blocks:



```powershell

.\\du.exe -k "C:\\ExampleFolder"

```



Use a custom block size:



```powershell

.\\du.exe --block-size=2048 "C:\\ExampleFolder"

```



Sort the current-directory report by descending size:



```powershell

.\\du.exe -zr

```



Replace `C:\\ExampleFolder` with an existing directory. Build and execution have not been independently verified for this review.



\## Current Limitations



\- The `-n` option is parsed but does not perform name sorting.

\- Size sorting and reversal are not applied to explicitly supplied directory reports.

\- Human-readable output (`-h`) uses incorrect unit thresholds and truncating integer division; some combinations also reverse the results twice.

\- Multiple directory arguments replace earlier results rather than preserving separate reports.

\- Invalid paths can fall through to scanning the current directory without a clear error.

\- Some scan failures can leave an empty result vector that later code accesses without checking.

\- Some invalid options are accepted, and reported command-line errors return a success exit code.



\## What This Project Demonstrates



\- Applying C++ filesystem tools to a practical command-line utility.

\- Aggregating data across nested directory structures.

\- Modelling report entries with structures and dynamic collections.

\- Connecting argument parsing, processing options, and formatted output.

\- Separating a sorting implementation from the main application.



\## Potential Enhancements



\- Parse and validate all arguments before scanning, and retain results for every requested directory.

\- Complete name sorting and apply ordering consistently across output modes.

\- Correct human-readable units and preserve precision during formatting.

\- Improve path validation, failure reporting, and exit codes.

\- Add automated tests using small directory trees with known file sizes and expected totals.



\## Academic Context



This project was developed as part of coursework in the Computer Programming and Analysis program at Fanshawe College. It is included in this portfolio to demonstrate programming concepts, software-development techniques, and practical problem-solving skills.



\## Author



Joshua Otto  

Computer Programming and Analysis  

Fanshawe College

