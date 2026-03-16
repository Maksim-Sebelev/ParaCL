module;

#define THELAST_EXPORT_CREATE_SPEZIALIZATIONS
#include "create-basic-node.hpp"

export module global_create_settings;

export import thelast;
export import ast_serializer;

CREATE_SAME(last::node::writable, last::node::serializable, last::node::dumpable)
