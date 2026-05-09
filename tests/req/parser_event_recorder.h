#pragma once

#include <xmlparser/xmlparser.h>

#include <string>
#include <string_view>
#include <vector>

struct RecordedAttribute {
  std::string qname;
  std::string value;
};

struct RecordedEvent {
  std::string type;
  std::string name;
  std::string value;
  std::vector<RecordedAttribute> attributes;
};

class RecordingHandler : public xmlparser::v1::SaxHandler {
 public:
  void start_document() override {
    events.push_back({"start_document", "", "", {}});
  }

  void end_document() override {
    events.push_back({"end_document", "", "", {}});
  }

  void start_element(
      const xmlparser::v1::QualifiedName& name,
      const std::vector<xmlparser::v1::AttributeView>& attributes) override {
    RecordedEvent event{"start_element", name.qualified_name, "", {}};
    for (const auto& attribute : attributes) {
      event.attributes.push_back({attribute.name.qualified_name,
                                  std::string(attribute.value)});
    }
    events.push_back(std::move(event));
  }

  void end_element(const xmlparser::v1::QualifiedName& name) override {
    events.push_back({"end_element", name.qualified_name, "", {}});
  }

  void characters(std::string_view text) override {
    events.push_back({"characters", "", std::string(text), {}});
  }

  void processing_instruction(std::string_view target,
                              std::string_view data) override {
    events.push_back({"processing_instruction", std::string(target),
                      std::string(data), {}});
  }

  void comment(std::string_view text) override {
    events.push_back({"comment", "", std::string(text), {}});
  }

  void cdata(std::string_view text) override {
    events.push_back({"cdata", "", std::string(text), {}});
  }

  std::vector<RecordedEvent> events;
};

inline bool has_event(const RecordingHandler& handler,
                      const std::string& type,
                      const std::string& value) {
  for (const auto& event : handler.events) {
    if (event.type == type && event.value == value) {
      return true;
    }
  }
  return false;
}
