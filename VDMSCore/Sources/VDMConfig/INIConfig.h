#ifndef __INICONFIG_H_
#define __INICONFIG_H_

#include <INIParser.h>

class CVDMConfig {
  public:
    enum section_t {
      SEC_LOADER  = 0,
      SEC_DEPENDS = 1,
      SEC_CONFIG  = 2
    };

  public:
    // Section was not found
    struct nosection_error : std::invalid_argument {
      nosection_error(
          const std::string& _section)
        : section(_section), invalid_argument(_section)
        { };
      const std::string section;
    };
    // Key was not found within section(s)
    struct nokey_error : std::invalid_argument {
      nokey_error(
          const std::string& _section,
          const std::string& _location,
          const std::string& _key)
        : section(_section), location(_location), key(_key), invalid_argument(_location + ", [" + _section + "] : " + _key + " = \?\?\?")
        { };
      const std::string section;
      const std::string location;
      const std::string key;
    };

  public:
    virtual void getModuleNames(striset_t& moduleNames) const = 0;
    virtual void getKeys(const std::string& moduleName, section_t sectionID, striset_t& keys) const = 0;
    virtual void getValue(const std::string& moduleName, section_t sectionID, const std::string& key, std::string& value) const = 0;
    virtual void getLocation(const std::string& moduleName, section_t sectionID, std::string& location) const = 0;
    virtual void getLocation(const std::string& moduleName, section_t sectionID, const std::string& key, std::string& location) const = 0;
};

class CINIConfig :
  public CINIParser, public CVDMConfig
{
  public:
    // Section must not be defined in INI file
    struct modulename_error : std::logic_error {
      modulename_error(
          const std::string& _sections)
        : sections(_sections), logic_error(_sections)
        { };
      const std::string sections;
    };
    // Section(s) cannot be associated with a module
    struct orphansection_error : std::logic_error {
      orphansection_error(
          const std::string& _sections)
        : sections(_sections), logic_error(_sections)
        { };
      const std::string sections;
    };
    // Dependency cycle was identified
    struct cyclic_error : std::logic_error {
      cyclic_error(
          const std::string& _cycle,
          const std::string& _modules)
        : cycle(_cycle), modules(_modules), logic_error(_cycle + "\n\n=>\n\n" + _modules)
        { };
      const std::string cycle;
      const std::string modules;
    };

  public:
    CINIConfig(void);

  public:
    virtual void validate(void) const;
    virtual void getModuleOrder(strvector_t& order) const;

  public:
    void getModuleNames(striset_t& moduleNames) const;
    void getKeys(const std::string& moduleName, section_t sectionID, striset_t& keys) const;
    void getValue(const std::string& moduleName, section_t sectionID, const std::string& key, std::string& value) const;
    void getLocation(const std::string& moduleName, section_t sectionID, std::string& location) const;
    void getLocation(const std::string& moduleName, section_t sectionID, const std::string& key, std::string& location) const;

  protected:
    std::string getSectionName(const std::string& moduleName, section_t sectionID) const;

  protected:
    const std::string prefix_default;
    const std::string suffix_depends;
    const std::string suffix_config;
};

#endif //__INICONFIG_H_
