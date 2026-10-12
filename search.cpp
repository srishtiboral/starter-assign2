/*
 * search.cpp
 * Names: Srishti Boral, Anika Mukherjee
 * Course: CS 106B
 * Description: This file contains a mini search engine. cleanToken and gatherTokens turn the body text of a web page
 * into a set of cleaned, lowercase words.
 * buildIndex reads a database file of pages and builds an
 * inverted index that maps each word to the set of page URLs containing it.
 * findQueryMatches uses the index to answer single or compound queries, and
 * searchEngine ties everything together into a console program.
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h"
using namespace std;


/*
 * Returns a cleaned version of a token: only its letters and digits are kept, and all letters are made lowercase.
 * Parameter: token (a single word from query)
 * Returns: the cleaned token, or the empty string if the token has no letters or digits
 */

string cleanToken(string s) {
    string fixed;
    for (char ch : s) {
        if (isalnum(ch)) {
            fixed += ch;
        }
    }
    return toLowerCase(fixed);
}

/* Splits body text on spaces, cleans each piece with cleanToken, and adds
 * every non-empty cleaned token to a Set, so each word is stored once.
 * Parameter: text (the body text of one web page)
 * Returns: a Set of the unique cleaned tokens in the text
*/

Set<string> gatherTokens(string text) {
    Set<string> tokens;
    string delim = " ";
    Vector<string> noSpaces = stringSplit(text, delim);

    for (int i=0; i<noSpaces.size(); i++) {
        string currToken = cleanToken(noSpaces[i]);
        if (!(tokens.contains(currToken)) && !(currToken.empty())) {
            tokens.add(currToken);
        }
    }

    return tokens;
}

/* Reads a database file and fills in an inverted index. The file alternates lines:
 * a page URL, then that page's body text. Even-numbered lines are saved as URLs and odd-numbered lines as body text.
 * For each page, every unique token in its body text is mapped to that page's URL.
 * Parameters: dbfile (path to the database file), index (the Map to fill, from each token to the set of URLs that contain it)
 * Returns: the number of pages processed
 * Errors: calls error if the file cannot be opened.
 */

int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    ifstream in;
    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines = stringSplit(readEntire(in), '\n');
    Vector<string> urls;
    Vector<string> items;

    for (int i=0; i<lines.size(); i++) {
        if (i % 2 == 0) {
            urls.add(lines[i]);
        } else {
            items.add(lines[i]);
        }
    }

    for (int i=0; i<urls.size(); i++) {
        Set<string> uniqueWords = gatherTokens(items[i]);
        for (string token : uniqueWords) {
            index[token].add(urls[i]);
        }
    }
    return urls.size();
}

/* Returns the set of URLs matching a query. The query is split on spaces
 * and processed left to right. The first term's matches start the result.
 * Each later term is cleaned and combined with the result based on its
 * modifier: no modifier means union, "+" means intersect, and "-" means
 * remove those pages.
 * Parameters: index (the inverted index), query (one or more search terms)
 * Returns: the set of URLs that match the query
 */

Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;
    Vector<string> terms = stringSplit(query, " ");
    string firstTerm = cleanToken(terms[0]);

    if (index.containsKey(firstTerm)) {
        result = index[firstTerm];
    }

    for (int i=1; i<terms.size(); i++) {
        string modifier = "";
        string currTerm = terms[i];
        if ((startsWith(currTerm, "+")) || (startsWith(currTerm, "-"))) {
            modifier = currTerm.substr(0,1);
        }
        currTerm = cleanToken(currTerm);

        if (modifier == "+") {
            result.intersect(index[cleanToken(currTerm)]);
        } else if (modifier == "-") {
            result.difference(index[cleanToken(currTerm)]);
        } else {
            result.unionWith(index[cleanToken(currTerm)]);
        }
    }

    return result;
}

/* Runs the search engine console program. It builds an inverted index from
 * the database file, prints how many pages and unique terms it found, then
 * repeatedly asks the user for a query and prints the matching URLs. Stops
 * when the user presses Enter on an empty line.
 * Parameter: dbfile (path to the database file)
 * Returns: nothing (prints results to the console)
 */

void searchEngine(string dbfile) {
    Map<string, Set<string>> index;

    cout << "Stand by while building index... " << endl;
    int numPages = buildIndex(dbfile, index);
    cout << "Indexed " << numPages << " containing " << index.size() << " unique terms." << endl;
    cout << "" << endl;

    while (true) {
        string query = getLine("Enter query sentence (RETURN/ENTER) to quit): ");
        if (query == "") {
            break;
        }

        Set<string> matches = findQueryMatches(index, query);
        cout << "Found " << matches.size() << " matching pages" << endl;
        cout << "{" << matches << "}" << endl;
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


STUDENT_TEST("cleanToken removes punctuation inside words and keeps digits") {
    EXPECT_EQUAL(cleanToken("5lb_m&ms"), "5lbmms");
    EXPECT_EQUAL(cleanToken("mid-quarter"), "midquarter");
    EXPECT_EQUAL(cleanToken("<<blue>>"), "blue");
}

STUDENT_TEST("cleanToken on empty string and only digits") {
    EXPECT_EQUAL(cleanToken(""), "");
    EXPECT_EQUAL(cleanToken("10"), "10");
}

STUDENT_TEST("gatherTokens skips tokens that clean to empty and extra spaces") {
    Set<string> expected = {"red", "blue"};
    EXPECT_EQUAL(gatherTokens("red  ** blue !!"), expected);
}

STUDENT_TEST("gatherTokens on empty text returns empty set") {
    Set<string> expected = {};
    EXPECT_EQUAL(gatherTokens(""), expected);
}

STUDENT_TEST("findQueryMatches is not case-sensitive") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(findQueryMatches(index, "RED"), findQueryMatches(index, "red"));
    EXPECT_EQUAL(findQueryMatches(index, "fish!"), findQueryMatches(index, "fish"));
}

STUDENT_TEST("findQueryMatches with words that are not in the index") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    EXPECT(findQueryMatches(index, "fish hippo +zebra").isEmpty());
    EXPECT_EQUAL(findQueryMatches(index, "red hippo"), findQueryMatches(index, "red"));
}