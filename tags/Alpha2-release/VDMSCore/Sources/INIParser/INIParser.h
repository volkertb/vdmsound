#ifndef __INIPARSER_H_
#define __INIPARSER_H_

#pragma warning ( disable : 4503 )
#pragma warning ( disable : 4786 )

#include <map>
#include <set>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <functional>
#include <stdexcept>

#include <string.h>

//
// Predicate to be used for case-insensitive string ordering
//
struct striLess {
  bool inline operator()(const std::string& lhs, const std::string& rhs) const {
    return (_stricmp(lhs.c_str(), rhs.c_str()) < 0);
  }
};

typedef std::set<std::string,striLess> striset_t;
typedef std::vector<std::string> strvector_t;


//
//
//
class CINIStore {
  public:
    virtual void getSections(striset_t& sections) const = 0;
    virtual bool getKeys(const std::string& section, striset_t& keys) const = 0;
    virtual bool getValue(const std::string& section, const std::string& key, std::string& value) const = 0;
    virtual bool getLocation(const std::string& section, std::string& location) const = 0;
    virtual bool getLocation(const std::string& section, const std::string& key, std::string& location) const = 0;
};

class CINIParser :
  public CINIStore
{
  public:
    // Generic I/O error
    struct io_error : std::runtime_error {
      io_error(
          const std::string& _location)
        : location(_location), runtime_error(_location)
        { }
      const std::string location;
    };

    // File-open error
    struct fopen_error : io_error {
      fopen_error(
          const std::string& _fName)
        : io_error(_fName)
        { }
    };

    // Generic parser error
    struct parse_error : std::logic_error {
      parse_error(
          const std::string& _location)
        : location(_location), logic_error(_location)
        { }
      const std::string location;
    };

    // File-format error
    struct format_error : parse_error {
      format_error(
          const std::string& _location)
        : parse_error(_location)
        { }
    };

    // Syntax error
    struct syntax_error : parse_error {
      syntax_error(
          const std::string& _location,
          const std::string& _offender,
          const std::string& _expected)
        : parse_error(_location), offender(_offender), expected(_expected)
        { }
      const std::string offender;
      const std::string expected;
    };

    // Definition-conflict error
    struct conflict_error : parse_error {
      conflict_error(
          const std::string& _location,
          const std::string& _redefined,
          const std::string& _defLocation)
        : parse_error(_location), redefined(_redefined), defLocation(_defLocation)
        { }
      const std::string redefined;
      const std::string defLocation;
    };

  protected:
    // INI-file key; can be a section name, or a string name, but additionally
    //  keeps track of what file and what line in that file it was defined on.
    struct INIKey {
      INIKey(const std::string& _key = "", const std::string& _file = "", int _line = -1)
        : key(_key), file(_file), line(_line)
        { }
      INIKey(const INIKey& src)
        : key(src.key), file(src.file), line(src.line)
        { }
      INIKey& operator=(const INIKey& src) {
        key  = src.key;
        file = src.file;
        line = src.line;
        return *this;
      }

      std::string key;        // key (by which to look-up)
      std::string file;       // file name
      int line;               // line number
    };

    // Predicate to be used on INIKey keys during std::map building & lookup
    struct INIKeyLess {
      bool inline operator()(const INIKey& lhs, const INIKey& rhs) const {
        return _Pr(lhs.key, rhs.key);
      }
      protected: striLess _Pr;
    };

    typedef std::map<INIKey,std::string,INIKeyLess> INI_strings_t;    // a group of key/value (string) pairs read from a file/stream
    typedef std::map<INIKey,INI_strings_t,INIKeyLess> INI_sections_t; // a group of sections (groups of strings) read from a file/stream

  public:
    CINIParser(void);
    virtual ~CINIParser(void);

  public:
    virtual void load(const std::string& fName);
    virtual void load(std::istream& stream);
    virtual void load(std::istream& stream, const std::string& streamID);
    virtual void clear(void);

  public:
    void getSections(striset_t& sections) const;
    bool getKeys(const std::string& section, striset_t& keys) const;
    bool getValue(const std::string& section, const std::string& key, std::string& value) const;
    bool getLocation(const std::string& section, std::string& location) const;
    bool getLocation(const std::string& section, const std::string& key, std::string& location) const;

  protected:
    INI_sections_t m_sections;    // stores INI sections
    std::string m_separators;     // separators, to be trimmed away from strings
};

#endif //__INIPARSER_H_
