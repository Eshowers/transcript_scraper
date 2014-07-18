transcript_scraper
==================
Developed for Mac OSX / Unix environment

Scrapes the transcript data, writing the data to a .csv file.

The transcript files must be formatted in the following manner:
<name>:<contribution>

Lines that do not contain a colon are ignored. If the contribution spills over onto
the next line, it is ignored. i.e. in this situation:

<name>:<contribution>
<more contribution is here>

The second line will not be considered in the tabulation. This is a result
of transcriber error.

 ------------------

 Usage:
 Takes a .txt file of all file names to be scraped. This can be generated
 by moving into the directory containing the .txt files and typing: "ls > list.txt"
 Move the list.txt into the directory of the executable, edit it, and remove
 the line that contains "list.txt"

 Usage: ./scraper list.txt

 Will generate a .csv file called "transcript_aggregate_data.csv" in the
 directory of the executable, which can be opened in excel.

 Important: If a file of the name "transcript_aggregate_data.csv" exists
 in the directory of the executable, it's contents will be overwritten.