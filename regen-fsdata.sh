#!/bin/sh

echo Building React app
cd www
npm i
npm run build
cd ..
echo Done

echo Regenerating lib/httpd/fsdata.c
./tools/makefsdata www/build/ -f:lib/httpd/fsdata.c
echo Done

echo Replace includes
perl -i -p0e 's/#include.*?"lwip\/def.h"/#include "fsdata.h"/s' lib/httpd/fsdata.c
echo Done
