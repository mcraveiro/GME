$!DEFINE("_FILENAMEBASE", "$!IFDEFAULT("FileNameBase","$(Name)")")
$!TO_FILE("$!IFDEFAULT("OutputPath","")$(_FILENAMEBASE).xml")

<?xml version="1.0" encoding="UTF-8"?>

<$Name>

$!EVAL_FORALL("R:Folder","<Folder>\n<Name>$Name</Name>\n<FolderContainment>\n\
$!EVAL_FORALL("Src:FolderContainment","<$Kind>$Name</$Kind>\n")\
</FolderContainment>\n</Folder>\n")

$!EVAL_FORALL("R:Model","<Model>\n<Name>$Name</Name>\n<Containment>\n\
$!EVAL_FORALL("Src:Containment","<$Kind>$Name</$Kind>\n")\
</Containment>\n</Model>\n")

$!EVAL_FORALL("R:Set","<Set>\n<Name>$Name</Name>\n<SetMembership>\n\
$!EVAL_FORALL("Src:SetMembership","<$Kind>$Name</$Kind>\n")\
</SetMembership>\n</Set>\n")

$!EVAL_FORALL("R:Reference","<Reference>\n<Name>$Name</Name>\n<ReferTo>\n\
$!EVAL_FORALL("Dst:ReferTo","<$Kind>$Name</$Kind>\n")\
</ReferTo>\n</Reference>\n")

$!EVAL_FORALL("R:FCO","<FCO>\n<Name>$Name</Name>\n<Inheritance>\n\
$!EVAL_FORALL("Dst:BaseInheritance","\
$!EVAL_FORALL("Dst:DerivedInheritance","<$Kind>$Name</$Kind>\n")")\
</Inheritance>\n</FCO>\n")

$!EVAL_FORALL("R:Atom","<Atom>\n<Name>$Name</Name>\n<Inheritance>\n\
$!EVAL_FORALL("Dst:BaseInheritance","\
$!EVAL_FORALL("Dst:DerivedInheritance","<$Kind>$Name</$Kind>\n")")\
</Inheritance>\n</Atom>\n")

$!EVAL_FORALL("R:Connector","<Connector>\n<SourceToConnector>\n\
$!EVAL_WITH("Src:SourceToConnector","<$Kind>$Name</$Kind>\n")\
</SourceToConnector>\n<ConnectorToDestination>\
$!EVAL_WITH("Dst:ConnectorToDestination","<$Kind>$Name</$Kind>\n")\
</ConnectorToDestination>\n<AssociationClass>\n\
$!EVAL_WITH("Dst:AssociationClass","<$Kind>$Name</$Kind>\n")\
</AssociationClass>\n</Connector>\n")

</$Name>
