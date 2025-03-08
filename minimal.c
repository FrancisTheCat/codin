#include "codin.h"
#include "fmt.h"
#include "xml.h"

i32 main() {
  XML_Object xml;
  xml_parse_file(LIT("<hello>World</hello>"), &xml, context.allocator);
  fmt_printflnc("%S", xml.type);
  fmt_printflnc("%S", xml.body);
}
