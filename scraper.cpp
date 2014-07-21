/* Scraper.cpp
 * --------------------
 * Scrapes the transcript data, writing the data to a .csv file.
 *
 * The transcript files must be formatted in the following manner:
 * <name>:<contribution>
 *
 * Lines that do not contain a colon are ignored. If the contribution spills over onto
 * the next line, it is ignored. i.e. in this situation:
 *
 * <name>:<contribution>
 * <more contribution is here>
 *
 * The second line will not be considered in the tabulation. This is a result
 * of transcriber error.
 *
 * ------------------
 *
 * Usage:
 * Takes a .txt file of all file names to be scraped. This can be generated
 * by moving into the directory containing the .txt files and typing: "ls > list.txt"
 * Move the list.txt into the directory of the executable, edit it, and remove
 * the line that contains "list.txt"
 *
 * Usage: ./scraper list.txt
 *
 * Will generate a .csv file called "transcript_aggregate_stream.csv" in the
 * directory of the executable, which can be opened in excel.
 *
 * Important: If a file of the name "transcript_aggregate_stream.csv" exists
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
#include <string>

using namespace std;

/* GLOBAL STATE VARIABLES */
static string transcripts_directory = "Data/Transcripts/"; //where the transcripts are located
static bool verbose = false; //will be silent unless an error occurs

/* UTILS */

//strips string of all alphanumeric chars, returns new string
string strip(string in) {
    string final;
    for(size_t i = 0; i < in.length(); i++) {
        if(isalnum(in[i])) final += in[i];
    }
    return final;
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
    if (isalpha(c_name[0]) == 0) return true;

    //try and filter out the majority of things we don't want
    //anything with this in the name is not a name
    if (name.find("Date") != string::npos) return true;
    if (name.find("Time") != string::npos) return true;
    if (name.find("Segment") != string::npos) return true;
    if (name.find("Location") != string::npos) return true;
    if (name.find("Day") != string::npos) return true;
    if (name.find("Discussion") != string::npos) return true;
    if (name.find("Presentation") != string::npos) return true;
    if (name.find("Project") != string::npos) return true;
    if (name.find("Message") != string::npos) return true;
    if (name.find("Record") != string::npos) return true;

    return false;
}

/* Function: digest(...) {}
 * -----------------------------------------------
 * Finds the first instance of ":" in the line, delineating the participant's name.
 * If the name is not blacklisted, the responses is added to the vector of contributions
 * associated with their name in the members map.
 */
void digest(string & line, map<string, vector<string>> & members) {
    size_t pos = line.find(":");

    string name;
    if (pos != string::npos) {
        name = strip(line.substr(0, pos)); //remove spaces from the name
    } else {
        return;
    }

    if (isBlacklisted(name)) return;

    //get vector from map, if it exists.
    vector<string> & contributions = members[name];

    //the "response" will be whatever follows the first location of ":"
    string response = line.substr(pos);
    contributions.push_back(response); //inherently chronological
}

/* Function: countWordsInString(...) {}
 * -----------------------------------------------
 * Counts words in a string using a stringstream
 */
size_t countWordsInString(string const& str) {
    std::stringstream stream(str);
    std::string oneWord;
    size_t count = 0;

    while(stream >> oneWord) { //separated by whitespace

        //convert to lowercase
        //std::transform(oneWord.begin(), oneWord.end(), oneWord.begin(), ::tolower);

        //modify definitions of word here
        // if (oneWord.find("[laughter]") != string::npos) continue;
        // if (oneWord.find("[crosstalk]") != string::npos) continue;
        // if (oneWord.find("[inaudible]") != string::npos) continue;
        // if (oneWord.find("[pause]") != string::npos) continue;

        // if (oneWord.find("(laughter)") != string::npos) continue;
        // if (oneWord.find("(crosstalk)") != string::npos) continue;
        // if (oneWord.find("(inaudible)") != string::npos) continue;
        // if (oneWord.find("(pause)") != string::npos) continue;

        ++count;
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
        num_chars += responses[i].size(); //will include whitespace
    }
    return;
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
    outFile.open("transcript_aggregate_stream.csv", fstream::out | fstream::app); //open the stream for writing, appending

    for(auto iterator = members.begin(); iterator != members.end(); iterator++) {

        size_t num_words = 0; //populated by analyzeContrbutions
        size_t num_chars = 0; //populated by analyzeContributions
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

        //Fields of the .csv. Modify these if you change the way the .csv is written.
        outFile << "GroupID,Participant,Response,Number of words,Number of characters" << endl;

        for (size_t i = 0; i < iterator->second.size(); i++) {
            outFile << group_name << "," << iterator->first << "," << iterator->second[i] << ","
                    << countWordsInString(iterator->second[i]) << "," << iterator->second[i].size() << endl;

        }
    }

    //also should generate a CSV with all of participant's responses!
    //(i.e. with name <group_name_PARTICIPANT_NAME> (just an interation of the vector))... can do this for the linear extraction of transcripts
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
    outFile.open("transcript_aggregate_stream.csv", fstream::out | fstream::trunc);
    if (!outFile) cerr << "Unable to initialize .csv file" << endl;

    //Fields of the .csv. Modify these if you change the way the .csv is written.
    outFile << "GroupID,Participant,Response,Number of words,Number of characters" << endl;

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
}