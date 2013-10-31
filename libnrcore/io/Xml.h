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
        XmlState(const XmlState &xmlstate);
        virtual ~XmlState();
        
        void moveParentNode();
        void moveChildrenNode();
        void moveNextNode();
        void movePreviousNode();
        
        String getName();
        String getContent();
        
        XmlState getChildren();
        
    protected:
        XmlState(xmlDocPtr doc);
        
        xmlDocPtr   doc;
        xmlNodePtr  current_node;
    };
    

    class Xml {
    public:
        Xml(String xml);
        virtual ~Xml();
        
        XmlState getState();
        
    private:
        xmlDocPtr doc;
    };
    
    
};

#endif /* defined(__libNRCore__Xml__) */
