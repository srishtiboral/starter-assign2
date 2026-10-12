// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
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


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
string cleanToken(string s) {
    string fixed;
    for (char ch : s) {
        if (isalnum(ch)) {
            fixed += ch;
        }
    }
    return toLowerCase(fixed);
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

searchEngine("res/website.txt");

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


// TODO: add your test cases here