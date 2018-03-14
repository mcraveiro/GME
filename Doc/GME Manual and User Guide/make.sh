#!/bin/bash

# for file in lib/resolver.jar lib/xalan/*jar .; do echo -n $file\;; done
XALANCP="lib/resolver.jar;lib/xalan/serializer.jar;lib/xalan/xalan.jar;lib/xalan/xercesImpl.jar;lib/xalan/xml-apis.jar;."

#for file in lib/fop/*jar; do echo -n $file\;; done
FOPCP="lib/fop/avalon-framework-4.2.0.jar;lib/fop/batik-all-1.7.jar;lib/fop/commons-io-1.3.1.jar;lib/fop/commons-logging-1.0.4.jar;lib/fop/fop.jar;lib/fop/serializer-2.7.0.jar;lib/fop/xalan-2.7.0.jar;lib/fop/xercesImpl-2.7.1.jar;lib/fop/xml-apis-1.3.04.jar;lib/fop/xml-apis-ext-1.3.04.jar;lib/fop/xmlgraphics-commons-1.3.1.jar"

java -cp "$XALANCP" org.apache.xalan.xslt.Process -ENTITYRESOLVER org.apache.xml.resolver.tools.CatalogResolver -URIRESOLVER  org.apache.xml.resolver.tools.CatalogResolver -in "GME Manual and User Guide.xml" -xsl htmlhelp.xsl -out unused
hhc htmlhelp.hhp
mv htmlhelp.chm GME\ Manual\ and\ User\ Guide.chm

java -Xmx256m -cp "$XALANCP" org.apache.xalan.xslt.Process -ENTITYRESOLVER org.apache.xml.resolver.tools.CatalogResolver -URIRESOLVER  org.apache.xml.resolver.tools.CatalogResolver -in "GME Manual and User Guide.xml" -xsl fo.xsl -out "GME Manual and User Guide".fo
java -Xmx256m -cp "$FOPCP" org.apache.fop.cli.Main -fo "GME Manual and User Guide".fo -pdf "GME Manual and User Guide".pdf



