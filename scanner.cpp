#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
using namespace std;

/* Look for all **'s and complete them */

//=====================================================
// File scanner.cpp written by: Group Number: 9 **
//=====================================================

// --------- Two DFAs ---------------------------------

// WORD DFA
// Done by: Anthony Perez and Javier Pacheco
// RE: (vowel | vowel n | consonant vowel | consonant vowel n |consonant-pair vowel | consonant-pair vowel n)^+
bool word(string s)
{
  // state encoding
  // 0=q0, 1=q0q1, 2=qy, 3=qsa, 4=qt, 5=qs, 6=qc, 7=q0qy
  int state = 0;
  int charpos = 0;

  // helper to categorize each character
  auto getClass = [](char c) -> string
  {
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u'
        || c == 'I' || c == 'E')
      return "V";
    if (c == 'b' || c == 'g' || c == 'k' || c == 'm' || c == 'p' || c == 'r')
      return "bgkmpr";
    if (c == 'd' || c == 'w' || c == 'z' || c == 'j')
      return "dwzj";
    if (c == 'h')
      return "h";
    if (c == 'n')
      return "n";
    if (c == 'y')
      return "y";
    if (c == 't')
      return "t";
    if (c == 's')
      return "s";
    if (c == 'c')
      return "c";
    return "?";
  };

  while (s[charpos] != '\0')
  {
    string cl = getClass(s[charpos]);

    switch (state)
    {
     case 0: // q0
  if (cl == "V")
    state = 1;
  else if (cl == "bgkmpr" || cl == "h" || cl == "n")
    state = 2;
  else if (cl == "dwzj" || cl == "y")
    state = 3;
  else if (cl == "t")
    state = 4;
  else if (cl == "s")
    state = 5;
  else if (cl == "c")
    state = 6;
  else
    return false;
  break;

case 1: // q0q1
  if (cl == "V")
    state = 1;
  else if (cl == "n")
    state = 7;
  else if (cl == "bgkmpr" || cl == "h")
    state = 2;
  else if (cl == "dwzj" || cl == "y")
    state = 3;
  else if (cl == "t")
    state = 4;
  else if (cl == "s")
    state = 5;
  else if (cl == "c")
    state = 6;
  else
    return false;
  break;

case 2: // qy
  if (cl == "V")
    state = 1;
  else if (cl == "y")
    state = 3;
  else
    return false;
  break;

case 3: // qsa
  if (cl == "V")
    state = 1;
  else if (cl == "s")
    state = 4;
  else if (cl == "h")
    state = 4;
  else
    return false;
  break;

case 4: // qt
  if (cl == "V")
    state = 1;
  else if (cl == "s")
    state = 3;
  else
    return false;
  break;

case 5: // qs
  if (cl == "V")
    state = 1;
  else if (cl == "h")
    state = 4;
  else
    return false;
  break;

case 6: // qc
  if (cl == "V")
    state = 1;
  else if (cl == "h")
    state = 5;
  else
    return false;
  break;

case 7: // q0qy
  if (cl == "V")
    state = 1;
  else if (cl == "bgkmpr" || cl == "h" || cl == "n")
    state = 2;
  else if (cl == "dwzj" || cl == "y")
    state = 3;
  else if (cl == "t")
    state = 4;
  else if (cl == "s")
    state = 5;
  else if (cl == "c")
    state = 6;
  else
    return false;
  break;

default:
  return false;
    }

    charpos++;
  }

  // final states are q0 (0) q1(q0q1) and q0qy (7)
  if (state == 0 || state == 1 || state == 7)
    return true;
  else
    return false;

/* replace the following todo the word dfa  **
while (s[charpos] != '\0')
  {
    if (state == 0 && s[charpos] == 'a')
    state = 1;
    else
    if (state == 1 && s[charpos] == 'b')
    state = 2;
    else
    if (state == 2 && s[charpos] == 'b')
    state = 2;
    else
  return(false);
    charpos++;
  }//end of while

// where did I end up????
if (state == 2) return(true);  // end in a final state
 else return(false);
*/
}

// PERIOD DFA
// Done by: Javier Pacheco
//RE: .
bool period(string s)
{ // complete this **
  return (s == ".");
}

// ------ Three  Tables -------------------------------------

// TABLES Done by: ** Andy Guadarrama and Daniel Hernandez

// ** Update the tokentype to be WORD1, WORD2, PERIOD, ERROR, EOFM, etc.
enum tokentype
{
  WORD1, WORD2, ERROR, PERIOD, EOFM, VERB, VERBNEG, VERBPAST, VERBPASTNEG, IS, WAS, OBJECT, SUBJECT, DESTINATION, PRONOUN, CONNECTOR
};

// ** For the display names of tokens - must be in the same order as the tokentype.
string tokenName[30] = {
"WORD1", "WORD2", "ERROR", "PERIOD", "EOFM", "VERB", "VERBNEG", "VERBPAST", "VERBPASTNEG", "IS", "WAS", "OBJECT", "SUBJECT", "DESTINATION", "PRONOUN", "CONNECTOR"
};

// ** Need the reservedwords table to be set up here.
struct ReserveWord{
  string word;
  tokentype type;
};

ReserveWord reserveWords[] = {
{"masu", VERB},
{"masen", VERBNEG},
{"mashita", VERBPAST},
{"masendeshita", VERBPASTNEG},
{"desu", IS},
{"deshita", WAS},
{"o", OBJECT},
{"wa", SUBJECT},
{"ni", DESTINATION},
{"watashi", PRONOUN},
{"anata", PRONOUN},
{"kare", PRONOUN},
{"kanojo", PRONOUN},
{"sore", PRONOUN},
{"mata", CONNECTOR},
{"soshite", CONNECTOR},
{"shikashi", CONNECTOR},
{"dakara", CONNECTOR}
};
// ** Do not require any file input for this. Hard code the table.
// ** a.out should work without any additional files.

// ------------ Scanner and Driver -----------------------

ifstream fin; // global stream for reading from the input file

// Scanner processes only one word each time it is called
// Gives back the token type and the word itself
// ** Done by: Andy Guadarrama and Anthony Perez and Daniel Hernandez
int scanner(tokentype &tt, string &w)
{
  // grabbing the next word
  if (!(fin >> w)){
    tt = EOFM;
    w = "eofm";
    return tt;
  }
  if (w == "eofm"){
    tt = EOFM;
    return tt;
  }
  // Word DFA
  if (word(w)){
    // checking reserved words
    for (auto &rw : reserveWords){
      if (rw.word == w){
        tt = rw.type;
        return tt;
      }
    }
    // Word1 or Word2
    char last = w[w.length() - 1];
    if (last == 'I' || last == 'E'){
      tt = WORD2;
      return tt;
    } else {
      tt = WORD1;
      return tt;
    }
  }
  // Period DFA
  if (period(w)){
    tt = PERIOD;
    return tt;
  } else {
  // lexical error
  cout << "Lexical error: " << w << " is not a valid token" << endl;
  tt = ERROR;
  return tt;
  }
} // the end of scanner

// The temporary test driver to just call the scanner repeatedly
// This will go away after this assignment
// DO NOT CHANGE THIS!!!!!!
// Done by:  Louis
int main()
{
  tokentype thetype;
  string theword;
  string filename;

  cout << "Enter the input file name: ";
  cin >> filename;

  fin.open(filename.c_str());

  // the loop continues until eofm is returned.
  while (true)
  {
    scanner(thetype, theword); // call the scanner which sets
                               // the arguments
    if (theword == "eofm")
      break; // stop now

    cout << "Type is:" << tokenName[thetype] << endl;
    cout << "Word is:" << theword << endl;
  }

  cout << "End of file is encountered." << endl;
  fin.close();

} // end
