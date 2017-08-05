<?php

global $PAGE_NAMES;

$PAGE_NAMES["Dataset"] = array(
	"id" => array("{Name}")
);
$PAGE_NAMES["Funding"] = array(
  "id" => array("{FundingAgency}", ".", "{GrantNumber}" )
);
$PAGE_NAMES["Publication"] = array(
  "id" => array("Publication.", "{HasDOI|>PublishedIn}")
);
$PAGE_NAMES["PaleoData"]= array(
  "id" => array("{>IncludesPaleoData}", ".PaleoData", "_index()")
);
$PAGE_NAMES["ChronData"]= array(
  "id" => array("{>IncludesChronData}", ".ChronData", "_index()")
);
$PAGE_NAMES["Model"] = array(
  "id" => array("{>ChronModeledBy|>PaleoModeledBy}", ".Model", "_index()")
);
$PAGE_NAMES["SoftwareCode"] = array(
  "id" => array("{>HasCode}", ".", "Name")
);
$PAGE_NAMES["DataTable"] = array(
  "id" => array("{HasFileName}", "_trunc(4)")
);
$PAGE_NAMES["Variable"] = array(
  "id" => array("{HasVariableID|>IncludesVariable}", ".", "{Name}")
);
$PAGE_NAMES["ProxySystem"] = array(
  "id" => array("ProxySystem.", "{ProxyArchiveType}", ".",
        "{ProxySensorType}", ".", "{ProxyObservationType}", ".",
        "{PaleoModeledBy|ChronModeledBy}"),
  "required" => true
);
$PAGE_NAMES["Resolution"] = array(
  "id" => array("{>HasResolution}", ".Resolution")
);
$PAGE_NAMES["Location"] = array(
  "id" => array("{>CollectedFrom}", ".Location")
);
$PAGE_NAMES["Interpretation"] = array(
  "id" => array("{>InterpretedAs}", ".Interpretation", "_index()")
);
$PAGE_NAMES["CalibrationModel"] = array(
  "id" => array("{>CalibratedVia}", ".Calibration")
);
$PAGE_NAMES["Uncertainty"] = array(
  "id" => array("{>HasUncertainty}", ".Uncertainty", "_index()")
);

class WTPageNaming {
  private $category;
  private $facts;
  private $incoming;

	public function __construct($category, $facts, $incoming) {
    $this->category = $category;
    $this->facts = $facts;
    $this->incoming = $incoming;
	}

  private function getReverseBindingKeyId($key) {
    global $wgCore;
    $vals = null;
    foreach($this->incoming as $inc) {
        if($inc["property"] == $key ||
            $inc["property"] == "$key $wgCore")
          return $inc["subject"];
    }
    return null;
	}

	private function getBindingKeyId($key) {
    global $wgCore;
    $vals = null;

    if(preg_match("/^>(.+)$/", $key, $m)) {
      return $this->getReverseBindingKeyId($m[1]);
    }

    if(array_key_exists($key, $this->facts))
      $vals = $this->facts[$key]->values;
    else {
      $key .= " ".$wgCore;
      if(array_key_exists($key, $this->facts)) {
        $vals = $this->facts[$key]["values"];
      }
    }
    if($vals && sizeof($vals))
      return $vals[0]["val"];

    return null;
	}

	private function getFunctionKeyId($fn, $arg, $curobjid) {
		switch($fn) {
			case "trunc":
				return substr($curobjid, 0, strlen($curobjid) - $arg);
				break;
      case "index":
        $index = 1;
  			$curobjid = $curobjid . $index;
        // TODO: Check that $pageid doesn't already exist, else increment index
  			break;
			case "uniqid":
				return $curobjid . uniqid($arg);
				break;
		}
		return $curobjid;
	}

	private function createIdFromPattern($pattern) {
    global $wgTitle;
		$objid = "";
		foreach($pattern["id"] as $key) {
			if(preg_match("/{(.+)}/", $key, $m)) {
        $key_options = explode("|", $m[1]);
        $keyid = null;
        foreach($key_options as $optkey) {
  				$keyid = $this->getBindingKeyId($optkey);
          if($keyid) {
            $objid .= $keyid;
            break;
          }
        }
        /*if(!$keyid)
          $objid .= "_uniq()";*/
			}
			else if(preg_match("/_(.+)\((.*)\)/", $key, $m)) {
				$fn = $m[1];
				$arg = $m[2];
				$objid = $this->getFunctionKeyId($fn, $arg, $objid);
			}
			else
				$objid .= $key;
		}
    $objid = preg_replace("/\.+$/", "", $objid);
    $objid = preg_replace("/\s+/", "_", $objid);
    if($objid != preg_replace("/\s+/", "_", $wgTitle))
		  return $objid;
    return null;
	}

	private function fixTitle($titleid) {
		return preg_replace('@\x{FFFD}@u', '_', $titleid);
	}

	public function getPageId() {
    global $PAGE_NAMES, $wgCore;
    $queue = array($this->category);
    $done = array();
    while(sizeof($queue) > 0) {
      $qcat = array_shift($queue);
      $done[] = $qcat;
      $qplain = str_replace($wgCore, "", $qcat);
      $qplain = preg_replace("/(\s|_)$/", "", $qplain);
      if(array_key_exists($qplain, $PAGE_NAMES))
        return $this->createIdFromPattern($PAGE_NAMES[$qplain]);
      $pcats = WTFactsAPI::getPageCategories("Category:".$qcat);
      $queue = array_diff(array_unique(array_merge($queue, $pcats)), $done);
    }
    return null;
  }
};
