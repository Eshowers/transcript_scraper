/* Scraper.cpp
 * --------------------
 * Scrapes the transcript data, writing the data to a .csv file.
 *
 * Usage:
 * Takes a .txt file of all file names to be scraped. This can be generated
 * by moving into the directory containing the .txt files and typing: "ls > list.txt"
 * Move the list.txt into the directory of the executable, edit it, and remove
 * the line that contains "list.txt"
 *
 * Usage: ./scraper list.txt
 *
 * Will generate a .csv file called "transcript_aggregate_data.csv" in the
 * directory of the executable, which can be opened in excel.
 *
 * Important: If a file of the name "transcript_aggregate_data.csv" exists
 * in the directory of the executable, it's contents will be overwritten.
 */

#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

using namespace std;

static string transcripts_directory = "Data/Transcripts/"; //where the transcripts are located
static bool verbose = false;

void Usage() {
    cout << "Proper usage: ./scraper <filename>" << endl;
    exit(1);
}

bool isBlackListed(string &name) {
    //names must begin with letters (takes care of timestamps)
    if (isalpha(name.c_str()[1]) == 0) return true;

    return false;
}

void digest(string & line, map<string, vector<string>> & members) {
    size_t pos = line.find(":", 0);

    //the first occurance of ":" in the string will delineate the name
    string name;
    if (pos != string::npos) {
        name = line.substr(0, pos);
        if (verbose) cout << name << endl;
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

void writeResults(string & filename, map<string, vector<string>> & members) {
    if (verbose) {
        cout << endl;
        cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        cout << "Outputting results" << endl;
        cout << "Filename: " << filename << endl;
        cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
        cout << endl;
    }


    size_t pos = filename.find_last_of('.');
    string group_name = filename.substr(0,pos);  //remove extension

    fstream outFile;
    outFile.open("transcript_aggregate_data.csv", fstream::out | fstream::app);
    if (!outFile) {
        cerr << "Unable to open file..." << endl;
        cerr << "Continuing..." << endl;
        return;
    }

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {

        size_t num_words = 0;
        size_t num_chars = 0;
        analyze_contributions(iterator->second, num_words, num_chars);
        if (verbose) {
            cout << "Participant: " << iterator->first << endl;
            cout << "Number of contributions: " << iterator->second.size() << endl;
            cout << "Number of words: " << num_words << endl;
            cout << "Number of characters: " << num_chars << endl;
            cout << "Average contribution length (words): " << num_words / iterator->second.size() << endl;
            cout << "Average contribution length (chars): " << num_chars / iterator->second.size() << endl;
            cout << endl;
        }

        outFile << group_name << "," << iterator->first << "," << iterator->second.size() << "," << num_words << ","
                << num_chars << "," << num_words / iterator->second.size() << "," << num_chars / iterator->second.size() << endl;
    }
}

int main(int argc, char **argv) {

    if (argc > 2) cout << "Ignoring excess arguments..." << endl;

    string filename = string(argv[1]);

    ifstream list;
    list.open(filename);
    if (!list) {
        cerr << "Error opening file." << endl;
        Usage();
    }

    //initialize outfile
    fstream outFile;
    outFile.open("transcript_aggregate_data.csv", fstream::out | fstream::trunc);
    if (!outFile) cerr << "Unable to initialize .csv file" << endl;

    //can farther parse group ID!

    //fields
    outFile << "GroupID,Participant,Number of contributions,Number of words,Number of characters," <<
               "Average contribution length (words),Average contribution length (chars)" << endl;

    string t_name;
    while (list.good()) {
        getline(list, t_name);

        map<string, vector<string>> members;

        ifstream transcript;
        transcript.open(transcripts_directory + t_name);
        if (!transcript) {
            cerr << "*** Error opening transcript file: " << t_name << endl;
            continue;
        }

        string line;
        while (transcript.good()) {
            getline(transcript, line);
            digest(line, members);
        }

        writeResults(t_name, members);
        cout << t_name << endl;
    }
}