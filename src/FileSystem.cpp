#include "FileSystem.h"


namespace FileSystem{
	std::string readUntilString(std::string& text, const char limit) {
		std::string bufferText;

		// Ignore delimiter, once a string is found return it
		int charRead{ 0 };
		for (char& c : text) {
			++charRead;
	
			if (c != limit && charRead < text.length()) { bufferText += c; }	//Make sure c is not the limit, and that it does not go over the end of the string.
			else {
				if (c != limit) { bufferText += c; }
				text.erase(0, charRead);										// Deletes read characters
				return bufferText;
			}
		}

		return bufferText;
	}

	bool createDirectory(const std::string path) {
		bool success{ false };

		// Before creating directory, check if it already exists. If not found, create directory, still return true
		if (doesFileExist(path)) {
			success = true;
		}
		else {
			// If the directory could not be created, log and return FALSE
			if (!std::filesystem::create_directory(path)) {
				success = false;
			}
			else{
				success = true;
			}
		}

		return success;
	}
	
	bool createFile(const std::string path) {
		
		// Error check if the file exists already
		if (!doesFileExist(path)) {
			std::ofstream file(path);	// Creates the file here

			// If the file still does not exist, return FALSE		
			if (!doesFileExist(path)) {
				return false;
			}
			else {
				// If creation successful, log and return TRUE
				return true;
			}
		}
		else {
			// If already found, no need to create. Log and return TRUE
			return true;
		}
	}

	bool doesFileExist(const std::string path) {
		const std::filesystem::path checkFile(path);

		// Check whether the file exists and return appropriately
		if (std::filesystem::exists(checkFile) ){
			return true;
		}
		else
		{
			return false;
		}
	}

	bool deleteFile(const std::string path){
		std::filesystem::path file{path};

		// Try to delete the file and return appropriately
		if(std::filesystem::remove(file)){
			return true;
		}
		else{
			return false;
		}
	}
	
	bool writeToFile(const std::string path, const std::string& text){
		// If the file doesnt exist, try to create and it and then write again. If that fails, returns false
		if (!doesFileExist(path)) {
			createFile(path);
			return writeToFile(path, text);
		}
		else {
			std::ofstream file{ path }; //Open file to write
			file << text;
			return true;
		}
		return false;
	}

	bool readFile(const std::string path, std::ostringstream& output) {
		// If the file does not exist, no need to read. Return FALSE
		if (!doesFileExist(path)) {
			return false;
		}
		else {
			// Open file for reading
			std::ifstream file{ path, std::ios_base::in};
			if(!file) {
				return false;
			}
			else {
				std::string line;
				// Read the file line by line
				while (file.good()) {
					std::getline(file, line);
					output << line;
				}
				return true;
			}
		}
	}

	void filesInDirectory(const std::string directoryToRead, std::vector<std::string>& writeTo) {
		std::ostringstream unbufferedText;
		std::string fileNameBuffer;

		for (std::filesystem::path dirEntry : std::filesystem::directory_iterator{ directoryToRead }) {
			unbufferedText << dirEntry.string() << "\n";
		}
		
		//Iterate through the string stream of files found in the directory
		bool writeChar{ true };
		for (const char& c : unbufferedText.str()) {

			//Iterate through the string and write each file name to the vector
			if (c == '\n') {								//once newline is reached, save the current buffer to string
				writeTo.push_back(fileNameBuffer); //save current string buffer to vector
				fileNameBuffer = "";					//reset buffer
				writeChar = false;						//do not copy the newling character
			}
			else {
				writeChar = true;						//allow writing of char
			}

			if (writeChar) {
				fileNameBuffer += c; //save the current character
			}
		}

		return;
	}

	bool 	renameFile		(const std::string oldPath, const std::string newPath){
		bool success {false};

		if(doesFileExist(oldPath)){
			std::filesystem::rename(oldPath, newPath);
			success = true;
		}
		else{
			success = false;
		}

		return success;
	}

}