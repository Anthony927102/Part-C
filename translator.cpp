#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

#include "scanner.cpp"

//=================================================
// File parser.cpp with semantics - Group 9 (Andy, Daniel, Javier, Anthony)
//=================================================

// ----- Globals for scanner / parser -----
tokentype saved_token;
string saved_lexeme;

// ----- Globals for translation -----
string saved_E_word;                 // English (or Japanese) word from lexicon
ofstream translated;                 // output file translated.txt

// Simple dictionary structure
struct DictEntry {
    string jp;
    string en;
};

vector<DictEntry> lexicon;

//=================================================
// Utility error functions
//=================================================

// syntaxerror1: unexpected token in a nonterminal
void syntaxerror1(string token, string nonterminal) {
    cout << "SYNTAX ERROR: unexpected " << token
         << " found in " << nonterminal << endl;
    exit(1);
}

// syntaxerror2: lexical error
void syntaxerror2(string token, string /*nonterminal*/) {
    cout << "Lexical error: " << token << " is not a valid token" << endl;
}

//=================================================
// Scanner interface helpers
//=================================================

tokentype next_token() {
    scanner(saved_token, saved_lexeme);
    if (saved_token == ERROR) {
        syntaxerror2(saved_lexeme, "");
    }
    return saved_token;
}

bool match(tokentype expected) {
    if (saved_token == expected) {
        cout << "Matched " << tokenName[saved_token] << endl;
        if (saved_token != EOFM)
            next_token();
        return true;
    } else {
        syntaxerror1(saved_lexeme, tokenName[expected]);
        return false;
    }
}

//=================================================
// Semantic routines: dictionary, getEword, gen
//=================================================

// Load lexicon.txt into memory
void loadDictionary() {
    ifstream fin("lexicon.txt");
    if (!fin) {
        cout << "ERROR: Could not open lexicon.txt" << endl;
        exit(1);
    }
    string jp, en;
    while (fin >> jp >> en) {
        DictEntry e;
        e.jp = jp;
        e.en = en;
        lexicon.push_back(e);
    }
    fin.close();
}

// getEword: look up saved_lexeme in lexicon, store result in saved_E_word
void getEword() {
    // default: if not found, use Japanese word
    saved_E_word = saved_lexeme;
    for (auto &e : lexicon) {
        if (e.jp == saved_lexeme) {
            saved_E_word = e.en;
            break;
        }
    }
}

// gen: generate one line of IR into translated.txt
void gen(string kind) {
    if (!translated.is_open()) {
        cout << "ERROR: translated.txt is not open" << endl;
        exit(1);
    }

    if (kind == "TENSE") {
        // Use token type name for tense (VERB, VERBNEG, etc.)
        translated << "TENSE: " << tokenName[saved_token] << endl;
    } else if (kind == "CONNECTOR") {
        translated << "CONNECTOR: " << saved_E_word << endl;
    } else if (kind == "ACTOR") {
        translated << "ACTOR: " << saved_E_word << endl;
    } else if (kind == "DESCRIPTION") {
        translated << "DESCRIPTION: " << saved_E_word << endl;
    } else if (kind == "OBJECT") {
        translated << "OBJECT: " << saved_E_word << endl;
    } else if (kind == "TO") {
        translated << "TO: " << saved_E_word << endl;
    } else if (kind == "ACTION") {
        translated << "ACTION: " << saved_E_word << endl;
    } else {
        // Fallback (should not happen in this project)
        translated << kind << ": " << saved_E_word << endl;
    }
}

//=================================================
// Forward declarations of nonterminals
//=================================================
void story();
void s();
void noun();
void afterSubject();
void afterNoun();
void afterObject();
void verb();
void tense();
void be();

//=================================================
// <story> ::= <s> { <s> } EOFM
//=================================================
void story() {
    cout << "Processing <story>" << endl;
    s();
    while (saved_token != EOFM) {
        s();
    }
    match(EOFM);
    cout << "Successfully parsed <story>." << endl;
}

//=================================================
// <s> ::= [CONNECTOR #getEword# #gen(CONNECTOR)#]
//         <noun> #getEword# SUBJECT #gen(ACTOR)# <afterSubject>
//=================================================
void s() {
    cout << "Processing <s>" << endl;

    if (saved_token == CONNECTOR) {
        // Optional connector
        match(CONNECTOR);
        // semantic: connector word
        getEword();
        gen("CONNECTOR");
    }

    // <noun> #getEword#
    noun();
    getEword();

    // SUBJECT #gen(ACTOR)#
    match(SUBJECT);
    gen("ACTOR");

    // <afterSubject>
    afterSubject();
}

//=================================================
// <afterSubject> ::= <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//                  | <noun> #getEword# <afterNoun>
//=================================================
void afterSubject() {
    cout << "Processing <afterSubject>" << endl;

    if (saved_token == WORD2) {
        // <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == WORD1 || saved_token == PRONOUN) {
        // <noun> #getEword# <afterNoun>
        noun();
        getEword();
        afterNoun();
    } else {
        syntaxerror1(saved_lexeme, "afterSubject");
    }
}

//=================================================
// <afterNoun> ::= <be> #gen(DESCRIPTION)# #gen(TENSE)# PERIOD
//               | DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//               | OBJECT #gen(OBJECT)# <afterObject>
//=================================================
void afterNoun() {
    cout << "Processing <afterNoun>" << endl;

    if (saved_token == IS || saved_token == WAS) {
        // <be> #gen(DESCRIPTION)# #gen(TENSE)# PERIOD
        be();
        // DESCRIPTION uses the noun's saved_E_word (already set before <be>)
        gen("DESCRIPTION");
        // TENSE uses IS/WAS token
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == DESTINATION) {
        // DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
        match(DESTINATION);
        // DESTINATION uses the noun's saved_E_word (already set)
        gen("TO");
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == OBJECT) {
        // OBJECT #gen(OBJECT)# <afterObject>
        match(OBJECT);
        gen("OBJECT");
        afterObject();
    } else {
        syntaxerror1(saved_lexeme, "afterNoun");
    }
}

//=================================================
// <afterObject> ::= <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//                 | <noun> #getEword# DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//=================================================
void afterObject() {
    cout << "Processing <afterObject>" << endl;

    if (saved_token == WORD2) {
        // <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == WORD1 || saved_token == PRONOUN) {
        // <noun> #getEword# DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
        noun();
        getEword();          // this noun will be the DESTINATION
        match(DESTINATION);
        gen("TO");
        verb();
        getEword();
        gen("ACTION");
        tense();
        gen("TENSE");
        match(PERIOD);
    } else {
        syntaxerror1(saved_lexeme, "afterObject");
    }
}

//=================================================
// <noun> ::= WORD1 | PRONOUN
//=================================================
void noun() {
    cout << "Processing <noun>" << endl;
    if (saved_token == PRONOUN) {
        match(PRONOUN);
    } else if (saved_token == WORD1) {
        match(WORD1);
    } else {
        syntaxerror1(saved_lexeme, "noun");
    }
}

//=================================================
// <verb> ::= WORD2
//=================================================
void verb() {
    cout << "Processing <verb>" << endl;
    if (saved_token == WORD2) {
        match(WORD2);
    } else {
        syntaxerror1(saved_lexeme, "verb");
    }
}

//=================================================
// <tense> ::= VERB | VERBNEG | VERBPAST | VERBPASTNEG
//=================================================
void tense() {
    cout << "Processing <tense>" << endl;
    if (saved_token == VERB) {
        match(VERB);
    } else if (saved_token == VERBNEG) {
        match(VERBNEG);
    } else if (saved_token == VERBPAST) {
        match(VERBPAST);
    } else if (saved_token == VERBPASTNEG) {
        match(VERBPASTNEG);
    } else {
        syntaxerror1(saved_lexeme, "tense");
    }
}

//=================================================
// <be> ::= IS | WAS
//=================================================
void be() {
    cout << "Processing <be>" << endl;
    if (saved_token == IS) {
        match(IS);
    } else if (saved_token == WAS) {
        match(WAS);
    } else {
        syntaxerror1(saved_lexeme, "be");
    }
}

//=================================================
// Driver
//=================================================
string filename;

int main() {
    cout << "Enter the input file name: ";
    cin >> filename;

    // Open input for scanner
    fin.open(filename.c_str());
    if (!fin) {
        cout << "ERROR: Could not open input file." << endl;
        return 1;
    }

    // Load dictionary
    loadDictionary();

    // Open translated.txt (truncate each run)
    translated.open("translated.txt");
    if (!translated) {
        cout << "ERROR: Could not open translated.txt for writing." << endl;
        return 1;
    }

    // Prime lookahead
    next_token();

    // Start parsing
    story();

    // Cleanup
    fin.close();
    translated.close();

    return 0;
}