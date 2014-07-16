/* Scraper.cpp
 * --------------------
 * Scrapes the transcript data, and prints out relevant data. (Will rewrite to output to data file later)
 *
 * Usage: ./scraper <filename>
 */


#include <climits>
#include <getopt.h>
#include <fstream>
#include <set>
#include <thread>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

static map<string, vector<pair<string, size_t> > > members;

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

    vector<pair<string, size_t>> contributions = members[name];

    string response = line.substr(pos);
    contributions.push_back(make_pair(response, response.size()));
    members[name] = contributions;

}

void displayResults() {
    cout << "Outputting results" << endl;

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {
        cout << "Participant: " << iterator->first << endl;
        cout << "Number of contributions: " << iterator->second.size() << endl;
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