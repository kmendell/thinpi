public static void printConfig (Xml.Node* node, string node_name) {
	assert (node->name == node_name);

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.TEXT_NODE) {
			print ("   - %s\n", iter->get_content ());
		} else {
			print ("Unexpected element %s\n", iter->name);
		}
	}

}

public static void readConfigFile (Xml.Node* node) {
	assert (node->name == "server");

	print (" * Server:\n");

	string? id = node->get_prop ("id");
	if (id != null) {
		print ("   - %s\n", id);
	} else {
		print ("Expected: <server id=...\n");
	}

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.ELEMENT_NODE) {
			switch (iter->name) {
			case "title":
				printConfig (iter, "title");
				break;

			case "ip":
				printConfig (iter, "ip");
				break;

            case "domain":
				printConfig (iter, "domain");
				break;

			case "usb":
				printConfig (iter, "usb");
				break;

			case "printer":
				printConfig (iter, "printer");
				break;

			case "home":
				printConfig (iter, "home");
				break;

			default:
				print ("Unexpected element %s\n", iter->name);
				break;
			}
		}
	}
}

public static void print_books (Xml.Node* node) {
	assert (node->name == "connections");

	print ("ThinPi Connections:\n");
	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.ELEMENT_NODE) {
			if (iter->name == "server") {
				readConfigFile (iter);
			} else {
				print ("Unexpected element %s\n", iter->name);
			}
		}
	}
}

public static int main (string[] args) {
	// Parse the document from path
	Xml.Doc* doc = Xml.Parser.parse_file ("src/vala/thinpi.xml");
	if (doc == null) {
		print ("File 'books.xml' not found or permissions missing\n");
		return 0;
	}

	Xml.Node* root = doc->get_root_element ();
	if (root == null) {
		print ("WANTED! root\n");
		delete doc;
		return 0;
	}

	if (root->name == "connections") {
		print_books (root);
	} else {
		print ("Unexpected element %s\n", root->name);
	}

	delete doc;
	return 0;
}
