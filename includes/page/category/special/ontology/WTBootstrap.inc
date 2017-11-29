<?php

class WTBootstrap extends SpecialPage {

	public function __construct() {
		parent::__construct( 'WTBootstrap', 'edit-core-ontology' );
	}

	function execute( $par ) {
		global $wgDir, $wgUseLiPD;
		if ( !$this->userCanExecute( $this->getUser() ) ) {
			$this->displayRestrictionError();
			return;
		}

		$request = $this->getRequest();
		$out = $this->getOutput();
		$out->addExtensionStyle( $wgDir . '/css/wtree.css' );
		$this->setHeaders();

		$items = $this->getOntologyItems();

		$this->handleOperation($request, $items, $out);
		$this->showBootstrapForm($items, $out);
		$this->showOntologyItems($items, $out);
	}

	function getItemDetails($item, $unionprops, $objprops, $dataprops) {
		$details = [];
		foreach($unionprops as $prop) {
			$list = $this->getUnionPropertyValues($item, $prop);
			if(sizeof($list) > 0) {
				$details[$prop] = [];
				foreach($list as $listitem)
					$details[$prop][] = $listitem->getUri();
			}
		}
		foreach($objprops as $prop) {
			$vals = $item->allResources($prop);
			if(sizeof($vals) > 0) {
				$details[$prop] = [];
				foreach($vals as $val)
					$details[$prop][] = $val->getUri();
			}
		}
		foreach($dataprops as $prop) {
			$vals = $item->allLiterals($prop);
			if(sizeof($vals) > 0) {
				$details[$prop] = [];
				foreach($vals as $val)
					$details[$prop][] = $val->getValue();
			}
		}
		return $details;
	}

	function getUnionPropertyValues($item, $prop) {
		$value = $item->get($prop);
		$list = array();
		if($value && $value->isBNode()) {
			$union = $value->get("owl:unionOf");
            if($union) {
				while($union->valid()) {
					array_push($list, $union->current());
					$union->next();
				}
			}
			$intersection = $value->get("owl:intersectionOf");
            if($intersection) {
				while($intersection->valid()) {
					array_push($list, $intersection->current());
					$intersection->next();
				}
			}
		}
		else if($value)
			array_push($list, $value);
		return $list;
	}

	function getClasses($graph) {
		$classes = array();
		foreach($graph->allOfType("owl:Class") as $cls) {
			if($cls && !$cls->isBNode()) {
				$clsid = $cls->getUri();
				$class = array("id"=>$clsid, "name"=>$this->localName($cls));
				$class["details"] = $this->getItemDetails($cls,
					array("rdfs:subClassOf", "owl:disjointWith"),
					array(),
					array("rdfs:comment", "rdfs:isDefinedBy"));
				$classes[$clsid] = $class;
			}
		}
		return $classes;
	}

	function isCrowdClass($cls, $allcls, $parent=false) {
		if($parent && preg_match("/^Proxy(Archive|Observation|Sensor)$/", $cls["name"]))
			return true;
		if($parent && preg_match("/^(Instrument|InferredVariable)$/", $cls["name"]))
			return true;
		if(array_key_exists("rdfs:subClassOf", $cls["details"]))
			foreach($cls["details"]["rdfs:subClassOf"] as $subclsid)
				if($this->isCrowdClass($allcls[$subclsid], $allcls, true))
					return true;
		return false;
	}

	function localName($item) {
		if(preg_match("/.*#/", $item->getUri()))
			return preg_replace("/^.*#/", "", $item->getUri());
		return preg_replace("/^.*\//", "", $item->getUri());
	}

	function getInstances($graph, $clses) {
		$instances = array();
		foreach($clses as $cls) {
			foreach($graph->allOfType($cls["id"]) as $inst) {
				if($inst && !$inst->isBNode()) {
					$instid = $inst->getUri();
					$instance = array("id"=>$instid, "name"=>$this->localName($inst));
					$instance["details"] = $this->getItemDetails($inst,
						array(),
						array(),
						array("rdfs:comment", "rdfs:isDefinedBy"));
					$instance["details"]["rdf:type"] = $cls["id"];
				}
				$instances[$instid] = $instance;
			}
		}
		return $instances;
	}

	function getObjectProperties($graph) {
		return $this->getProperties("owl:ObjectProperty", $graph);
	}

	function getDataProperties($graph) {
		return $this->getProperties("owl:DatatypeProperty", $graph);
	}

	function getProperties($type, $graph) {
		$properties = array();
		foreach($graph->allOfType($type) as $prop) {
			if($prop && !$prop->isBNode()) {
				$propid = $prop->getUri();
				$property = array("id"=>$propid, "name"=>$this->localName($prop));
				$property["details"] = $this->getItemDetails($prop,
					array("rdfs:subPropertyOf", "rdfs:domain", "rdfs:range", "owl:propertyDisjointWith"),
					array("rdf:type"),
					array("rdfs:comment", "rdfs:isDefinedBy"));
				$properties[$propid] = $property;
			}
		}
		return $properties;
	}

	function handleOperation($request, $items, $out) {
		$op = $request->getText("op");
		$onturl = $request->getText("onturl");
		$loc = preg_replace("/\?.*$/", "", $request->getFullRequestURL());
		switch($op) {
			case "bootstrap":
				$this->bootstrapOntology($onturl, $out);
				$request->response()->header("Location: $loc");
				break;
			case "delete":
				$this->deleteOntology($items, $out);
				$request->response()->header("Location: $loc");
				break;
		}
	}

	function getCoreWikiName($pname) {
		global $wgCore;
		return ucfirst($pname)." $wgCore";
	}

	function bootstrapOntology($onturl, $out) {
		global $wgOntName, $wgOntNS;
		require 'vendor/autoload.php';
		$ont = new EasyRdf_Graph();
		$options = array('http' => array(
		    'header' => "Accept: application/rdf+xml\r\n"
		));
		$context = stream_context_create($options);
		$data = file_get_contents($onturl, FALSE, $context);
		$ont->parse($data);
		$clses = $this->getClasses($ont);
		$insts = $this->getInstances($ont, $clses);
		$props = $this->getObjectProperties($ont);
		$dprops = $this->getDataProperties($ont);

		// Fill in the ontologies
		$ontologies = array(
			"core"=>array(
				"ns"=>$wgOntNS,
				"name"=>$wgOntName,
				"terms"=>array()
			),
			"dcterms"=>array(
				"ns"=>"http://purl.org/dc/terms/",
				"name"=>"Dublin Core Terms",
				"terms"=>array()
			),
			"schema"=>array(
				"ns"=>"http://schema.org/",
				"name"=>"Schema.org",
				"terms"=>array()
			),
			"foaf"=>array(
				"ns"=>"http://xmlns.com/foaf/0.1/",
				"name"=>"Friend of a Friend",
				"terms"=>array()
			),
			"geo"=>array(
				"ns"=>"http://www.opengis.net/ont/geosparql#",
				"name"=>"GeoSparQL",
				"terms"=>array()
			),
			"wgs84"=>array(
				"ns"=>"http://www.w3.org/2003/01/geo/wgs84_pos#",
				"name"=>"World Geodetic System (1984)",
				"terms"=>array()
			),
		);

		// Create the ontology page first
		$allprops = array_merge($props, $dprops);
		foreach($allprops as $propid=>$prop) {
			foreach($ontologies as $pfx=>$details) {
				if(strpos($propid, $details["ns"]) === 0) {
					list($type, $cats) = $this->getPropertyWikiType($prop["details"]);
					$ontologies[$pfx]["terms"][] = array("name"=>$prop["name"], "type"=>"Type:$type");
					$allprops[$propid]["importid"] = "$pfx:" . $prop["name"];

					// Change the name to use prefix
					$pname = ($pfx == "core") ? $prop["name"] : "$pfx:". ucfirst($prop["name"]);
					$allprops[$propid]["name"] = $pname;
				}
			}
		}
		foreach($clses as $clsid=>$cls) {
			foreach($ontologies as $pfx=>$details) {
				if(strpos($clsid, $details["ns"]) === 0) {
					$ontologies[$pfx]["terms"][] = array("name"=>$cls["name"], "type"=>"Category");
					$clses[$clsid]["importid"] = "$pfx:" . $cls["name"];

					// Change the name to use prefix
					$cname = ($pfx == "core") ? $cls["name"] : "$pfx:". ucfirst($cls["name"]);
					$clses[$clsid]["name"] = $cname;
				}
			}
		}

		foreach($ontologies as $pfx=>$details) {
			$ns = $details["ns"];
			$name = $details["name"];
			$pageid = "MediaWiki:Smw_import_$pfx";
			$text = "$ns|[$ns $name]\n";
			foreach($details["terms"] as $term)
				$text .= " ".$term["name"]."|".$term["type"]."\n";

			// $out->addWikiText($text);
			$ret = WTFactsAPI::createWikiPage($pageid, $text, "WTBootstrap: Creating $name ontology imports page");
			$out->addHTML("<pre>".print_r($ret, true)."</pre>");
		}

		// Create wiki names for categories (distinguish between core and crowd)
		foreach($clses as $clsid=>$cls) {
			$clses[$clsid]["wikiname"] = $cls["name"];
			if(!$this->isCrowdClass($cls, $clses))
				$clses[$clsid]["wikiname"] = $this->getCoreWikiName($cls["name"]);
		}

		// Now create the properties and categories
		$catprops = [];
		$allcatprops = [];
		foreach($allprops as $propid=>$prop) {
			// Create property
			$this->createProperty($prop, $allprops, $clses, $out);

			// Get category properties (by looking at domain)
			if(array_key_exists("rdfs:domain", $prop["details"]))
				foreach($prop["details"]["rdfs:domain"] as $domain)
					$catprops[$domain][] = $prop;

			// Special properties (that are used by all categories)
			else if($prop["name"] == "name")
				$allcatprops[] = $prop;
		}

		foreach($clses as $clsid=>$cls) {
			if(!array_key_exists($clsid, $catprops))
				$catprops[$clsid] = [];
		}

		// Add parent properties as well
		foreach($catprops as $cat=>$props) {
			if(!array_key_exists($cat, $clses))
				continue;
			$cls = $clses[$cat];
			$queue = array($cls);
			while(sizeof($queue) > 0) {
				$qcls = array_shift($queue);
				if(array_key_exists("rdfs:subClassOf", $qcls["details"])) {
					foreach($qcls["details"]["rdfs:subClassOf"] as $parent) {
						$catprops[$cat] = array_merge($catprops[$cat], $catprops[$parent]);
						array_push($queue, $clses[$parent]);
					}
				}
			}

			// Keep only more specific properties in the list (if we have parent/child properties)
			$newprops = [];
			$propids = array();
			foreach($catprops[$cat] as $prop1) {
				$ignoreProp = false;
				foreach($catprops[$cat] as $prop2) {
					if($this->isSubPropertyOf($prop2, $prop1["id"])) {
						$ignoreProp = true;
						break;
					}
				}
				if(array_key_exists($prop1["id"], $propids))
					$ignoreProp = true;

				if(!$ignoreProp) {
					$newprops[] = $prop1;
					$propids[$prop1["id"]] = true;
				}
			}
			$catprops[$cat] = $newprops;
		}

		foreach($clses as $clsid=>$cls) {
			$this->createCategory($cls, array_merge($catprops[$clsid], $allcatprops), $clses, $out);
		}

		// Now create the instances
		foreach($insts as $instid=>$inst) {
			$this->createInstance($inst, $clses, $out);
		}
	}

	private function getOntologyItems() {
		$query = "[[imported from::+]] | ?Imported_from";
		$rawparams = array("[[imported from::+]]", "?Imported_from", "limit=5000");

		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );
		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);
		$results = smwfGetStore()->getQueryResult( $queryobj );

		$items = array();
		$nsToDelete = array();
		$res = $results->getNext();
		while($res != null) {
			$itemid = null;
			while($item = $res[0]->getNextObject()) {
				$itemid = $item->getTitle()->getFullText();
				while($nsitem = $res[1]->getNextObject()) {
					$ns = $nsitem->getNSID();
					$ref = $nsitem->getNS() . $nsitem->getLocalName();
					$items[$itemid] = array("ns"=>$ns, "ref"=>$ref);
				}
			}
			$res = $results->getNext();
		}
		return $items;
	}

	function showOntologyItems( $items, $out ) {
		$out->addHTML("<h3>Classes</h3>\n");
		$out->addHTML("<table class='sortable wikitable' width='100%'>\n");
		$out->addHTML("<tr><th>Wiki page</th><th>Ontological term</th></tr>");
		foreach($items as $id=>$det) {
			if(preg_match("/^Category:/", $id)) {
				$out->addHTML("<tr><td><a href='./$id'>$id</a></td>");
				$out->addHTML("<td><a href='${det['ref']}'>${det['ref']}</td></tr>");
			}
		}
		$out->addHTML("</table>\n");
		$out->addHTML("<h3>Properties</h3>\n");
		$out->addHTML("<table class='sortable wikitable' width='100%'>\n");
		$out->addHTML("<tr><th>Wiki page</th><th>Ontological term</th></tr>");
		foreach($items as $id=>$det) {
			if(preg_match("/^Property:/", $id)) {
				$out->addHTML("<tr><td><a href='./$id'>$id</a></td>");
				$out->addHTML("<td><a href='${det['ref']}'>${det['ref']}</td></tr>");
			}
		}
		$out->addHTML("</table>\n");
	}

	private function deleteOntology($items, $out) {
		$nsToDelete = array();
		foreach($items as $itemid=>$details) {
			$ns = $details["ns"];
			if(!in_array($ns, $nsToDelete))
				$nsToDelete[] = $ns;
			WTFactsAPI::deleteWikiPage($itemid, "Removing as the existing ontology was deleted");
		}
		foreach($nsToDelete as $ns) {
			WTFactsAPI::deleteWikiPage("MediaWiki:Smw_import_$ns", "Removing as the existing ontology was deleted");
		}
	}

	private function isSubPropertyOf($prop, $parentid) {
		if(array_key_exists("rdfs:subPropertyOf", $prop["details"]))
			foreach($prop["details"]["rdfs:subPropertyOf"] as $pid)
				if($parentid == $pid)
					return true;
		return false;
	}

	private function commentsToWiki($details) {
		$comment = $details["rdfs:comment"][0];
		return preg_replace(
			"/<a .+? href=\"(.+?)\">(.+?)<\/a>/", "[$1 $2]", $comment);
	}

	private function createCategory( $category, $properties, $allcats, $out ) {
		$catname = $category["wikiname"];
		$pageid = "Category:$catname";

		$title = Title::newFromText($pageid);
    if($title->exists())
			WTFactsAPI::deleteWikiPage($pageid, "Re-import");

		$text = "===Category: $catname [".$category["id"]."]===\n\n";
		$text .= "Imported from: [[imported from::" . $category["importid"] . "]]\n\n";
		if(array_key_exists("rdfs:comment", $category["details"]))
			$text .= $this->commentsToWiki($category["details"])."\n\n";

		$text .= "<!-- Do not edit below this unless you know what you are doing -->\n";
		$text .= "\nCategory Semantics\n";
    $pcat_text = "\n";
		if(array_key_exists("rdfs:isDefinedBy", $category["details"]))
			$text .= "* Defined by: [[is defined by::".$category["details"]["rdfs:isDefinedBy"][0]. "]]\n";

		if(sizeof($properties) > 0) {
			$text .= "* Standard Properties:\n";
			foreach($properties as $propid=>$prop)
				$text .= "** [[has property::Property:".$this->getCoreWikiName($prop["name"])."]]\n";
		}
		if(array_key_exists("rdfs:subClassOf", $category["details"])) {
			$text .= "* Sub-Class of\n";
			foreach($category["details"]["rdfs:subClassOf"] as $parent)
				if(array_key_exists($parent, $allcats)) {
					$parentname = $allcats[$parent]["wikiname"];
					$text .= "** [[subcategory of::Category:$parentname]]\n";
					$pcat_text .= "[[Category:$parentname]]\n";
				}
		}
		if(array_key_exists("owl:disjointWith", $category["details"])) {
			$text .= "* Disjoint with\n";
			foreach($category["details"]["owl:disjointWith"] as $peer)
				if(array_key_exists($peer, $allcats))
					$text .= "** [[disjoint with::Category:".$allcats[$peer]["wikiname"]."]]\n";
		}
		$text .= $pcat_text;
		// $out->addWikiText($text);
		// $out->addHTML(print_r($properties, true));
    $summary = "WTBootstrap: New Category $catname";
    $ret = WTFactsAPI::createWikiPage($pageid, $text, $summary);
		$out->addHTML("<pre>".print_r($ret, true)."</pre>");
	}

	private function createInstance($inst, $allcats, $out) {
		$pageid = $inst["name"];

		$title = Title::newFromText($pageid);
    if($title->exists())
			WTFactsAPI::deleteWikiPage($pageid, "Re-import");

		$text = "=== $pageid ===\n\n";
		if(array_key_exists("rdfs:comment", $inst["details"]))
			$text .= $this->commentsToWiki($inst["details"])."\n\n";

		$text .= "<!-- Do not edit below this unless you know what you are doing -->\n";
		if(array_key_exists("rdfs:isDefinedBy", $inst["details"]))
			$text .= "* Defined by: [[is defined by::".$inst["details"]["rdfs:isDefinedBy"][0]. "]]\n";

		if(array_key_exists("rdf:type", $inst["details"])) {
			$cat = $inst["details"]["rdf:type"];
			if(array_key_exists($cat, $allcats))
				$text .= "[[Category:". $allcats[$cat]["wikiname"]."]]\n";
		}
		// $out->addWikiText($text);
    $summary = "WTBootstrap: New Instance $pageid";
    $ret = WTFactsAPI::createWikiPage($pageid, $text, $summary);
		$out->addHTML("<pre>".print_r($ret, true)."</pre>");
	}

	function getPropertyWikiType($pdetails) {
		$literal = "http://www.w3.org/2000/01/rdf-schema#Literal";
		$xsd = "http://www.w3.org/2001/XMLSchema#";

		$type = "Text"; // Default type
		$cats = [];
		if(array_key_exists("rdfs:range", $pdetails)) {
			foreach($pdetails["rdfs:range"] as $range) {
				$pos1 = strpos($range, $xsd);
				if($pos1 !== false && $pos1 == 0) {
					$xsdtype = str_replace($xsd, "", $range);
					if($xsdtype == "float" || $xsdtype == "int")
						$type = "Number";
					else if($xsdtype == "string")
						$type = "Text";
					else if($xsdtype == "anyURI")
						$type = "URL";
					else if($xsdtype == "boolean")
						$type = "Boolean";
					else if($xsdtype == "dateTime")
						$type = "Date";
				}
				else if($range == $literal)
					$type = "Text";
				else {
					$type = "Page";
					$cats[] = $range;
				}
			}
		}
		return [$type, $cats];
	}

	private function getPropertyTypeWikiText($pdetails, $allprops, $allcats) {
		list($type, $cats) = $this->getPropertyWikiType($pdetails);
		$me = $this;

		$text = "";
		if(array_key_exists("rdfs:isDefinedBy", $pdetails))
			$text .= "* Defined by: [[is defined by::".$pdetails["rdfs:isDefinedBy"][0]. "]]\n";
		$text .= "* Property has type [[has type::$type]]";
		if(sizeof($cats) > 0) {
			$text .= " of Category ";
			$text .= implode (", or " ,
				array_map(function($c) use ($allcats, $me) {
					if(array_key_exists($c, $allcats))
						return "[[has page category::Category:".$allcats[$c]["wikiname"]."]]";
				}, $cats)
			);
		}

		$text .= "\n";
		if(array_key_exists("rdf:type", $pdetails) && !in_array("http://www.w3.org/2002/07/owl#FunctionalProperty", $pdetails["rdf:type"]))
			$text .= "* Can have multiple values ? [[has multiple values::true]]\n";
		if(array_key_exists("icon", $pdetails) && $pdetails["icon"])
			$text .= "* Font-awesome Icon: [[fontawesome icon::".$pdetails["icon"]. "]]\n";
		if(array_key_exists("rdfs:subPropertyOf", $pdetails)) {
			$text .= "* Sub-Property of\n";
			foreach($pdetails["rdfs:subPropertyOf"] as $parent)
				if(array_key_exists($parent, $allprops))
					$text .= "** [[subproperty of::Property:".$this->getCoreWikiName($allprops[$parent]["name"])."]]\n";
		}
		if(array_key_exists("owl:propertyDisjointWith", $pdetails)) {
			$text .= "* Disjoint with\n";
			foreach($pdetails["owl:propertyDisjointWith"] as $peer)
				if(array_key_exists($peer, $allprops))
					$text .= "** [[disjoint with::Property:".$this->getCoreWikiName($allprops[$peer]["name"])."]]\n";
		}
		return $text;
	}

	private function createProperty( $prop, $allprops, $allcats, $out ) {
		$propname = $this->getCoreWikiName($prop["name"]);
		$pageid = "Property:$propname";
        $ptitle = Title::newFromText($pageid);

    if($ptitle->exists())
			WTFactsAPI::deleteWikiPage($pageid, "Re-import");

		$text = "===Property: $propname [".$prop["id"]."]===\n";
		$text .= "Imported from: [[imported from::" . $prop["importid"] . "]]\n\n";
		if(array_key_exists("rdfs:comment", $prop["details"]))
			$text .= $this->commentsToWiki($prop["details"])."\n\n";
		$text .= "<!-- Do not edit below this unless you know what you are doing -->\n";
		$text .= "\nProperty Semantics\n";
		$text .= $this->getPropertyTypeWikiText($prop["details"], $allprops, $allcats);
		// $out->addWikiText($text);
    $summary = "WTBootstrap: New Property $propname";
    $ret = WTFactsAPI::createWikiPage($pageid, $text, $summary);
		$out->addHTML("<pre>".print_r($ret, true)."</pre>");
	}

	function showBootstrapForm( $items, $out ) {
		// Show the form
		if(sizeof($items) == 0) {
			$html  = "<h3>Load Ontology</h3>\n";
			$html .= "<form method='post'>\n";
			$html .= "<input type='hidden' name='op' value='bootstrap' />\n";
			$html .= "<input size='80' name='onturl'/> \n";
			$html .= "<input type='submit' />\n";
			$html .= "</form>\n";
		}
		else {
			$html = "<a class='lodbutton x-small' ";
			$html .= "onclick='return confirm(\"Are you sure you want to remove all ontological terms from the wiki ?\");' ";
			$html .= "href='?op=delete'>Delete all terms</a>";
		}
		$out->addHtml($html);
	}

	function getGroupName() {
		return "ODSGroup";
	}

}
