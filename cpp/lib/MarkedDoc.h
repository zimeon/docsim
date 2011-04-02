// Marked up version of one document
// Simeon Warner - 2005-07-20
// $Id: MarkedDoc.h,v 1.1 2007/04/23 23:47:08 simeon Exp $

#ifndef __INC_MarkedDoc
#define __INC_MarkedDoc 1

#include "definitions.h"

class MarkedDoc
{
public:
  // DATA
  string text;     // complete document (with ' ' or '\n' word breaks)
  intv spaces;     // positions of word breaks
  intv flags;      // per word flags indicating status of word
  kgramkeyv keys;  // per word kgramkey where value indicates positions of start of kgrams
  intv alignments; // per word alignment information, indicates lower alignment index following sentence
  intv anchors;    // per word anchors to be written out in the HTML
  
  // METHODS
  MarkedDoc(void);
  ~MarkedDoc(void);
  void addLine(intv& lspaces, intv& lwords, kgramkeyv& lkeys, char* sentence);
  void addAlignments(kgramkeyv& akeys);
  void writeTdChunk(ostream& out, int startword, int endword);
  void writeTdShortForm(ostream& out, int startword, int endword);
  void writeSimilarityTd(ostream& out, bool HTML=true);
  int writeLink(ostream& out, int word, int num=-1); 
  int writeAnchor(ostream& out, int word, int num=0); 
  int writeAnchorLabel(ostream& out, int word, int num); 
  
  friend void extendMarkups(MarkedDoc& doc1, MarkedDoc& doc2);
  friend void writeMarkedUpDocs(ostream& out, MarkedDoc& doc1, MarkedDoc& doc2, bool shortForm=false);
  friend void writeSimilaritySummary(ostream& out, MarkedDoc& doc1, MarkedDoc& doc2, bool HTML=true);
  friend ostream& operator<<(ostream& out, MarkedDoc& doc);
  friend bool wordsMatch(char* ch1, char* ch2);
  friend void findBestMatchingSequence(kgramkeyv& a, kgramkeyv& b, kgramkeyv& c);
  friend void findAlignments(MarkedDoc& doc1, MarkedDoc& doc2);

};

#endif /* #ifndef __INC_MarkedDoc */
