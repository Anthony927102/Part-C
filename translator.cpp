#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;

/* INSTRUCTION:  copy your parser.cpp here
      cp ../ParserFiles/parser.cpp .
   Then, insert or append its contents into this file and edit.
   Complete all ** parts.
*/

//=================================================
// File parser.cpp with semantics - Group 9 (Andy, Daniel, Javier, Anthony)
//=================================================

// --------- Two DFAs ---------------------------------

// WORD DFA
// Done by: Anthony Perez and Javier Pacheco
// RE: (vowel | vowel n | consonant vowel | consonant vowel n |consonant-pair vowel | consonant-pair vowel n)^+
bool word(string s)
{
    int state = 0;
    int charpos = 0;

    auto getClass = [](char c) -> string {
        if (c=='a'||c=='e'||c=='i'||c=='o'||c=='u'||c=='I'||c=='E') return "V";
        if (c=='b'||c=='g'||c=='k'||c=='m'||c=='p'||c=='r') return "bgkmpr";
        if (c=='d'||c=='w'||c=='z'||c=='j') return "dwzj";
        if (c=='h') return "h";
        if (c=='n') return "n";
        if (c=='y') return "y";
        if (c=='t') return "t";
        if (c=='s') return "s";
        if (c=='c') return "c";
        return "?";
    };

    while (s[charpos] != '\0') {
        string cl = getClass(s[charpos]);
        switch(state) {
            case 0: // q0
                if (cl=="V") state=1;
                else if (cl=="bgkmpr"||cl=="h"||cl=="n") state=2;
                else if (cl=="dwzj"||cl=="y") state=3;
                else if (cl=="t") state=4;
                else if (cl=="s") state=5;
                else if (cl=="c") state=6;
                else return false;
                break;
            case 1: // q0q1
                if (cl=="V") state=1;
                else if (cl=="n") state=7;
                else if (cl=="bgkmpr"||cl=="h") state=2;
                else if (cl=="dwzj"||cl=="y") state=3;
                else if (cl=="t") state=4;
                else if (cl=="s") state=5;
                else if (cl=="c") state=6;
                else return false;
                break;
            case 2: // qy
                if (cl=="V") state=1;
                else if (cl=="y") state=3;
                else return false;
                break;
            case 3: // qsa
                if (cl=="V") state=1;
                else if (cl=="s"||cl=="h") state=4;
                else return false;
                break;
            case 4: // qt
                if (cl=="V") state=1;
                else if (cl=="s") state=3;
                else return false;
                break;
            case 5: // qs
                if (cl=="V") state=1;
                else if (cl=="h") state=4;
                else return false;
                break;
            case 6: // qc
                if (cl=="V") state=1;
                else if (cl=="h") state=5;
                else return false;
                break;
            case 7: // q0qy
                if (cl=="V") state=1;
                else if (cl=="bgkmpr"||cl=="h"||cl=="n") state=2;
                else if (cl=="dwzj"||cl=="y") state=3;
                else if (cl=="t") state=4;
                else if (cl=="s") state=5;
                else if (cl=="c") state=6;
                else return false;
                break;
            default: return false;
        }
        charpos++;
    }
    return (state==1 || state==7);
}

// PERIOD DFA
// Done by: Javier Pacheco
// RE: .
bool period(string s)
{
    return (s == ".");
}

// ------ Three Tables -------------------------------------

// Done by: Andy Guadarrama and Daniel Hernandez
enum tokentype {
    WORD1, WORD2, ERROR, PERIOD, EOFM,
    VERB, VERBNEG, VERBPAST, VERBPASTNEG,
    IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR
};

string tokenName[30] = {
    "WORD1","WORD2","ERROR","PERIOD","EOFM",
    "VERB","VERBNEG","VERBPAST","VERBPASTNEG",
    "IS","WAS","OBJECT","SUBJECT","DESTINATION","PRONOUN","CONNECTOR"
};

struct ReserveWord {
    string word;
    tokentype type;
};

ReserveWord reserveWords[] = {
    {"masu",VERB}, {"masen",VERBNEG}, {"mashita",VERBPAST},
    {"masendeshita",VERBPASTNEG}, {"desu",IS}, {"deshita",WAS},
    {"o",OBJECT}, {"wa",SUBJECT}, {"ni",DESTINATION},
    {"watashi",PRONOUN}, {"anata",PRONOUN}, {"kare",PRONOUN},
    {"kanojo",PRONOUN}, {"sore",PRONOUN},
    {"mata",CONNECTOR}, {"soshite",CONNECTOR},
    {"shikashi",CONNECTOR}, {"dakara",CONNECTOR}
};

// ------------ Scanner ----------------------------------

ifstream fin;

// Scanner: reads one token at a time
// Done by: Andy Guadarrama, Anthony Perez, Daniel Hernandez
int scanner(tokentype &tt, string &w)
{
    if (!(fin >> w)) { tt=EOFM; w="eofm"; return tt; }
    if (w=="eofm")   { tt=EOFM; return tt; }
    if (word(w)) {
        for (auto &rw : reserveWords)
            if (rw.word==w) { tt=rw.type; return tt; }
        char last = w[w.length()-1];
        tt = (last=='I'||last=='E') ? WORD2 : WORD1;
        return tt;
    }
    if (period(w)) { tt=PERIOD; return tt; }
    cout << "Lexical error: " << w << " is not a valid token" << endl;
    tt=ERROR;
    return tt;
}

// ----- Additions to the parser.cpp ---------------------

// Declare Lexicon (i.e. dictionary) that will hold the content of lexicon.txt
// Make sure it is easy and fast to look up the translation.
// Do not change the format or content of lexicon.txt
// Done by: Anthony P
struct DictEntry { string jp; string en; };
vector<DictEntry> lexicon;

// getEword() - using the current saved_lexeme, look up the English word
//              in Lexicon if it is there -- save the result in saved_E_word
// Done by: Andy
// gen(line_type) - using the line type,
//                 sends a line of an IR to translated.txt
//                 (saved_E_word or saved_token is used)
// Done by: Daniel H

// ----- Changes to the parser.cpp content ---------------------

// Four Utility Functions and Globals
tokentype saved_token;
string saved_lexeme;
string saved_E_word;
tokentype saved_tense_token;
ofstream translated;

// syntaxerror1: called when an unexpected token is found in a non-terminal
// Done by: Andy
void syntaxerror1(string token, string nonterminal) {
    cout << "SYNTAX ERROR: unexpected " << token
         << " found in " << nonterminal << endl;
    exit(1);
}

// syntaxerror2: called when the scanner returns a word that is not a valid token
// Done by: Daniel H
void syntaxerror2(string token, string nonterminal) {
    cout << "Lexical error: " << token << " is not a valid token" << endl;
}

// next_token: calls scanner to get next token; reports lexical errors
// Done by: Anthony P
tokentype next_token() {
    scanner(saved_token, saved_lexeme);
    if (saved_token == ERROR)
        syntaxerror2(saved_lexeme, "");
    return saved_token;
}

// match: verifies the current lookahead is the expected token, then advances
// Done by: Daniel H
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

// getEword: looks up saved_lexeme in lexicon, stores result in saved_E_word
// if word not found in lexicon, keeps the Japanese word as-is
// Done by: Andy
void getEword() {
    saved_E_word = saved_lexeme;
    for (auto &e : lexicon)
        if (e.jp == saved_lexeme) { saved_E_word = e.en; break; }
}

// gen: generates one line of IR to translated.txt
// for TENSE: uses saved_tense_token; for all others: uses saved_E_word
// Done by: Daniel H
void gen(string kind) {
    if (kind == "TENSE")
        translated << "TENSE: " << tokenName[saved_tense_token] << endl;
    else if (kind == "CONNECTOR")
        translated << "CONNECTOR: " << saved_E_word << endl;
    else if (kind == "ACTOR")
        translated << "ACTOR: " << saved_E_word << endl;
    else if (kind == "DESCRIPTION")
        translated << "DESCRIPTION: " << saved_E_word << endl;
    else if (kind == "OBJECT")
        translated << "OBJECT: " << saved_E_word << endl;
    else if (kind == "TO")
        translated << "TO: " << saved_E_word << endl;
    else if (kind == "ACTION")
        translated << "ACTION: " << saved_E_word << endl;
}

// -------- RDP functions - one per non-term -------------------

void story();
void s();
void noun();
void afterSubject();
void afterNoun();
void afterObject();
void verb();
void tense();
void be();

// Grammar: <story> -> <s> { <s> } EOFM
// Done by: Andy & Javier
void story() {
    cout << "Processing <story>" << endl;
    s();
    while (saved_token != EOFM)
        s();
    match(EOFM);
    cout << "Successfully parsed <story>." << endl;
}

// Grammar: <s> -> [CONNECTOR #getEword# #gen(CONNECTOR)#]
//                 <noun> #getEword# SUBJECT #gen(ACTOR)# <afterSubject>
// Done by: Javier
void s() {
    cout << "Processing <s>" << endl;
    if (saved_token == CONNECTOR) {
        getEword();
        match(CONNECTOR);
        gen("CONNECTOR");
    }
    getEword();
    noun();
    match(SUBJECT);
    gen("ACTOR");
    afterSubject();
}

// Grammar: <afterSubject> -> <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//                          | <noun> #getEword# <afterNoun>
// Done by: Anthony P
void afterSubject() {
    cout << "Processing <afterSubject>" << endl;
    if (saved_token == WORD2) {
        getEword();
        verb();
        gen("ACTION");
        saved_tense_token = saved_token;
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == WORD1 || saved_token == PRONOUN) {
        getEword();
        noun();
        afterNoun();
    } else {
        syntaxerror1(saved_lexeme, "afterSubject");
    }
}

// Grammar: <afterNoun> -> <be> #gen(DESCRIPTION)# #gen(TENSE)# PERIOD
//                       | DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//                       | OBJECT #gen(OBJECT)# <afterObject>
// Done by: Javier
void afterNoun() {
    cout << "Processing <afterNoun>" << endl;
    if (saved_token == IS || saved_token == WAS) {
        saved_tense_token = saved_token;
        be();
        gen("DESCRIPTION");
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == DESTINATION) {
        match(DESTINATION);
        gen("TO");
        getEword();
        verb();
        gen("ACTION");
        saved_tense_token = saved_token;
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == OBJECT) {
        match(OBJECT);
        gen("OBJECT");
        afterObject();
    } else {
        syntaxerror1(saved_lexeme, "afterNoun");
    }
}

// Grammar: <afterObject> -> <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
//                         | <noun> #getEword# DESTINATION #gen(TO)# <verb> #getEword# #gen(ACTION)# <tense> #gen(TENSE)# PERIOD
// Done by: Andy
void afterObject() {
    cout << "Processing <afterObject>" << endl;
    if (saved_token == WORD2) {
        getEword();
        verb();
        gen("ACTION");
        saved_tense_token = saved_token;
        tense();
        gen("TENSE");
        match(PERIOD);
    } else if (saved_token == WORD1 || saved_token == PRONOUN) {
        getEword();
        noun();
        match(DESTINATION);
        gen("TO");
        getEword();
        verb();
        gen("ACTION");
        saved_tense_token = saved_token;
        tense();
        gen("TENSE");
        match(PERIOD);
    } else {
        syntaxerror1(saved_lexeme, "afterObject");
    }
}

// Grammar: <noun> -> WORD1 #getEword# | PRONOUN #getEword#
// Done by: Javier
void noun() {
    cout << "Processing <noun>" << endl;
    if (saved_token == PRONOUN)     match(PRONOUN);
    else if (saved_token == WORD1)  match(WORD1);
    else syntaxerror1(saved_lexeme, "noun");
}

// Grammar: <verb> -> WORD2 #getEword#
// Done by: Anthony P
void verb() {
    cout << "Processing <verb>" << endl;
    if (saved_token == WORD2) match(WORD2);
    else syntaxerror1(saved_lexeme, "verb");
}

// Grammar: <tense> -> VERB #gen(TENSE)# | VERBNEG #gen(TENSE)# | VERBPAST #gen(TENSE)# | VERBPASTNEG #gen(TENSE)#
// Done by: Anthony P
void tense() {
    cout << "Processing <tense>" << endl;
    if (saved_token == VERB)             match(VERB);
    else if (saved_token == VERBNEG)     match(VERBNEG);
    else if (saved_token == VERBPAST)    match(VERBPAST);
    else if (saved_token == VERBPASTNEG) match(VERBPASTNEG);
    else syntaxerror1(saved_lexeme, "tense");
}

// Grammar: <be> -> IS #gen(DESCRIPTION)# #gen(TENSE)# | WAS #gen(DESCRIPTION)# #gen(TENSE)#
// Done by: Daniel
void be() {
    cout << "Processing <be>" << endl;
    if (saved_token == IS)       match(IS);
    else if (saved_token == WAS) match(WAS);
    else syntaxerror1(saved_lexeme, "be");
}

// ---------------- Driver ---------------------------

// The final test driver to start the translator
// Done by: Andy
string filename;

int main()
{
    // opens the lexicon.txt file and reads it into Lexicon
    ifstream lex("lexicon.txt");
    if (!lex) { cout << "ERROR: Could not open lexicon.txt" << endl; return 1; }
    string jp, en;
    while (lex >> jp >> en) {
        DictEntry e; e.jp=jp; e.en=en;
        lexicon.push_back(e);
    }
    // closes lexicon.txt
    lex.close();

    // opens the output file translated.txt
    translated.open("translated.txt");
    if (!translated) { cout << "ERROR: Could not open translated.txt" << endl; return 1; }

    cout << "Enter the input file name: ";
    cin >> filename;
    fin.open(filename.c_str());
    if (!fin) { cout << "ERROR: Could not open input file." << endl; return 1; }

    next_token();

    // calls the <story> to start parsing
    story();

    // closes the input file
    fin.close();
    // closes translated.txt
    translated.close();

    // display translated.txt contents
    cout << "\n--- Translation (translated.txt) ---" << endl;
    ifstream result("translated.txt");
    string line;
    while (getline(result, line)) cout << line << endl;
    result.close();

}// end
// require no other input files!
// syntax error EC requires producing errors.txt of error messages
// tracing On/Off EC requires sending a flag to trace message output functions