#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
using namespace std;
#include "scanner.cpp"
/* INSTRUCTION: Complete all ** parts.
You may use any method to connect this file to scanner.cpp
that you had written.
e.g. You can copy scanner.cpp here by:
cp ../ScannerFiles/scanner.cpp .
and then append the two files into one:
cat scanner.cpp parser.cpp > myparser.cpp
*/
//=================================================
// File parser.cpp written by Group Number:9
//=================================================
// ----- Four Utility Functions and Globals -------------------
// Global saved token and lexeme for one-token lookahead
tokentype saved_token;
string saved_lexeme;
// syntaxerror1: called when an unexpected token is found in a non-terminal
// Type of error: unexpected token found while parsing a non-terminal
// Done by: Andy 
void syntaxerror1(string token, string nonterminal) {
    cout << "SYNTAX ERROR: unexpected " << token
         << " found in " << nonterminal << endl;
    exit(1);
}
// syntaxerror2: called when the scanner returns a word that is not a valid token
// Type of error: lexical error - unrecognized token
// Done by: Daniel H
void syntaxerror2(string token, string nonterminal) {
    cout << "Lexical error: " << token << " is not a valid token" << endl;
}
// next_token: calls scanner to get next token; reports lexical errors via syntaxerror2
// Purpose: advance the lookahead; on lexical error, report it and keep the ERROR token
// Done by: Anthony P
tokentype next_token() {
    scanner(saved_token, saved_lexeme);
    if (saved_token == ERROR) {
        syntaxerror2(saved_lexeme, "");
    }
    return saved_token;
}
// match: verifies the current lookahead is the expected token, then advances
// Purpose: consume an expected token or report a syntax error
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
// -------- RDP functions - one per non-term -------------------
// Forward declarations
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
// Done by: Andy &  Javier
void story() {
    cout << "Processing <story>" << endl;
    s();
    while (saved_token != EOFM) {
        s();
    }
    match(EOFM);
    cout << "Successfully parsed <story>." << endl;
}
// Grammar: <s> -> [CONNECTOR] <noun> SUBJECT <afterSubject> PERIOD
// Done by: Javier
void s() {
    cout << "Processing <s>" << endl;
    if (saved_token == CONNECTOR) {
        match(CONNECTOR);
    }
    noun();
    match(SUBJECT);
    afterSubject();
    match(PERIOD);
}
// Grammar: <noun> -> PRONOUN | WORD1
// Done by: Javier
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
// Grammar: <afterSubject> -> <noun> <afterNoun>
//                          | <verb> <tense>
// Done by: Anthony P
void afterSubject() {
    cout << "Processing <afterSubject>" << endl;
    if (saved_token == PRONOUN || saved_token == WORD1) {
        noun();
        afterNoun();
    } else if (saved_token == WORD2) {
        verb();
        tense();
    } else {
        syntaxerror1(saved_lexeme, "afterSubject");
    }
}
// Grammar: <afterNoun> -> OBJECT <afterObject>
//                       | DESTINATION <verb> <tense>
//                       | <be>
// Done by: Javier
void afterNoun() {
    cout << "Processing <afterNoun>" << endl;
    if (saved_token == OBJECT) {
        match(OBJECT);
        afterObject();
    } else if (saved_token == DESTINATION) {
        match(DESTINATION);
        verb();
        tense();
    } else if (saved_token == IS || saved_token == WAS) {
        be();
    } else {
        syntaxerror1(saved_lexeme, "afterNoun");
    }
}
// Grammar: <afterObject> -> <noun> DESTINATION <verb> <tense>
//                         | <verb> <tense>
// Done by: Andy
void afterObject() {
    cout << "Processing <afterObject>" << endl;
    if (saved_token == PRONOUN || saved_token == WORD1) {
        noun();
        match(DESTINATION);
        verb();
        tense();
    } else if (saved_token == WORD2) {
        verb();
        tense();
    } else {
        syntaxerror1(saved_lexeme, "afterObject");
    }
}
// Grammar: <verb> -> WORD2
// Done by: Andy
void verb() {
    cout << "Processing <verb>" << endl;
    if (saved_token == WORD2) {
        match(WORD2);
    } else {
        syntaxerror1(saved_lexeme, "verb");
    }
}
// Grammar: <tense> -> VERB | VERBNEG | VERBPAST | VERBPASTNEG
// Done by: Anthony P
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
// Grammar: <be> -> IS | WAS
// Done by: Daniel
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
string filename;
// ----------- Driver ----------------------------
// The new test driver to start the parser
// Done by: Andy
int main() {
    cout << "Enter the input file name: ";
    cin >> filename;
    fin.open(filename.c_str());
    // Prime the lookahead with the first token
    next_token();
    // Call <story> to start parsing
    story();
    // Close the input file
    fin.close();
    return 0;
} // end
// ** require no other input files!
// ** syntax error EC requires producing errors.txt of error messages
