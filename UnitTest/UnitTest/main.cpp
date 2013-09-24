//
//  main.cpp
//  UnitTest
//
//  Created by Nyhl Rawlings on 9/21/13.
//  Copyright (c) 2013 Nyhl Rawlings. All rights reserved.
//

#include <libnrcore/memory/String.h>

#include <iostream>

using namespace nrcore;

int main(int argc, const char * argv[])
{

    // insert code here...
    String str = "test % test % test % test \r\n :) \r\n";
    str.arg(21).arg(22).arg(23);
    str.insert(0, "-> ");
    str.escape();
    printf("%s\r\n", str.operator char *());
    return 0;
}

