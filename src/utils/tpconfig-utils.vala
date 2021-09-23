using Gtk;

class ConfigUtils {
    public static void loadServers(ComboBoxText box) {
        foreach (TPServer? i in ThinPiPublic.publicServerArray) {
            box.append_text(i.serverName);
            
        }
        box.append_text("<Add New>");
    }

    public static void loadServerFields(int id, Entry sname, Entry sip, Entry sdomain, Entry sscreen) {

        sname.set_text(ThinPiPublic.publicServerArray.get(id).serverName);
        sip.set_text(ThinPiPublic.publicServerArray.get(id).serverIP);
        sdomain.set_text(ThinPiPublic.publicServerArray.get(id).serverDomain);
        sscreen.set_text(ThinPiPublic.publicServerArray.get(id).serverScreen);
    }

    public static void setupLoad(int id, Entry sname, Entry sip, Entry sdomain, Entry sscreen, ComboBoxText list) {
        sname.set_text(ThinPiPublic.publicServerArray.get(id).serverName);
        sip.set_text(ThinPiPublic.publicServerArray.get(id).serverIP);
        sdomain.set_text(ThinPiPublic.publicServerArray.get(id).serverDomain);
        sscreen.set_text(ThinPiPublic.publicServerArray.get(id).serverScreen);
    }

    public static void clearFields(Entry sname, Entry sip, Entry sdomain, Entry sscreen) {
        sname.set_text("");
        sip.set_text("");
        sdomain.set_text("");
        sscreen.set_text("");
    }
}