<?php

require_once("LiPDSchema.inc");
require_once("BagitHelper.inc");

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
				$this->bootstrapWiki();
				self::$bootstrapped = true;
			}

			$obj = Util::readJson($jsonld);
			$this->createWikiItems($obj, null, "LiPD", "LiPD");

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
		return $title->exists();
	}

	private function createWikiItems($obj, $parent, $category, $schemaname) {
		global $SCHEMA;
		$schema = array_key_exists($schemaname, $SCHEMA) ? $SCHEMA[$schemaname] : null;

		$objid = $this->getObjectId($obj, $parent, $category, $schema);
		$subobjects = array();

		// Create category
		if($category)
			$this->createCategory( $category, $schema );

		// Create Page
		$data = $this->createPage($objid, $category);

		if(is_array($obj)) {
			foreach($obj as $key=>$value) {
				if($key[0] == "@") continue;

				$details = $this->getPropertyDetails($key, $schema);
				$prop = $details["name"];
				$type = $details["type"];
				$icon = array_key_exists("icon", $details) ? $details["icon"] : null;
				$cat = array_key_exists("category", $details) ? $details["category"] : null;
				$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
				$fromJson = array_key_exists("fromJson", $details) ? $details["fromJson"] : null;
				$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
				$subobject = array_key_exists("subobject", $details) ? $details["subobject"] : false;
	
				// Create Property
				$propDI = $this->createProperty( $prop, $type, $cat, $icon, $multiple );
	
				if($fromJson) {
					$value = call_user_func($fromJson, $value, $objid);
					if(!$value)
						continue;

					if(is_array($value) && !Util::is_assoc($value)) {
						foreach($value as $subvalue) {
							if(Util::is_assoc($subvalue)) {
								$ret = $this->createWikiItems($subvalue, $obj, $cat, $sch);
								$data = $this->setProperty( $data, $propDI, $ret["id"]);
								$data = $this->setSubobjects( $data, $objid, $ret["id"], $ret["subobjects"], $schema);
							}
						}
						continue;
					}
					else if(Util::is_assoc($value)) {
						$ret = $this->createWikiItems($value, $obj, $cat, $sch);
						$data = $this->setProperty( $data, $propDI, $ret["id"]);
						$data = $this->setSubobjects( $data, $objid, $ret["id"], $ret["subobjects"], $schema);
						continue;
					}
				}
	
				if($subobject) {
					$subobjects[] = array($prop=>$value);
					continue;
				}
	
				if(is_array($value) && !Util::is_assoc($value)) {
					foreach($value as $subvalue) {
						if($type == "Page") {
							$ret = $this->createWikiItems($subvalue, $obj, $cat, $sch);
							$data = $this->setProperty( $data, $propDI, $ret["id"]);
							$data = $this->setSubobjects( $data, $objid, $ret["id"], $ret["subobjects"], $schema);
						}
						else 
							$data = $this->setProperty( $data, $propDI, $subvalue);
					}
				}
				else if($type == "Page") {
					$ret = $this->createWikiItems($value, $obj, $cat, $sch);
					$data = $this->setProperty( $data, $propDI, $ret["id"]);
					$data = $this->setSubobjects( $data, $objid, $ret["id"], $ret["subobjects"], $schema);
				}
				else if($type == "File") {
					$fileid = $this->uploadFile( $value );
					if($fileid)
						$data = $this->setProperty( $data, $propDI, $fileid);
				}
				else {
					$data = $this->setProperty( $data, $propDI, $value);
				}
			}
		}
		$this->savePage( $objid, $data );
		return array("id"=>$objid, "subobjects"=>$subobjects);
	}


	private function setProperty( $data, $propDI, $value ) {
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
				$details = $this->getPropertyDetails($key, $schema);
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
		print "\n";
		return $data;
	}

	private function createPage($objid, $category) {
		// Delete current page if one currently exists
		$title = Title::newFromText($objid);
        /*if($title->exists())
			WTFactsAPI::deleteWikiPage($objid, "Re-import");*/

        if(!$title->exists()) {
			$text = $category ? "[[Category:$category]]\n" : "";
			$summary = "Creating new page" . ($category ? " with Category: $category" : "");
			$ret = WTFactsAPI::createWikiPage($objid, $text, $summary);
			if($ret[0] != "Success")
				return null;
		}

		$subject = SMWDIWikiPage::newFromTitle( $title );
		$data = new SMWSemanticData($subject);
		return $data;
	}

	private function createCategory( $category, $schema ) {
		$pageid = "Category:$category";

		/*$title = Title::newFromText($pageid);
        if($title->exists())
			WTFactsAPI::deleteWikiPage($pageid, "Re-import");*/

		$text = "===Category: $category===\n";
		if($schema) {
			foreach($schema as $key=>$val) {
				$details = $this->getPropertyDetails($key, $schema);
				$standard = array_key_exists("standard", $details) ? $details["standard"] : false;
				if($standard)
					$text .= "*[[has property::Property:".$details["name"]."]]\n";
			}
		}
		$summary = "Create category $category";
		$ret = WTFactsAPI::createWikiPage($pageid, $text, $summary);
		if($ret[0] != "Success")
			return false;
		return true;
	}

	private function savePage( $objid, $data ) {
		WTFactsAPI::updateWiki( $objid, "Initial Properties Import", $data );
	}

	private function createProperty( $prop, $type, $cat, $icon, $multiple ) {
        $ptitle = Title::newFromText("Property:".$prop);
		if($type == "File")
			$type = "Page";

        if(!$ptitle->exists()) {
			$text = "* Property Type: [[has type::$type]]";
			if($cat)
				$text .= " of [[has page category::Category:$cat]]";
			$text .= "\n";
			if($multiple)
				$text .= "* Can have multiple values ? [[has multiple values::true]]\n";
			if($icon)
				$text .= "* Font-awesome Icon: [[fontawesome icon::$icon]]\n";
            $summary = "Creating new Property";
            WTFactsAPI::createWikiPage("Property:$prop", $text, $summary);
        }
		return SMWDIProperty::newFromUserLabel( $prop );
	}

	private function uploadFile( $filename ) {
		$repo = RepoGroup::singleton()->getLocalRepo();
		$file = new LocalFile($filename, $repo);
		$status = $file->upload($this->lipddir."/$filename", "Initial upload", "");
		if($status->isGood()) {
			return "File:$filename";
		}
		return null;
	}

	private function getCompoundKeyId($compound_key, $obj, $parent) {
		$tobj = $obj;
		foreach($compound_key as $key) {
			if($key == "_parent") {
				$tobj = $parent;
				continue;
			}
			if(is_array($tobj) && array_key_exists($key, $tobj)) {
				$parent = $tobj;
				$tobj = $tobj[$key];
			}
			else
				return null;
		}
		if(!is_array($tobj))
			return $tobj;
		return null;
	}

	private function getBindingKeyId($key, $obj, $parent) {
		$key_options = explode("|", $key);
		foreach($key_options as $optkey) {
			$compound_key = explode(".", $optkey);
			$keyid = $this->getCompoundKeyId($compound_key, $obj, $parent);
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

	private function getObjectId($obj, $parent, $category, $schema) {
		$objid = is_array($obj) ? uniqid($category) : str_replace(" ", "_", ucfirst($obj));
		if($schema && array_key_exists("_id", $schema)) {
			$objid = "";
			foreach($schema["_id"] as $key) {
				if(preg_match("/{(.+)}/", $key, $m)) {
					$objid .= $this->getBindingKeyId($m[1], $obj, $parent);
				}
				else if(preg_match("/_(.+)\((.*)\)/", $key, $m)) {
					$fn = $m[1];
					$arg = $m[2];
					$objid = $this->getFunctionKeyId($fn, $arg, $objid);
				}
				else 
					$objid .= $key;
			}
		}
		return $this->fixTitle($objid);
	}

	private function fixTitle($titleid) {
		return preg_replace('@\x{FFFD}@u', '_', $titleid);
	}

	private function getPropertyDetails($key, $schema) {
		$pname = $this->fromCamelCase($key);
		$details = array("name"=>$pname, "type"=>"Text");
		// Create property 
		if($schema && array_key_exists($key, $schema)) {
			foreach($schema[$key] as $skey=>$svalue) 
				$details[$skey] = $svalue;
		}

		return $details;
	}

	private function fromCamelCase($str) {
		return ucfirst(preg_replace('/([^A-Z])([A-Z])/', "$1_$2", $str));
	}

	private function bootstrapWiki() {
		global $SCHEMA;

		// Create Specific Pages
		$pages = array(
			"Template:LiPDLocation" => array("text"=>"<includeonly>'''[[{{{1}}}]]'''\n\nSite name: [[{{{2}}}]]</includeonly>")
		);
		foreach($pages as $pageid=>$details) {
			WTFactsAPI::createWikiPage($pageid, $details["text"], "Bootstrap");
		}

		// Create Properties
		$properties = array(
			"Has_User_ID" => array("type"=>"Text"),
			"Has_multiple_values" => array("type"=>"Boolean"),
			"Fontawesome_icon" => array("type"=>"Text"),
		);
		$catschemamap = array();
		foreach($SCHEMA as $schid=>$schema) {
			foreach($schema as $prop=>$det) {
				if($prop == "_id") continue;

				$details = $this->getPropertyDetails($prop, $schema);
				$standard = array_key_exists("standard", $details) ? $details["standard"] : false;
				$pname = $details["name"];
				$properties[$pname] = $details;
				
				if(array_key_exists("category", $details)
						&& array_key_exists("schema", $details)) {
					$catschemamap[$details["category"]] = $details["schema"];
				}
			}
		}
		foreach($properties as $prop=>$details) {
			$cat = array_key_exists("category", $details) ? $details["category"] : null;
			$icon = array_key_exists("icon", $details) ? $details["icon"] : null;
			$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
			$this->createProperty($prop, $details["type"], $cat, $icon, $multiple);
		}


		// Create Category Pages
		foreach($catschemamap as $category=>$schid) {
			$schema = array_key_exists($schid, $SCHEMA) ? $SCHEMA[$schid] : null;
			$this->createCategory($category, $schema);
		}
	}
	
}

class Util {
  public static function readFile($filename) {
    $rval = @file_get_contents($filename);
    if($rval === false) {
      throw new Exception('File read error: ' . $filename);
    }
    return $rval;
  }
  public static function readJson($filename) {
    $rval = json_decode(self::readFile($filename), true);
    if($rval === null) {
      throw new Exception('JSON parse error');
    }   
    return $rval;
  }     
  public static function readNQuads($filename) {
    return self::readFile($filename);
  }
  public static function jsonldEncode($input) {
    // newer PHP has a flag to avoid escaped '/'
    if(defined('JSON_UNESCAPED_SLASHES')) {
      $options = JSON_UNESCAPED_SLASHES;
      if(defined('JSON_PRETTY_PRINT')) {
        $options |= JSON_PRETTY_PRINT;
      }
      $json = json_encode($input, $options);
    } else {
      // use a simple string replacement of '\/' to '/'.
      $json = str_replace('\\/', '/', json_encode($input));
    }
    return $json;
  }

  public static function is_assoc($arr) {
    return is_array($arr) && (array_keys($arr) !== range(0, count($arr) - 1));
  }
}
?>
