#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <iomanip>
#include <fstream>


namespace fs = std::filesystem;


bool isValidFileType(std::unordered_set<std::string> fileTypes, fs::path file) {

    std::string ext = file.extension().string();

    // Remove dot before checking extension type
    if (!ext.empty() && ext[0] == '.') {
        ext.erase(0, 1);
    }

    // Convert ext to lower case if it wasn't already
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // Retrun true if the file has a valid extension type
    return fileTypes.contains(ext);
}


std::pair<std::string, uintmax_t> getFileData(fs::path file) {

    std::string filename = file.filename().string();
    uintmax_t filesize = fs::file_size(file); // Use largest int size on system

    return { filename, filesize };
}


// Convert filesize in bytes to human readable format
std::string humanReadableSize(uintmax_t bytes) {
    const char* units[] = { "B", "KB", "MB", "GB" };
    size_t unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < std::size(units) - 1) {
        size /= 1024.0;
        ++unitIndex;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << size << ' ' << units[unitIndex];
    return out.str();
}


int main(int argc, char* argv[]) {

    fs::path inputFile = "";
    fs::path outputFile = "";
    bool summaryOnly = false;
    std::vector<fs::path> filePaths;
    uintmax_t totalFilesSize = 0;

    // Idealy load these from a config file
    std::unordered_set<std::string> fileTypes = { "png", "bmp", "dds", "tga", "mp4", "avi", "mov", "mkv" };

    // Parse arguments
    for (int i = 1; i < argc; i++) {

        std::string arg = argv[i];

        // Was an output file specified?
        if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i]; // this could be empty in which no file will be created later
        }

        // Summarise total file sizes?
        else if (arg == "--summary-only")
        {
            summaryOnly = true;
        }

        // Assume first non flag argument is path to search
        else if (inputFile.empty()) {
            inputFile = arg;
        }
    }

    // Convert input file to path
    fs::path path(inputFile);

    // Validate file path
    if (path.empty())
    {
        std::cerr << "Please provide a directory or file path\n";
        return 1;
    }

    if (!fs::exists(path))
    {
        std::cerr << "File path does not exist.\n";
        return 1;
    }

    // If passed a directory, recursivly iterate over all files inside
    else if (fs::is_directory(path)) {

        // Get all the valid files in the directory
        for (const auto& entry : fs::recursive_directory_iterator(path)) {

            if (entry.is_regular_file() && isValidFileType(fileTypes, entry)) {
                filePaths.push_back(entry);
            }
        }
    }

    // If passed a file, read it and add each entry to filePaths
    else if (fs::is_regular_file(path)) {

        std::ifstream inFile(path);

        // Get extension as string
        std::string ext = path.extension().string();

        // Convert to lowercase
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext != ".txt")
        {
            std::cerr << "Input file must be of type .txt";
            return 1;
        }

        else {

            std::string line;

            // Read each line
            while (std::getline(inFile, line))
            {
                if (line.empty()) continue;

                fs::path filePath(line);

                // If the line isn't a real file or diectory skip
                if (!fs::exists(filePath))
                {
                    continue;
                }

                // Look inside directories and add their files to the list
                if (fs::is_directory(filePath))
                {
                    for (const auto& entry : fs::directory_iterator(filePath))
                    {
                        if (entry.is_regular_file() && isValidFileType(fileTypes, entry))
                        {
                            filePaths.push_back(entry.path());
                        }
                    }
                }

                // Or just add each file path
                else if (fs::is_regular_file(filePath))
                {
                    if (isValidFileType(fileTypes, filePath))
                    {
                        filePaths.push_back(filePath);
                    }
                }
            }
        }
    }

    // If its neither
    else {
        std::cerr << "Filepath could not be determined.\n";
        return 1;
    }

    // Output to console

    // Work out widest filename in the list for spacing
    int maxPathLength = 0;

    for (const auto& path : filePaths) {
        int length = static_cast<int>(path.string().length());
        if (length > maxPathLength) {
            maxPathLength = length;
        }
    }

    int columnWidth = maxPathLength;

    // Set spacing for outputing data
    if (!summaryOnly)
    {
        std::cout << std::left << std::setw(columnWidth) << "\nFilename" << "Size (bytes)" << '\n';
        std::cout << std::string(columnWidth + 12, '-') << '\n'; // Make a seperator line
    }

    // Loop over filepaths
    for (const auto& path : filePaths) {

        auto [filename, filesize] = getFileData(path);
        totalFilesSize += filesize;

        // Output file data
        if (!summaryOnly)
        {
            std::string filesizeReadable = humanReadableSize(filesize);

            std::string outline = (std::ostringstream{} << std::left << std::setw(columnWidth) << filename << filesizeReadable).str();

            // Console
            std::cout << outline << '\n';

            // File
            if (!outputFile.empty()) {

                std::ofstream outFile(outputFile, std::ios::app); // open in append mode

                if (outFile.is_open()) {
                    // Append outline to it
                    outFile << outline << "\n";
                }
                else {
                    std::cerr << "Error: Could not open file " << outputFile << " for writing.\n";
                }
            }
        }
    }

    // Total files size
    std::string totalFileSizeReadable = humanReadableSize(totalFilesSize);
    std::cout << "\nFiles found: " << filePaths.size() << " Total file size: " << totalFileSizeReadable;
}

