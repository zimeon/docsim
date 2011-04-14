// docsim-overlap
//
// Find kgrams in a normalized text file and write a marked up version
//
// Simeon Warner - 2005...

#include "definitions.h"
#include "options.h"
#include "Logger.h"
#include "DocSet.h"
#include "DocInfo.h"
#include "kgrams.h"
#include "files.h"
#include <string.h> // for strncpy()
#include <unistd.h> // assume GNU getopt
#include <fstream>

const string myname="docsim-overlap";
const string mydesc="Write HTML snippets from the direct comparison of two normalized text files. Typical use:\n"
"Does not yet have the ability to import a set a common kgrams which could then be marked accordingly.";

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;

  // Read options using standard code for all of DocSim programs
  readOptions(argc, argv, (const char*)"f:F:l:L:cC:s", myname, mydesc);

  if (compare || comparisonStatsOnly) {
    DocSet docs;
    docid id1=docs.addFile(filename1);
    docid id2=docs.addFile(filename2);
    
    KeyMap allkeys;
    cout << myname << ": Not using winnowing" << endl;
    docs.getKeymap(allkeys, MAX_DUPES_TO_COUNT, false);
    cout << myname << ": built KeyMap, " << allkeys.size() << " keys\n";
    
    KeyMap sharedkeys;
    docs.stripCommon(allkeys, sharedkeys, 2);
    cout << myname << ": extracted " << sharedkeys.size() << " shared keys\n";
    
    if (!comparisonStatsOnly) {
      string allkeysFile=prependPath(baseDir,"allkeys.txt");
      ofstream akout;
      akout.open(allkeysFile.c_str(),ios_base::out);
      akout << allkeys;
      akout.close();
 
      string sharedkeysFile=prependPath(baseDir,"sharedkeys.txt");
      ofstream skout;
      skout.open(sharedkeysFile.c_str(),ios_base::out);
      skout << sharedkeys;
      skout.close();
    }
 
    // Extract simple keyhashset of shared keys from the KeyMap
    keyhashset shared;
    for (KeyMap::iterator kit = sharedkeys.begin(); kit != sharedkeys.end(); kit++) {
       shared.insert(kit->first);
    }
    if (!comparisonStatsOnly) {
      string sharedFile=prependPath(baseDir,"shared.txt");
      ofstream sout;
      sout.open(sharedFile.c_str(),ios_base::out);
      sout << shared;
      sout.close();
    }

    MarkedDoc mud1;
    docs[id1].markupCompleteDoc(mud1,shared);
    cout << myname << ": Marked up document " << id1 << ", " <<  filename1 << endl;
    MarkedDoc mud2;
    docs[id2].markupCompleteDoc(mud2,shared);
    cout << myname << ": Marked up document " << id2 << ", " <<  filename2 << endl;
    extendMarkups(mud1,mud2);    
      
    if (comparisonStatsOnly) {
      // Write just summary stats
      cout << "#STATS#:";
      writeSimilaritySummary(cout, mud1, mud2, false);
      cout << endl;
    } else {
      // Write out full comparison as HTML
      findAlignments(mud1, mud2);
      if (link1.size()==0) { link1=filename1; }
      if (link2.size()==0) { link2=filename2; }
      ofstream out;
      string comparisonFileFullLong=prependPath(baseDir,comparisonFile+"_long.html");
      out.open(comparisonFileFullLong.c_str(),ios_base::out);
      if (out.good()) {
        out << "<table>\n<tr>\n"
            << "<td width=\"50%\">" << link1 << "</td>\n"
            << "<td width=\"50%\">" << link2 << "</td>\n"
            << "</tr>\n"
            << "<!--AAA-->\n";
        writeSimilaritySummary(out, mud1, mud2);
        writeMarkedUpDocs(out, mud1, mud2);
        out << "</table>\n";
        out.close(); 
      } else {
        cerr << myname << ": Failed to write " << comparisonFileFullLong << endl;
      }      

      string comparisonFileFullShort=prependPath(baseDir,comparisonFile+"_short.html");
      out.open(comparisonFileFullShort.c_str(),ios_base::out);
      if (out.good()) {
        out << "<table>\n<tr>\n"
            << "<td width=\"50%\">" << link1 << "</td>\n"
            << "<td width=\"50%\">" << link2 << "</td>\n"
            << "</tr>\n"
            << "<!--AAA-->\n";
        writeSimilaritySummary(out, mud1, mud2);
        writeMarkedUpDocs(out, mud1, mud2, 1);
        out << "</table>\n";
        out.close();     
      } else {
        cerr << myname << ": Failed to write " << comparisonFileFullShort << endl;
      }
    }
  } else if (keyMapFile.size()>0) {

    keyhashset keys;
    readKeyhashset(keyMapFile.c_str(),keys);
    cout << myname << ": Read " << keys.size() << " from " << keyMapFile << endl;
    if (filename2.size()==0) {
      DocInfo doc;
      doc.filename=filename1.c_str();
      doc.markupDoc(cout, keys);
    } else { // filename2.size()>0
      DocInfo doc1;
      doc1.filename=filename1.c_str();
      MarkedDoc mud1;
      doc1.markupCompleteDoc(mud1,keys);
      cout << myname << ": Gone through marking up " <<  filename1 << endl;
      DocInfo doc2;
      doc2.filename=filename2.c_str();
      MarkedDoc mud2;
      doc2.markupCompleteDoc(mud2,keys);
      cout << myname << ": Gone through marking up " <<  filename2 << endl;
      extendMarkups(mud1,mud2);
   
      ofstream out;
      findAlignments(mud1, mud2);
      out.open("similar/comparison.html",ios_base::out);
      out << "<table>\n<tr>\n"
          << "<td width=\"50%\">" << filename1 << "</td>\n"
          << "<td width=\"50%\">" << filename2 << "</td>\n"
          << "</tr>\n"
        << "<!--AAA-->\n";
      writeSimilaritySummary(out, mud1, mud2);
      writeMarkedUpDocs(out, mud1, mud2);
      out << "</table>\n";
      out.close();
    }
  } else {
    //
    // Just look for one kgramkey specified on the command line
    //
    char keystr[18];
    (void)strncpy(keystr,key.c_str(),18);
    //  char* key1="01fda04908cdbef3";
    kgramkey kk=stringToKgramkey(keystr);
    cout << myname << ": looking for " << kgramkeyToString(kk) << endl;
        
    DocInfo doc;
    doc.filename=filename1.c_str();
    char* match=doc.findKgramInDoc(kk);
    if (match!=(char*)NULL) {
      cout << myname << ": Found kgram: '" << match << "'" << endl;
    } else {
      cout << myname << ": kgram not found in " << filename1 << endl;
    }
  }
  
  return 0;
}
