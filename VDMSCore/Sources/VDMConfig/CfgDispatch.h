#ifndef __CFGDISPATCH_H_
#define __CFGDISPATCH_H_

class CINIStore;

// Predicate to be used for case-insensitive string ordering
struct striLess;

typedef std::map<std::string,IUnknownPtr,striLess> modulemap_t;

struct CfgEnvironment {
  CfgEnvironment(
      const CfgEnvironment& src)
    : name(src.name), config(src.config), modules(src.modules), loggingLevel(src.loggingLevel)
    { }
  CfgEnvironment(
      const std::string& _name,
      const CVDMConfig& _config,
      const modulemap_t& _modules,
      int _loggingLevel )
    : name(_name), config(_config), modules(_modules), loggingLevel(_loggingLevel)
    { }

  const std::string& name;
  const CVDMConfig& config;
  const modulemap_t& modules;
  const int loggingLevel;
};

#endif //__CFGDISPATCH_H_
