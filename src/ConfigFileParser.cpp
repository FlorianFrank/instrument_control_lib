//
// Created by florianfrank on 30.05.22.
//
#include <iostream>
#include "pugixml.hpp"

#include "ConfigFileParser.h"


/*static*/ PIL_ERROR_CODE ConfigFileParser::ParseSupportedDevices(std::string &configFile, std::vector<DeviceDescription> &configFiles)
{
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(configFile.c_str(), pugi::parse_default | pugi::parse_declaration);
    if (result.status != pugi::status_ok)
        return PIL_XML_PARSING_ERROR;

    pugi::xml_node root = doc.document_element();

    for (auto child: root.children())
    {
        DeviceDescription devDescription;
        for (auto attrib: child.attributes())
        {
            std::string name =  attrib.name();
            std::string value  = attrib.value();

            if(name == "name")
                devDescription.m_DeviceName = value;
            if(name == "description")
                devDescription.m_DeviceDescription = value;
        }
        configFiles.push_back(devDescription);
    }
    return PIL_NO_ERROR;
}
