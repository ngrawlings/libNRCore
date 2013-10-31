//
//  Xml.cpp
//  libNRCore
//
//  Created by Nyhl Rawlings on 29/10/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#include "Xml.h"

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
    
    void XmlState::moveParentNode() {
        if (current_node->parent)
            current_node = current_node->parent;
    }
    
    void XmlState::moveChildrenNode() {
        if (current_node->children)
            current_node = current_node->children;
    }
    
    void XmlState::moveNextNode() {
        if (current_node->next)
            current_node = current_node->next;
    }
    
    void XmlState::movePreviousNode() {
        if (current_node->prev)
            current_node = current_node->prev;
    }
    
    String XmlState::getName() {
        return (const char*)current_node->name;
    }
    
    String XmlState::getContent() {
        return (const char*)xmlNodeListGetString(doc, current_node->xmlChildrenNode, 1);
    }

    Xml::Xml(String xml) {
        doc = xmlReadMemory(xml, xml.length(), "xml", NULL, 0);
        if (doc == NULL)
            throw "invalid xml";
    }

    Xml::~Xml() {
        xmlFreeDoc(doc);
        xmlCleanupParser();
    }

};