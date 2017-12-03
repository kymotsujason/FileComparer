// FileComparer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#define GetCurrentDir _getcwd

using namespace std;
namespace fs = experimental::filesystem::v1;

vector<string> split(const string& text, const string& delims)
{
	vector<string> tokens;
	size_t start = text.find_first_not_of(delims), end = 0;
	while ((end = text.find_first_of(delims, start)) != string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = text.find_first_not_of(delims, end);
	}
	if (start != string::npos)
		tokens.push_back(text.substr(start));

	return tokens;
}

string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	string current_working_dir(buff);
	return current_working_dir;
}

vector<string> GetFiles(string filePath) {
	string path = filePath;
	vector<string> files;
	for (auto & filesFound : fs::directory_iterator(path)) {
		vector<string> tmp = split(filesFound.path().string(), "\\");
		files.push_back(tmp.at(tmp.size()-1));
	}
	return files;
}

int main()
{
	string filePath = GetCurrentWorkingDir();
	vector<string> files = GetFiles(filePath);
	string userInput;
	cout << "Commands: dir, ls, compare, exit" << endl;
	while (userInput != "exit") {
		cin >> userInput;

		if (userInput == "dir") {
			cout << "Displaying working directory (where we look for the files)" << endl;
			cout << filePath << endl;
		}

		if (userInput == "ls") {
			cout << "Shows detected files" << endl;
			for (int i = 0; i < files.size(); i++) {
				cout << files.at(i) << endl;
			}
		}

		if (userInput == "compare") {
			int fileCount;
			cout << "How many files are you comparing?" << endl;
			cin >> fileCount;
			if (fileCount == files.size()) {
				
			}
			else {
				if (fileCount > files.size()) {
					cout << "There appears to be less files than specified, please check again" << endl;
				}
				else {
					cout << "There appears to be more files than specified, please check again" << endl;
				}
			}
		}
	}
	return 0;
}

