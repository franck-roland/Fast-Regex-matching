#!/bin/sh
gcc -pthread -Wall -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -c libRegex.c -o libRegex.o;
gcc -pthread -Wall -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-z,relro libRegex.o -o _libRegex.so
#-L/usr/lib/python2.7/config -lpthread -ldl -lutil -lm -lpython2.7 -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
