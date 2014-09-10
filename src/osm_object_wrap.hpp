#ifndef OSM_OBJECT_WRAP_HPP
#define OSM_OBJECT_WRAP_HPP

// v8
#include <v8.h>

// node
#include <node.h>
#include <node_version.h>
#include <node_object_wrap.h>

// osmium
#include <osmium/osm/object.hpp>

// node-osmium
#include "osm_entity_wrap.hpp"

namespace node_osmium {

    class OSMObjectWrap : public OSMEntityWrap {

        static v8::Handle<v8::Value> tags(const v8::Arguments& args);
        static v8::Handle<v8::Value> get_id(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_version(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_changeset(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_visible(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_timestamp(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_uid(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> get_user(v8::Local<v8::String> property, const v8::AccessorInfo& info);

    public:

        static v8::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> New(const v8::Arguments& args);

        static const osmium::OSMObject& wrapped(v8::Local<v8::Object> object) {
            return static_cast<const osmium::OSMObject&>(OSMEntityWrap::wrapped(object));
        }

        OSMObjectWrap(const osmium::OSMEntity& entity) :
            OSMEntityWrap(entity) {
        }

    }; // class OSMObjectWrap

} // namespace node_osmium

#endif // OSM_OBJECT_WRAP_HPP