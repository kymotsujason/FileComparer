// FileComparer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <sstream>

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

int MatchCharacters(string const &s1, string const &s2) {
	int s1_char_frequencies[256] = {};
	int s2_char_frequencies[256] = {};
	for_each(begin(s1), end(s1),
		[&](unsigned char c) { ++s1_char_frequencies[c]; });
	for_each(begin(s2), end(s2),
		[&](unsigned char c) { ++s2_char_frequencies[c]; });

	return inner_product(begin(s1_char_frequencies),
		end(s1_char_frequencies),
		begin(s2_char_frequencies), 0, plus<>(),
		[](auto l, auto r) { return min(l, r); });
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
		if (tmp.at(tmp.size() - 1) != "FileComparer.exe") {
			files.push_back(tmp.at(tmp.size() - 1));
		}
	}
	return files;
}

int main()
{
	string filePath = GetCurrentWorkingDir();
	vector<string> files = GetFiles(filePath);
	string userInput;
	vector<double> ratioDiff;
	vector<string> results;
	vector<string> allRatios;
	cout << "Commands: dir, ls, compare, show, showall, exit" << endl;
	while (userInput != "exit") {
		cin >> userInput;

		if (userInput == "dir") {
			cout << "Displaying working directory (where we look for the files)" << endl;
			cout << filePath << endl;
		}

		if (userInput == "ls") {
			cout << "Showing detected files" << endl;
			for (int i = 0; i < files.size(); i++) {
				cout << files.at(i) << endl;
			}
		}

		if (userInput == "showall") {
			for (int i = 0; i < results.size(); i++) {
				cout << results.at(i) << endl;
			}
		}

		if (userInput == "show") {
			cout << "At what % should we alert you of possible plagurism? (80 = 80% similar)" << endl;
			double minSim;
			double maxSim;
			cin >> minSim;
			maxSim = 100 - minSim;
			cout << "The shown % is how much of the file on the immediate left is similar to the other file" << endl;;
			for (int i = 0; i < ratioDiff.size(); i++) {
				vector<string> tmp = split(allRatios.at(i), "|");
				string::size_type sz;
				if ((ratioDiff.at(i) <= maxSim) || (stod(tmp.at(0), &sz) >= minSim) || (stod(tmp.at(1), &sz) >= minSim)) {
					cout << results.at(i) << endl;
				}
			}
		}

		if (userInput == "compare") {
			int fileCount;
			cout << "How many files are you comparing?" << endl;
			cin >> fileCount;
			if (fileCount == files.size()) {
				ifstream inFile;
				vector<string> data;
				for (int i = 0; i < files.size(); i++) {
					
					inFile.open(files.at(i));
					if (!inFile) {
						cout << "Couldn't open file " << files.at(i) << ", make sure it's a text file...";
						exit(1);
					}
					else {
						string tmp;
						string allText;
						while (inFile >> tmp) {
							allText = allText + tmp;
						}
						locale loc;
						for (string::size_type i = 0; i < allText.length(); ++i) {
							toupper(allText[i], loc);
						}
						allText.erase(remove(allText.begin(), allText.end(), ' '), allText.end());
						data.push_back(allText);
					}
					inFile.close();
				}
				for (int i = 0; i < data.size()-1; i++) {
					for (int j = i+1; j < data.size(); j++) {
						string baseText = data.at(i);
						string compareText = data.at(j);
						double baseLength = baseText.length();
						double compareLength = compareText.length();
						double baseChar = MatchCharacters(baseText, compareText);
						double compareChar = MatchCharacters(compareText, baseText);
						double baseRatio = (baseChar / baseLength) * 100;
						double compareRatio = (compareChar / compareLength) * 100;
						ostringstream strs;
						ostringstream strs2;
						strs << files.at(i) << "(" << baseRatio << "%) | " << files.at(j) << "(" << compareRatio << "%)";
						strs2 << baseRatio << "|" << compareRatio;
						allRatios.push_back(strs2.str());
						results.push_back(strs.str());
						ratioDiff.push_back(fabs(baseRatio-compareRatio));
					}
				}
			}
			else {
				if (fileCount > files.size()) {
					cout << "There appears to be less files than specified, please check again" << endl;
				}
				else {
					cout << "There appears to be more files than specified, please check again" << endl;
				}
			}
			cout << "Compare complete, use show or showall to see results" << endl;
		}
	}
	return 0;
}

