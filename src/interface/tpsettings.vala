using Gtk;

int showSettings (string[] args) {
    Gtk.init (ref args);

    try {
        var builder = new Builder ();
        builder.add_from_file ("/thinpi/Interface/settings.glade");
        //  builder.connect_signals (null);
        var window2 = builder.get_object ("settingsWindow") as Window;
        var notebookMenu = builder.get_object("settingMenuBar") as Notebook;
        window2.destroy.connect (window2.close);
        
        //  window.show_all ();

        window2.show();
    } catch (Error e) {
        stderr.printf ("Could not load ThinPi UI: %s\n", e.message);
        return 1;
    }

    return 0;
}