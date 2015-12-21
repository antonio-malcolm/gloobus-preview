#include "../document-plugin.h"
#include "office-loader.h"

extern "C" interface * create() {
    return new DocumentPlugin(new OfficeLoader());
}

extern "C" void destroy(interface * p) {
    delete static_cast<DocumentPlugin*>(p);
}

extern "C" int getVersion() {
    return MANAGER_VERSION;
}