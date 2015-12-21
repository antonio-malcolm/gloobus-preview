#include "gloobus-preview-interface-free.h"

iFree::iFree()
{
	g_debug("Creating iFree");
}

iFree::~iFree()
{
	g_debug("Destroying iFree");
}

void iFree::end()
{
	g_debug("Ending iFree");
}

bool iFree::load()
{
	g_debug("Loading iFree");
	return true;
};
