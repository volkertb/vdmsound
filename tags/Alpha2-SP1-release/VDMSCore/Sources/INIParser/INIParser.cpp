#include "INIParser.h"

//
// Trims a basic_string on the left and/or on the right.
//   The characters that can be trimmed on either side are specified in two
//   sets, one for the left and one for the right.  Passing a NULL pointer
//   for either will prevent trimming on the corresponding side.
// Returns true if after trimming the string is not empty, false otherwise.
//
template<class _E, class _Tr, class _A>
  bool str_trim(
        std::basic_string<_E,_Tr,_A>& str,  // string to trim
        const _E* left,                     // characters to trim from the left
        const _E* right)                    // characters to trim from the right
{
  // positions in <str> where trimmed data would start and end
  std::basic_string<_E,_Tr,_A>::size_type first, last;

  if (str.length == 0)
    return false;             // empty strings cannot be trimmed

  first = (left != NULL)  ? str.find_first_not_of(left) : 0;                 // where left trimming stops
  last  = (right != NULL) ? str.find_last_not_of(right) : str.length() - 1;  // where right trimming stops

  if ((first == std::basic_string<_E,_Tr,_A>::npos) || (last == std::basic_string<_E,_Tr,_A>::npos)) {
    str = "";
    return false;             // the string consists entirely of separators
  } else {
    str.assign(str, first, last - first + 1);   // trim away
    return true;              // non-empty string resulted after trimming
  }
}

//
// Trims a basic_string both on the left and on the right.
//   The characters that can be trimmed on either side are specified in one
//   set.  Passing a NULL pointer will prevent trimming.
// Returns true if after trimming the string is not empty, false otherwise.
//
template<class _E, class _Tr, class _A>
  bool str_trim(
        std::basic_string<_E,_Tr,_A>& str,  // string to trim
        const _E* separators)               // characters to trim from left and right
{
  // use same left and right trimming sets
  //   in a call to bi-dir trim.
  return str_trim(str, separators, separators);
}

//
// Splits a basic_string in two around the occurence of a given character.
//   If several instances of the given splitter character occur, the first
//   or last occurence will be chosen based on the <pickFirst> flag.
// Returns true if the splitter character was found and the string was split,
//   false otherwise.  The two halves are returned in the <left> and <right>
//   parameters.
//
template<class _E, class _Tr, class _A>
  bool str_split(
        const std::basic_string<_E,_Tr,_A>& str,  // string to split
        std::basic_string<_E,_Tr,_A>& left,       // receives the left half
        std::basic_string<_E,_Tr,_A>& right,      // receives the right half
        _E separator,                             // separator character to split around
        bool pickFirst = true)                    // pick first occurence of separator (or last)
{
  // positions in <str> where the split would occur
  std::basic_string<_E,_Tr,_A>::size_type splitPos;

  // find position where the string can be split
  splitPos = pickFirst ? str.find_first_of(separator) : str.find_last_of(separator);

  if (splitPos == std::string::npos) {
    left  = pickFirst ? str : "";   // could not find the splitting character; assume it
    right = pickFirst ? "" : str;   //   is either at the very beginning or very end.
    return false;
  } else {
    left.assign(str, 0, splitPos);                                  // left half
    right.assign(str, splitPos + 1, str.length() - splitPos - 1);   // right half
    return true;
  }
}

//
// Formats a file name and line number into a string of the form
//   'file_name:line_number'
//
std::string getLocation(
    const std::string fileName,     // file name
    int lineNumber)                 // line number
{
  std::ostringstream locationStream;
  locationStream << fileName << ":" << lineNumber;
  return locationStream.str();
}



/////////////////////////////////////////////////////////////////////////////
//
//  CINIParser
//
/////////////////////////////////////////////////////////////////////////////

CINIParser::CINIParser(void)
  : m_separators("\t ")
{
}

CINIParser::~CINIParser(void) {
}

//
// Parses a file formatted according to the INI-file conventions.
//   INI files can consist of comment lines (start with a ';'), section lines
//   (start with a '[' and end with a ']'), empty lines, and data lines (a
//   pair of one key and one value, separated by an '=' sign).
//
void CINIParser::load(
      const std::string& fName)     // file name
{
  std::ifstream stream;

  stream.open(fName.c_str());

  if (stream.fail()) {
    throw fopen_error(fName);       // could not open file
  }

  try {
    load(stream, fName);
  } catch (...) {
    stream.close();
    throw;
  }
}

//
// Parses a stream of data formatted according to the INI-file conventions.
//   INI files can consist of comment lines (start with a ';'), section lines
//   (start with a '[' and end with a ']'), empty lines, and data lines (a
//   pair of one key and one value, separated by an '=' sign).
//
void CINIParser::load(
      std::istream& stream)         // data stream
{
  load(stream, "<unknown>");
}

void CINIParser::load(
      std::istream& stream,         // data stream
      const std::string& streamID)  // ID of stream, used for labeling positions for debug info
{
  std::string buf;                  // holds one line of the INI stream at a time
  std::string data, comment;        // the data and the comment part of every line
  int lineID = 0;                   // which line in the stream we're at

  // a key/value pair, and its position in the keys map
  std::pair<INIKey,std::string> string;
  std::pair<INI_strings_t::iterator,bool> string_pos;
  // a section/keys-map pair, and its position in the sections map
  std::pair<INIKey,INI_strings_t> section;
  std::pair<INI_sections_t::iterator,bool> section_pos;

  // indicates whether the first section was encountered (yet)
  bool hasStarted = false;

  // Start parsing
  while (stream.good()) {                 // while there still is readable data in the stream
    lineID++;                             // next line
    std::getline(stream, buf);            // read one line from the stream

    ::str_split(buf, data, comment, ';'); // split the line into a useful (data) and disposable (comment) part

    if (!::str_trim(data, m_separators.c_str()))
      continue;                           // empty line (after trimming), skip

    switch (data[0]) {                    // first (non-blank) character on the line
      /** section line **/
      case '[':
        if ((data.length() < 2) || (data[data.length() - 1] != ']')) {
          throw syntax_error(::getLocation(streamID, lineID), data.substr(data.length() - 1), "]");
        }

        section.first.key.assign(data, 1, data.length() - 2); // get rid of the enclosing '[]' pair
        ::str_trim(section.first.key, m_separators.c_str());  // trim spaces immediately inside the '[]'
        section.first.file = streamID;                        // store stream ID and position in stream, used for
        section.first.line = lineID;                          //   debug info to user (in case of parse errors).
        section_pos = m_sections.insert(section);             // insert in sections array

        if (!section_pos.second) {  // duplicate section found?
          // warning: section-name already exists; appending
        }

        hasStarted = true;          // first section heading processed, can start processing key/value pairs

        break;

      /** data line **/
      default:
        if (!hasStarted) {          // corresponding section heading found yet?
          throw format_error(::getLocation(streamID, lineID));
        }

        if (!::str_split(data, string.first.key, string.second, '=')) {
          throw syntax_error(::getLocation(streamID, lineID), data, data + "= ...");
        }

        ::str_trim(string.first.key, m_separators.c_str());   // trim key
        ::str_trim(string.second, m_separators.c_str());      // trim value

        string.first.file = streamID;
        string.first.line = lineID;

        string_pos = section_pos.first->second.insert(string);

        if (!string_pos.second) {       // duplicate key found?
          throw conflict_error(::getLocation(streamID, lineID), string.first.key, ::getLocation(string_pos.first->first.file, string_pos.first->first.line));
        }

        break;
    }
  }

  if (stream.eof()) {
    return;
  } else {
    throw io_error(streamID);           // parser finished abnormally
  }
}

//
// Clears all the sections parsed until now, reverting the object to its
//  initial state, before any data was loaded
//
void CINIParser::clear(void) {
  m_sections.clear();
}

//
// Fills in a list with the names of all loaded sections.
// Returns false if no sections were loaded; true otherwise.
//
void CINIParser::getSections(
    striset_t& sections) const    // array to receive the name of all loaded sections
{
  sections.clear();

  for (INI_sections_t::const_iterator itSection = m_sections.begin(); itSection != m_sections.end(); itSection++) {
    sections.insert(itSection->first.key);
  }
}

//
// Fills in a list with the names of all loaded key/value pairs under a
//   given section.
// Returns true if any keys were found, in which case ir also fills in
//   the return-value parameter.  Returns false otherwise.
//
bool CINIParser::getKeys(
    const std::string& section,   // section under which the keys should be listed
    striset_t& keys) const        // array to receive the name of all keys under the given section
{
  INI_sections_t::const_iterator itSection = m_sections.find(INIKey(section));

  keys.clear();

  if (itSection == m_sections.end())
    return false;

  const INI_strings_t& strings = itSection->second;

  for (INI_strings_t::const_iterator itKey = strings.begin(); itKey != strings.end(); itKey++) {
    keys.insert(itKey->first.key);
  }

  return true;
}

//
// Retrieves the value associated with a given key in a given section (if
//   both given section and key exist).
// Returns true if the key and section were found, and fills in the
//   return-value parameter.  Returns false otherwise.
//
bool CINIParser::getValue(
    const std::string& section,   // section under which the key should be found
    const std::string& key,       // key associated with value to be returned
    std::string& value) const     // value to be returned
{
  INI_sections_t::const_iterator itSection = m_sections.find(INIKey(section));

  if (itSection == m_sections.end())
    return false;

  INI_strings_t::const_iterator itKey = itSection->second.find(INIKey(key));

  if (itKey == itSection->second.end())
    return false;

  value.assign(itKey->second);
  return true;
}

//
// Retrieves the file-location of a given section heading (if
//   the section exist).
// Returns true if the section was found, and fills in the
//   return-value parameter.  Returns false otherwise.
//
bool CINIParser::getLocation(
    const std::string& section,   // section to be found
    std::string& location) const  // location (of the form file_name:line_number)
{
  INI_sections_t::const_iterator itSection = m_sections.find(INIKey(section));

  if (itSection == m_sections.end())
    return false;

  location.assign(::getLocation(itSection->first.file, itSection->first.line));
  return true;
}

//
// Retrieves the file-location of a given key in a given section (if
//   both given section and key exist).
// Returns true if the key and section were found, and fills in the
//   return-value parameter.  Returns false otherwise.
//
bool CINIParser::getLocation(
    const std::string& section,   // section under which the key should be found
    const std::string& key,       // key whose location we desire to know
    std::string& location) const  // location (of the form file_name:line_number)
{
  INI_sections_t::const_iterator itSection = m_sections.find(INIKey(section));

  if (itSection == m_sections.end())
    return false;

  INI_strings_t::const_iterator itKey = itSection->second.find(INIKey(key));

  if (itKey == itSection->second.end())
    return false;

  location.assign(::getLocation(itKey->first.file, itKey->first.line));
  return true;
}
