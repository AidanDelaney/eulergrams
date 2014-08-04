#ifndef JSONABSTRACTDESCRIPTION_H
#define JSONABSTRACTDESCRIPTION_H

#include <sstream>
#include <memory>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <libcola/cola.h>

using std::stringstream;
using std::shared_ptr;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

using cola::RootCluster;

class JSONAbstractDescription {
 public:
  JSONAbstractDescription(shared_ptr<stringstream> json);
  shared_ptr<RootCluster> toAbstractGraph();
 private:
  shared_ptr<ptree> propertyMap;
};

#endif // JSONABSTRACTDESCRIPTION_H
