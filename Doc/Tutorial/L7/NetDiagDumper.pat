$!COMMENT(NAME=NetDiag2HTML, DESCRIPTION=Dump NetDiagrams in HTML format, PARADIGM=networking, VERSION=1;)
$!TO_FILE("Netdiagram-$(Name).html")
<html>
<head>
<title>Network list</title>
</head>
<body bgcolor="#B0E0FF">
<font color="#800080"><b>
<h2>Network device list for $Name</h2>
Routers
</b></font>
<table BORDER COLS=4 WIDTH="100%">
<tr>
<th>Name</th>
<th>Port name</th>
<th>Speed</th>
<th>IP Address</th>
</tr>
$!EVAL_FORALL(R:Router, $!SEQ("<tr><td>$Name</td></tr>\n",   \
$!EVAL_FORALL(R:Port, "<tr><td></td><td>$Name</td><td>$IFSpeed</td><td>$IPAddress</td></tr>\n")))
</table>
<p>
<font color="#800080"><b>Hosts</b></font>
<table BORDER COLS=2 WIDTH="50%">
<tr>
<th>Name</th>
<th>IP Address</th>
</tr>
$!EVAL_FORALL(R:Host, "<tr><td>$Name</td><td>$IPAddress</td></tr>\n")
</table>
</body>
</html>