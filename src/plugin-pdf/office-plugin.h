#ifndef OFFICE_PLUGIN_H
#define OFFICE_PLUGIN_H

#ifdef PLUGIN_LOADER

PluginManager::register_plugin("office");

if (interface::is_program_available("unoconv")) {

	PluginManager::register_filetype("application/msword", 1);
	PluginManager::register_filetype("application/rtf", 1);
	PluginManager::register_filetype("application/vnd.ms-excel", 1);
	PluginManager::register_filetype("application/vnd.ms-powerpoint", 1);
	PluginManager::register_filetype("application/vnd.ms-powerpoint.template.macroEnabled.12", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.graphics", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.graphics-flat-xml", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.graphics-template", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.presentation", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.presentation-flat-xml", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.presentation-template", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.spreadsheet", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.spreadsheet-flat-xml", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.spreadsheet-template", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.text", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.text-flat-xml", 1);
	PluginManager::register_filetype("application/vnd.oasis.opendocument.text-template", 1);
	PluginManager::register_filetype("application/vnd.openxmlformats-officedocument.presentationml.presentation", 1);
	PluginManager::register_filetype("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", 1);
	PluginManager::register_filetype("application/vnd.openxmlformats-officedocument.wordprocessingml.document", 1);
	PluginManager::register_filetype("application/x-abiword", 1);
	PluginManager::register_filetype("application/x-dbf", 1);
	PluginManager::register_filetype("text/spreadsheet", 1);

	if (interface::is_program_available("ssconvert")) {
		PluginManager::register_filetype("application/x-gnumeric", 1);
	}

} else {

	if (interface::is_program_available("abiword")) {

		PluginManager::register_filetype("application/msword", 1);
		PluginManager::register_filetype("application/rtf", 1);
		PluginManager::register_filetype("application/x-abiword", 1);
	}

	if (interface::is_program_available("ssconvert")) {

		PluginManager::register_filetype("application/vnd.ms-excel", 1);
		PluginManager::register_filetype("application/vnd.oasis.opendocument.spreadsheet", 1);
		PluginManager::register_filetype("application/vnd.oasis.opendocument.spreadsheet-template", 1);
		PluginManager::register_filetype("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", 1);
		PluginManager::register_filetype("application/x-gnumeric", 1);
	}

}

#endif

#endif /* OFFICE_PLUGIN_H */
