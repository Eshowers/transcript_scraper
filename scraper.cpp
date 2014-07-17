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

bool isBlackListed(string &name) {
    //names must begin with letters (takes care of timestamps)
    if (isalpha(name.c_str()[1]) == 0) return true;

    return false;
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

    if (isBlackListed(name)) return;

    vector<string> & contributions = members[name];

    string response = line.substr(pos);
    contributions.push_back(response);
}

size_t countWordsInString(string const& str) {
    std::stringstream stream(str);
    std::string oneWord;
    size_t count = 0;

    while(stream >> oneWord) {
        ++count;

        //modify definitions of word here
    }

    return count;
}

void analyze_contributions(vector<string> & responses, size_t & num_words, size_t & num_chars) {
    for (size_t i = 0; i < responses.size(); i++) {
        num_words += countWordsInString(responses[i]);
        num_chars += responses[i].size();
    }
}

void displayResults(string & filename) {
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << "Outputting results" << endl;
    cout << "Filename: " << filename << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << endl;

    size_t pos = filename.find_last_of('.');
    string file_path = filename.substr(0,pos);  //remove extension
    size_t pos2 = file_path.find_last_of('/');
    string group_name = file_path.substr(pos2 + 1);

    fstream outFile;
    outFile.open("transcript_aggregate_data.csv", fstream::out | fstream::ate);
    if (!outFile) {
        cout << "Unable to open file..." << endl;
        cout << "Continuing..." << endl;
        return;
    }

    outFile << "GroupID,Participant,Number of contributions,Number of words,Number of characters," <<
               "Average contribution length (words),Average contribution length (chars)" << endl;

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {

        size_t num_words = 0;
        size_t num_chars = 0;
        analyze_contributions(iterator->second, num_words, num_chars);
        cout << "Participant: " << iterator->first << endl;
        cout << "Number of contributions: " << iterator->second.size() << endl;
        cout << "Number of words: " << num_words << endl;
        cout << "Number of characters: " << num_chars << endl;
        cout << "Average contribution length (words): " << num_words / iterator->second.size() << endl;
        cout << "Average contribution length (chars): " << num_chars / iterator->second.size() << endl;
        cout << endl;

        outFile << group_name << "," << iterator->first << "," << iterator->second.size() << "," << num_words << ","
                << num_chars << "," << num_words / iterator->second.size() << "," << num_chars / iterator->second.size() << endl;
    }

    //*** NOTE: WILL WRITE OUT TO .csv files
}

int main(int argc, char **argv) {

    if (argc > 2) cout << "Ignoring excess arguments..." << endl;

    string filename = string(argv[1]);
    ifstream transcript;
    transcript.open(filename);
    if (!transcript) {
        cerr << "Error opening file." << endl;
        Usage();
    }

    string line;
    while (transcript.good()) {
        getline(transcript, line);
        digest(line);
    }

    displayResults(filename);
}