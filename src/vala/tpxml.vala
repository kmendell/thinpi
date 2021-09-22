public static void print_simple (Xml.Node* node, string node_name) {
	assert (node->name == node_name);

	for (Xml.Node* iter = node->children; iter != null; iter = iter->next) {
		if (iter->type == Xml.ElementType.TEXT_NODE) {
			print ("   - %s\n", iter->get_content ());
		} else {
			print ("Unexpected element %s\n", iter->name);
		}
	}

}

public static void print_book (Xml.Node* node) {
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
				print_simple (iter, "title");
				break;

			case "ip":
				print_simple (iter, "ip");
				break;

            case "domain":
				print_simple (iter, "domain");
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
				print_book (iter);
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