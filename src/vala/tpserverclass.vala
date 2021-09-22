using Gee;

public struct TPServer {
    public string serverName;
    public string serverIP;
    public string serverDomain;
    //  public string serverUsername;
    //  public string serverPassword;
    //  public string serverScreen;
    public string usbPass;
    public string printerPass;
    public string homePass;
}

public class ThinPiPublic {
    public static ThinPiPublic instance() {
        return new ThinPiPublic();
    }
    public static TPServer loadServer;
    public static ArrayList<TPServer?> publicServerArray = new ArrayList<TPServer?>();
    public static void addToArray(TPServer server) {
        var inst = ThinPiPublic.instance();
        if (!inst.publicServerArray.contains(server)) {
            inst.publicServerArray.add(server);
            print("CHECK: %s\n\n", server.serverName);
        } else {
            print("Error");
        }
    }
    public static void seeServers() {
        foreach (TPServer? i in ThinPiPublic.publicServerArray) {
            stdout.printf ("SEESERVERS: %s , %s\n", i.serverName, i.serverIP);
        }
    }
}

