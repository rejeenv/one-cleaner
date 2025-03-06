#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

namespace fs = std::filesystem;

void deleteTempFiles(const fs::path& directory) {
    try {
        // Check if the directory exists
        if (!fs::exists(directory)) {
            std::cout << "Directory does not exist: " << directory << std::endl;
            return;
        }

        // iterate
        for (const auto& entry : fs::directory_iterator(directory)) {
            const auto& entryPath = entry.path();

            try {
                if (fs::is_regular_file(entry)) {
                    std::cout << "Deleting file: " << entryPath << std::endl;
                    fs::remove(entryPath); // Delete the file
                }
                else if (fs::is_directory(entry)) {
                    std::cout << "Deleting folder: " << entryPath << std::endl;
                    deleteTempFiles(entryPath); // Recursively delete files in the folder
                    fs::remove(entryPath); // Delete the empty folder
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error while deleting file/folder: " << entryPath << " - " << e.what() << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    // welcome
    std::cout << "=============================================" << std::endl;
    std::cout << "          Welcome to One Cleaner!           " << std::endl;
    std::cout << "  The best tool for cleaning your TEMP  " << std::endl;
    std::cout << "               files and folders.          " << std::endl;
    std::cout << "=============================================" << std::endl;

    char tempDir[MAX_PATH];

    if (GetEnvironmentVariableA("TEMP", tempDir, MAX_PATH) > 0) {
        fs::path targetDir(tempDir); // 

        // Ask 
        std::cout << "Dont worry, it can take some time.";
        std::cout << "Are you sure you want to clean the following directory: " << targetDir << "? (y/n): ";
        char userChoice;
        std::cin >> userChoice;

        // If the user confirms 
        if (userChoice == 'y' || userChoice == 'Y') {
            std::cout << "Cleaning directory: " << targetDir << std::endl;

            // Call the function to clean the directory
            deleteTempFiles(targetDir);

            std::cout << "Cleaning completed!" << std::endl;
            return 1;
        }
        else {
            std::cout << "Cleaning operation canceled." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to retrieve the TEMP directory!" << std::endl;
    }

    return 0;
}

//c++ my beloved