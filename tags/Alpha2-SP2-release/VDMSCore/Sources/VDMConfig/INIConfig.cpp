#include "StdAfx.h"

#include "INIConfig.h"



//
// Formats a section name and the location where it was defined in a single
//   string of the form ' - [section] (location)'
//
std::string formatSectionInfo(
    const std::string& section,
    const std::string& location)
{
  return ("-  [" + section + "] (" + location + ")");
}



/////////////////////////////////////////////////////////////////////////////
//
//  CINIConfig
//
/////////////////////////////////////////////////////////////////////////////

CINIConfig::CINIConfig(void)
  : CINIParser(), prefix_default("*"), suffix_debug("debug"), suffix_depends("depends"), suffix_config("config")
{
}

//
// Performs a quick check of section names; it identifies sections that don't
//   seem to correspond to any module (this is usually due to spelling
//   mistakes), or sections whose names are reserved/invalid.
//
void CINIConfig::validate(void) const {
  striset_t sections, modules;
  std::string errLocation;

  getModuleNames(modules);          // retrieve all module names

  striset_t::iterator itDefault = modules.find(prefix_default);

  if (itDefault != modules.end()) {
    static_cast<const CINIStore*>(this)->getLocation(*itDefault, errLocation);
    throw modulename_error(::formatSectionInfo(*itDefault, errLocation));
  }

  getSections(sections);            // retrieve all sections

  modules.insert(prefix_default);   // assume '*' is a module, so that *.config and *.depends become 'valid'

  for (striset_t::iterator itModule = modules.begin(); itModule != modules.end(); itModule++) {
    sections.erase(getSectionName(*itModule, SEC_LOADER));
    sections.erase(getSectionName(*itModule, SEC_DEBUG));
    sections.erase(getSectionName(*itModule, SEC_DEPENDS));
    sections.erase(getSectionName(*itModule, SEC_CONFIG));
  }

  if (sections.empty())
    return;

  std::string errDescription;

  for (striset_t::iterator itSection = sections.begin(); itSection != sections.end(); itSection++) {
    static_cast<const CINIStore*>(this)->getLocation(*itSection, errLocation);
    errDescription += ::formatSectionInfo(*itSection, errLocation) + "\n";
  }

  throw orphansection_error(errDescription);
}

//
// Performs a topological sort on the modules, so that least dependent
//   modules are first, most dependent are last. Thus, all dependencies
//   of a module are listed before the module itself.
//
void CINIConfig::getModuleOrder(
    strvector_t& order) const       // initialization order
{
  typedef std::map<std::string,std::pair<int,striset_t>,striLess> striintsetmap_t;

  striset_t names;
  striset_t keys;
  striintsetmap_t vertices;

  order.clear();
  getModuleNames(names);

  striset_t::const_iterator itName, itKey;
  striintsetmap_t::iterator itVertex, itDepend;

  // make a map of "vertices <-> edges coming in"
  for (itName = names.begin(); itName != names.end(); itName++) {
    // get all dependencies of a module
    getKeys(*itName, CVDMConfig::SEC_DEPENDS, keys);
    // add to list of vertices
    vertices.insert(std::make_pair(*itName, std::make_pair(0, keys)));
  }

  // fill in the "in"-edge counts
  for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++) {
    // go through dependencies and increment edge counts
    for (itKey = itVertex->second.second.begin(); itKey != itVertex->second.second.end(); itKey++) {
      std::string moduleName;
      getValue(itVertex->first, CVDMConfig::SEC_DEPENDS, *itKey, moduleName);

      itDepend = vertices.find(moduleName);     // try to find the declared dependency

      if (itDepend != vertices.end()) {         // does the dependency really exist?
        itDepend->second.first++;               // increment edge count
      }
    }
  }

  while (!vertices.empty()) {
    bool isCyclic = true;

    // look for the most dependent module
    for (itVertex = vertices.begin(); itVertex != vertices.end(); ) {
      if (itVertex->second.first == 0) {
        isCyclic = false;
        // insert before dependent modules
        order.insert(order.begin(), itVertex->first);
        // go through dependencies and decrement edge counts
        for (itKey = itVertex->second.second.begin(); itKey != itVertex->second.second.end(); itKey++) {
          std::string moduleName;
          getValue(itVertex->first, CVDMConfig::SEC_DEPENDS, *itKey, moduleName);

          itDepend = vertices.find(moduleName); // try to find the declared dependency

          if (itDepend != vertices.end()) {     // does the dependency really exist?
            itDepend->second.first--;           // decrement edge count
          }
        }
        // remove vertex
        itVertex = vertices.erase(itVertex);
      } else {
        itVertex++;
      }
    }

    if (isCyclic) {                     // is there a dependency cycle?
      std::string modules;

      for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
        modules += "-  " + itVertex->first + "\n";

      throw cyclic_error("module1 > module2 > module1", modules);
    }
  }
}



/////////////////////////////////////////////////////////////////////////////



//
// Fills in a list with the names of all module names.
//
void CINIConfig::getModuleNames(
    striset_t& moduleNames) const   // array to receive all the module names
{
  getSections(moduleNames);         // retrieve all section names first

  // remove all section names that are not module names as well
  for (striset_t::iterator itSection = moduleNames.begin(); itSection != moduleNames.end(); ) {
    if (itSection->find('.') != std::string::npos) {
      itSection = moduleNames.erase(itSection); // contains '.', => not a module name but a derived section
    } else {
      itSection++;                  // does not contain '.', => qualifies as a module name
    }
  }
}

//
// Fills in a list with the names of all loaded key/value pairs under a
//   given section for a given module.
//
void CINIConfig::getKeys(
    const std::string& moduleName,  // name of module whose sections to use
    section_t sectionID,            // section under which the keys should be listed
    striset_t& keys) const          // array to receive the name of all keys under the given section
{
  striset_t defaultKeys;  // all the keys that are listed under the sectionID defaults

  static_cast<const CINIStore*>(this)->getKeys(getSectionName(moduleName, sectionID), keys);            // get all keys under a module's section
  static_cast<const CINIStore*>(this)->getKeys(getSectionName(prefix_default, sectionID), defaultKeys); // get all keys under the defaults section

  // merge the two sets of key names
  for (striset_t::iterator itDefaultKey = defaultKeys.begin(); itDefaultKey != defaultKeys.end(); itDefaultKey++) {
    keys.insert(*itDefaultKey);
  }
}

//
// Retrieves the value associated with a given key in a given section (if
//   both given section and key exist).
//
void CINIConfig::getValue(
    const std::string& moduleName,  // name of module whose sections to use
    section_t sectionID,            // section under which the key should be found
    const std::string& key,         // key associated with value to be returned
    std::string& value) const       // value to be returned
{
  std::string section1 = getSectionName(moduleName, sectionID);
  std::string section2 = getSectionName(prefix_default, sectionID);

  // attempt to locate the key within the module's section
  if (static_cast<const CINIStore*>(this)->getValue(section1, key, value))
    return;
  // attempt to locate the key within the defaults section
  if (static_cast<const CINIStore*>(this)->getValue(section2, key, value))
    return;

  std::string errLocation;
  getLocation(moduleName, sectionID, errLocation);

  throw nokey_error(section1, errLocation, key);
}

//
// Retrieves the file-location of a given section heading (if
//   the section exist).
//
void CINIConfig::getLocation(
    const std::string& moduleName,  // name of module whose sections to use
    section_t sectionID,            // section under which the key should be found
    std::string& location) const    // location (of the form file_name:line_number)
{
  std::string section = getSectionName(moduleName, sectionID);

  // attempt to locate the section
  if (static_cast<const CINIStore*>(this)->getLocation(section, location))
    return;

  throw nosection_error(section);
}

//
// Retrieves the file-location of a given key in a given section (if
//   both given section and key exist).
//
void CINIConfig::getLocation(
    const std::string& moduleName,  // name of module whose sections to use
    section_t sectionID,            // section under which the key should be found
    const std::string& key,         // key whose location we desire to know
    std::string& location) const    // location (of the form file_name:line_number)
{
  std::string section1 = getSectionName(moduleName, sectionID);
  std::string section2 = getSectionName(prefix_default, sectionID);

  // attempt to locate the key within the module's section
  if (static_cast<const CINIStore*>(this)->getLocation(section1, key, location))
    return;
  // attempt to locate the key within the defaults section
  if (static_cast<const CINIStore*>(this)->getLocation(section2, key, location))
    return;

  std::string errLocation;
  getLocation(moduleName, sectionID, errLocation);

  throw nokey_error(section1, errLocation, key);
}



/////////////////////////////////////////////////////////////////////////////



//
// Constructs a full section anme from a module's name and a suffix
//   identifier
//
std::string CINIConfig::getSectionName(
    const std::string& moduleName,
    section_t sectionID) const
{
  switch (sectionID) {
    case SEC_LOADER:
      return moduleName;
    case SEC_DEBUG:
      return moduleName + "." + suffix_debug;
    case SEC_DEPENDS:
      return moduleName + "." + suffix_depends;
    case SEC_CONFIG:
      return moduleName + "." + suffix_config;
    default:
      std::ostringstream errStream;
      errStream << "sectionID = " << std::setbase(10) << sectionID << " (0x" << std::setbase(16) << sectionID << ")";
      throw std::invalid_argument(errStream.str());
  }
}
