//
//  Formula.h
//  libNRCore
//
//  Created by Nyhl Rawlings on 23/08/2013.
//  Copyright (c) 2013 N G Rawlings. All rights reserved.
//

#ifndef __libNRCore__Formula__
#define __libNRCore__Formula__

namespace nrcore {

    class SimpleFormula {
    public:
        
        typedef enum {
            ADD, SUBTRACT,
            MULTIPLY, DIVIDE,
            MODULO
        } OPERATION;
        
        typedef struct {
            OPERATION op;
            
        } FORMULA_NODE;
        
    private:
        
    };
    
};

#endif /* defined(__libNRCore__Formula__) */
