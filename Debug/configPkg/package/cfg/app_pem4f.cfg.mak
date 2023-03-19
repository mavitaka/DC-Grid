# invoke SourceDir generated makefile for app.pem4f
app.pem4f: .libraries,app.pem4f
.libraries,app.pem4f: package/cfg/app_pem4f.xdl
	$(MAKE) -f C:\Users\Yacine\workspace_v7\ProjectDCGrid/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Yacine\workspace_v7\ProjectDCGrid/src/makefile.libs clean

