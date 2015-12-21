#include "../document-plugin.h"
#include "xps-loader.h"

extern "C" interface * create() {
    return new DocumentPlugin(new XpsLoader());
}

extern "C" void destroy(interface * p) {
    delete static_cast<DocumentPlugin*>(p);
}

extern "C" int getVersion() {
    return MANAGER_VERSION;
}
