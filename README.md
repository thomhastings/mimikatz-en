mimikatz-en
===========

This is an English language localisation of [mimikatz](http://blog.gentilkiwi.com/mimikatz). Mimikatz uses admin rights on a Windows system to display passwords of currently logged in users in plaintext. Mimikatz was written by Benjamin "gentilkiwi" Delpy.  

In partictular, the tool is quite powerful when combined with [netview](http://www.room362.com/blog/2012/10/8/compiling-and-release-of-netview.html). Netview is a network scanner with the capability to display active RDP connections, helping to point out systems on which multiple network users are currently logged in. These make good targets for mimikatz after a privledge escalation. Netview was released by Rob "mubix" Fuller.  

The translation of r46 code from the svn [repo](http://code.google.com/p/mimikatz/source/checkout) was done first with Google Translate and then edited by Rich Rumble. I did a diff/patch against r47 and further edited the translation.  

Customarily, I am not responsible for malicious use of this tool, and I hope that releasing it and its source code leads to better information security for the community at large.  


mimikatz
========

This repository contains the source code. Extracts of different source codes are available: http://blog.gentilkiwi.com/mimikatz
This utility can be used only for study, pentests and other legal actions in accordance with the license specified below.

More generally, thank you for respecting the philosophy with which this tool was created, ie not allowing its use for malicious purposes.
I would be in no way responsible for the use made of these technologies.

Binaries, source code, and uses derivatives under license: http://creativecommons.org/licenses/by-nc-sa/3.0/en/
Except for the library sekurlsa: http://creativecommons.org/licenses/by/3.0/en/
_____________________________________________________
Benjamin DELPY `gentilkiwi`
http://blog.gentilkiwi.com/mimikatz
benjamin@gentilkiwi.com

anti-gpo standalone
===================

These tools replaced by direct use of mimikatz

* KiwiRegedit
	mimikatz nogpo::regedit exit

* KiwiCmd
	mimikatz nogpo::cmd exit

* KiwiRegedit
	mimikatz nogpo::taskmgr exit


tools incorporated
==================

* psexec.exe
	PsExec v1.98 - Execute processes remotely
	Mark Russinovich (Sysinternals / Microsoft)
	
* tee.exe
	GNU shellutils 1.9.4
	http://unxutils.sourceforge.net/

* winmine.exe
	Windows XP Minesweeper
	Robert Donner and Curt Johnson (Microsoft)
