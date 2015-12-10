<?php

require_once("LiPDSchema.inc");
require_once("BagitHelper.inc");

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
			$out->addHtml("*** Could not parse $id<br/>\n");
			$out->addHtml($e);
		}
	}

	public function export($out) {
		try {
			$this->tmpdir = BagitHelper::tempdir();
			$this->R_SCHEMA = $this->getSchemaReverseMap(); 
			$facts = $this->getFactsRecursively($this->id);

			$lipd = $this->convertToLiPD($this->id, $facts, "LiPD");
			file_put_contents($this->tmpdir."/".$this->id.".jsonld", json_encode($lipd, JSON_PRETTY_PRINT));

			return BagitHelper::createLiPD($this->id, $this->tmpdir);
		}
		catch (Exception $e) {
			$out->addHtml("*** Could not parse $id<br/>\n");
			$out->addHtml($e);
		}
	}

	private function deleteFromWiki($pageid, $pagesdone=array()) {
		if(array_key_exists($pageid, $pagesdone))
			return;
		$pagesdone[$pageid] = true;

		list($status, $facts, $txt) = WTFactsAPI::getFacts($pageid);
		foreach($facts as $prop=>$details) {
			if(array_key_exists("values", $details)) {
				foreach($details["values"] as $value) {
					if($value["type"] == "WikiPage") {
						$valpage = $value["key"];

						// Check that the value page is not being used anywhere else
						$subprops = WTFactsAPI::getInSubjectProperties($valpage);
						if(sizeof($subprops) <= 1)
							$this->deleteFromWiki($value["key"], $pagesdone);
					}
				}
			}
		}

		if(preg_match("/^File:(.+)/", $pageid, $m)) {
			$this->deleteFileFromWiki($m[1]);
		}

		WTFactsAPI::deleteWikiPage($pageid, $this->id." LiPD Dataset was deleted");
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
				if($prop == "_id") continue;
				if(array_key_exists("hack", $details)) continue;
				$pdetails = $this->getPropertyDetails($prop, $sch);
				$pname = $pdetails["name"];
				$pdetails["name"] = $prop;
				$newsch[$pname] = $pdetails;
			}
			$newschema[$schid] = $newsch;
		}
		return $newschema;
	}

	private function convertToLiPD($id, $facts, $schemaname) {
		$obj = array();
		if(is_array($facts)) {
			foreach($facts as $pname=>$pfacts) {
				$prop = preg_replace("/\s/", "_", $pname);
				$schema = array_key_exists($schemaname, $this->R_SCHEMA) ? $this->R_SCHEMA[$schemaname] : null;
				$details = $this->getWikiPropertyDetails($prop, $schema);
	
				$name = $details["name"];
				$type = $details["type"];
				$cat = array_key_exists("category", $details) ? $details["category"] : null;
				$sch = array_key_exists("schema", $details) ? $details["schema"] : null;
				$toJson = array_key_exists("toJson", $details) ? $details["toJson"] : null;
				$multiple = array_key_exists("multiple", $details) ? $details["multiple"] : false;
				$subobject = array_key_exists("subobject", $details) ? $details["subobject"] : false;
	
				if(array_key_exists("values", $pfacts)) {
					if($multiple) 
						$obj[$name] = array();
	
					foreach($pfacts["values"] as $value) {
						$val = $value["val"];
						if($value["type"] == "WikiPage") 
							$val = $this->convertToLiPD($value["key"], $value["facts"], $sch);
						
						if($toJson)
							$val = call_user_func($toJson, $val);

						if($type == "File") {
							$val =  preg_replace("/^File:/", "", $val);
							$this->downloadFile($val);
						}

						if($multiple)
							$obj[$name][] = $val;
						else
							$obj[$name] = $val;
					}
				}
			}
		}
		if(empty(array_keys($obj)))
			return preg_replace("/_/", " ", $id);

		return $obj;
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

	private function getWikiPropertyDetails($key, $schema) {
		$pname = $this->toCamelCase($key);
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

	private function toCamelCase($prop) {
		$prop = preg_replace("/_(.?)/e","strtoupper('$1')", $prop); 
		if(!preg_match("/^[A-Z][A-Z]/", $prop))
			$prop = lcfirst($prop);
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
