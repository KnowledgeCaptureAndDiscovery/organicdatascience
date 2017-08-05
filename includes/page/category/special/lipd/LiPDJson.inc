<?php

require_once("LiPDSchema.inc");
require_once("LiPDWiki.inc");
require_once("BagitHelper.inc");
require_once("Util.inc");

class LiPDJson {
	var $id;
	var $lipdfile;
	var $lipddir;

	static $bootstrapped = false;

	public function __construct($fileid, $lipdfile) {
		$this->id = preg_replace("/.lpd$/", "", $fileid);
		$this->lipdfile = $lipdfile;
	}

	public function import($out) {
		try {
			if($this->pageExists($this->id)) {
            $deleter = new LiPDWiki($this->id);
            $deleter->delete($out);
            $out->addHtml("<br/>LiPD dataset ". $this->id. " already exists. Please delete it before trying to import.<br/>\n");
            return false;
			}

			$zipfile = dirname($this->lipdfile). "/" .$this->id. ".zip";
			rename($this->lipdfile, $zipfile);

			$this->lipddir = BagitHelper::extractLiPD($zipfile);

			$jsonld = $this->getLiPDJson($this->lipddir);
			if(!$jsonld) {
				$out->addHtml("<br/>Could not get JsonLD file for ". $this->id. "<br/>\n");
				return false;
			}

			if(!self::$bootstrapped) {
				$this->bootstrapWiki($out);
				self::$bootstrapped = true;
			}

			$this->expandSchema($out);
			$obj = Util::readJson($jsonld);

			$hash = array();
			$this->mapLipdJson($obj, null, null, "Dataset", "Dataset", $hash, $out);
			//$out->addHTML("<pre>".print_r($hash, true)."</pre>");

			foreach($hash as $key=>$item)
				$this->createWikiItem($item, $out);

			BagitHelper::removeDir($this->lipddir);
			return true;
		}
		catch (Exception $e) {
			$out->addHtml("*** Could not parse ". $this->id ."<br/>\n");
			$out->addHtml($e);
		}
	}

	private function getLiPDJson( $datadir ) {
		$files = array_diff(scandir($datadir), array('.','..'));
		foreach ($files as $file) {
			if(preg_match("/\.jsonld$/", $file))
				return "$datadir/$file";
		}
		return null;
    }

	private function pageExists($id) {
		$title = Title::newFromText($id);
		return $title && $title->exists();
	}

	private function expandSchema($out) {
		global $SCHEMA, $wgCore;
		$xschema = [];
		foreach($SCHEMA as $key=>$props) {
			// Add core schema too
			$corekey = $key."_$wgCore";
			$xschema[$key] = $props;
			$xschema[$corekey] = $props;
			foreach($props as $lipdkey=>$pdetails) {
				if(!is_array($pdetails))
					continue;
				if(array_key_exists("alternates", $pdetails)) {
					foreach($pdetails["alternates"] as $altkey) {
						$xschema[$key][$altkey] = $pdetails;
						$xschema[$corekey][$altkey] = $pdetails;
					}
				}
			}
		}
		$SCHEMA = $xschema;
	}

	private function modifyStructureIfNeeded($obj, $hash, $schema) {
		global $SCHEMA;
		if(array_key_exists("@fromJson", $schema)) {
			foreach($schema["@fromJson"] as $func) {
				list($obj, $hash, $newids) = call_user_func($func, $obj, $hash);
				foreach($newids as $newid) {
					if(array_key_exists($newid, $hash)) {
						$newobj = $hash[$newid];
						if(is_array($newobj) && array_key_exists("@category", $newobj)) {
							$newschid = $newobj["@category"];
							$newschema = array_key_exists($newschid, $SCHEMA) ? $SCHEMA[$newschid] : [];
							list($hash[$newid], $hash) = $this->modifyStructureIfNeeded($newobj, $hash, $newschema);
						}
					}
				}
			}
		}
		return [$obj, $hash];
	}

	private function mapLipdJson($obj, $parent, $index, $category, $schemaname, &$hash, $out) {
		global $SCHEMA, $BLACKLIST;
		$schema = array_key_exists($schemaname, $SCHEMA) ? $SCHEMA[$schemaname] : [];
		$SCHEMA[$schemaname] = $schema;

		if(!is_array($obj))
			return $obj;

		$obj["@parent"] = $parent;
		$obj["@index"] = $index;
		$obj["@schema"] = $schemaname;

		$objid = $this->getObjectId($obj, $category, $schema);
		if(array_key_exists("@id", $obj))
			$objid = $obj["@id"];

		if(array_key_exists($objid, $hash))
			return $objid;

		$obj["@id"] = $objid;

		list($obj, $hash) = $this->modifyStructureIfNeeded($obj, $hash, $schema);

		if(array_key_exists("@category", $obj))
			$category = $obj["@category"];

		$hash[$objid] = array("@id"=>$objid, "@category"=>$category, "@schema"=>$schemaname);
		$item = &$hash[$objid];

		if(is_array($obj)) {
			foreach($obj as $propkey=>$value) {
				if($propkey[0] == "@") continue;
				if(array_key_exists($propkey, $BLACKLIST))
					continue;

				$details = [];
				$pname = $propkey;
				if(array_key_exists($propkey, $schema)) {
					$details = $schema[$propkey];
					$pname = array_key_exists("name", $details) ? $details["name"] : $propkey;
				}
				$type = array_key_exists("type", $details) ? $details["type"] : null;
				$cat = array_key_exists("category", $details) ? $details["category"] : null;
				$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
				$fromJson = array_key_exists("fromJson", $details) ? $details["fromJson"] : null;
				$subobject = array_key_exists("subobject", $details) ? $details["subobject"] : false;
				if($sch && !$cat)
					$cat = $sch;

				if($fromJson) {
					$value = call_user_func($fromJson, $value, $obj);
					if(!$value)
						continue;
					if($pname) {
						if(is_array($value) && !Util::is_assoc($value)) {
							$index=1;
							foreach($value as $subvalue) {
								if(Util::is_assoc($subvalue)) {
									$item[$propkey][] = $this->mapLipdJson($subvalue, $obj, $index, $cat, $sch, $hash, $out);
									$index++;
								}
							}
						}
						else if(Util::is_assoc($value)) {
							$item[$propkey] = $this->mapLipdJson($value, $obj, null, $cat, $sch, $hash, $out);
						}
						else
							$item[$propkey] = $value;
					}
					else if(Util::is_assoc($value)) {
						foreach($value as $subpropkey=>$subvalue)
							$item[$subpropkey] = $subvalue;
					}
					continue;
				}

				if(!$pname)
					continue;

				if($subobject) {
					$item["@subobjects"][] = array($prop=>$value);
					continue;
				}

				if(is_array($value) && !Util::is_assoc($value)) {
					$index = 1;
					foreach($value as $subvalue) {
						$item[$propkey][] = $this->mapLipdJson($subvalue, $obj, $index, $cat, $sch, $hash, $out);
						$index++;
					}
				}
				else if(Util::is_assoc($value)) {
					$item[$propkey] = $this->mapLipdJson($value, $obj, null, $cat, $sch, $hash, $out);
				}
				else if($type == "Page") {
					$item[$propkey] = $value;
					if(!array_key_exists($value, $hash))
						$hash[$value] = array("@id"=>$value, "@category"=>$cat, "@schema"=>$sch);
				}
				else {
					$item[$propkey] = $value;
				}
			}
		}
		$hash[$objid] = $item;

		return $objid;
	}

	private function createWikiItem($obj, $out) {
		global $SCHEMA;
		$category = $obj["@category"];
		$extracats = array_key_exists("@extracats", $obj) ? $obj["@extracats"] : [];
		$schemaname = array_key_exists("@schema", $obj) ? $obj["@schema"] : $category;
		$schema = array_key_exists($schemaname, $SCHEMA) ? $SCHEMA[$schemaname] : [];
		$objid = $obj["@id"];
		if(!$objid)
			return;

		$subobjects = array();

		// Create category
		if($category)
			$category = $this->createCategory( $category, $schema, $out );

		// Create Page
		$data = $this->createPage($objid, $category, $extracats, $out);

		foreach($obj as $key=>$value) {
			if($key[0] == "@") continue;

			$details = $this->getPropertyDetails($key, $schema, $value);
			$prop = $details["name"];
			$type = $details["type"];
			$icon = array_key_exists("icon", $details) ? $details["icon"] : null;
			$cat = array_key_exists("category", $details) ? $details["category"] : null;
			$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
			if($sch && !$cat)
				$cat = $sch;
			$fromJson = array_key_exists("fromJson", $details) ? $details["fromJson"] : null;
			$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
			$subobject = array_key_exists("subobject", $details) ? $details["subobject"] : false;

			if(!$prop)
				continue;

			// Create Property
			$propDI = $this->createProperty( $prop, $type, $cat, $icon, $multiple, $out );

			// Set property value
			if(is_array($value) && !Util::is_assoc($value)) {
				foreach($value as $subvalue) {
					$data = $this->setProperty( $data, $propDI, $subvalue);
				}
			}
			else if($type == "Page" || Util::is_assoc($value)) {
				$data = $this->setProperty( $data, $propDI, $value);
			}
			else if($type == "File") {
				$fileid = $this->uploadFile( $value, $out );
				if($fileid) {
					$this->protectPage($fileid);
					$data = $this->setProperty( $data, $propDI, $fileid);
				}
			}
			else {
				$data = $this->setProperty( $data, $propDI, $value );
			}
		}

		$this->savePage( $objid, $data, $out );
	}

	private function protectPage( $title ) {
		global $wgUser;
		$protection = "sysop";
		$restrictions = [ 'edit' => $protection, 'move' => $protection, 'upload' => $protection ];
		$titleobj = Title::newFromText($title);
		$page = WikiPage::factory( $titleobj );
		$status = $page->doUpdateRestrictions( $restrictions, [], $cascade, "Dataset restrictions", $wgUser );
		return $status->isOK();
	}

	private function setProperty( $data, $propDI, $value ) {
		if(!$data)
			return $data;

		$objid = $data->getSubject()->getTitle();
		$prop = $propDI->getLabel();

        $valDV = SMWDataValueFactory::newPropertyObjectValue( $propDI, $value );
        $valDI = $valDV->getDataItem();
        if($valDI->getDIType() == SMWDataItem::TYPE_ERROR)
            return $data;

        $data = WTFactsAPI::addPropValToData($data, $propDI, $valDI, false);

		return $data;
	}


	private function setSubobjects( $data, $objid, $subobjid, $subpropvals, $schema ) {
		if(!$subpropvals)
			return $data;

		$subobjectid = $objid."#".$subobjid;
		$subtitle = Title::newFromText($subobjectid);
		$subpage = SMWDIWikiPage::newFromTitle($subtitle);

		$subdata = $data->findSubSemanticData($subobjectid);
		if($subdata)
			$data->removeSubSemanticData($subdata);

		$subdata = new SMWSemanticData($subpage);

		foreach($subpropvals as $pval) {
			foreach($pval as $key=>$value) {
				if($key[0] == "@") continue;

				$details = $this->getPropertyDetails($key, $schema, $value);
				$prop = $details["name"];
				$type = $details["type"];
				$icon = array_key_exists("icon", $details) ? $details["icon"] : null;
				$cat = array_key_exists("category", $details) ? $details["category"] : null;
				$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
				$fromJson = array_key_exists("fromJson", $details) ? $details["fromJson"] : null;
				$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;

				// Create & set Property
				$propDI = $this->createProperty( $prop, $type, $cat, $icon, $multiple );
				$subdata = $this->setProperty( $subdata, $propDI, $value);

				//print "|$key=$value";
			}
		}
		$data->addSubSemanticData($subdata);
		//print "\n";
		return $data;
	}

	private function createPage($objid, $category, $extracats, $out) {
		// Delete current page if one currently exists
		$title = Title::newFromText($objid);
		if(!$title)
			return;
      /*if($title->exists())
			WTFactsAPI::deleteWikiPage($objid, "Re-import");*/

		$subject = SMWDIWikiPage::newFromTitle( $title );
		if(!$title->exists()) {
			$text = $category ? "[[Category:$category]]\n" : "";
			foreach($extracats as $ecat) {
				$text .= "[[Category:$ecat]]\n";
			}
			$summary = "WTLiPD: New page $objid" . ($category ? " with Category: $category" : "");
			$ret = WTFactsAPI::createWikiPage($objid, $text, $summary);
			if($ret[0] != "Success")
				return null;
			//$out->addWikiText("$summary\n");

			return new SMWSemanticData($subject);
		}
		else {
			return smwfGetStore()->getSemanticData($subject);
		}
	}

	private function createCategory( $category, $schema, $out ) {
		$cname = $this->resolveCategory($category);
		if($cname) // If category already exists
			return $cname;

		$pageid = "Category:$category";

		/*$title = Title::newFromText($pageid);
        if($title->exists())
			WTFactsAPI::deleteWikiPage($pageid, "Re-import");*/

		$text = "===Category: $category===\n";
		foreach($schema as $key=>$val) {
			if($key[0] == "@") continue;

			$details = $this->getPropertyDetails($key, $schema, null);
			$standard = array_key_exists("standard", $details) ? $details["standard"] : false;
			if($standard)
				$text .= "*[[has property::Property:".$details["name"]."]]\n";
		}
		$summary = "WTLiPD: New Category $category";
		$ret = WTFactsAPI::createWikiPage($pageid, $text, $summary);
		if($ret[0] != "Success")
			return false;
		//$out->addWikiText("$summary\n");
		return $category;
	}

	private function getDataAsString($data) {
		$str = "";
		foreach ( $data->getProperties() as $prop ) {
			$pkey = $prop->getKey();
			if($pkey == "_MDAT" || $pkey == "_SKEY")
				continue;
			$curvals = [];
			foreach ( array_unique($data->getPropertyValues($prop), SORT_REGULAR) as $dataItem )
                $curvals[] = WTFactsAPI::getDIText($dataItem). " (".$dataItem->getDIType().")";
			$str .= " $pkey = ".implode(",", $curvals)."\n";
		}
		return $str;
	}

	private function savePage( $objid, $data, $out ) {
		//$out->addWikiText("Saving $objid\n".$this->getDataAsString($data));
		WTFactsAPI::updateWiki( $objid, "WTLiPD: Edit Page", $data );
	}

	private function createProperty( $prop, $type, $cat, $icon, $multiple, $out ) {
		$pname = $this->resolveProperty($prop);
    if(!$pname) {
			if($type == "File")
				$type = "Page";

			$text = "* Property Type: [[has type::$type]]";
			if($cat)
				$text .= " of [[has page category::Category:$cat]]";
			$text .= "\n";
			if($multiple)
				$text .= "* Can have multiple values ? [[has multiple values::true]]\n";
			if($icon)
				$text .= "* Font-awesome Icon: [[fontawesome icon::$icon]]\n";
      $summary = "WTLiPD: New Property";
			//$out->addWikiText("$summary: $prop\n");
      WTFactsAPI::createWikiPage("Property:$prop", $text, $summary);
    }
		return SMWDIProperty::newFromUserLabel( $prop );
	}

	private function uploadFile( $filename, $out ) {
		$repo = RepoGroup::singleton()->getLocalRepo();
		$file = new LocalFile($filename, $repo);
		$status = $file->upload($this->lipddir."/$filename", "WTLiPD: Initial upload", "");
		if($status->isGood()) {
			return "File:$filename";
		}
		//$out->addWikiText("Uploading file $filename\n");
		return null;
	}

	private function getCompoundKeyId($compound_key, $obj) {
		$tobj = $obj;
		foreach($compound_key as $key) {
			if(is_array($tobj) && array_key_exists($key, $tobj)) {
				$tobj = $tobj[$key];
			}
			else
				return null;
		}
		if(!is_array($tobj))
			return $tobj;
		return null;
	}

	private function getBindingKeyId($key, $obj) {
		$key_options = explode("|", $key);
		foreach($key_options as $optkey) {
			$compound_key = explode(".", $optkey);
			$keyid = $this->getCompoundKeyId($compound_key, $obj);
			if($keyid)
				return $keyid;
		}
		return uniqid();
	}

	private function getFunctionKeyId($fn, $arg, $curobjid) {
		switch($fn) {
			case "trunc":
				return substr($curobjid, 0, strlen($curobjid) - $arg);
				break;
			case "uniqid":
				return $curobjid . uniqid($arg);
				break;
		}
		return $curobjid;
	}

	private function createIdFromPattern($pattern, $obj) {
		$objid = "";
		foreach($pattern as $key) {
			if(preg_match("/{(.+)}/", $key, $m)) {
				$objid .= $this->getBindingKeyId($m[1], $obj);
			}
			else if(preg_match("/_(.+)\((.*)\)/", $key, $m)) {
				$fn = $m[1];
				$arg = $m[2];
				$objid = $this->getFunctionKeyId($fn, $arg, $objid);
			}
			else
				$objid .= $key;
		}
		return $objid;
	}

	private function getObjectId($obj, $category, $schema) {
		$objid = is_array($obj) ? "Unknown." .$category.uniqid($category) : str_replace(" ", "_", ucfirst($obj));
		if(array_key_exists("@id", $schema)) {
			$objid = $this->createIdFromPattern($schema["@id"], $obj);
		}
		return $this->fixTitle($objid);
	}

	private function fixTitle($titleid) {
		return preg_replace('@\x{FFFD}@u', '_', $titleid);
	}

	private function resolveProperty($name) {
		global $wgCore;
		$corename = $name."_$wgCore";
		if($this->pageExists("Property:$corename"))
			return $corename;
		if($this->pageExists("Property:$name"))
			return $name;
		return null;
	}

	private function resolveCategory($name) {
		global $wgCore;
		$corename = $name."_$wgCore";
		if($this->pageExists("Category:$corename"))
			return $corename;
		if($this->pageExists("Category:$name"))
			return $name;
		return null;
	}

	private function guessValueType($value) {
		if($value && is_array($value)) {
			if(!Util::is_assoc($value)) {
				foreach($value as $subvalue)
					return $this->guessValueType($subvalue);
			}
			else
				return "Page";
		}
		else if($value) {
			$valtype = WTFactsAPI::guessValueType($value);
			if($valtype == "String" || $valtype == "Page")
				$valtype = "Text";
			return $valtype;
		}
		return "Text";
	}

	private function getPropertyDetails($key, &$schema, $value) {
		$pname = $this->fromCamelCase($key);
		$details = array("name"=>$pname);

		if(array_key_exists($key, $schema) &&
				array_key_exists("@@processed", $schema[$key]))
			return $schema[$key];

		// Get details from schema
		if(array_key_exists($key, $schema)) {
			foreach($schema[$key] as $skey=>$svalue)
				$details[$skey] = $svalue;
		}
		if(array_key_exists("schema", $details))
			$details["type"] = "Page";
		$pname = ucfirst($details["name"]);

		// Get more details from the actual property (if it exists)
		$newname = $this->resolveProperty($pname);
		if($newname) {
			$pdetails = WTFactsAPI::getPropertyDetails($newname);
			$details["name"] = $newname;
			if(array_key_exists("type", $details) && $details["type"] != "File")
				$details["type"] = WTFactsAPI::getPropertyTypeFromDIType($pdetails["type"]);
		}
		if(!array_key_exists("type", $details)) {
			$details["type"] = $this->guessValueType($value);
			if(!array_key_exists("type", $details))
				$details["type"] = "Text";
		}
		$details["@@processed"] = true;
		$schema[$key] = $details;

		return $details;
	}

	private function fromCamelCase($str) {
		return ucfirst($str);
		//return ucfirst(preg_replace('/([^A-Z])([A-Z])/', "$1_$2", $str));
	}

	private function bootstrapWiki($out) {
		global $SCHEMA;

		// Create Specific Pages
		$pages = array(
			"Template:LiPDLocation" => array("text"=>"<includeonly>'''[[{{{1}}}]]'''\n\nSite name: {{{2}}}</includeonly>")
		);
		foreach($pages as $pageid=>$details) {
			if(!$this->pageExists($pageid))
				WTFactsAPI::createWikiPage($pageid, $details["text"], "WTLiPD: Bootstrap");
		}

		// Create Properties
		$properties = array(
			"Has_User_ID" => array("type"=>"Text"),
			"Has_multiple_values" => array("type"=>"Boolean"),
			"Fontawesome_icon" => array("type"=>"Text"),
		);
		foreach($properties as $prop=>$details) {
			$cat = array_key_exists("category", $details) ? $details["category"] : null;
			$icon = array_key_exists("icon", $details) ? $details["icon"] : null;
			$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
			$this->createProperty($prop, $details["type"], $cat, $icon, $multiple, $out);
		}
	}
}

?>
