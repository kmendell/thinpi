
VALAFLAGS=--pkg gtk+-3.0 --pkg glib-2.0 --pkg gee-0.8 --pkg libxml-2.0 

vala:
	valac interface/tpconnect.vala interface/tpconfig.vala classes/tppublic.vala classes/tpserverclass.vala utils/tpxml.vala $(VALAFLAGS) -o build/tpc
	build/tpc