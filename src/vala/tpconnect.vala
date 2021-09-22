using Gtk;

int main (string[] args) {
    Gtk.init (ref args);

    

    //  var test1 = TPServer() {
    //      serverName = "TPTest 1",
    //      serverDomain = ".",
    //      serverScreen = "1920x1080",
    //      serverUsername = "kmlab",
    //      serverPassword = "Onalaska!8",
    //      usbPass = 1,
    //      printerPass = 1,
    //      homePass = 1
    //  };

    //  TPConfig.servers += test1;
    
    try {
        var builder = new Builder ();
        builder.add_from_file ("/thinpi/Interface/connect-manager.glade");
        var serverList = builder.get_object("serverSelect") as ComboBoxText;
        var wrongLabel = builder.get_object("wrongLabel") as Label;
        var settingsButton = builder.get_object("settingsButton") as Button;
        builder.connect_signals (null);
        var window = builder.get_object ("thinpiMain") as Window;
        window.destroy.connect (Gtk.main_quit);

        settingsButton.clicked.connect (() => {
            Gtk.main_quit();
        });

        
        window.show_all ();
        window.fullscreen();
        
        wrongLabel.hide();

        serverList.append_text(TPConfig.servers[0].serverName);
        serverList.set_active(0);

        Gtk.main ();
    } catch (Error e) {
        stderr.printf ("Could not load ThinPi UI: %s\n", e.message);
        return 1;
    }

    return 0;
}