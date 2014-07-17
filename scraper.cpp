/* Scraper.cpp
 * --------------------
 * Scrapes the transcript data, and prints out relevant data. (Will rewrite to output to data file later)
 *
 * Usage: ./scraper <filename>
 */


#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

using namespace std;

static map<string, vector<string>> members;

void Usage() {
    cout << "Proper usage: ./scraper <filename>" << endl;
    exit(1);
}

void digest(string & line) {
    size_t pos = line.find(":", 0);

    //the first occurance of ":" in the string will delineate the name
    string name;
    if (pos != string::npos) {
        name = line.substr(0, pos);
        //cout << name << endl;
    } else {
        return;
    }

    vector<string> & contributions = members[name];

    string response = line.substr(pos);
    contributions.push_back(response);
}

size_t countWordsInString(string const& str) {
    std::stringstream stream(str);
    std::string oneWord;
    size_t count = 0;

    while(stream >> oneWord) { ++count;}

    return count;
}

void analyze_contributions(vector<string> & responses, size_t & num_words, size_t & num_chars) {
    for (size_t i = 0; i < responses.size(); i++) {
        num_words += countWordsInString(responses[i]);
        num_chars += responses[i].size();
    }
}

void displayResults() {
    cout << "Outputting results" << endl;

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {

        size_t num_words = 0;
        size_t num_chars = 0;
        analyze_contributions(iterator->second, num_words, num_chars);
        cout << "Participant: " << iterator->first << endl;
        cout << "Number of contributions: " << iterator->second.size() << endl;
        cout << "Number of words: " << num_words << endl;
        cout << endl;
    }
}

int main(int argc, char **argv) {

    if (argc > 2) cout << "Ignoring excess arguments..." << endl;

    ifstream transcript;
    transcript.open(argv[1]);
    if (!transcript) {
        cerr << "Error opening file." << endl;
        Usage();
    }

    string line;
    while (transcript.good()) {
        getline(transcript, line);
        digest(line);
    }

    displayResults();
}