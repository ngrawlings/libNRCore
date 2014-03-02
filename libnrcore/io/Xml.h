//
//  Xml.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 29/10/13.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Xml__
#define __libNRCore__Xml__

#include <libxml/parser.h>

#include <libnrcore/memory/String.h>

namespace nrcore {
    
    
    class XmlState {
    public:
        friend class Xml;
        
        XmlState();
        XmlState(const XmlState &xmlstate);
        virtual ~XmlState();
        
        bool isValid();
        
        bool moveParentNode();
        bool moveChildrenNode();
        bool moveNextNode();
        bool movePreviousNode();
        
        XmlState getChildNode(String name);
        
        String getName();
        String getContent();
        String getAttribute(String name);
        
        XmlState getChildren();
        
    protected:
        XmlState(xmlDocPtr doc);
        
        xmlDocPtr   doc;
        xmlNodePtr  current_node;
    };
    

    class Xml {
    public:
        Xml();
        Xml(String xml);
        virtual ~Xml();
        
        bool load(String xml);
        bool loadFile(String path);
        XmlState getState();
        
    private:
        xmlDocPtr doc;
    };
    
    
};

#endif /* defined(__libNRCore__Xml__) */
