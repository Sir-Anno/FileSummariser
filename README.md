# File Summariser
A command-line tool written in C++ that scans directories or file lists, filters by supported file types, and outputs file sizes in a human-readable format. 

## Features
-   Scans a directory for valid media files (`.png`, `.bmp`, `.dds`, `.tga`, `.mp4`, `.avi`, `.mov`, `.mkv`).
-   Or read a `.txt` file containing paths to directories or files.
-   Output file size in human-readable format.
-   Optionally export results to a file.
-   Summary-only mode for quick totals.

## Usage
`\.FileSummariser.exe <input_path> [--output <output_file>] [--summary-only]`

### Notes
-   Enclose paths in quotes if they contain spaces.
-   Use double backslashes (`\\`) or forward slashes (`/`) for paths in `.txt` files.

## Developer Guide
### Requirements
- C++ 20
### Code Structure

-   **Main Function**:
    -   Parses arguments.
    -   Handles input as directory or `.txt` file.
    -   Filters valid media files.
    -   Displays and writes file sizes.
        
-   **Helper Functions**:
    -   `bool isValidFileType(std::unordered_set<std::string> fileTypes, fs::path file)`:
	    Checks if a file matches supported extensions.
    -   `std::pair<std::string, uintmax_t> getFileData(fs::path file)`:  Returns file name and size.
    -   `std::string humanReadableSize(uintmax_t bytes)`: Converts size in bytes to KB, MB, etc.

### Supported File Types
Currently hardcoded in the app:
`{ "png", "bmp", "dds", "tga", "mp4", "avi", "mov", "mkv" }`
To add more, modify the `fileTypes` set in `main()`.

### Output Formatting
-   Uses `std::setw` and alignment to print columns.
-   Automatically adjusts based on longest filename.
-   If `--output` is used, appends to the specified file.
