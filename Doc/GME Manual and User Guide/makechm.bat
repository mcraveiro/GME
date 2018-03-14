SET XALANCP="lib\resolver.jar;lib\xalan\serializer.jar;lib\xalan\xalan.jar;lib\xalan\xercesImpl.jar;lib\xalan\xml-apis.jar;."

REM SET HTMLHELP="%GME_ROOT%\Doc\GME Manual and User Guide\lib\docbook-xsl\htmlhelp\htmlhelp.xsl"
SET HTMLHELP="%GME_ROOT%\Doc\GME Manual and User Guide\htmlhelp.xsl"

java -Xmx1500m -cp %XALANCP% org.apache.xalan.xslt.Process -ENTITYRESOLVER org.apache.xml.resolver.tools.CatalogResolver -URIRESOLVER org.apache.xml.resolver.tools.CatalogResolver -in "GME Manual and User Guide.xml" -xsl %HTMLHELP% -out out.test -param chunk.section.depth 8 -param chunk.first.sections 1

IF %PROCESSOR_ARCHITECTURE% == AMD64 (
	"c:\Program Files (x86)\HTML Help Workshop\hhc.exe" htmlhelp.hhp
) ELSE (
	"c:\Program Files\HTML Help Workshop\hhc.exe" htmlhelp.hhp
)
