#include "JSONAbstractDescription.h"

JSONAbstractDescription::JSONAbstractDescription(shared_ptr<stringstream> json) : propertyMap(new ptree) {
  read_json(*json, *propertyMap);
}

shared_ptr<RootCluster> JSONAbstractDescription::toAbstractGraph() {
  return std::make_shared<RootCluster>();
}
