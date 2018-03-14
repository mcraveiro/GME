SET XALANCP="lib\resolver.jar;lib\xalan\serializer.jar;lib\xalan\xalan.jar;lib\xalan\xercesImpl.jar;lib\xalan\xml-apis.jar;."

SET FOPCP="lib\fop\avalon-framework-4.2.0.jar;lib\fop\batik-all-1.7.jar;lib\fop\commons-io-1.3.1.jar;lib\fop\commons-logging-1.0.4.jar;lib\fop\fop.jar;lib\fop\serializer-2.7.0.jar;lib\fop\xalan-2.7.0.jar;lib\fop\xercesImpl-2.7.1.jar;lib\fop\xml-apis-1.3.04.jar;lib\fop\xml-apis-ext-1.3.04.jar;lib\fop\xmlgraphics-commons-1.3.1.jar"

java -Xmx1500m -cp %XALANCP% org.apache.xalan.xslt.Process -ENTITYRESOLVER org.apache.xml.resolver.tools.CatalogResolver -URIRESOLVER org.apache.xml.resolver.tools.CatalogResolver -in "GME Manual and User Guide.xml" -xsl fo.xsl -out "htmlhelp".fo

java -Xmx1500m -cp %FOPCP% org.apache.fop.cli.Main -fo "htmlhelp".fo -pdf "htmlhelp".pdf
