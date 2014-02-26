//
//  Xml.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 29/10/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#include "Xml.h"
#include "File.h"

namespace nrcore {
    
    XmlState::XmlState(xmlDocPtr doc) {
        this->doc = doc;
        this->current_node = doc->children;
    }
    
    XmlState::XmlState(const XmlState &xmlstate) {
        this->doc = xmlstate.doc;
        this->current_node = xmlstate.current_node;
    }
    
    XmlState::~XmlState() {
        
    }
    
    XmlState XmlState::getChildren() {
        XmlState xs(*this);
        xs.moveChildrenNode();
        return xs;
    }
    
    bool XmlState::moveParentNode() {
        if (current_node->parent)
            current_node = current_node->parent;
        else
            return false;
        
        return true;
    }
    
    bool XmlState::moveChildrenNode() {
        if (current_node->children)
            current_node = current_node->children;
        else
            return false;
        
        return true;
    }
    
    bool XmlState::moveNextNode() {
        if (current_node->next)
            current_node = current_node->next;
        else
            return false;
        
        return true;
    }
    
    bool XmlState::movePreviousNode() {
        if (current_node->prev)
            current_node = current_node->prev;
        else
            return false;
        
        return true;
    }
    
    String XmlState::getName() {
        return (const char*)current_node->name;
    }
    
    String XmlState::getContent() {
        return (const char*)xmlNodeListGetString(doc, current_node->xmlChildrenNode, 1);
    }
    
    String XmlState::getAttribute(String name) {
        for (struct _xmlAttr *attr = current_node->properties; attr; attr = attr->next) {
            if (String((char*)attr->name) == name)
                return (char*)attr->children->content;
        }
        
        return "";
    }
    
    Xml::Xml() {
        doc = 0;
    }

    Xml::Xml(String xml) {
        if (!load(xml))
            throw "invalid xml";
    }

    Xml::~Xml() {
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }

    bool Xml::load(String xml) {
        if (doc)
            xmlFreeDoc(doc);
        
        doc = xmlReadMemory(xml, xml.length(), "xml", NULL, 0);
        return doc != NULL;
    }
    
    bool Xml::loadFile(String path) {
        try {
            File file(path);
            int len = file.length();
        
            Ref<char> mem = file.getMemory();
        
            if (!load(mem.getPtr()))
                return false;
        
        } catch (...) {
            return false;
        }
        
        return true;
    }
    
    XmlState Xml::getState() {
        return XmlState(doc);
    }
};
