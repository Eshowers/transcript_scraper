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
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

/* GLOBAL STATE VARIABLES */
static string transcripts_directory = "Data/Transcripts/"; //where the transcripts are located
static bool verbose = false; //will be silent unless an error occurs

/* UTILS */

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}


/* Function: Usage(...) {}
 * -----------------------------------------------
 * Prints proper usage and exits
 */
void Usage() {
    cout << "Proper usage: ./scraper <filename>" << endl;
    exit(1);
}

/* Function: isBlacklisted(...) {}
 * -----------------------------------------------
 * Does basic error checking to try to prevent erroneous errors from
 * being written to CSV.
 */
bool isBlacklisted(string &name) {

    const char * c_name = name.c_str();
    //names must begin with letters (takes care of timestamps)
    if (isalpha(c_name[1]) == 0) return true;

    if (name == "Date") return true;
    if (name == "Time") return true;
    return false;
}


bool string_is_valid(const std::string &str)
{
    return find_if(str.begin(), str.end(),
        [](char c) { return !(isalnum(c)); }) == str.end();
}

static int num_got = 0;
static int num_missed = 0;

/* Function: digest(...) {}
 * -----------------------------------------------
 * Finds the first instance of ":" in the line, delineating the participant's name.
 * If the name is not blacklisted, the responses is added to the vector of contributions
 * associated with their name in the members map.
 */
void digest(string & line, map<string, vector<string>> & members) {
    size_t pos = line.find(":", 0);

    //the first occurance of ":" in the string will delineate the name
    string name;
    if (pos != string::npos) {
        name = line.substr(0, pos);
        trim(name);
        num_got++;
    } else {
        if (line.size() > 3) {
            //cout << "[LEN: " << line.size() << "]" << endl;
            //cout << line << endl;
            num_missed++;
        }
    }

    if (isBlacklisted(name)) return;

    vector<string> & contributions = members[name];

    string response = line.substr(pos);
    contributions.push_back(response);
}

/* Function: countWordsInString(...) {}
 * -----------------------------------------------
 * Counts words in a string using a stringstream
 */
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

/* Function: analyzeContributions(...) {}
 * -----------------------------------------------
 * Iterates through vector of contributions to tabulate the number of words in their responses,
 * and the number of characters in their responses
 */
void analyzeContributions(vector<string> & responses, size_t & num_words, size_t & num_chars) {
    for (size_t i = 0; i < responses.size(); i++) {
        num_words += countWordsInString(responses[i]);
        num_chars += responses[i].size();
    }
}

/* Function: writeResults(...) {}
 * -----------------------------------------------
 * Takes data structure, tabulates necessary statistics, and writes it to the .csv
 */
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

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {

        size_t num_words = 0;
        size_t num_chars = 0;
        analyzeContributions(iterator->second, num_words, num_chars);
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

    //also should generate a CSV with all of participant's responses!
    //(i.e. with name <group_name_PARTICIPANT_NAME> (just an interation of the vector))
}

int main(int argc, char **argv) {

    if (argc > 2) cout << "Ignoring excess arguments..." << endl;

    ifstream list;
    list.open(string(argv[1]));
    if (!list) {
        cerr << "Error opening file." << endl;
        Usage();
    }

    //initialize outfile
    fstream outFile;
    outFile.open("transcript_aggregate_data.csv", fstream::out | fstream::trunc);
    if (!outFile) cerr << "Unable to initialize .csv file" << endl;

    //can farther parse group ID!

    //Fields of the .csv. Modify these if you change the way the .csv is written.
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
        if (verbose) cout << t_name << endl;
    }

    cout << "Number analyzed (total) " << num_missed+num_got << endl;
    cout << "Number missed " << num_missed << endl;
    cout << "Number valid " << num_got << endl;
}