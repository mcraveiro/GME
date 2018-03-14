
seven_z=7z
UNAME_S=$(uname -s)
if [ ${UNAME_S:0:6} == "CYGWIN" ]; then
 seven_z=/cygdrive/c/Program\ Files/7-Zip/7z.exe
fi
if [ ${UNAME_S:0:7} == "MINGW32" ]; then
 seven_z=/c/Program\ Files/7-Zip/7z.exe
fi
if [ ${UNAME_S:0:7} == "MSYS_NT" ]; then
 seven_z=/c/Program\ Files/7-Zip/7z.exe
fi

rm *zip
( cd CSharpInterpreter && "$seven_z" a -x\!.svn -r ../CSharpInterpreter.zip * ) && ( cd CSharpAddon/ && "$seven_z" a -x\!.svn -r ../CSharpAddon.zip * )
