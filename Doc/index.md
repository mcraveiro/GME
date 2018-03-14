<link href="markdown.css" rel="stylesheet"></link>

<!---
This file is synced manually to https://forge.isis.vanderbilt.edu/gme/
See also  https://svn.isis.vanderbilt.edu/GMESRC/trunk/Doc/GME_release_howto.txt
-->

GME
===

GME is the generic modeling environment.

[GME overview](http://www.isis.vanderbilt.edu/Projects/gme/)

Download GME  
&nbsp; [README/changelog](http://repo.isis.vanderbilt.edu/GME/17.12.6/README.txt)  
&nbsp; <a href="http://repo.isis.vanderbilt.edu/GME/17.12.6/GME-17.12.6.msi" id="win32_installer">GME-17.12.6.msi</a>  
&nbsp; <a href="http://repo.isis.vanderbilt.edu/GME/17.12.6/GME_x64-17.12.6.msi" id="win64_installer">GME_x64-17.12.6.msi</a>  
&nbsp; [older releases, source code, and debugging symbols](http://repo.isis.vanderbilt.edu/GME/old/)  

[GME User's Manual](GME Manual and User Guide/GME Manual and User Guide.pdf)

[GME Tutorials](Tutorial/index.html)

[Issue tracking](http://escher.isis.vanderbilt.edu/JIRA/browse/GME)

[gme-users Mailing list](http://list.isis.vanderbilt.edu/mailman/listinfo/gme-users)

<script>
var platform = 'Unknown';
function find_platform() {
  ua = navigator.userAgent;

  if (ua.search(/Win64; x64/i) >= 0 || ua.search(/WOW64/i) >= 0) {
    platform = 'win64';
  } else if (ua.search(/Intel Mac OS X 10.[6789]/i) >= 0) {
    platform = 'macintel64';
  } else if (navigator.platform) {
    platform = navigator.platform.toLowerCase();
  }
};
find_platform();
document.getElementById(platform + "_installer").style.fontWeight = "bold";
</script>