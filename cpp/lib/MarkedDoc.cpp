// Object to build and refine marked up version of a document
// Simeon Warner - 2005-07-31...
//
// $Id: MarkedDoc.cpp,v 1.3 2008-10-01 20:14:05 simeon Exp $
//
#include "MarkedDoc.h"
#include "definitions.h"
#include "options.h"
#include "kgrams.h"

MarkedDoc::MarkedDoc(void)
{
  spaces.push_back(-1);     // 'space' before first character
}


MarkedDoc::~MarkedDoc(void)
{
}


// Add another line (sentence) to the MarkedDoc. This is how we build
// the object. The per-word flags entries are set based on the lword input
// (which is a count of the number of kgrams including the word), and on
// whether the sentence is too small to have been hashed (<MINSENL).
//
// Flags (bitwise or-ed): 
//   1 - in a too-small sentence which was ignore
//   2 - in a matching kgram
//   
void MarkedDoc::addLine(intv& lspaces, intv& lwords, kgramkeyv& lkeys, char* sentence)
{
  //cout << "MarkedDoc::addLine: #" << text << "#\n";
  unsigned int numWords=flags.size();
  // First take last space off spaces vector
  if (spaces.size()!=(numWords+1)) {
    cerr << "MarkedDoc::addLine: ERROR - spaces.size(" << spaces.size() << ")!=(flags.size(" << numWords << ")+1)" << endl;
    exit(2);
  }
  unsigned int startChar=spaces[numWords]+1;
  // and check size makes sense
  if (startChar!=text.size()) {
    cerr << "MarkedDoc::addLine: ERROR - text.size(" << text.size() << ")!=startChar(" << startChar << ")" << endl;
    exit(2);
  }
  // Add on extra text from sentence  
  text+=sentence;
  unsigned int j;
  int flagAll=0;
  if (lwords.size()<(unsigned int)MINSENL) {
    flagAll|=1;
  }
  for (j=0; j<lwords.size(); j++) {
    if (j>0) {                  // skip first space since we already have it
      spaces.push_back(lspaces[j]+startChar);
    }
    int flag=flagAll;
    if (lwords[j]>0) flag|=2;
    flags.push_back(flag);
    anchors.push_back(0);
    keys.push_back(lkeys[j]);
  }
  spaces.push_back(lspaces[j]+startChar); // there is one extra space position
  text+='\n';                             // terminate each line
}


// Add alignment information to the document. This marks for first word of
// each sentence (line) with the lower numbered allignment key that appears
// within the sentence (line). This information can then be used for 
// by-sentence aligmnent of multiple output documents.
//
void MarkedDoc::addAlignments(kgramkeyv& akeys)
{
  int numWords=keys.size();
  int numAlignments=akeys.size();
  int alignmentNumber=0;
  int sentenceStartWord=-1;
  for (int word=0; word<numWords; word++) {
    alignments.push_back(-1);
    if (word==0 || text[spaces[word]]=='\n') {
      // new sentence
      sentenceStartWord=word;
    }
    if ((alignmentNumber<numAlignments) && (akeys[alignmentNumber]==keys[word])) {
      // got a matching key, add alignment number if this sentence not already marked
      //cout << "aligment sent=" << sentenceNumber << "  ano=" << alignmentNumber << endl;
      if (alignments[sentenceStartWord]<0) {
        //cout << "MARKED aligment[sentenceStartWord=" << sentenceStartWord << "]=" << alignmentNumber << endl;
        alignments[sentenceStartWord]=alignmentNumber;
      }
      alignmentNumber++;
    }
  }
  // Mark numWords+1 as alignment 9999
  alignments.push_back(9999);
}


// PRIVATE utility function to compare two words. Passed the starting
// chars it compares forward until a mismatch or non-a-z characeter is
// found
//
bool wordsMatch(char* ch1, char* ch2)
{
  // search forward until there is a mismatch or a non-word character
  while ((*ch1==*ch2) && (*ch1>='a' && *ch1<='z')) {
    //cout << "ch1=" << *ch1 << " ch2=" << *ch2 << "\n"; 
    ch1++;
    ch2++;
  }
  // return true only if both pointers are not to non-word character
  return(!(*ch1>='a' && *ch1<='z') && !(*ch2>='a' && *ch2<='z'));
}


// Given two MarkedDoc objects, extend the matching markup to 
// include adjoining words that continue matches. Modifies the flags
// in both doc1 and doc2 by setting the 3rd bit true for matching
// words.
//  
void extendMarkups(MarkedDoc& doc1, MarkedDoc& doc2) 
{
  int numWords1=doc1.keys.size();
  int numWords2=doc2.keys.size();
  for (int word1=0; word1<numWords1; word1++) {
    if (doc1.keys[word1]>0) {
      for (int word2=0; word2<numWords2; word2++) {
	if (doc1.keys[word1]==doc2.keys[word2]) {
          //cout << "matching key " << doc1.keys[word1] << " at word " << word1 << " and " << word2 << endl;
          // Work backwards marking matching words
          for (int back=1; ((((int)word1-back)>=0) && (((int)word2-back)>=0)); back++) {
	    if (wordsMatch(&doc1.text[doc1.spaces[word1-back]+1],&doc2.text[doc2.spaces[word2-back]+1])) {
              doc1.flags[word1-back]|=4;
              doc2.flags[word2-back]|=4;
            } else {
              break;
            }
	    //cout << "back=" << back << "\n";
	  }
          // Work forward (after kgram of WINK words) marking matching words
          for (int fwd=WINK; ((int)word1+fwd)<numWords1 && ((int)word2+fwd)<numWords2; fwd++) {
	    if (wordsMatch(&doc1.text[doc1.spaces[word1+fwd]+1],&doc2.text[doc2.spaces[word2+fwd]+1])) {
              doc1.flags[word1+fwd]|=4;
              doc2.flags[word2+fwd]|=4;
            } else {
              break;
            }
	    //cout << "fwd=" << fwd << "\n";
          }
        }
      }
    }
  }
}


string css_class[] = {
  "ds-plain",   // 0, no flags, just plain old text
  "ds-short",   // 1, sentence too short so ignored (< MINSENL words)
  "ds-matchk",  // 2, in matching kgram
  "ds-error",   // 3, not possible
  "ds-matche",  // 4, matched by extending beyond kgram
  "ds-matches", // 5, matched by extending beyond kgram 
                //    (into too small sentence)
  "ds-matchke", // 6, in matching kgram (also matched 
                //    from extending other kgram)
  "ds-error"    // 7, not possible
};

string indent="&nbsp;&nbsp;";

// Write out the two document in XHTML format, side by side in a table
// If aligment information is present then this is used to align 
// corresponding portions of the two documents.
//
// We assume the 2 column XHTML table has already be started and will be
// completed after this call. Data is written in the form of table
// rows (<tr> elements).
//
void writeMarkedUpDocs(ostream& out, MarkedDoc& doc1, MarkedDoc& doc2, bool shortForm)
{
  unsigned int numWords1=doc1.flags.size();
  unsigned int numWords2=doc2.flags.size();
  unsigned int word1=0; // next word to write in doc1
  unsigned int word2=0; // next word to write in doc2
  unsigned int endword1=0;
  unsigned int endword2=0;
  while ((word1<numWords1) && (word2<numWords2)) {
    // Look ahead to find next matching alignment boundary
    while (endword1<numWords1 && endword2<numWords2
        && ( doc1.alignments[endword1+1]!=doc2.alignments[endword2+1]
	     || doc1.alignments[endword1+1]<0 || doc2.alignments[endword2+1]<0)) {
      while (endword1<numWords1 && (doc1.alignments[endword1+1]<0 || doc1.alignments[endword1+1]<doc2.alignments[endword2+1])) {
        endword1++;
	//cout << "alignment1[endword1=" << endword1 << "+1]=" << doc1.alignments[endword1+1] << endl;
      }
      while (endword2<numWords2 && (doc2.alignments[endword2+1]<0 || doc2.alignments[endword2+1]<doc1.alignments[endword1+1])) {
        endword2++;
	//cout << "alignment2[endword2=" << endword2 << "+1]=" << doc2.alignments[endword2+1] << endl;
      }
    }
    // Write next chunks of doc1 and doc2
    out << "<tr>" << endl;
    if (shortForm) {
      doc1.writeTdShortForm(out,word1,endword1);
      doc2.writeTdShortForm(out,word2,endword2);
    } else {
      doc1.writeTdChunk(out,word1,endword1);
      doc2.writeTdChunk(out,word2,endword2);
    }
    out << "</tr>" << endl;
    //
    word1=(++endword1);
    word2=(++endword2);
  }
}


// Write out a block of one marked document from startword to endword. Put
// inside an HTML <td> block.
//
void MarkedDoc::writeTdChunk(ostream& out, int startword, int endword) {
  int lastFlag=flags[startword];
  int word=startword;
  out << "<td valign=\"top\">\n" << indent << "<span class =\"" << css_class[lastFlag] << "\">";
  for (int j=spaces[startword]+1; j<spaces[endword+1]; j++) {
    if (text[j]==' ' || text[j]=='\n') {
      // new word
      lastFlag=flags[word];
      word++;
      if (word>0 && anchors[word-1]) {
        out << "</a>";
      }
      if (css_class[lastFlag]!=css_class[flags[word]]) {
        out << "</span>";
      }
      if (text[j]=='\n') {
        out << "<br />\n" << indent;
      } else {
        out << " ";
      }
      if (css_class[lastFlag]!=css_class[flags[word]]) {
        out << "<span class=\"" << css_class[flags[word]] << "\">";
      }
      if (anchors[word]) {
        writeAnchor(out,word);
      }
    } else {
      out << (char)text[j];
    }
  }
  out << "</span>\n</td>\n"; 
}

// Write out a short form of a block of one marked document from startword to 
// endword. Put inside an HTML <td> block and include only lines that have 
// matching words.
//
void MarkedDoc::writeTdShortForm(ostream& out, int startword, int endword) {
  out << "<td valign=\"top\">\n";
  //
  bool inSkip=false;
  for (int word=startword; word<=endword; ) {
    // Look ahead and find last word on this line, also set flag to
    // see if there are any matching words on this line
    int endlineword;
    bool match=false;
    for (endlineword=word; endlineword<=endword; endlineword++) {
      if (flags[endlineword]>=2) match=true;
      if (text[spaces[endlineword+1]]=='\n') break;
    }
    //
    if (match) {
      // Write out this line
      int lastFlag=flags[word];
      out << indent << "<span class=\"" << css_class[lastFlag] << "\">";
      for (int j=spaces[word]+1; j<spaces[endlineword+1]; j++) {
        if (text[j]==' ' || text[j]=='\n') {
          // new word
          lastFlag=flags[word];
          word++;
          if (word>0 && anchors[word-1]) {
            out << "</a>";
          }
          if (css_class[lastFlag]!=css_class[flags[word]]) {
            out << "</span>";
          }
          if (text[j]=='\n') {
            out << "<br />\n" << indent;
          } else {
            out << " ";
          }
          if (css_class[lastFlag]!=css_class[flags[word]]) {
            out << "<span class=\"" << css_class[flags[word]] << "\">";
          }
          if (anchors[word]) {
            writeAnchor(out,word);
          }
        } else {
          out << (char)text[j];
        }
      }      
      out << "</span><br />\n"; 
      inSkip=false;
    } else {
      if (!inSkip) {
        // Write elipsis to indicate omitted text
        out << indent << "...<br />\n";
      }
      inSkip=true;
    }
    word=endlineword+1;
  }
  out << "</td>\n";
}


//
void writeSimilaritySummary(ostream& out, MarkedDoc& doc1, MarkedDoc& doc2, bool HTML)
{
  // Write next chunks of doc1 and doc2
  if (HTML) out << "<tr>" << endl;
  doc1.writeSimilarityTd(out,HTML);
  doc2.writeSimilarityTd(out,HTML);
  if (HTML) out << "</tr>" << endl;
  //
}


void MarkedDoc::writeSimilarityTd(ostream& out, bool HTML) {
  unsigned int numWords=flags.size();
  unsigned int numMatches=0;
  unsigned int longestChunkSize=0;
  unsigned int longestChunkStart=0;
  unsigned int thisChunkSize=0;
  unsigned int thisChunkStart=0;
  bool lastWasMatch=false;
  for (unsigned int j=0; j<numWords; j++) {
    if (flags[j]>=2) { 
      // current word is in a matching area
      if (!lastWasMatch) {
        thisChunkStart=j;
      }
      numMatches++;
      lastWasMatch=true;
      thisChunkSize++;  
    } else if (lastWasMatch) {
      // current word isn't a match but last was
      if (longestChunkSize<thisChunkSize) {
        longestChunkSize=thisChunkSize;
        longestChunkStart=thisChunkStart;
      }
      thisChunkSize=0;
      lastWasMatch=false;
    }
  }
  // If we have reached end still in a match then check again
  // whether this is the longest match
  if (lastWasMatch && (longestChunkSize<thisChunkSize)) {
    longestChunkSize=thisChunkSize;
    longestChunkStart=thisChunkStart;
  }

  if (HTML) {
    out << "<td class=\"ds-title\">" << endl;
    out << "Matching blocks include " << numMatches 
        << " of the document's " << numWords << " words";
    if (numWords>0) { 
      out.precision(3);
      out << " (" << (100.0*(float)numMatches/(float)numWords) << "%)";
    }
    out << "<br />" << endl;
    if (longestChunkSize>0) {
      writeLink(out,longestChunkStart);
      out << "Longest matching block is " << longestChunkSize << " words</a>" << endl;
    }
    out << "</td>" << endl; 
  } else {
    out << " " << numWords << " " << numMatches << " " << longestChunkSize;
  }
}


int MarkedDoc::writeLink(ostream& out, int word, int num) {
  out << "<a href=\"#";
  num=writeAnchorLabel(out,word,num);
  out << "\">";
  return(num);
} 


int MarkedDoc::writeAnchor(ostream& out, int word, int num) {
  out << "<a name=\"";
  num=writeAnchorLabel(out,word,num);
  out << "\">";
  return(num);
}


// num: 
//  -1 - create new num and add to anchors[word]
//   0 - use anchor present in anchors[word]
//  >0 - use this explicit num, ignore word
//
int MarkedDoc::writeAnchorLabel(ostream& out, int word, int num) {
  // if num<0 we create a new label and return it
  if (num<0) {
    for (unsigned int j=0; j<anchors.size(); j++) {
      if (anchors[j]>num) { num=anchors[j]; } 
    }
    num++;
    anchors[word]=num;
  } else if (num==0) {
    num=anchors[word];
  }
  // use object ref to get a unique part of the anchor for this document
  char anchor[20];
  sprintf(anchor,"%8x_%d",(unsigned int)((ptr_to_int)(this) & 0xFFFFFFFF),num);
  out << anchor;
  return(num);
}


// Write out the marked up document in HTML with words in spans
// corresponding to the flags (rely on CSS to color or othwerwise
// markup the spans). 
// 
ostream& operator<<(ostream& out, MarkedDoc& doc) {
  string indent="&nbsp;&nbsp;";
  //
  int word=0;
  int lastFlag=-1;
  out << indent << "<span class=\"" << css_class[0] << "\">";
  for (unsigned int j=0; j<doc.text.size(); j++) {
    if (doc.text[j]==' ' || doc.text[j]=='\n') {
      // new word
      lastFlag=doc.flags[word];
      word++;
      if (lastFlag!=doc.flags[word]) {
        out << "</span>";
      }
      if (doc.text[j]=='\n') {
        out << "<br />\n" << indent;
      } else {
        out << " ";
      }
      if (lastFlag!=doc.flags[word]) {
	out << "<span class=\"" << css_class[doc.flags[word]] << "\">";
      }
   } else {
      out << (char)doc.text[j];
    }
  }
  out << "</span>\n";
  return(out);
}


// Find c, the best matching sequence of kgramkeys that is a sub-sequence
// of both a and b. I.e. order of kgramkeys in c is the same as in both
// and b but there may be items from a and/or b missing. Algorithm is the
// same as is used by the Unix diff program (except that uses a linear
// space refinement given in the same paper), taken from:
//
// \cite{MYERS86}
// Myers, E. W. [1986]. 
// "An O(ND) difference algorithm and its variations",
// Algorithmica 1, 251-266. 
// http://citeseer.ist.psu.edu/myers86ond.html 
//
void findBestMatchingSequence(kgramkeyv& a, kgramkeyv& b, kgramkeyv& c) {
  int n=a.size();
  int m=b.size();
  int max=m+n;
  int voff=max;
  int v[2*max+1];

  v[voff+1]=0;
  for (int d=0; d<=max; d++) {
    //cout << "d=" << d << endl;
    for (int k=-d; k<=d; k+=2) {
      int x;
      if (k==-d || (k!=d && v[voff+k-1]<v[voff+k+1])) {
        x=v[voff+k+1];
      } else {
        x=v[voff+k-1]+1;
      }
      int y=x-k;
      while ((x<n) && (y<m) && a[x+1]==b[y+1]) {
        x++; y++;
	//cout << "x=" << x << " y=" << y << endl;
        c.push_back(a[x]);
      }
      v[voff+k]=x;
      if ((x>=n) && (y>=m)) {
        //cout << "Completed, d=" << d << " max=" << max << " x=" << x << " y=" << y << endl;
        return;
      }
    }
  }
  cerr << "MarkedDoc::findBestMatchingSequence: FAILED - this should not be possible!" << endl;
  exit(2);
}


// Finds kgramkey aligments between doc1 and doc2 and add this information
// to the aligments arrays in each.
// doc1 and doc2 must be already marked up with the keys arrays populated.
//
void findAlignments(MarkedDoc& doc1, MarkedDoc& doc2)
{
  // Build vectors a and b which are the sequences of keys with no holes
  kgramkeyv a;
  for (unsigned int j=0; j<doc1.keys.size(); j++) { 
    if (doc1.keys[j]!=0) a.push_back(doc1.keys[j]);
  }
  kgramkeyv b;
  for (unsigned int j=0; j<doc2.keys.size(); j++) {
    if (doc2.keys[j]!=0) b.push_back(doc2.keys[j]);
  }
  // Use the arrays a and b to extract the 'best' (most complete) 
  // common sequence 
  kgramkeyv c;
  findBestMatchingSequence(a,b,c);
  
  // Add alignment information from c to each doc, discard c on return
  doc1.addAlignments(c);
  doc2.addAlignments(c);
}

