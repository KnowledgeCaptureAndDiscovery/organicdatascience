<?php

require_once("LiPDSchema.inc");
require_once("BagitHelper.inc");
require_once("Util.inc");

class LiPDWiki {
	var $id;
	var $tmpdir;
	var $R_SCHEMA;

	public function __construct($id) {
		$this->id = $id;
	}

	public function delete($out) {
		try {
			$this->deleteFromWiki($this->id);
		}
		catch (Exception $e) {
			$out->addHtml($e);
		}
	}

	public function export($out) {
		try {
			$this->tmpdir = BagitHelper::tempdir();
			$this->R_SCHEMA = $this->getSchemaReverseMap();
			$facts = $this->getFactsRecursively($this->id);
			$lipd = $this->convertToLiPD($this->id, $facts, "Dataset", "Dataset");
			$lipd = $this->postProcessing($lipd, null);
			$this->cleanUp($lipd);

			file_put_contents($this->tmpdir."/".$this->id.".jsonld", json_encode($lipd, JSON_PRETTY_PRINT));

			return BagitHelper::createLiPD($this->id, $this->tmpdir);
		}
		catch (Exception $e) {
			error_log($e);
		}
	}

	private function deleteFromWiki($pageid) {
		$deletionQ = array($pageid);
		$deletionPageList = array();

		// Get the whole list of pages to be deleted
		while(sizeof($deletionQ) > 0) {
			$delpageid = array_shift($deletionQ);
			if(in_array($delpageid, $deletionPageList))
				continue;
			$deletionPageList[] = $delpageid;

			list($status, $facts, $txt) = WTFactsAPI::getFacts($delpageid);
			foreach($facts as $prop=>$details)
				if(array_key_exists("values", $details))
					foreach($details["values"] as $value)
						if(($value["type"] == "WikiPage") &&
								!in_array($value["key"], $deletionPageList))
							$deletionQ[] = $value["key"];
						else if(preg_match("/^File:(.+)/", $value["text"]))
							$deletionPageList[] = $value["text"];
		}

		foreach($deletionPageList as $delpageid) {
			if($this->isCategory($delpageid)) {
				//print "Is Category. Not deleting $delpageid\n";
				continue;
			}
			if($this->isObservation($delpageid)) {
				//print "Is Observation. Not deleting $delpageid\n";
				continue;
			}
			$subprops = WTFactsAPI::getInSubjectProperties($delpageid);
			$numExternalReferences = 0;
			foreach($subprops as $subprop) {
				if(!in_array($subprop["subject"], $deletionPageList))
					$numExternalReferences++;
			}
			if($numExternalReferences == 0) {
				if(preg_match("/^File:(.+)/", $delpageid, $m))
					$this->deleteFileFromWiki($m[1]);
				WTFactsAPI::deleteWikiPage($delpageid, $this->id." LiPD Dataset was deleted");
			}
		}
	}

	private function isCategory($pageid) {
		return preg_match("/Category:/i", $pageid);
	}

	private function isObservation($pageid, $pages=array()) {
		global $wgCore;
		$categories = WTFactsAPI::getPageCategories($pageid);
		if(array_key_exists($pageid, $pages))
			return false;
		$pages[$pageid] = true;

		foreach($categories as $cat) {
			if($cat == "ProxyObservation_$wgCore") {
				return true;
			}
			else {
				$is = $this->isObservation("Category:$cat", $pages);
				if($is)
					return true;
			}
		}
		return false;
	}

	private function deleteFileFromWiki($fileid) {
		$repo = RepoGroup::singleton()->getLocalRepo();
    $file = new LocalFile($fileid, $repo);
		$file->delete($this->id." LiPD Dataset was deleted");
	}

	private function getFactsRecursively($pageid, $pagesdone=array()) {
		if(array_key_exists($pageid, $pagesdone))
			return;
		$pagesdone[$pageid] = true;

		$pageobjects = array();
		list($status, $facts, $txt) = WTFactsAPI::getFacts($pageid);
		foreach($facts as $prop=>&$details) {
			if($prop == "subobjects") continue;

			if(array_key_exists("values", $details)) {
				foreach($details["values"] as &$value) {
					if($value["type"] == "WikiPage") {
						$valpage = $value["key"];
						$value["facts"] = $this->getFactsRecursively($valpage, $pagesdone);
						$pageobjects[$valpage] = &$value;
					}
					unset($value["_BC_bools"]);
				}
			}
			unset($details["_BC_bools"]);
		}

		// Add subobject facts to page objects
		foreach($facts as $prop=>&$details) {
			if($prop == "subobjects") {
				foreach($details as $key=>$subfacts) {
					if(array_key_exists($key, $pageobjects)) {
						$pageobjects[$key]["facts"] = array_merge($pageobjects[$key]["facts"], $subfacts);
					}
				}
			}
			unset($details["subobjects"]);
		}
		unset($facts["subobjects"]);

		return $facts;
	}

	private function getSchemaReverseMap() {
		global $SCHEMA;
		$newschema = array();
		foreach($SCHEMA as $schid=>$sch) {
			$newsch = array();
			foreach($sch as $prop=>$details) {
				if($prop[0] == "@") continue;
				if(array_key_exists("hack", $details)) continue;
				$pdetails = $this->getPropertyDetails($prop, $sch);
				$pname = ucfirst($pdetails["name"]);
				$pdetails["name"] = $prop;

				$newsch[$pname] = $pdetails;
				if(array_key_exists("category", $pdetails)) {
					$catpname = $pname . "." . ucfirst($pdetails["category"]);
					$newsch[$catpname] = $pdetails;
				}
				if(array_key_exists("schema", $pdetails)) {
					$schpname = $pname . "." . ucfirst($pdetails["schema"]);
					$newsch[$schpname] = $pdetails;
				}
			}
			$newschema[$schid] = $newsch;
		}
		return $newschema;
	}

	private function convertToLiPD($id, $facts, $category, $schemaname) {
		if(is_array($facts) && sizeof($facts) > 0) {
			$schema = array_key_exists($schemaname, $this->R_SCHEMA) ? $this->R_SCHEMA[$schemaname] : null;
			if($schemaname && !$category)
				$category = $schemaname;

			$cats = WTFactsAPI::getPageCategories($id);
			if($cats && sizeof($cats) > 0)
				$category = $this->toLipdId($cats[0]);

			$obj = array("@id"=>$id, "@category"=>$category, "@schema"=>$schemaname);
			foreach($facts as $pname=>$pfacts) {
				$prop = $this->toLipdId($pname);
				$prop = preg_replace("/\s/", "_", $prop);

				// Get a sample value page category, and use to make a property key
				$propkey = $prop;
				if(array_key_exists("values", $pfacts)) {
					foreach($pfacts["values"] as $value) {
						$val = $value["val"];
						if($value["type"] == "WikiPage") {
							$valcats = WTFactsAPI::getPageCategories($val);
							if($valcats && sizeof($valcats) > 0) {
								$propkey .= "." . $this->toLipdId($valcats[0]);
								break;
							}
						}
					}
				}

				$details = $this->getWikiPropertyDetails($prop, $propkey, $schema);
				$name = $details["name"];
				$type = array_key_exists("type", $details) ? $details["type"] : null;
				$cat = array_key_exists("category", $details) ? $details["category"] : null;
				$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
            	if($cat && !$sch)
                	$sch = $cat;
				$toJson = array_key_exists("toJson", $details) ? $details["toJson"] : null;
				$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
				$nofollow = array_key_exists("nofollow", $details) ? $details["nofollow"] : false;
				$subobject = array_key_exists("subobject", $details) ? $details["subobject"] : false;

				if(array_key_exists("values", $pfacts)) {
					if($multiple)
						$obj[$name] = array();

					foreach($pfacts["values"] as $value) {
						$val = $value["val"];
						if($value["type"] == "WikiPage" && !$nofollow)
							$val = $this->convertToLiPD($value["key"], $value["facts"], $cat, $sch);

						if($toJson)
							$val = call_user_func($toJson, $val);

						if($type == "File") {
							$val =  preg_replace("/^File:/", "", $val);
							$this->downloadFile($val);
						}

						// If there is already a value present
						// - Then this need to be marked as "multiple"
						if(!$multiple && array_key_exists($name, $obj) &&
								!is_array($obj[$name]) && !Util::is_assoc($obj[$name])) {
							$multiple = true;
							$obj[$name] = [$obj[$name]];
						}

						if($multiple)
							$obj[$name][] = $val;
						else
							$obj[$name] = $val;
					}
				}
			}
			if(empty(array_keys($obj)))
				return preg_replace("/_/", " ", $id);

			return $obj;
		}
		else {
			return preg_replace("/_/", " ", $id);
		}
	}

	private function postProcessing($obj, $parent) {
		global $SCHEMA;
		if(!Util::is_assoc($obj))
			return $obj;
		if(!array_key_exists("@schema", $obj))
			return $obj;

		foreach($obj as $key=>$value) {
			if(is_array($value) && !Util::is_assoc($value)) {
				for($i=0; $i<sizeof($value); $i++)
					$obj[$key][$i] = $this->postProcessing($value[$i], $obj);
			}
			else
				$obj[$key] = $this->postProcessing($value, $obj);
		}

		$schemaname = $obj["@schema"];
		$tschema = array_key_exists($schemaname, $SCHEMA) ? $SCHEMA[$schemaname] : null;
		if($tschema && array_key_exists("@toJson", $tschema))
			foreach($tschema["@toJson"] as $func)
				$obj = call_user_func($func, $obj, $parent);

		return $obj;
	}

	private function cleanUp(&$obj) {
		if(is_array($obj))
			foreach($obj as $key=>$value)
				if($key[0] == "@")
					unset($obj[$key]);
				else
					$this->cleanup($obj[$key]);
	}


	private function getPropertyDetails($key, $schema) {
		$pname = $this->toWikiId($key);
		$details = array("name"=>$pname);
		// Create property
		if($schema && array_key_exists($key, $schema)) {
			foreach($schema[$key] as $skey=>$svalue)
				$details[$skey] = $svalue;
		}
		return $details;
	}

	private function getWikiPropertyDetails($pname, $fullkey, $schema) {
		$key = $pname;
		$pname = lcfirst($this->toLipdId($pname));
		$details = array("name"=>$pname);
		// Check for full key in schema
		if($schema && array_key_exists($fullkey, $schema)) {
			foreach($schema[$fullkey] as $skey=>$svalue)
				$details[$skey] = $svalue;
		}
		// Or check for just pname in schema
		else if($schema && array_key_exists($key, $schema)) {
			foreach($schema[$key] as $skey=>$svalue)
				$details[$skey] = $svalue;
		}
		return $details;
	}

	private function toWikiId($str) {
		//return ucfirst(preg_replace('/([^A-Z])([A-Z])/', "$1_$2", $str));
		return ucfirst($str);
	}

	private function toLipdId($prop) {
		global $wgCore;
		$prop = preg_replace("/[\s_].*$wgCore$/", "", $prop);
		/*$prop = preg_replace("/_(.?)/e","strtoupper('$1')", $prop);
		if(!preg_match("/^[A-Z][A-Z]/", $prop))
			$prop = lcfirst($prop);*/
		return $prop;
	}

	private function downloadFile( $filename ) {
		$repo = RepoGroup::singleton()->getLocalRepo();
		$file = new LocalFile($filename, $repo);
		if($file->exists()) {
			$contents = file_get_contents($file->getFullUrl());
			file_put_contents($this->tmpdir."/$filename", $contents);
		}
		return null;
  }
}
